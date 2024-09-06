package robot

import (
	"fmt"
	"io"
	"net"
)

type Robot struct {
	active bool
	err    error
	c      net.Conn

	cmds chan []byte
}

const queueSize = 16

func NewRobot(c net.Conn) *Robot {
	r := &Robot{
		active: true,
		c:      c,

		cmds: make(chan []byte, queueSize),
	}

	go r.handle()

	return r
}

func (r *Robot) Active() bool {
	return r.active
}

func (r *Robot) Send(cmd []byte) {
	r.cmds <- cmd
}

func (r *Robot) handle() {
	defer func() {
		r.c.Close()
		r.active = false
	}()

	err := r.init()
	if err != nil {
		fmt.Printf("failed to initialize: %s\n", err)
		r.err = err
		return
	}

	for {
		cmd := <-r.cmds
		r.c.Write(cmd)

		tmp, err := r.read()
		if err != nil {
			r.err = err
			return
		}

		if tmp[0] != FLAG_ACK {
			msg := fmt.Sprintf("failed to acknowledge, receieved %d instead", tmp[0])
			fmt.Printf("%s\n", msg)
			r.err = fmt.Errorf(msg)
			return
		}
	}
}

func (r *Robot) init() error {
	// wait to be acknowledged
	// in the future, this will include versioning and hardware checks
	tmp, err := r.read()
	if err != nil {
		return err
	}
	if tmp[0] != FLAG_ACK {
		r.c.Write([]byte{
			FLAG_NIL,
		})
		return fmt.Errorf("failed to acknowledge, receieved %d instead", tmp[0])
	}

	r.c.Write([]byte{
		FLAG_ACK,
	})
	return nil
}

func (r *Robot) read() ([]byte, error) {
	tmp := make([]byte, 4096)

	_, err := r.c.Read(tmp)
	if err != nil {
		if err != io.EOF {
			fmt.Printf("read error: %s\n", err)
			return nil, err
		}
	}

	return tmp, nil
}
