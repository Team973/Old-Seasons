package main

import (
	"errors"
	"net"
	"strconv"
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
