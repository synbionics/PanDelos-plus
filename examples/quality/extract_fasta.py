import sys
from pathlib import Path
from Bio import SeqIO

gbff_dir = sys.argv[1]
fasta_dir = sys.argv[2]

for gbff in Path(gbff_dir).glob("*.gbff"):
    records = list(SeqIO.parse(str(gbff), "genbank"))
    out_path = Path(fasta_dir) / f"{gbff.stem}.fasta"
    SeqIO.write(records, str(out_path), "fasta")
    print(f"{gbff.name} -> {out_path.name} ({len(records)} records)")