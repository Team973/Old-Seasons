package main

import (
	"bitbucket.org/zombiezen/ftp"
	"errors"
	"fmt"
	"log"
	"net"
)

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

// hostTeamNumber returns the team number of the host machine.  This is found by
// querying the host's network interfaces.
func hostTeamNumber() (int, error) {
	addrs, err := net.InterfaceAddrs()
	if err != nil {
		return 0, err
	}
	for _, ia := range addrs {
		if tcpAddr, ok := ia.(*net.TCPAddr); ok {
			if team, err := extractTeamNumber(tcpAddr.IP); err == nil {
				return team, nil
			}
		}
	}
	return 0, errors.New("Host has no IP address with a team number")
}
