#!/usr/bin/python3

import sys
import math

def main():
    """
    Calculate the base logarithm of the total number of unique characters in the odd lines of a file.
    The filename to analyze should be provided as a command-line argument (furst argument).
    Execution:
    python calculate_k.py path_to_file

    
    @param None
    @return None
    """

    # argv[0] corrisponde al file python
    file = sys.argv[1]

    # print(file)
    alphabet = set()
    totLength = 0


    i = 0
    for line in open(file, "r"):
        if i % 2 != 0:
            totLength += len(line.strip())
            for s in line.strip():
                alphabet.add(s)
        i+=1
    # Calcola il logaritmo base del numero totale di caratteri nel file
    result = math.floor(math.log(totLength, len(alphabet)))
    print(result)

if __name__ == "__main__":
    main()