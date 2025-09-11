import re
import sys
import subprocess
from pprint import pprint

import pycparser

def parse_defs(defs, lines):
    for i, line in enumerate(lines, 1):
        line = line.rstrip()
        if "asmdef_offset:" in line:
            asmdef = line.split('asmdef_offset:')[1]
            asmdef = asmdef.split(' ')[0]
            yield i, asmdef
        if "asmdef_size:" in line:
            asmdef = line.split('asmdef_size:')[1]
            asmdef = asmdef.split(' ')[0]
            yield i, asmdef

class StructFieldVisitor(pycparser.c_ast.NodeVisitor):
    def __init__(self):
        self.campos = []
    def visit_TypeDecl(self, node):
        nombre = node.declname
        typ = node.type
        if type(typ) == pycparser.c_ast.Struct:
            for decl in typ.decls:
                campo = decl.name
                linea = decl.coord.line
                self.campos.append((linea, (nombre, "offset", campo)))

            linea = node.coord.line
            self.campos.append((linea, (nombre, "size")))

def sacar_comments(text):
    text = subprocess.check_output("gcc -fpreprocessed -dD -E -".split(), input=text.encode())
    return text.decode()

def main(c_source, *args):
    defs = {}
    lines = None
    with open(c_source) as f:
       lines = f.readlines()

    asmdef_offsets = dict(parse_defs(defs, lines))
    # print(asmdef_offsets)

    parser = pycparser.c_parser.CParser()
    text = "".join(lines)
    text = sacar_comments(text)
    text = text.replace("__attribute__((__packed__)) ", "")
    text = re.sub(r"#include.*\n", "\n", text)
    text = text.replace("uint64_t", "int") #Estos tipos son placeholder, no importa cual usemos. Se reemplaza para que compile sin librerías.
    text = text.replace("uint32_t", "int")
    text = text.replace("uint16_t", "int")
    text = text.replace("uint8_t",  "int")
    text = text.replace("int64_t", "int")
    text = text.replace("int32_t", "int")
    text = text.replace("int16_t", "int")
    text = text.replace("int8_t",  "int")
    text = text.replace("bool",  "int")
    
    ast = parser.parse(text)

    # ast.show(showcoord=True)

    v = StructFieldVisitor()
    v.visit(ast)

    # pprint(v.campos)
    # exit(1)

    out = []
    out.append(f"""// generado por la cosa rara de la abi
#include "stddef.h"
#include "stdio.h"
#include "stdint.h"

#include "{c_source}"

int main() {{""")
    for l, nc in v.campos:
        asmname = asmdef_offsets.get(l)
        if not asmname:
            print(f"WARNING: skipping undecorated line {l}", file=sys.stderr)
            continue

        typ, kind, *rest = nc
        if kind == "offset":
            campo, *_ = rest
            out.append(f"""    printf("{asmname} EQU %lu\\n", offsetof({typ}, {campo})); // :{l}""")
        elif kind == "size":
            out.append(f"""    printf("{asmname} EQU %lu\\n", sizeof({typ}));""")
    out.append("""}""")
    print('\n'.join(out))

if __name__ == "__main__":
    main(*sys.argv[1:])

