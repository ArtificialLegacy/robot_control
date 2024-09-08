package robot

type command []byte

func Command() command {
	return command{FLAG_CMD, 0}
}

func (c command) Action(b ...byte) command {
	c = append(c, b...)
	c[1] += byte(len(b))
	return c
}
