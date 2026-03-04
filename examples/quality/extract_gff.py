import sys
from pathlib import Path
from Bio import SeqIO

gbff_dir = sys.argv[1]
gff_dir = sys.argv[2]

for gbff in Path(gbff_dir).glob("*.gbff"):
    out_path = Path(gff_dir) / f"{gbff.stem}.gff3"
    with open(out_path, "w") as out:
        out.write("##gff-version 3\n")
        for record in SeqIO.parse(str(gbff), "genbank"):
            for feature in record.features:
                if feature.type in ("gene", "CDS", "mRNA", "tRNA", "rRNA"):
                    start = int(feature.location.start) + 1  # GFF è 1-based
                    end = int(feature.location.end)
                    strand = "+" if feature.location.strand == 1 else "-"
                    
                    attrs = []
                    if "locus_tag" in feature.qualifiers:
                        attrs.append(f"ID={feature.qualifiers['locus_tag'][0]}")
                    if "product" in feature.qualifiers:
                        attrs.append(f"product={feature.qualifiers['product'][0]}")
                    
                    attr_str = ";".join(attrs) if attrs else "."
                    out.write(f"{record.id}\t.\t{feature.type}\t{start}\t{end}\t.\t{strand}\t.\t{attr_str}\n")
    
    print(f"{gbff.name} -> {out_path.name}")