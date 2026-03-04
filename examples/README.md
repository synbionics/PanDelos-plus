# pdp-examples

Benchmark suite for comparing PanDelos-plus against PanDelos, PPanGGOLiN and PanTools on pangenome detection tasks. The tests are divided into three categories: **scalability** (execution time and memory with varying genome counts and thread counts), **quality** (gene family prediction accuracy against a known ground truth), and **families** (gene family variation analysis when adding genomes to an existing pangenome).

## Directory structure

```
.
├── install-requirements.sh      # Setup script (conda envs, tool compilation)
├── support_files/               # Conda requirement files
├── scalability/
│   ├── synthetic.sh             # Scalability test on synthetic genomes
│   ├── real.sh                  # Scalability test on real genomes
│   ├── parse_logs_synthetic.py  # Log parser for synthetic results
│   ├── parse_logs_real.py       # Log parser for real results
│   ├── extract_synthetic.py     # Extracts genome subsets from synthetic data
│   └── input_file_stats.py      # Generates input stats (LaTeX table)
├── quality/
│   ├── run.sh                   # Quality test runner (all steps)
│   ├── evaluate.py              # Evaluation script (ground truth vs predicted)
│   ├── parse_*_gene_families.py # Parsers per tool output
│   ├── extract_fasta.py         # GBFF to FASTA converter (for PanTools)
│   ├── extract_gff.py           # GBFF to GFF converter (for PanTools)
│   └── generate_latex_tables.py # LaTeX table generator for results
├── families/
│   └── notebook.ipynb           # Gene family mapping analysis notebook
└── README.md
```

## Prerequisites

- [Conda](https://docs.conda.io/en/latest/) (Miniconda or Anaconda)
- Java (required by PanDelos and PanTools)
- Git

## Installation

Run the installation script from the project root:

```bash
bash install-requirements.sh
```

This will:

1. Create the required conda environments (`das`, `panprova`, `pdp`, `pantools`, `ppanggo`).
2. Clone and compile the tools (PANPROVA, PanDelos-plus, PanDelos) inside both `scalability/` and `quality/`.
3. Patch `pandelosp.sh` and `pandelos.sh` in the scalability directory to capture timing and memory usage.

## Scalability tests

All scripts are inside `scalability/`. There are two test suites: one on **synthetic** genomes and one on **real** genomes.

### Synthetic genomes

```bash
cd scalability
bash synthetic.sh
```

Steps performed:

1. Generates synthetic genomes using PANPROVA (`panprova` env).
2. Converts genomes to `.pdi` format and extracts subsets of increasing size (50, 100, ..., 600 genomes).
3. Runs **PanDelos-plus** on a fixed 50-genome dataset with increasing thread counts (1, 2, 4, ..., up to `nproc`).
4. Runs **PanDelos** on the same 50-genome dataset.
5. Runs **PanDelos-plus** on all genome subsets using all available threads.
6. Parses logs and generates result tables.

**Results:** `scalability/synthetic_results/`
**Logs:** `scalability/synthetic_logs/` (subdirectories `pd/` and `pdp/`)

### Real genomes

```bash
cd scalability
bash real.sh
```

Steps performed:

1. Copies real genome `.pdi` files from `PanDelos-plus/files/pdi/`.
2. For each dataset, runs **PanDelos-plus** with increasing thread counts (1, 2, 4, ..., up to `nproc`).
3. For each dataset, runs **PanDelos**.
4. Parses logs and generates result tables.

**Results:** `scalability/real_results/`
**Logs:** `scalability/real_logs/` (subdirectories `pd/` and `pdp/`)

## Quality tests

All scripts are inside `quality/`. The test compares gene family predictions from multiple tools against a PANPROVA-generated ground truth.

### Usage

```bash
cd quality
bash run.sh <ngenomes> [options]
```

Where `<ngenomes>` is the number of synthetic genomes to generate (e.g. 50, 100, 500).

### Options

| Flag | Description |
|------|-------------|
| `--no-generate` | Skip PANPROVA genome generation |
| `--no-pdp` | Skip PanDelos-plus |
| `--no-ppanggolin` | Skip PPanGGOLiN |
| `--no-pantools` | Skip PanTools |
| `--no-eval` | Skip evaluation |
| `--only-generate` | Run only PANPROVA generation |
| `--only-pdp` | Run only PanDelos-plus |
| `--only-ppanggolin` | Run only PPanGGOLiN |
| `--only-pantools` | Run only PanTools |
| `--only-eval` | Run only evaluation |
| `--yes` | Auto-confirm directory removal prompts |

### Examples of execution

```bash
bash run.sh 50
bash run.sh 100 --no-pantools --no-ppanggolin
bash run.sh 500 --only-eval
bash run.sh 1000 --yes
```

### Steps performed

1. **PANPROVA generation** — generates synthetic genomes with known gene families (ground truth).
2. **PanDelos-plus** — runs pangenome detection on the generated genomes.
3. **PPanGGOLiN** — runs pangenome detection on the same genomes.
4. **PanTools** — runs pangenome detection on the same genomes (requires FASTA/GFF conversion).
5. **Evaluation** — compares each tool's predicted gene families against the PANPROVA ground truth.

**Results:** `quality/evaluation/<ngenomes>/evaluation_results.txt`

## Gene family mapping analysis

The notebook `families/notebook.ipynb` analyzes how gene families evolve when additional genomes are added to a pangenome dataset. It compares a specified reduced genome subset (Set 1) against the complete dataset (Set 2) for four real bacterial species.


### Prerequisites

- Python 3 with `pandas`, `numpy`, `matplotlib`, `jupyter`
- Docker (for running PanDelos-plus)

### Usage

Open the notebook and run all cells:


To change the genome subsets, edit the `manual_set_1` dictionary in the notebook. Set 2 is automatically derived from all genomes in the corresponding `.pdi` file.

### Steps performed

1. **Setup** — clones PanDelos-plus, builds the Docker image, copies experiment `.pdi` files.
2. **Genome subsetting** — uses manually defined genome lists for Set 1; Set 2 includes all genomes from the `.pdi` file.
3. **PanDelos-plus analysis** — runs gene family detection on both subsets via Docker.
4. **Family mapping** — maps each Set 1 family to its best matching Set 2 family (>=80% gene overlap), classifying matches as:
   - **Unaltered** — identical gene content in both sets.
   - **Enlarged** — Set 2 family contains all Set 1 genes plus additional ones.
   - **Partially enlarged** — Set 2 family gained genes but lost some from Set 1.
   - **Collapsed** — Set 1 family has no match in Set 2.
   - **New** — family exists only in Set 2.
5. **Visualization** — generates diffusivity heatmaps (fixed and adaptive sizes) and bar charts per organism.
6. **LaTeX tables** — exports dataset overview, correspondence tables, and match type breakdowns.

### Output

- `families/output/` — heatmap images (PNG and TIF) and LaTeX tables (`latex_tables/` subdirectory).
