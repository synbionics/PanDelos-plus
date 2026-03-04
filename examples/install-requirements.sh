#!/bin/bash

# load conda into the current shell session
source $(conda info --base)/etc/profile.d/conda.sh

current_dir=$(pwd)

if ! conda env list | grep -q "das"; then
    conda create -n das python=3.10 -y
    conda activate das
    conda install -c conda-forge --file support_files/DAS-requirements.txt
else
    echo "Conda environment 'das' already exists. Skipping creation and package installation."
fi

# create conda environment and install dependencies if not already presents
if ! conda env list | grep -q "panprova"; then
    conda create -n panprova python=3.10 -y
    conda activate panprova
    conda install -c conda-forge --file support_files/PANPROVA-requirements.txt
else
    echo "Conda environment 'panprova' already exists. Skipping creation and package installation."
fi

### ----------------- Scalability ----------------- ###

cd "${current_dir}/scalability"

# PANPROVA
git clone https://github.com/InfOmics/PANPROVA.git
cd PANPROVA
bash compile.sh

cd "$(dirname "$0")/../"

# PanDelos-plus
git clone https://github.com/synbionics/PanDelos-plus.git
cd PanDelos-plus
bash compile.sh

if ! conda env list | grep -q "pdp"; then
    conda create -n pdp python=3.10 -y
    conda activate pdp
    conda install -c conda-forge --file pip-requirements.txt
else
    echo "Conda environment 'pdp' already exists. Skipping creation and package installation."
fi

sed -i '/\$mainCommand > \$tmp 2>&1/ a tail \$tmp -n 1' pandelosp.sh
sed -i '/\echo "Computing clusters"/ a exit 0' pandelosp.sh
sed -i '/\echo "Computing clusters"/ a rm "$outfile.net"' pandelosp.sh
sed -i '/\echo "Computing clusters"/ a rm "$tmp"' pandelosp.sh
cd ../

# PanDelos
git clone https://github.com/InfOmics/PanDelos.git
cd PanDelos
sed -i 's@^\(java -cp ${sdir}/ext/commons-io-2\.6\.jar -cp \$ig infoasys\.cli\.pangenes\.Pangenes \$idb \$k \$dnet >\$tmp\)@/usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" -o $tmp \1@' pandelos.sh
sed -i '/^echo "de-clustering ..."/i cat $tmp | grep time' pandelos.sh

cd ig
bash compile.sh
bash compile.sh

cd ../..


### ----------------- Quality ----------------- ###

cd "${current_dir}/quality"

# PANPROVA
git clone https://github.com/InfOmics/PANPROVA.git
cd PANPROVA
bash compile.sh

cd "$(dirname "$0")/../"

# PanDelos-plus
git clone https://github.com/synbionics/PanDelos-plus.git
cd PanDelos-plus
bash compile.sh

if ! conda env list | grep -q "pdp"; then
    conda create -n pdp python=3.10 -y
    conda activate pdp
    conda install -c conda-forge --file pip-requirements.txt
else
    echo "Conda environment 'pdp' already exists. Skipping creation and package installation."
fi


# PanTools
conda create -n pantools -c conda-forge -c bioconda pantools

# PPanGGOLiN
conda create -n ppanggo -c conda-forge -c bioconda ppanggolin

# Activate the environment
conda activate ppanggo

# Verify the installation
ppanggolin --version