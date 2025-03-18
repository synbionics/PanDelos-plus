import sys
from os import listdir
from os.path import isfile, join
import re
import shutil


idir = sys.argv[1]
odir = sys.argv[2]

gbffFiles = [f for f in listdir(idir) if isfile(join(idir, f)) and re.match(r'^.+\.gbff$', f)]
print(gbffFiles)

for file in gbffFiles:
    input_file_path = join(idir, file)
    new_file_name = re.sub(r'\.gbff$', '.gbk', file)
    output_file_path = join(odir, new_file_name)
    shutil.copy(input_file_path, output_file_path)