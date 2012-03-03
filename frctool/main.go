package main

import (
	"bitbucket.org/zombiezen/ftp"
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	slashpath "path"
	"path/filepath"
	"strings"
	"sync"
)

var (
	Address *net.TCPAddr
)

func main() {
	parseGlobalFlags()

	c := lookupCommand(flag.Arg(0))
	if c == nil {
		fmt.Fprintf(os.Stderr, "unrecognized command %q\n", flag.Arg(0))
		printUsage()
		os.Exit(2)
	}

	err := c.Func()
	if err != nil {
		log.Fatal(err)
	}
}

func parseGlobalFlags() {
	Address = &net.TCPAddr{IP: robotAddress(1234)}
	if team, err := hostTeamNumber(); err == nil {
		Address.IP = robotAddress(team)
	}

	flag.Usage = printUsage
	flag.Var(IPFlag{&Address.IP}, "address", "IP address to connect to")
	flag.Var(RobotAddressFlag{&Address.IP}, "team", "Set the IP address via team number")
	flag.IntVar(&Address.Port, "port", 21, "Port to connect to")
	flag.Parse()
	if flag.NArg() == 0 {
		printUsage()
		os.Exit(2)
	}
}

func lookupCommand(name string) *Command {
	for i := range commands {
		if name == commands[i].Name {
			return &commands[i]
		}
		for _, alias := range commands[i].Aliases {
			if name == alias {
				return &commands[i]
			}
		}
	}
	return nil
}

func printUsage() {
	fmt.Println("usage: frctool [options] command [options] ...")
	fmt.Println("\nGlobal options:")
	flag.PrintDefaults()
	fmt.Println("\nCommands:")
	for i := range commands {
		fmt.Printf("  %-7s %s\n", commands[i].Name, commands[i].Description)
	}
}

func printCommandUsage(c *Command, fs *flag.FlagSet) {
	fmt.Printf("usage: frctool [options] %s %s\n", c.Name, c.Usage)
	fmt.Println("\nGlobal options:")
	flag.PrintDefaults()
	if fs != nil {
		fmt.Printf("\n%s Options:\n", c.Name)
		fs.PrintDefaults()
	}
}

type Command struct {
	Name        string
	Aliases     []string
	Usage       string
	Description string
	Func        func() error
}

var commands []Command

func init() {
	commands = []Command{
		{
			"c",
			nil,
			"FILE",
			"Upload C/C++ program",
			uploadC,
		},
		{
			"lua",
			nil,
			"DIR",
			"Upload Lua directory",
			uploadLua,
		},
		{
			"error",
			nil,
			"",
			"Print Lua error",
			downloadLuaError,
		},
	}
}

const (
	frcPath      = "/ni-rt/system/FRC_UserProgram.out"
	luaRoot      = "/lua"
	luaErrorPath = "/lua-error.txt"
)

func uploadC() error {
	if flag.NArg() != 2 {
		printCommandUsage(lookupCommand("c"), nil)
		os.Exit(2)
	}

	client, err := connect(Address)
	if err != nil {
		return err
	}
	defer client.Quit()

	return upload(client, frcPath, flag.Arg(1))
}

func uploadLua() error {
	fs := flag.NewFlagSet("lua", flag.ContinueOnError)
	fs.Usage = func() {}
	nclients := fs.Int("nthreads", 2, "Set the number of simultaneous downloads")
	err := fs.Parse(flag.Args()[1:])

	if err != nil || flag.NArg() != 1 {
		printCommandUsage(lookupCommand("lua"), fs)
		os.Exit(2)
	}

	// Get root directory, ensuring it ends in a trailing slash
	root := filepath.Clean(fs.Arg(0))
	if !strings.HasSuffix(root, string(filepath.Separator)) {
		root += string(filepath.Separator)
	}
	if st, err := os.Stat(root); err == nil {
		if !st.IsDir() {
			return &os.PathError{"open", fs.Arg(0), errors.New("not a directory")}
		}
	} else {
		return err
	}

	// Create clients
	clients, err := createClientPool(*nclients, Address)
	if err != nil {
		return err
	}
	defer func() {
		for i := range clients {
			clients[i].Quit()
		}
	}()

	// Change to Lua root directory
	if reply, err := clients[0].Do("CWD " + luaRoot); err != nil || !reply.Positive() {
		if err != nil {
			return err
		}

		if reply.Code == ftp.CodeFileUnavailable {
			// Directory doesn't exist, try to create it
			log.Printf("Creating directory %q", luaRoot)
			reply, err = clients[0].Do("MKD " + luaRoot)
			if err != nil {
				return err
			} else if !reply.PositiveComplete() {
				return reply
			}

			// Directory created, change to it
			reply, err := clients[0].Do("CWD " + luaRoot)
			if err != nil {
				return err
			} else if !reply.Positive() {
				return reply
			}
		} else {
			return reply
		}
	}

	for _, client := range clients[1:] {
		reply, err := client.Do("CWD " + luaRoot)
		if err != nil {
			return err
		} else if !reply.Positive() {
			return reply
		}
	}

	// Start uploading pool
	ch := make(chan uploadRequest)
	var wg sync.WaitGroup
	wg.Add(len(clients))
	for i := range clients {
		go func(client *ftp.Client) {
			for r := range ch {
				err := upload(client, r.DestPath, r.Path)
				if err == nil {
					log.Printf("Uploaded %q to %q", r.Path, slashpath.Join(luaRoot, r.DestPath))
				} else {
					// Log error, but try to continue uploading
					log.Printf("ERROR for %q: %v", r.Path, err)
				}
			}
			wg.Done()
		}(clients[i])
	}

	// Walk files
	err = filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() || filepath.Ext(path) != ".lua" {
			return nil
		}

		// Remove root directory prefix
		destPath := path
		if strings.HasPrefix(path, root) {
			destPath = path[len(root):]
		}
		destPath = filepath.ToSlash(destPath)

		// Start upload
		ch <- uploadRequest{path, destPath}
		return nil
	})

	close(ch)
	wg.Wait()
	return err
}

type uploadRequest struct {
	Path     string
	DestPath string
}

func downloadLuaError() error {
	if flag.NArg() != 1 {
		printCommandUsage(lookupCommand("error"), nil)
		os.Exit(2)
	}

	client, err := connect(Address)
	if err != nil {
		return err
	}
	defer client.Quit()

	conn, err := client.Text("RETR " + luaErrorPath)
	if err != nil {
		return err
	}
	defer conn.Close()

	_, err = io.Copy(os.Stdout, conn)
	return err
}
