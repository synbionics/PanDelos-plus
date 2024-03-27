import sys

def duplicate_genes(ifile, ofile):

    with open(ifile, 'r') as f:
        lines = f.readlines()
    
    with open(ofile, "w") as out:
        i = 0
        # print(len(lines))
        while i < len(lines):
            # print("i: " + lines[i])
            # print("i + 1: " + lines[i+1])
            out.write(lines[i])
            out.write(lines[i+1])
            newOut = lines[i].split("\t")
            newOut[1] += ":duplicated"
            line = ""
            
            for cell in range(0, len(newOut)):
                line += newOut[cell]
                if cell < len(newOut) -1:
                    line += "\t"
            out.write(line)
            out.write(lines[i+1])
            i += 2
        

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input_file.faa output_file.faa")
        sys.exit(1)
    duplicate_genes(sys.argv[1], sys.argv[2])
