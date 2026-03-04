#!/bin/bash

# load conda into the current shell session
source $(conda info --base)/etc/profile.d/conda.sh

current_dir=$(pwd)

LOG_FOLDER="${current_dir}/real_logs"
PD_LOG_FOLDER="${LOG_FOLDER}/pd/"
PDP_LOG_FOLDER="${LOG_FOLDER}/pdp/"

mkdir -p "$LOG_FOLDER"
mkdir -p "$PD_LOG_FOLDER"
mkdir -p "$PDP_LOG_FOLDER"

REAL_DIR="${current_dir}/real"
mkdir -p "$REAL_DIR"

RESULTS_DIR="${current_dir}/real_results"
mkdir -p "$RESULTS_DIR"

cp PanDelos-plus/files/pdi/* "$REAL_DIR/"
rm "$REAL_DIR"/mycoplasma5.pdi

# per ogni file lancia PanDelos-plus e PanDelos, e salva i risultati in due
# file diversi, uno pdp e uno pd
TMP_OUT_DIR="${current_dir}/tmp_out"
mkdir -p "$TMP_OUT_DIR"

conda activate pdp

# print the information about the files

python input_file_stats.py "$REAL_DIR" "${RESULTS_DIR}/input_file_stats.tex"

nproc=$(nproc)
for file in "$REAL_DIR"/*.pdi; do
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    
    # PanDelos-plus
    # esegue con i thread da 1 a nproc disponibili facendo un *2 ogni volta, e salva i risultati in file diversi, quindi eseguira con 1, 2, 4, 8, 16, ....
    t=1
    while [ $t -le $nproc ]; do
        echo "Running PanDelos-plus on $filename.pdi with $t threads"
        bash PanDelos-plus/pandelosp.sh -i "$file" -o "${TMP_OUT_DIR}/${filename}-pdp-${t}t" -t $t >> "${PDP_LOG_FOLDER}/${filename}-pdp-${t}t.log" 2>&1
        t=$((t * 2))
    done

    # PanDelos
    echo "Running PanDelos on $filename.pdi"
    bash PanDelos/pandelos.sh "$file" "${TMP_OUT_DIR}/${filename}-pd" > "${PD_LOG_FOLDER}/${filename}-pd.log" 2>&1
done

conda deactivate

conda activate das
python parse_logs_real.py "$PD_LOG_FOLDER" "$PDP_LOG_FOLDER" "$RESULTS_DIR"

rm -rf "$TMP_OUT_DIR"

print "Done. Results in ${RESULTS_DIR}"