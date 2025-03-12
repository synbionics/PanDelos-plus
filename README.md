# PanDelos-plus

PanDelos-plus: a parallel algorithm for computing sequence homology in pangenomic analysis.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [](#lang-en)

<br>

## Contents

-   [PanDelos-plus](#pandelos-plus)
    -   [Contents](#contents)
    -   [Introduction](#introduction)
    -   [Use on your local machine](#use-on-your-local-machine)
        -   [Installation](#installation)
        -   [Usage](#usage)
    -   [Use with docker](#use-with-docker)
        -   [Installation](#installation-1)
        -   [Usage](#usage-1)
    -   [Run with a custom file](#run-with-a-custom-file)
        -   [Local execution with custom file](#local-execution-with-custom-file)
        -   [Docker execution with custom file](#docker-execution-with-custom-file)
    -   [Advanced usage](#advanced-usage)
        -   [Using gbff files as input](#using-gbff-files-as-input)
        -   [Custom execution](#custom-execution)
        -   [Discard value](#discard-value)
        -   [Fragmented genes](#fragmented-genes)
        -   [Similarity parameter](#similarity-parameter)
    -   [License](#license)
    -   [Citation](#citation)

<br>

## Introduction

PanDelos-plus implements a dictionary-based method for pan-genome content discovery. This updated version is a re-engineered and parallelized C++ implementation of the original PanDelos. It integrates several Python modules with a C++ library, coordinated via the `execute.sh` Bash script, which facilitates streamlined access to the complete PanDelos-plus pipeline.

<br>

## Use on your local machine

**Important**

Make sure to have git installed on your machine.

If you don't have git installed, you can install it with the following command on Ubuntu machine:

```bash
sudo apt-get install git
```

> If you don't have an ubuntu machine you can follow the instruction on the [Git website](https://git-scm.com/downloads).

Tool installation:

```bash
git clone https://github.com/vbonnici/PanDelos-plus.git
```

### Installation

For the local installation on a ubuntu machine, you can run the following commands:

Required dependencies:

```bash
sudo apt update
sudo apt-get install -y bash python3 python3-pip build-essential time git
```

Python packages:

```bash
pip3 install biopython
pip3 install networkx
pip3 install matplotlib
```

If you are running the one of the latest version of Ubuntu, you may probably get this error:

```bash
× This environment is externally managed
╰─> To install Python packages system-wide, try apt install
    python3-xyz, where xyz is the package you are trying to
    install.
```

So you need to install the package using `apt` as follows

```bash
sudo apt install python3-biopython
sudo apt install python3-networkx
sudo apt install python3-matplotlib
```

Tool compilation:

```bash
cd PanDelos-plus
bash compile.sh
```

### Usage

```bash
bash execute.sh -i files/faa/mycoplasma5.pdi -o mycoplasma5
```

This script will run the PanDelos-plus pipeline on the input file `files/faa/mycoplasma5.pdi` and save the output in the `mycoplasma5.clus` file.
The output file will contain the gene families computed by the pipeline.

## Use with docker

sure to have git installed on your machine.

If you don't have git installed, you can install it with the following command on Ubuntu machine:

```bash
sudo apt-get install git
```

> If you don't have an ubuntu machine you can follow the instruction on the [Git website](https://git-scm.com/downloads).

Tool installation:

```bash
git clone https://github.com/vbonnici/PanDelos-plus.git
```

### Installation

Make sure that you have docker installed on your machine.

If you don't have docker installed, you can install it following the instructions on the [Docker](https://docs.docker.com/engine/install/ubuntu/) website.

Move inside the `PanDelos-plus` folder:

```bash
cd PanDelos-plus
```

If you are running a linux machine, you probably need to change the permissions of the following folders:

```bash
chmod -R 777 input
chmod -R 777 output
```

Build the container:

```bash
docker compose build --no-cache
```

> Note that `docker compose` command may raise some errors so try also with `docker-compose`

### Usage

**Important**

Check that the input and output folders are writable by the user running the docker container.

Copy file inside the input folder:

```bash
cp files/faa/mycoplasma5.pdi input/mycoplasma5.pdi
```

> If you are on windows you probably have to use `cp .\files\faa\mycoplasma5.pdi input\mycoplasma5.pdi`

Run the pipeline:

```bash
docker compose run --rm pandelosplus bash execute.sh -i input/mycoplasma5.pdi -o output/mycoplasma5
```

> Note that `docker compose` command may raise some errors so try also with `docker-compose`

This script will run the PanDelos-plus pipeline inside the docker on the input file `input/mycoplasma5.pdi` and save the output in the `output/mycoplasma5.clus` file.

The output file will contain the gene families computed by the pipeline.

## Run with a custom file

PanDelos-plus takes as input a complete set of gene sequences stored in a `.pdi` text file belonging to any of the studied genomes.

This file must have a "2 line pattern" where:

-   The first line represents the identification line, composed of 3 parts (genome identifier, the gene identifier and the gene product) separated by a **tabulation** character.
-   The second line consists of the complete gene sequence in FASTA amino acid format reported in a single line.

**_IMPORTANT_**
No blank lines are admitted in the entire file.

Example of valid file composed of 5 genes grouped in 2 genomes

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

**_IMPORTANT_**
Make sure that gene identifiers are unique within the input file. A suggested format to build unique gene identifier is `genome_identifier:gene_identifier:unque_integer`.

### Local execution with custom file

After you have prepared your input file (supposing it is named as `input.pdi`), you can run the pipeline as follows:

```bash
bash execute.sh -i input.pdi -o output
```

The output file will contain the gene families computed by the pipeline.

### Docker execution with custom file

After you have prepared your input file (supposing it is in the `input` folder and is named `custom.pdi`), you can run the pipeline as follows:

Run the pipeline:

```bash
docker compose run --rm pandelosplus bash execute.sh -i input/custom.pdi -o output/custom
```

This script will run the PanDelos-plus pipeline inside the docker on the input file `input/custom.pdi` and save the output in the `output/custom.clus` file.
The output file will contain the gene families computed by the pipeline.

## Advanced usage

**Important**
If you installed pandeslos-plus with docker you must enter inside the container to execute the following steps.

```bash
docker compose run --rm pandelosplus bash
```

### Using gbff files as input

You can generate an input file from a set of `.gbff` files following these steps:

1.  Download the genbank files of your interest.
2.  Create a folder and copy all `.gbff` files in it.
3.  Create a folder to store the converted files.
    ```bash
    mkdir gbk_files
    ```
4.  Run the following command to convert files:
    ```bash
    python3 scripts/gbff2gbk.py <path_to_input_folder> <path_to_output_folder>
    ```
5.  Run the following command to check converted files:
    ```bash
    python3 scripts/gbk_checker.py <path_to_input_folder>
    ```
6.  Run the following command to generate the input file:
    ```bash
    python3 scripts/gbk2ig.py <path_to_input_folder> <path_to_output_file>.pdi
    ```
7.  Now you can use the input file as usual, or you can add `-g <path_to_folder_with_gbk_files>` flag to the execution command to generate a json file in addition to the output file.

Example:

> Extract files from `fils/gbff.zip`.

In this example it will be used files contained in `files/gbff/` folder which contains 2 genomes downloaded from NIH Database:

-   GCF_016028495.1, for Salmonella enterica (ASM1602849v1).
-   GCF_000006945.2, for Salmonella enterica (ASM694v2).

> So steps 1 and 2 are already done.

Step 3:

```bash
mkdir gbk_files
```

Step 4:

```b
python3 scripts/gbff2gbk.py files/gbff/ gbk_files/
```

You will get this output `['GCA_000006945.2.gbff', 'GCA_016028495.1.gbff']`, and 2 files in the `gbk_files` folder.

Step 5:

```bash
python3 scripts/gbk_checker.py gbk_files/
```

You will get this output:

```bash
Reading gbk files from: gbk_files/
Files found: ['GCA_016028495.1.gbk', 'GCA_000006945.2.gbk']
Processing file: GCA_016028495.1.gbk
Processing genome: GCA_016028495.1
	Genome ID: GCA_016028495.1	Sequence ID: CP065718.1
	Genome ID: GCA_016028495.1	Sequence ID: CP065719.1
Processing file: GCA_000006945.2.gbk
Processing genome: GCA_000006945.2
	Genome ID: GCA_000006945.2	Sequence ID: AE006468.2
	Genome ID: GCA_000006945.2	Sequence ID: AE006471.2
All files processed successfully
```

Step 6:

```bash
python3 scripts/gbk2ig.py gbk_files/ custom.pdi
```

You will get this output:

```bash
reading gbk files from gbk_files/
['GCA_016028495.1.gbk', 'GCA_000006945.2.gbk']
GCA_016028495.1.gbk
GCA_016028495.1
	GCA_016028495.1	CP065718.1
	GCA_016028495.1	CP065719.1
GCA_000006945.2.gbk
GCA_000006945.2
	GCA_000006945.2	AE006468.2
	GCA_000006945.2	AE006471.2
writing to custom.pdi
```

Step 7:

```bash
bash execute.sh -i custom.pdi -o custom -g gbk_files/
```

You will get this output:

```bash
ven 7 mar 2025, 16:09:59, CET
custom.pdi
k = 4
Checking input file (.pdi)
Executing main
Computing clusters
Converting clusters to json
ven 7 mar 2025, 16:10:07, CET
```

Now you can check de output file `custom.clus` and the json file `custom.json`.

### Custom execution

You can customize the execution of the pipeline by using the following flags:
| Flag | Description |
|------|-------------|
| -i | Input file path |
| -o | Output file path |
| -t | Number of threads |
| -m | Enable a slower mode which requires less memory (default: False) |
| -d | Discard value (0 <= d <= 1, default 0.5), check the section below |
| -g | Path to gbk folder |
| -f | For fragmented genes, check the section below |
| -p | For a stronger threshold (similarity parameter), check the section below |
| -h | Display this help message |

You can also check the help message by running:

```bash
bash execute.sh -h
```

You will get this output:

```bash
Usage: execute.sh [-i input_file] [-o output_file] [-t thread_num] [-m] [-d discard_value] [-g path to gbks][-h]
Options:
  -i: Input file path
  -o: Output file path
  -t: Number of threads
  -m: Enable a different mode
  -d: Discard value (0 <= d <= 1, default 0.5)
  -g: Path to gbk folder
  -f: For fragmented genes
  -p: For a stronger threshold (similarity parameter)
  -h: Display this help message
```

### Discard value

The discard value (`-d`) is a threshold that is used to decide whether to compare two genes.
This type of decision is made based on the length of the genes.

```faa
NC_000913	NC_000913:NC_000913.3:b0001:1	thr operon leader peptide
MKRISTTITTTITITTGNGAG
NC_000913	NC_000913:NC_000913.3:b0018:1	regulatory protein MokC
MLNTCRVPLTDRKVKEKRAMKQHKAMIVALIVICITAVVAALVTRKDLCEVHIRTGQTEVAVFTAYESE
```

In the case of this pair the gene identified by `NC_000913:NC_000913.3:b0001:1` has a length equal to $69$ and the other has length equal to $21$ so a discard greater than $0.2$ will skip this comparison

### Fragmented genes

PanDelos-plus can handle fragmented genes. To use this feature the input file must be formatted as described in the following lines.

This file must have a "2 line pattern" where:

-   The first line represents the identification line, composed of 4 parts (genome identifier, the gene identifier, the gene product and the number of inferred characters) separated by a **tabulation** character.
-   The second line consists of the complete gene sequence in FASTA amino acid format reported in a single line.

**_IMPORTANT_**
No blank lines are admitted in the entire file.

Example of valid file composed of 5 genes grouped in 2 genomes for fragmented genes

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

**_IMPORTANT_**
Make sure that gene identifiers are unique within the input file. A suggested format to build unique gene identifier is `genome_identifier:gene_identifier:unque_integer`.

### Similarity parameter

The similarity parameter (`-p`) is a threshold that is used to decide whether keep or discard a value obtained from the comparison of two genes.
This type of decision is made on the similarity of the two genes, if the similarity is greater than an specific value, the computed value is kept.
Otherwise, it is discarded.

Using the `-p` flag you will use an higher threshold to make the decision, so more values will not be considered.

## License

PanDelos-plus is distributed under the MIT license. This means that it is free for both academic and commercial use. Note, however, that some third-party components in PanDelos-plus require you to reference certain works in scientific publications. You are free to link or use PanDelos-plus inside the source code of your own program. If you do so, please reference (cite) PanDelos-plus and this website. Bug fixes and collaboration for improvements are appreciated.

## Citation

PanDelos-Plus has been presented at BBCC2024 - the 19th annual edition of the conference, November 27-29, 2024, in Naples, Italy.

<br>

Published 18 nov 2024 [https://doi.org/10.7490/f1000research.1120001.1](https://doi.org/10.7490/f1000research.1120001.1)

Original PanDelos software:

     Bonnici, V., Giugno, R., Manca, V.
     PanDelos: a dictionary-based method for pan-genome content discovery
     BMC bioinformatics 19.15 (2018): 437.

If you have used any of the PanDelos-plus project software, please cite the the following paper:

<!-- TODO add citation -->

```

```
