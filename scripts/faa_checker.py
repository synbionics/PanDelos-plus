import sys


if len(sys.argv) != 3:
    print("Usage: python script.py <input_file> <k>")
    sys.exit(1)

ifile = sys.argv[1]
k = int(sys.argv[2])

wrongFile = False

def check_sequences():
    seq = False
    last = ""
    with open(ifile, "r") as file:
        for line in file:
            if seq:
                if len(line.strip())< k:
                    wrongFile = True
                    print("Sequence too short")
                    print(last)
                    print(line)
                seq = False
            else:
                seq = True
                last = line




check_sequences()

if wrongFile:
    print("File is wrong")
    sys.exit(1)

