// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package golang

import (
	"fmt"
	"io"
	"strconv"
	"strings"
	"text/template"

	fidlcommon "fidl/compiler/backend/common"
	fidlir "fidl/compiler/backend/types"
	gidlir "gidl/ir"
	gidlmixer "gidl/mixer"
)

var tmpl = template.Must(template.New("tmpls").Parse(`
package fidl_test

import (
	"testing"
	"reflect"

	"syscall/zx/fidl/conformance"
	"syscall/zx/fidl"
)

func TestAllEncodeSuccessCases(t *testing.T) {
{{ range .EncodeSuccessCases }}
{
{{ .ValueBuild }}
encodeSuccessCase{
	name: {{ .Name }},
	context: {{ .Context }},
	input: &{{ .Value }},
	bytes: {{ .Bytes }},
}.check(t)
}
{{ end }}
}

func TestAllDecodeSuccessCases(t *testing.T) {
	{{ range .DecodeSuccessCases }}
	{
	{{ .ValueBuild }}
	decodeSuccessCase{
		name: {{ .Name }},
		context: {{ .Context }},
		input: &{{ .Value }},
		bytes: {{ .Bytes }},
	}.check(t)
	}
	{{ end }}
	}

func TestAllEncodeFailureCases(t *testing.T) {
{{ range .EncodeFailureCases }}
{
{{ .ValueBuild }}
encodeFailureCase{
	name: {{ .Name }},
	context: {{ .Context }},
	input: &{{ .Value }},
	code: {{ .ErrorCode }},
}.check(t)
}
{{ end }}
}

func TestAllDecodeFailureCases(t *testing.T) {
{{ range .DecodeFailureCases }}
{
decodeFailureCase{
	name: {{ .Name }},
	context: {{ .Context }},
	valTyp: reflect.TypeOf((*{{ .ValueType }})(nil)),
	bytes: {{ .Bytes }},
	code: {{ .ErrorCode }},
}.check(t)
}
{{ end }}
}
`))

type tmplInput struct {
	EncodeSuccessCases []encodeSuccessCase
	DecodeSuccessCases []decodeSuccessCase
	EncodeFailureCases []encodeFailureCase
	DecodeFailureCases []decodeFailureCase
}

type encodeSuccessCase struct {
	Name, Context, ValueBuild, Value, Bytes string
}

type decodeSuccessCase struct {
	Name, Context, ValueBuild, Value, Bytes string
}

type encodeFailureCase struct {
	Name, Context, ValueBuild, Value, ErrorCode string
}

type decodeFailureCase struct {
	Name, Context, ValueType, Bytes, ErrorCode string
}

// Generate generates Go tests.
func Generate(wr io.Writer, gidl gidlir.All, fidl fidlir.Root) error {
	encodeSuccessCases, err := encodeSuccessCases(gidl.EncodeSuccess, fidl)
	if err != nil {
		return err
	}
	decodeSuccessCases, err := decodeSuccessCases(gidl.DecodeSuccess, fidl)
	if err != nil {
		return err
	}
	encodeFailureCases, err := encodeFailureCases(gidl.EncodeFailure, fidl)
	if err != nil {
		return err
	}
	decodeFailureCases, err := decodeFailureCases(gidl.DecodeFailure, fidl)
	if err != nil {
		return err
	}
	input := tmplInput{
		EncodeSuccessCases: encodeSuccessCases,
		DecodeSuccessCases: decodeSuccessCases,
		EncodeFailureCases: encodeFailureCases,
		DecodeFailureCases: decodeFailureCases,
	}
	return tmpl.Execute(wr, input)
}

func marshalerContext(wireFormat gidlir.WireFormat) string {
	switch wireFormat {
	case gidlir.OldWireFormat:
		return `fidl.MarshalerContext{
			DecodeUnionsFromXUnionBytes: false,
			EncodeUnionsAsXUnionBytes:   false,
		}`
	case gidlir.V1WireFormat:
		return `fidl.MarshalerContext{
			DecodeUnionsFromXUnionBytes: true,
			EncodeUnionsAsXUnionBytes:   true,
		}`
	default:
		panic(fmt.Sprintf("unexpected wire format %v", wireFormat))
	}
}

func encodeSuccessCases(gidlEncodeSuccesses []gidlir.EncodeSuccess, fidl fidlir.Root) ([]encodeSuccessCase, error) {
	var encodeSuccessCases []encodeSuccessCase
	for _, encodeSuccess := range gidlEncodeSuccesses {
		decl, err := gidlmixer.ExtractDeclaration(encodeSuccess.Value, fidl)
		if err != nil {
			return nil, fmt.Errorf("encodeSuccess %s: %s", encodeSuccess.Name, err)
		}

		if gidlir.ContainsUnknownField(encodeSuccess.Value) {
			continue
		}
		var valueBuilder goValueBuilder
		gidlmixer.Visit(&valueBuilder, encodeSuccess.Value, decl)

		encodeSuccessCases = append(encodeSuccessCases, encodeSuccessCase{
			Name:       strconv.Quote(encodeSuccess.Name),
			Context:    marshalerContext(encodeSuccess.WireFormat),
			ValueBuild: valueBuilder.String(),
			Value:      valueBuilder.lastVar,
			Bytes:      bytesBuilder(encodeSuccess.Bytes),
		})
	}
	return encodeSuccessCases, nil
}

func decodeSuccessCases(gidlDecodeSuccesses []gidlir.DecodeSuccess, fidl fidlir.Root) ([]decodeSuccessCase, error) {
	var decodeSuccessCases []decodeSuccessCase
	for _, decodeSuccess := range gidlDecodeSuccesses {
		decl, err := gidlmixer.ExtractDeclaration(decodeSuccess.Value, fidl)
		if err != nil {
			return nil, fmt.Errorf("decodeSuccess %s: %s", decodeSuccess.Name, err)
		}

		if gidlir.ContainsUnknownField(decodeSuccess.Value) {
			continue
		}
		var valueBuilder goValueBuilder
		gidlmixer.Visit(&valueBuilder, decodeSuccess.Value, decl)

		decodeSuccessCases = append(decodeSuccessCases, decodeSuccessCase{
			Name:       strconv.Quote(decodeSuccess.Name),
			Context:    marshalerContext(decodeSuccess.WireFormat),
			ValueBuild: valueBuilder.String(),
			Value:      valueBuilder.lastVar,
			Bytes:      bytesBuilder(decodeSuccess.Bytes),
		})
	}
	return decodeSuccessCases, nil
}

func encodeFailureCases(gidlEncodeFailures []gidlir.EncodeFailure, fidl fidlir.Root) ([]encodeFailureCase, error) {
	var encodeFailureCases []encodeFailureCase
	for _, encodeFailure := range gidlEncodeFailures {
		decl, err := gidlmixer.ExtractDeclarationUnsafe(encodeFailure.Value, fidl)
		if err != nil {
			return nil, fmt.Errorf("encodeFailure %s: %s", encodeFailure.Name, err)
		}

		if gidlir.ContainsUnknownField(encodeFailure.Value) {
			continue
		}
		var valueBuilder goValueBuilder
		gidlmixer.Visit(&valueBuilder, encodeFailure.Value, decl)

		code, err := goErrorCode(encodeFailure.Err)
		if err != nil {
			return nil, err
		}

		encodeFailureCases = append(encodeFailureCases, encodeFailureCase{
			Name:       strconv.Quote(encodeFailure.Name),
			Context:    marshalerContext(encodeFailure.WireFormat),
			ValueBuild: valueBuilder.String(),
			Value:      valueBuilder.lastVar,
			ErrorCode:  code,
		})
	}
	return encodeFailureCases, nil
}

func decodeFailureCases(gidlDecodeFailures []gidlir.DecodeFailure, fidl fidlir.Root) ([]decodeFailureCase, error) {
	var decodeFailureCases []decodeFailureCase
	for _, decodeFailure := range gidlDecodeFailures {
		code, err := goErrorCode(decodeFailure.Err)
		if err != nil {
			return nil, err
		}

		decodeFailureCases = append(decodeFailureCases, decodeFailureCase{
			Name:      strconv.Quote(decodeFailure.Name),
			Context:   marshalerContext(decodeFailure.WireFormat),
			ValueType: goType(decodeFailure.Type),
			Bytes:     bytesBuilder(decodeFailure.Bytes),
			ErrorCode: code,
		})
	}
	return decodeFailureCases, nil
}

func goType(irType string) string {
	return "conformance." + irType
}

func bytesBuilder(bytes []byte) string {
	var builder strings.Builder
	builder.WriteString("[]byte{\n")
	for i, b := range bytes {
		builder.WriteString(fmt.Sprintf("0x%02x", b))
		builder.WriteString(",")
		if i%8 == 7 {
			builder.WriteString("\n")
		}
	}
	builder.WriteString("}")
	return builder.String()
}

type goValueBuilder struct {
	strings.Builder
	varidx int

	lastVar string
}

func (b *goValueBuilder) newVar() string {
	b.varidx++
	return fmt.Sprintf("v%d", b.varidx)
}

func (b *goValueBuilder) OnBool(value bool) {
	newVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf(
		"%s := %t\n", newVar, value))
	b.lastVar = newVar
}

func (b *goValueBuilder) OnInt64(value int64, typ fidlir.PrimitiveSubtype) {
	newVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf(
		"var %s %s = %d\n", newVar, typ, value))
	b.lastVar = newVar
}

func (b *goValueBuilder) OnUint64(value uint64, typ fidlir.PrimitiveSubtype) {
	newVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf(
		"var %s %s = %d\n", newVar, typ, value))
	b.lastVar = newVar
}

func (b *goValueBuilder) OnFloat64(value float64, typ fidlir.PrimitiveSubtype) {
	newVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf(
		"var %s %s = %f\n", newVar, typ, value))
	b.lastVar = newVar
}

func (b *goValueBuilder) OnString(value string, decl *gidlmixer.StringDecl) {
	newVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf(
		"%s := %s\n", newVar, strconv.Quote(value)))
	if decl.IsNullable() {
		pointee := newVar
		newVar = b.newVar()
		b.Builder.WriteString(fmt.Sprintf("%s := &%s\n", newVar, pointee))
	}
	b.lastVar = newVar
}

func (b *goValueBuilder) OnStruct(value gidlir.Object, decl *gidlmixer.StructDecl) {
	b.onObject(value, decl)
}

func (b *goValueBuilder) onObject(value gidlir.Object, decl gidlmixer.KeyedDeclaration) {
	containerVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf("%s := %s{}\n", containerVar, typeLiteral(decl)))
	for _, field := range value.Fields {
		if field.Key.Name == "" {
			panic("unknown field not supported")
		}
		fieldDecl, _ := decl.ForKey(field.Key)
		gidlmixer.Visit(b, field.Value, fieldDecl)
		fieldVar := b.lastVar

		switch decl.(type) {
		case *gidlmixer.StructDecl:
			b.Builder.WriteString(fmt.Sprintf(
				"%s.%s = %s\n", containerVar, fidlcommon.ToUpperCamelCase(field.Key.Name), fieldVar))
		default:
			b.Builder.WriteString(fmt.Sprintf(
				"%s.Set%s(%s)\n", containerVar, fidlcommon.ToUpperCamelCase(field.Key.Name), fieldVar))
		}
	}
	b.lastVar = containerVar
}

func (b *goValueBuilder) OnTable(value gidlir.Object, decl *gidlmixer.TableDecl) {
	b.onObject(value, decl)
}

func (b *goValueBuilder) OnXUnion(value gidlir.Object, decl *gidlmixer.XUnionDecl) {
	b.onObject(value, decl)
}

func (b *goValueBuilder) OnUnion(value gidlir.Object, decl *gidlmixer.UnionDecl) {
	b.onObject(value, decl)
}

func (b *goValueBuilder) onList(value []interface{}, decl gidlmixer.ListDeclaration) {
	var argStr string
	elemDecl, _ := decl.Elem()
	for _, item := range value {
		gidlmixer.Visit(b, item, elemDecl)
		argStr += b.lastVar + ", "
	}
	sliceVar := b.newVar()
	b.Builder.WriteString(fmt.Sprintf("%s := %s{%s}\n", sliceVar, typeLiteral(decl), argStr))
	b.lastVar = sliceVar
}

func (b *goValueBuilder) OnArray(value []interface{}, decl *gidlmixer.ArrayDecl) {
	b.onList(value, decl)
}

func (b *goValueBuilder) OnVector(value []interface{}, decl *gidlmixer.VectorDecl) {
	b.onList(value, decl)
}

func (b *goValueBuilder) OnNull(decl gidlmixer.Declaration) {
	newVar := b.newVar()
	b.WriteString(fmt.Sprintf("var %s %s = nil\n", newVar, typeName(decl)))
	b.lastVar = newVar
}

func typeName(decl gidlmixer.Declaration) string {
	return typeNameHelper(decl, "*")
}

func typeLiteral(decl gidlmixer.Declaration) string {
	return typeNameHelper(decl, "&")
}

func typeNameHelper(decl gidlmixer.Declaration, pointerPrefix string) string {
	if !decl.IsNullable() {
		pointerPrefix = ""
	}

	switch decl := decl.(type) {
	case *gidlmixer.BoolDecl:
		return "bool"
	case *gidlmixer.NumberDecl:
		return string(decl.Typ)
	case *gidlmixer.FloatDecl:
		return string(decl.Typ)
	case *gidlmixer.StringDecl:
		return pointerPrefix + "string"
	case *gidlmixer.StructDecl:
		return pointerPrefix + identifierName(decl.Name)
	case *gidlmixer.TableDecl:
		return pointerPrefix + identifierName(decl.Name)
	case *gidlmixer.UnionDecl:
		return pointerPrefix + identifierName(decl.Name)
	case *gidlmixer.XUnionDecl:
		return pointerPrefix + identifierName(decl.Name)
	case *gidlmixer.ArrayDecl:
		return fmt.Sprintf("[%d]%s", decl.Size(), elemName(decl))
	case *gidlmixer.VectorDecl:
		return fmt.Sprintf("%s[]%s", pointerPrefix, elemName(decl))
	default:
		panic("unhandled case")
	}
}

func identifierName(eci fidlir.EncodedCompoundIdentifier) string {
	parts := strings.Split(string(eci), "/")
	return strings.Join(parts, ".")
}

func elemName(parent gidlmixer.ListDeclaration) string {
	if elemDecl, ok := parent.Elem(); ok {
		return typeName(elemDecl)
	}
	panic("missing element")
}

// Go errors are defined in third_party/go/src/syscall/zx/fidl/errors.go
var goErrorCodeNames = map[gidlir.ErrorCode]string{
	gidlir.StringTooLong:               "ErrStringTooLong",
	gidlir.NullEmptyStringWithNullBody: "ErrUnexpectedNullRef",
	gidlir.StrictXUnionFieldNotSet:     "ErrInvalidXUnionTag",
	gidlir.StrictXUnionUnknownField:    "ErrInvalidXUnionTag",
}

func goErrorCode(code gidlir.ErrorCode) (string, error) {
	if str, ok := goErrorCodeNames[code]; ok {
		return fmt.Sprintf("fidl.%s", str), nil
	}
	return "", fmt.Errorf("no go error string defined for error code %s", code)
}
