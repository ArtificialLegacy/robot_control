package main

import (
	"fmt"
	"io"
	"net"
)

const ADDRESS = ":3131"

func main() {
	l, err := net.Listen("tcp", ADDRESS)
	if err != nil {
		panic(err)
	}
	defer l.Close()

	fmt.Printf("%s\n", l.Addr())

	for {
		c, err := l.Accept()
		if err != nil {
			panic(err)
		}

		go handleConnection(c)
	}
}

const (
	FLAG_NIL byte = 0b0000_0000
	FLAG_ACK byte = 0b0000_0001
	FLAG_CMD byte = 0b0000_0010
	FLAG_RPT byte = 0b0000_0011
)

const (
	ACTION_NIL byte = 0b0000_0000 // len: 0
	ACTION_MDI byte = 0b0000_0001 // len: 1 [MOTOR]
	ACTION_MSP byte = 0b0000_0010 // len: 2 [DIR|MOTOR] [SPEED]
	ACTION_SET byte = 0b0000_0011 // len: 4 [CLASS] [DEVICE] [VAR] [VAL]
)

// largest 2 bits of motor byte are the direction, other 6 bits are motor id.
const (
	MOTOR_NIL byte = 0b0000_0000
	MOTOR_FWD byte = 0b1000_0000
	MOTOR_BAK byte = 0b0100_0000
	MOTOR_MDR byte = 0b1100_0000
	MOTOR_MID byte = 0b0011_1111
)

const (
	REPORT_NIL byte = 0b0000_0000 // len: 0
	REPORT_DEV byte = 0b0000_0001 // len: 1 [CLASS] > len: 1 [COUNT]
	REPORT_MOT byte = 0b0000_0010 // len: 1 [MOTOR] > len: 2 [STATUS] [SPEED]
	REPORT_GET byte = 0b0000_0011 // len 3 [CLASS] [DEVICE] [VAR] > len: 1 [VAL]
)

var speed byte = 255

func handleConnection(c net.Conn) {
	fmt.Printf("Connection: %s\n", c.RemoteAddr().String())
	defer c.Close()

	// wait to be acknowledged
	tmp, err := read(c)
	if err != nil {
		return
	}
	if tmp[0] != FLAG_ACK {
		c.Write([]byte{
			FLAG_NIL,
		})
		fmt.Printf("failed to acknowledge, receieved %d instead.\n", tmp[0])
		return
	}

	c.Write([]byte{
		FLAG_ACK,
	})

	for {
		c.Write([]byte{
			FLAG_CMD,
			6,
			ACTION_MSP, 1 | MOTOR_FWD, speed,
			ACTION_MSP, 2 | MOTOR_FWD, speed,
		})

		tmp, err := read(c)
		if err != nil {
			return
		}
		if tmp[0] != FLAG_ACK {
			fmt.Printf("failed to acknowledge, receieved %d instead.\n", tmp[0])
			return
		}
	}
}

func read(c net.Conn) ([]byte, error) {
	tmp := make([]byte, 4096)

	_, err := c.Read(tmp)
	if err != nil {
		if err != io.EOF {
			fmt.Printf("read error: %s\n", err)
			return nil, err
		}
	}

	return tmp, nil
}
