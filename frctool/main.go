package main

import (
	"bitbucket.org/zombiezen/ftp"
	"flag"
	"io"
	"log"
	"net"
	"os"
	slashpath "path"
	"path/filepath"
	"strings"
)

func main() {
	address := &net.TCPAddr{IP: robotAddress(1234)}
	if team, err := hostTeamNumber(); err == nil {
		address.IP = robotAddress(team)
	}

	flag.Var(IPFlag{&address.IP}, "address", "IP address to connect to")
	flag.Var(RobotAddressFlag{&address.IP}, "team", "Set the IP address via team number")
	flag.IntVar(&address.Port, "port", 21, "Port to connect to")
	flag.Parse()

	args := flag.Args()
	if len(args) == 0 {
		log.Fatal("usage: frctool [options] command [options] ...")
	}

	var err error
	switch args[0] {
	case "lua":
		err = uploadLua(args[1:], address)
	case "c":
		err = uploadC(args[1:], address)
	case "error":
		err = downloadLuaError(args[1:], address)
	default:
		log.Fatalf("usage: unrecognized command %q", args[0])
	}

	if err != nil {
		log.Fatal(err)
	}
}

const frcPath = "/ni-rt/system/FRC_UserProgram.out"

func uploadC(args []string, address *net.TCPAddr) error {
	if len(args) != 1 {
		log.Fatal("usage: frctool c FILE")
	}

	client, err := connect(address)
	if err != nil {
		return err
	}
	defer client.Quit()

	return upload(client, frcPath, args[0])
}

const luaRoot = "/lua"

func uploadLua(args []string, address *net.TCPAddr) error {
	if len(args) != 1 {
		log.Fatal("usage: frctool lua DIR")
	}

	client, err := connect(address)
	if err != nil {
		return err
	}
	defer client.Quit()

	// Change to Lua root directory
	if reply, err := client.Do("CWD " + luaRoot); err != nil || !reply.Positive() {
		if err != nil {
			return err
		}

		if reply.Code == ftp.CodeFileUnavailable {
			// Directory doesn't exist, try to create it
			log.Printf("Creating directory %q", luaRoot)
			reply, err = client.Do("MKD " + luaRoot)
			if err != nil {
				return err
			} else if !reply.PositiveComplete() {
				return reply
			}

			// Directory created, change to it
			reply, err := client.Do("CWD " + luaRoot)
			if err != nil {
				return err
			} else if !reply.Positive() {
				return reply
			}
		} else {
			return reply
		}
	}

	// Get root directory, ensuring it ends in a trailing slash
	root := filepath.Clean(args[0])
	if !strings.HasSuffix(root, string(filepath.Separator)) {
		root += string(filepath.Separator)
	}

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

		// Upload
		log.Printf("Uploading %q to %q", path, slashpath.Join(luaRoot, destPath))
		err = upload(client, destPath, path)
		if err != nil {
			// Log error, but try to continue uploading
			log.Printf("ERROR for %q: %v", path, err)
		}
		return nil
	})

	return err
}

func downloadLuaError(args []string, address *net.TCPAddr) error {
	if len(args) != 0 {
		log.Fatal("usage: frctool error")
	}

	client, err := connect(address)
	if err != nil {
		return err
	}
	defer client.Quit()

	conn, err := client.Text("RETR /lua-error.txt")
	if err != nil {
		return err
	}
	defer conn.Close()

	_, err = io.Copy(os.Stdout, conn)
	return err
}

// upload copies a file via FTP
func upload(client *ftp.Client, dest, source string) error {
	f, err := os.Open(source)
	if err != nil {
		return err
	}
	defer f.Close()

	dataConn, err := client.Binary("STOR " + dest)
	if err != nil {
		return err
	}
	defer dataConn.Close()

	_, err = io.Copy(dataConn, f)
	return err
}
