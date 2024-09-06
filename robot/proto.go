package robot

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

type command []byte

func Command() command {
	return command{FLAG_CMD, 0}
}

func (c command) Action(b ...byte) command {
	c = append(c, b...)
	c[1] += byte(len(b))
	return c
}
