package main

import (
	"fmt"
	"net"
	"time"

	"github.com/ArtificialLegacy/robot_control/pkg/robot"
)

const ADDRESS = ":3131"

var robots []*robot.Robot

func main() {
	go server()

	speed := byte(255)
	dir := robot.MOTOR_FWD

	for {
		for _, r := range robots {
			if !r.Active() {
				continue
			}

			r.Send(robot.Command().
				Action(robot.ACTION_MSP, 1|dir, speed).
				Action(robot.ACTION_MSP, 2|dir, speed),
			)

			if speed == 0 {
				if dir == robot.MOTOR_FWD {
					dir = robot.MOTOR_BAK
				} else {
					dir = robot.MOTOR_FWD
				}

				speed = 255
			} else if speed == 255 {
				speed = 0
			}
		}

		time.Sleep(1000 * time.Millisecond)
	}
}

func server() {
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

		r := robot.NewRobot(c)
		robots = append(robots, r)
	}
}
