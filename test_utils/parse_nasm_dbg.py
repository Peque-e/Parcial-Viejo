# nasm -f dbg ej1.asm -o /dev/stdout
import fileinput

def defs():
    skip_labels=[]
    with open("global_foos.txt", "w") as global_file:
        for line in fileinput.input():
            line = line.strip()
            parts = line.split(' ')
            if parts[0] == "deflabel":
                if parts[-2] == 'global':
                    global_file.write(f"global_foo_label {parts[1]}\n")
                elif parts[1] in skip_labels:
                    continue
                else:
                    value = parts[3].partition(':')[2]
                    print(parts[1], "EQU", int(value, 16))
            elif parts[0] == "herelabel" or parts[0] == "section_names":
                skip_labels.append(parts[1])

def regs():
    with open("global_foos.txt", "r") as global_file:
        for line in global_file:
            pass
        for line in fileinput.input():
            line = line.strip()
        

if __name__ == "__main__":
    defs()