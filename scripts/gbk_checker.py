#!/usr/bin/python3

import sys
from os import listdir
from os.path import isfile, join
import re
from Bio import SeqIO

def read_gbk(ifile, genome_id):
    print(f"Processing genome: {genome_id}")

    for record in SeqIO.parse(ifile, "genbank"):
        sequence_id = record.id
        print(f"\tGenome ID: {genome_id}\tSequence ID: {sequence_id}")
        
        has_source = False
        has_cds = False
        has_translation = False

        for feature in record.features:
            if feature.type == 'source':
                has_source = True
            elif feature.type == 'CDS':
                has_cds = True
                if 'translation' in feature.qualifiers:
                    has_translation = True

        if not has_source:
            raise Exception("'source' feature not found")
        if not has_cds:
            raise Exception("'CDS' feature not found")
        if not has_translation:
            raise Exception("'translation' feature.qualifier not found")


idir = sys.argv[1]
print(f"Reading gbk files from: {idir}")

gbkfiles = [f for f in listdir(idir) if isfile(join(idir, f)) and re.match(r'^.+\.gbk$', f)]
print(f"Files found: {gbkfiles}")

try:
    for gbk in gbkfiles:
        print(f"Processing file: {gbk}")
        read_gbk(idir + gbk, re.sub('\.gbk$', '', gbk))
    print("All files processed successfully")
except Exception as e:
    print(f"Error processing a gbk file: {e}")
    sys.exit(1)
