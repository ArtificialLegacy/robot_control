package proto

import (
	"encoding/json"
	"fmt"
	"io"
	"os"
)

type Proto struct {
	Keywords map[string][]string        `json:"Keywords,omitempty"`
	Masks    map[string]map[string]byte `json:"Masks,omitempty"`
}

type ProtoBuilder interface {
	BuildKeyword(name string, values []string) error
	BuildMask(name string, values map[string]byte) error
	BuildEnd() error
}

func Build(p ProtoBuilder) error {
	b, err := os.ReadFile("./pkg/proto/proto.json")
	if err != nil {
		return err
	}

	var data Proto
	err = json.Unmarshal(b, &data)
	if err != nil {
		return err
	}

	for k, v := range data.Keywords {
		err = p.BuildKeyword(k, v)
		if err != nil {
			return err
		}
	}

	for k, v := range data.Masks {
		err = p.BuildMask(k, v)
		if err != nil {
			return err
		}
	}

	err = p.BuildEnd()
	if err != nil {
		return err
	}

	return nil
}

type ProtoGo struct {
	W io.Writer
}

func NewProtoGo(w io.Writer) *ProtoGo {
	w.Write([]byte("package robot\n\n"))

	return &ProtoGo{W: w}
}

func (p *ProtoGo) BuildKeyword(name string, values []string) error {
	_, err := p.W.Write([]byte("const (\n"))
	if err != nil {
		return err
	}
	_, err = p.W.Write([]byte(fmt.Sprintf("\t%s_NIL byte = 0b0000_0000\n", name)))
	if err != nil {
		return err
	}
	for i, v := range values {
		_, err = p.W.Write([]byte(fmt.Sprintf("\t%s_%s byte = 0b%s\n", name, v, fmtByte(byte(i+1)))))
		if err != nil {
			return err
		}
	}
	_, err = p.W.Write([]byte(")\n\n"))
	if err != nil {
		return err
	}
	return nil
}

func (p *ProtoGo) BuildMask(name string, values map[string]byte) error {
	_, err := p.W.Write([]byte("const (\n"))
	if err != nil {
		return err
	}
	_, err = p.W.Write([]byte(fmt.Sprintf("\t%s_NIL byte = 0b0000_0000\n", name)))
	if err != nil {
		return err
	}
	for k, v := range values {
		_, err = p.W.Write([]byte(fmt.Sprintf("\t%s_%s byte = 0b%s\n", name, k, fmtByte(v))))
		if err != nil {
			return err
		}
	}
	_, err = p.W.Write([]byte(")\n\n"))
	if err != nil {
		return err
	}
	return nil
}

func (p *ProtoGo) BuildEnd() error {
	return nil
}

func fmtByte(b byte) string {
	return fmt.Sprintf("%04b_%04b", b>>4, b&0x0F)
}

type ProtoC struct {
	Wcpp io.Writer
	Wh   io.Writer
}

func NewProtoC(wcpp, wh io.Writer) *ProtoC {
	wh.Write([]byte("#ifndef Proto\n"))
	wh.Write([]byte("#define Proto\n\n"))

	wcpp.Write([]byte("#include \"proto.h\"\n\n"))

	return &ProtoC{Wcpp: wcpp, Wh: wh}
}

func (p *ProtoC) BuildKeyword(name string, values []string) error {
	_, err := p.Wh.Write([]byte(fmt.Sprintf("extern const unsigned char %s_NIL;\n", name)))
	if err != nil {
		return err
	}
	_, err = p.Wcpp.Write([]byte(fmt.Sprintf("const unsigned char %s_NIL = 0;\n", name)))
	if err != nil {
		return err
	}

	for i, v := range values {
		_, err = p.Wh.Write([]byte(fmt.Sprintf("extern const unsigned char %s_%s;\n", name, v)))
		if err != nil {
			return err
		}
		_, err = p.Wcpp.Write([]byte(fmt.Sprintf("const unsigned char %s_%s = %d;\n", name, v, i+1)))
		if err != nil {
			return err
		}
	}

	_, err = p.Wh.Write([]byte{'\n'})
	if err != nil {
		return err
	}
	_, err = p.Wcpp.Write([]byte{'\n'})
	if err != nil {
		return err
	}

	return nil
}

func (p *ProtoC) BuildMask(name string, values map[string]byte) error {
	_, err := p.Wh.Write([]byte(fmt.Sprintf("extern const unsigned char %s_NIL;\n", name)))
	if err != nil {
		return err
	}
	_, err = p.Wcpp.Write([]byte(fmt.Sprintf("const unsigned char %s_NIL = 0;\n", name)))
	if err != nil {
		return err
	}

	for k, v := range values {
		_, err = p.Wh.Write([]byte(fmt.Sprintf("extern const unsigned char %s_%s;\n", name, k)))
		if err != nil {
			return err
		}
		_, err = p.Wcpp.Write([]byte(fmt.Sprintf("const unsigned char %s_%s = %d;\n", name, k, v)))
		if err != nil {
			return err
		}
	}

	_, err = p.Wh.Write([]byte{'\n'})
	if err != nil {
		return err
	}
	_, err = p.Wcpp.Write([]byte{'\n'})
	if err != nil {
		return err
	}

	return nil
}

func (p *ProtoC) BuildEnd() error {
	_, err := p.Wh.Write([]byte("#endif\n"))
	if err != nil {
		return err
	}
	return nil
}
