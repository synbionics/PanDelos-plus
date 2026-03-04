#!/bin/bash

# load conda into the current shell session
source $(conda info --base)/etc/profile.d/conda.sh

current_dir=$(pwd)

EXAMPLE_DIR="${current_dir}/PANPROVA/examples/test_3"
OGENOMES_DIR="${EXAMPLE_DIR}/example/ogenomes"
SYNTHETIC_DIR="${current_dir}/synthetic_ogenomes"

RESULTS_DIR="${current_dir}/synthetic_results"
mkdir -p "$RESULTS_DIR"

TMP_OUT_DIR="${current_dir}/tmp_out"
mkdir -p "$TMP_OUT_DIR"

LOG_FOLDER="${current_dir}/synthetic_logs"
PD_LOG_FOLDER="${LOG_FOLDER}/pd/"
PDP_LOG_FOLDER="${LOG_FOLDER}/pdp/"

mkdir -p "$LOG_FOLDER"
mkdir -p "$PD_LOG_FOLDER"
mkdir -p "$PDP_LOG_FOLDER"

conda activate panprova

echo "Moving to example directory ${EXAMPLE_DIR} and running the example script to generate the input files for the synthetic test"
cd "$EXAMPLE_DIR"

bash run_example.sh

# # conda deactivate

echo "Generating synthetic genomes from ${OGENOMES_DIR} and running PanDelos and PanDelos-plus on them"

cd "$current_dir"

conda activate pdp

mkdir -p "${SYNTHETIC_DIR}"

mv "${OGENOMES_DIR}/"*.gbff "${SYNTHETIC_DIR}/"

for f in "${SYNTHETIC_DIR}/"*.gbff; do
    cp "$f" "${f%.gbff}.gbk"
done

SUB_GENOMES="${SYNTHETIC_DIR}/sub_genomes"
ALL_GENOMES="${SYNTHETIC_DIR}/all_genomes.pdi"

mkdir -p "${SUB_GENOMES}"

python PanDelos-plus/scripts/gbk2ig.py ${SYNTHETIC_DIR}/ ${ALL_GENOMES}

python extract_synthetic.py ${ALL_GENOMES} ${SUB_GENOMES}

python input_file_stats.py "${SUB_GENOMES}" "${RESULTS_DIR}/input_file_stats.tex"

FIXED_FILENAME="50.pdi"
# FIXED_FILENAME="all_genomes.pdi"
FIXED_GENOMES="${SUB_GENOMES}/${FIXED_FILENAME}"

nproc=$(nproc)
t=1
while [ $t -le $nproc ]; do
    echo "Running PanDelos-plus on ${FIXED_GENOMES} with $t threads"
    bash PanDelos-plus/pandelosp.sh -i "${FIXED_GENOMES}" -o "${TMP_OUT_DIR}/50-${t}t" -t $t >> "${PDP_LOG_FOLDER}/50-${t}t.log" 2>&1
    t=$((t * 2))
done
echo "Running PanDelos on ${FIXED_GENOMES}"

bash PanDelos/pandelos.sh "${FIXED_GENOMES}" "${TMP_OUT_DIR}/50-pd" > "${PD_LOG_FOLDER}/50-pd.log" 2>&1

for file in "${SUB_GENOMES}"/*.pdi; do
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    echo "Running PanDelos-plus on $file with $nproc threads"
    bash PanDelos-plus/pandelosp.sh -i "$file" -o "${TMP_OUT_DIR}/${filename}-pdp" -t $nproc >> "${PDP_LOG_FOLDER}/${filename}-pdp.log" 2>&1
done

conda deactivate
conda activate das
python parse_logs_synthetic.py "$PD_LOG_FOLDER" "$PDP_LOG_FOLDER" "$RESULTS_DIR"


rm -rf "$TMP_OUT_DIR"

echo "Done. Results in ${RESULTS_DIR}"