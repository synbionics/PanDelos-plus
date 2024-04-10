# ParPanDelos

---

## Description
ParPanDelos a dictionary-based method for pan-genome content discovery. It's the C++ parallelized revisitation of [PanDelos](https://github.com/InfOmics/PanDelos).
It's structured into different python modules and C++ library, piped together by a bash script (`execute.sh`).

```bash
bash execute.sh <path_to_input_file.faa>
```

### Input format
ParPanDelos takes as imput a complete set of gene sequence stored in a `.faa` text file. This file must have a "2 line pattern" where:
- First line represents the idetification line, composed of 3 parts (genome identifier, thene identifier gene produc) separated by a tabulation character.
- Secondo line consists in the complete gene sequence in FASTA amino acid format reported in a single line.

Example of valid file composed by 5 gene grouped in 2 genomes 
```faa
NC_000913	NC_000913:NC_000913.3:b0001:1	thr operon leader peptide
MKRISTTITTTITITTGNGAG
NC_000913	NC_000913:NC_000913.3:b0005:1	DUF2502 domain-containing protein YaaX
MKKMQSIVLALSLVLVAPMAAQAAEITLVPSVKLQIGDRDNRGYYWDGGHWRDHGWWKQHYEWRGNRWHLHGPPPPPRHHKKAPHDHHGGHGPGKHHR
NC_000913	NC_000913:NC_000913.3:b0018:1	regulatory protein MokC
MLNTCRVPLTDRKVKEKRAMKQHKAMIVALIVICITAVVAALVTRKDLCEVHIRTGQTEVAVFTAYESE
NC_007946	NC_007946:NC_007946.1:UTI89_RS06140:1	DUF1382 family protein
MHKASPVELRTSIDLAHSLAQIGVRFVPIPAETDEEFHTLATSLSQKLEMMVAKAEADERDQV
NC_007946	NC_007946:NC_007946.1:UTI89_RS06145:1	DUF1317 domain-containing protein
MTHPHDNIRVGAITFVYSVTKRGWVFHGLSVIRNPLKAQRLAEEINNKRGAVCTKHLLLS
```

***IMPORTANT***
Make sure that gene identifiers are unique within the input file.

### Output format
The execution of ParPanDelos produces 2 output files named `[prefix_input_file].clus` and `[prefix_input_file].json` that reports the gene families discovered during the analysis.

### Tools
This repository has a folder (`tools`) that contains a set of tools concatenated by `execute.sh` script. Current tools are:
- `calculate_k.py` used to calculate kmers length given a `path_to_file.faa` file.
- `netclu_ng.py`, that takes (in this ordere) `path_to_file.faa` file and the respective `path_to_file.net` calculated by cpp software, and generate `.clus`.
- `netclu_ng_plot.py`, that takes (in this ordere) `path_to_file.faa` file and the respective `path_to_file.net` calculated by cpp software, and generate `.clus` and a `.pdf` file
- `clus2json.py` that takes a `path_to_file.clus` and generate a `.json` file.
- `genesDistributions.py` that takes `path_to_file.faa` and generate a bar plot with the distribution of genes for each genome (`file.png`).

---

## Installation

ParPanDelos can run on any operating system where Bash, Python3 (or higher), C++11 and [Boost](https://www.boost.org) are installed.

### Compilation

The current repository contains a bash script `compile.sh` with a selection of compilation flags.

```bash
bash compile.sh
```

If something goes wrong with the script, is possible compile as follow:
```bash
g++ -std=c++11 <list_of_compilation_flags> -o main
```
> For compilation flags check this [list of optimization options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html).
> It's recommended to use at least `-O1` flag

***IMPORTANT***
The ouput file (`-o`) must be named `main` for a correct work of the pipeline.


### Execution

The current repository contains a bash script `execute.sh` that takes an argument (the path to the input file), execute a sequence off tools (tools folder) and execute cpp software.

```bash
bash execute.sh <path_to_input_file.faa>
```

If you want a customized execution, you can run `main -h` to see all possible options

---

## Examples



---

## License




---

## Citation
