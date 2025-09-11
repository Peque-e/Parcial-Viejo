# ./test | venv/bin/python3 diff.py
import fileinput

class bcolors:
    ANSI_COLOR_RED      = "\x1b[31m"
    ANSI_COLOR_GREEN    = "\x1b[32m"
    ANSI_COLOR_YELLOW   = "\x1b[33m"
    ANSI_COLOR_BLUE     = "\x1b[34m"
    ANSI_COLOR_MAGENTA  = "\x1b[35m"
    ANSI_COLOR_CYAN     = "\x1b[36m"
    ANSI_COLOR_RESET    = "\x1b[0m"


def diff():
    c_defs = {}
    for c_line in fileinput.input():
        c_xx = c_line.split(" ")
        c_defs[c_xx[0]] = c_xx[2]

    asm_defs = {}
    with open("asm_defs.txt", "r") as asm_input:
        for asm_line in asm_input:
            asm_xx = asm_line.split(" ")
            asm_defs[asm_xx[0]] = asm_xx[2]

    errors = set()
    
    for c_decl, c_val in c_defs.items():
        if c_decl not in asm_defs:
            print(f"{bcolors.ANSI_COLOR_RED}error: [ABI_enforcer] falta definir {c_decl}{bcolors.ANSI_COLOR_RESET}")
            errors.add(c_decl)
            continue
        if c_val != asm_defs[c_decl]:
            errors.add(c_decl)
        
    if errors:
        print(f"{bcolors.ANSI_COLOR_RED}error: [ABI_enforcer] Hay offsets o tamaños erróneos{bcolors.ANSI_COLOR_RESET}")
    else:
        print(f"{bcolors.ANSI_COLOR_GREEN}ok: [ABI_enforcer] Todos los offsets son correctos{bcolors.ANSI_COLOR_RESET}")


if __name__ == "__main__":
    diff()
