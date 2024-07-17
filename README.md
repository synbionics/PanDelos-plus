# PanDelos-plus
PanDelos-plus: a dictionary-based method for pan-genome content discovery

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [](#lang-en)

<br><br>

## Description
PanDelos-plus a dictionary-based method for pan-genome content discovery. It's the C++ revisitation of [PanDelos](https://github.com/InfOmics/PanDelos).
It's structured into different python modules and C++ library, piped together by a bash script, `execute.sh`, that provides the access point to the PanDelos-plus pipeline.

Simple usage
```bash
bash execute.sh -i <path_to_input_file.faa>
```

Custom usage (`bash execute.sh -h`)
```bash
Usage: execute.sh [-i input_file] [-o output_file] [-t thread_num] [-m] [-d discard_value] [-g path to gbks][-h]
Options:
  -i: Input file path
  -o: Output file path
  -t: Number of threads
  -m: Enable a different mode
  -d: Discard value (0 <= d <= 1, default 0.5)
  -h: Display this help message
  -g: Path to gbk files folder
  -f: for fragmented genes\n
```

***IMPORTANT***
Note that discard value will filter compared genes basing on their length. For example:

```faa
NC_000913	NC_000913:NC_000913.3:b0001:1	thr operon leader peptide
MKRISTTITTTITITTGNGAG
NC_000913	NC_000913:NC_000913.3:b0018:1	regulatory protein MokC
MLNTCRVPLTDRKVKEKRAMKQHKAMIVALIVICITAVVAALVTRKDLCEVHIRTGQTEVAVFTAYESE
```
In case of this pair the gene identified by `NC_000913:NC_000913.3:b0001:1` has a length equal to $69$ and the other has length equal to $21$ so a discard greater than $0.2$ will skip this comparison



### Input format
PanDelos-plus takes as imput a complete set of gene sequence stored in a `.faa` text file, belonging to any of the studied genomes.

This file must have a "2 line pattern" where:
- First line represents the idetification line, composed of 3 parts (genome identifier, the gene identifier and gene product) separated by a **tabulation** character.
- Secondo line consists in the complete gene sequence in FASTA amino acid format reported in a single line.

***IMPORTANT***
No black lines are admitted in the entire file.


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

### Input format for fragmented genes


PanDelos-plus takes as imput a complete set of gene sequence stored in a `.faa` text file, belonging to any of the studied genomes.

This file must have a "2 line pattern" where:
- First line represents the idetification line, composed of 4 parts (genome identifier, the gene identifier, gene product and the number of infered characters) separated by a **tabulation** character.
- Secondo line consists in the complete gene sequence in FASTA amino acid format reported in a single line.

***IMPORTANT***
No black lines are admitted in the entire file.


Example of valid file composed by 5 gene grouped in 2 genomes for fragmented genes
```faa
NC_000913	NC_000913:NC_000913.3:b0001:1	thr operon leader peptide   5
MKRISTTITTTITITTGNGAG
NC_000913	NC_000913:NC_000913.3:b0005:1	DUF2502 domain-containing protein YaaX  20
MKKMQSIVLALSLVLVAPMAAQAAEITLVPSVKLQIGDRDNRGYYWDGGHWRDHGWWKQHYEWRGNRWHLHGPPPPPRHHKKAPHDHHGGHGPGKHHR
NC_000913	NC_000913:NC_000913.3:b0018:1	regulatory protein MokC   30
MLNTCRVPLTDRKVKEKRAMKQHKAMIVALIVICITAVVAALVTRKDLCEVHIRTGQTEVAVFTAYESE
NC_007946	NC_007946:NC_007946.1:UTI89_RS06140:1	DUF1382 family protein  10
MHKASPVELRTSIDLAHSLAQIGVRFVPIPAETDEEFHTLATSLSQKLEMMVAKAEADERDQV
NC_007946	NC_007946:NC_007946.1:UTI89_RS06145:1	DUF1317 domain-containing protein   5
MTHPHDNIRVGAITFVYSVTKRGWVFHGLSVIRNPLKAQRLAEEINNKRGAVCTKHLLLS
```


***IMPORTANT***
Make sure that gene identifiers are unique within the input file. A suggested format to build unique gene identifier is `genome_identifier:gene_identifier:unque_integer`.


### Output format
The execution of PanDelos-plus produces 2 output files named `[prefix_input_file].clus` and `[prefix_input_file].json` that reports the gene families discovered during the analysis. `[prefix_input_file].json` will be generated only if the `-g` flag is set to a correct gbk folder.


<br><br>

## Installation

### System requirements

PanDelos-plus can run on any operating system where Bash, Python3 (or higher), C++11 and [GCC](https://gcc.gnu.org) are installed.


```bash
sudo apt update
sudo apt-get install -y bash python3 python3-pip build-essential time
```

Python packages:
```bash
pip install biopython
pip install networkx
pip install matplotlib
```
If you are running latest version of Ubuntu, you may probably get this error:
```bash
× This environment is externally managed
╰─> To install Python packages system-wide, try apt install
    python3-xyz, where xyz is the package you are trying to
    install.
```
So you need to install the package using `apt` as follow
```bash
sudo apt install python3-biopython
sudo apt install python3-networkx
sudo apt install python3-matplotlib
```

### Compilation

The current repository contains a bash script `compile.sh` with a selection of compilation flags.

```bash
bash compile.sh
```

If something goes wrong with the script, is possible compile as follow:
```bash
g++ -std=c++11 -O1 -o main
```

### Execution

The current repository contains a bash script `execute.sh` that takes an argument (the path to the input file), execute a sequence off tools (tools folder) and execute cpp software.

```bash
bash execute.sh -i <path_to_input_file.faa>
```


<br><br>

## Running the examples

### Requirements

To run examples following packages are required:
```bash
sudo apt-get update
sudo apt-get -y install gzip curl bash python3 python3-pip build-essential time
```

```bash
pip install biopython
pip install networkx
pip install matplotlib
```

As in the installation section, if you are running latest version of Ubuntu, you may probably get this error:
```bash
× This environment is externally managed
╰─> To install Python packages system-wide, try apt install
    python3-xyz, where xyz is the package you are trying to
    install.
```
So you need to install the package using `apt` as follow
```bash
sudo apt install python3-biopython
sudo apt install python3-networkx
sudo apt install python3-matplotlib
```

### Run examples

To run all examples for PanDelos-plus' benchmarks

```bash
cd examples
python run_tests.py
```

The script will test a set of genomes retrieved from [NCBI](https://ncbi.nlm.nih.gov) databases. Every test corresponds to a `.list.txt` file in `examples/lists/`, that contains different number of lines, each formatted in 3 column, separated by a tabulation character `\t`, representing, in this order, Genome Assembly, GenBank and RefSeq.

For every test will be created a folder named as basename of corresponding `.list.txt` file with this structure:
```bash
basename
├── banks
│  └── # all .gbk files used in the test
├── basename.faa # .faa file extracted from all gbk files contained in 'banks' folder
├── basename.png # with a bar plot with gene number for each genome
├── nets
│  └── # all .net file
├── plots
│  └── # list of .png files with plots
└── tmp.txt # a temporal file
```

---


## For developers

### Tools
This repository has a folder (`tools`) that contains a set of tools concatenated by `execute.sh` script. Current tools are:
- `calculate_k.py` used to calculate kmers length given a `path_to_file.faa` file.
- `netclu_ng.py`, that takes (in this order) `path_to_file.faa` file and the respective `path_to_file.net` calculated by cpp software, and generate `.clus`.
- `netclu_ng_plot.py`, that takes (in this order) `path_to_file.faa` file and the respective `path_to_file.net` calculated by cpp software, and generate `.clus` and a `.pdf` file
- `clus2json.py` that takes a `path_to_file.clus` and generate a `.json` file.
- `genesDistributions.py` that takes `path_to_file.faa` and generate a bar plot with the distribution of genes for each genome (`file.png`).

### Installation

If the `compile.sh` script not works properly is possible compile as follow:
```bash
g++ -std=c++11 <list_of_compilation_flags> -o main
```
> For compilation flags check this [list of optimization options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html).
> It's recommended to use at least `-O1` flag

***IMPORTANT***
The output file (`-o`) must be named `main` for a correct work of the pipeline.

### Execution

If you want a customized execution, you can run `./main -h` to see all possible options

```bash
+-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+
|P| |a| |r| |P| |a| |n| |D| |e| |l| |o| |s|
+-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+
Usage:
-i to select the input file (path_to_file/file.faa)
-o to specify the file for output(path_to_file/file_name.extension)
-k to indicate the size of kmers
-t to indicate the number of threads
-m to activate specific mode with lower RAM cost (0 default)
-d to select a discard value (0 <= d <= 1) for similarity computation (0.5 default, a grater value implies a more aggressive discard)
-f for fragmented genes
```
<br><br>

## License

PanDelos-plus is distributed under the MIT license. This means that it is free for both academic and commercial use. Note however that some third party components in PanDelos-plus require that you reference certain works in scientific publications. You are free to link or use PanDelos-plus inside the source code of your own program. If do so, please reference (cite) PanDelos-plus and this website. Bug fixes and collaboration for improvements are appreciated. 

<br><br>

## Citation

Original PanDelos software:

     Bonnici, V., Giugno, R., Manca, V.
     PanDelos: a dictionary-based method for pan-genome content discovery
     BMC bioinformatics 19.15 (2018): 437.


If you have used any of the PanDelos-plus project software, please cite the the following paper:

<!-- TODO add citation -->