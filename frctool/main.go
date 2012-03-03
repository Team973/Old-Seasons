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
	"strconv"
	"strings"
)

type IPFlag struct {
	IP *net.IP
}

func (f IPFlag) String() string {
	return f.IP.String()
}

func (f IPFlag) Set(value string) error {
	*f.IP = net.ParseIP(value)
	if *f.IP == nil {
		return errors.New("Can't parse IP: " + value)
	}
	return nil
}

type RobotAddressFlag struct {
	IP *net.IP
}

func (f RobotAddressFlag) String() string {
	team, err := extractTeamNumber(*f.IP)
	if err != nil {
		return ""
	}
	return strconv.Itoa(team)
}

func (f RobotAddressFlag) Set(value string) error {
	team, err := strconv.Atoi(value)
	if err != nil {
		return err
	}
	if team < 1 || team > 9999 {
		return errors.New("Bad team number: " + value)
	}
	*f.IP = robotAddress(team)
	return nil
}

func main() {
	address := &net.TCPAddr{IP: robotAddress(1234)}
	if addrs, err := net.InterfaceAddrs(); err == nil {
		for _, ia := range addrs {
			if tcpAddr, ok := ia.(*net.TCPAddr); ok {
				if ip, err := extractTeamNumber(tcpAddr.IP); err == nil {
					address.IP = robotAddress(ip)
					break
				}
			}
		}
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
		if info.IsDir() {
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

// connect dials to a FTP server.
func connect(addr *net.TCPAddr) (*ftp.Client, error) {
	log.Printf("Connecting to %v", addr)
	conn, err := net.DialTCP("tcp", nil, addr)
	if err != nil {
		return nil, err
	}
	client, err := ftp.NewClient(conn)
	if err != nil {
		return nil, err
	}
	log.Print("Logging in anonymously")
	if err := client.Login("anonymous", ""); err != nil {
		return nil, err
	}
	return client, nil
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

// extractTeamNumber extracts a team number from a FIRST IP address.
func extractTeamNumber(ip net.IP) (team int, err error) {
	ip4 := ip.To4()
	if ip4 == nil {
		return 0, fmt.Errorf("%v is not an IPv4 address", ip)
	}
	if ip4[0] != 10 {
		return 0, fmt.Errorf("%v is not a FIRST address (must be in 10.0.0.0/24)", ip)
	}
	return int(ip4[1])*100 + int(ip4[2]), nil
}

// robotAddress returns the IP address of the robot with the given team number.
func robotAddress(team int) net.IP {
	return net.IPv4(10, byte(team/100), byte(team%100), 2)
}
