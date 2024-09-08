package main

import (
	"os"

	"github.com/ArtificialLegacy/robot_control/pkg/proto"
)

func main() {
	gf, err := os.OpenFile("./pkg/robot/proto_gen.go", os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0666)
	if err != nil {
		panic(err)
	}
	defer gf.Close()

	pg := proto.NewProtoGo(gf)
	err = proto.Build(pg)
	if err != nil {
		panic(err)
	}

	cpf, err := os.OpenFile("./esp32_driver/proto.cpp", os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0666)
	if err != nil {
		panic(err)
	}
	defer cpf.Close()

	chf, err := os.OpenFile("./esp32_driver/proto.h", os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0666)
	if err != nil {
		panic(err)
	}
	defer chf.Close()

	pc := proto.NewProtoC(cpf, chf)
	err = proto.Build(pc)
	if err != nil {
		panic(err)
	}
}
