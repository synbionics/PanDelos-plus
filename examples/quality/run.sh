#!/bin/bash

source $(conda info --base)/etc/profile.d/conda.sh

# ============================================================================
# CLI parsing
# ============================================================================

SKIP_PANPROVA=false
SKIP_PDP=false
SKIP_PPANGGOLIN=false
SKIP_PANTOOLS=false
SKIP_EVAL=false
YES=false
NGENOMES=""

# --only-* tracking (if any is set, all other steps are skipped)
ONLY_GENERATE=false
ONLY_PDP=false
ONLY_PPANGGOLIN=false
ONLY_PANTOOLS=false
ONLY_EVAL=false
ONLY_MODE=false

usage() {
  echo "Usage: $0 <ngenomes> [options]"
  echo ""
  echo "Skip options:"
  echo "  --no-generate    Skip PANPROVA generation step"
  echo "  --no-pdp         Skip PanDelos-plus step"
  echo "  --no-ppanggolin  Skip PPanGGOLiN step"
  echo "  --no-pantools    Skip PanTools step"
  echo "  --no-eval        Skip evaluation step"
  echo ""
  echo "Only options (run only the specified steps):"
  echo "  --only-generate    Run only PANPROVA generation"
  echo "  --only-pdp         Run only PanDelos-plus"
  echo "  --only-ppanggolin  Run only PPanGGOLiN"
  echo "  --only-pantools    Run only PanTools"
  echo "  --only-eval        Run only evaluation"
  echo ""
  echo "Other:"
  echo "  --yes            Auto-confirm all directory removal prompts"
  echo ""
  echo "Example: $0 1000"
  echo "Example: $0 1000 --no-generate --no-pdp"
  echo "Example: $0 1000 --only-eval --only-plots"
  echo "Example: $0 1000 --yes"
  exit 1
}

confirm_remove() {
  local dir="$1"
  if [ "$YES" = true ]; then
    echo "Removing '$dir'..."
    rm -rf "$dir"
  else
    read -p "Directory '$dir' already exists. Delete it and continue? (y/n): " choice
    case "$choice" in
      y|Y)
        echo "Removing '$dir'..."
        rm -rf "$dir"
        ;;
      *)
        echo "Aborted."
        exit 0
        ;;
    esac
  fi
}

# Parse arguments
for arg in "$@"; do
  case "$arg" in
    --no-generate) SKIP_PANPROVA=true ;;
    --no-pdp)      SKIP_PDP=true ;;
    --no-ppanggolin) SKIP_PPANGGOLIN=true ;;
    --no-pantools) SKIP_PANTOOLS=true ;;
    --no-eval)     SKIP_EVAL=true ;;
    --only-generate)   ONLY_GENERATE=true; ONLY_MODE=true ;;
    --only-pdp)        ONLY_PDP=true; ONLY_MODE=true ;;
    --only-ppanggolin) ONLY_PPANGGOLIN=true; ONLY_MODE=true ;;
    --only-pantools)   ONLY_PANTOOLS=true; ONLY_MODE=true ;;
    --only-eval)       ONLY_EVAL=true; ONLY_MODE=true ;;
    --yes)         YES=true ;;
    -h|--help)     usage ;;
    *)
      if [ -z "$NGENOMES" ]; then
        NGENOMES=$arg
      else
        echo "Unknown option: $arg"
        usage
      fi
      ;;
  esac
done

if [ -z "$NGENOMES" ]; then
  usage
fi

# --only-* mode: skip everything except explicitly requested steps
if [ "$ONLY_MODE" = true ]; then
  [ "$ONLY_GENERATE"   = true ] && SKIP_PANPROVA=false   || SKIP_PANPROVA=true
  [ "$ONLY_PDP"        = true ] && SKIP_PDP=false        || SKIP_PDP=true
  [ "$ONLY_PPANGGOLIN" = true ] && SKIP_PPANGGOLIN=false || SKIP_PPANGGOLIN=true
  [ "$ONLY_PANTOOLS"   = true ] && SKIP_PANTOOLS=false   || SKIP_PANTOOLS=true
  [ "$ONLY_EVAL"       = true ] && SKIP_EVAL=false       || SKIP_EVAL=true
fi

CURRENT_DIR=$(pwd)
# in order ground truth file and predicted file
EVALUATION_SCRIPT="${CURRENT_DIR}/evaluate.py"

GBFF2FASTA_SCRIPT="${CURRENT_DIR}/gbff_to_fasta.py"
GBFF2GFF_SCRIPT="${CURRENT_DIR}/gbff_to_gff.py"

PANPROVA_OUTDIR="${CURRENT_DIR}/PANPROVA/examples/quality_tests_${NGENOMES}"

# ============================================================================
# PANPROVA generation
# ============================================================================

if [ "$SKIP_PANPROVA" = false ]; then

conda activate panprova
  # Check if output directory already exists
  if [ -d "$PANPROVA_OUTDIR" ]; then
    confirm_remove "$PANPROVA_OUTDIR"
  fi

  # copy template
  cp -r PANPROVA/examples/test_3 "$PANPROVA_OUTDIR"

  # Generate the run script inside the output directory
  cat > "${PANPROVA_OUTDIR}/run_panprova.sh" << 'SCRIPT_END'
#!/bin/bash

NGENOMES=__NGENOMES__

echo "================================================================================"
echo "PANPROVA - quality test with ${NGENOMES} genomes"
echo "================================================================================"

echo "HGT genome list is in hgt_list.txt"
echo "Extracting HGT pool..."
cmd="../../create_hgt_pool hgt_list.txt hgt_pool"
echo "$cmd"
date
/usr/bin/time -f"%E %M" $cmd
date

echo "================================================================================"
echo "Evolving..."
cmd="bash ../../PANPROVA.sh --igenome ../genomes/escherichia_coli_O157H7.peg --hgtpool hgt_pool --ngenomes ${NGENOMES} --trans-table 11 --oprefix ./${NGENOMES}"
echo "$cmd"
date
/usr/bin/time -f"%E %M" $cmd
date
SCRIPT_END

  # replace placeholder with actual value
  sed -i "s/__NGENOMES__/${NGENOMES}/" "${PANPROVA_OUTDIR}/run_panprova.sh"
  chmod +x "${PANPROVA_OUTDIR}/run_panprova.sh"

  echo "Script generated: ${PANPROVA_OUTDIR}/run_panprova.sh"

  # Launch PANPROVA
  cd "$PANPROVA_OUTDIR" || exit 1
  echo "Running PANPROVA..."
  bash run_panprova.sh

  echo "PANPROVA run completed. Output is in ${PANPROVA_OUTDIR}"

  # back to the current directory
  cd "$CURRENT_DIR" || exit 1

conda deactivate
else
  echo "[SKIPPED] PANPROVA generation (--no-generate)"
fi

# ============================================================================
# PanDelos-plus
# ============================================================================

PANDELOS_PLUS_INPUTDIR="${CURRENT_DIR}/PanDelos-plus/input/quality_tests_${NGENOMES}"
PANDELOS_PLUS_OUTPUTDIR="${CURRENT_DIR}/PanDelos-plus/output/quality_tests_${NGENOMES}"

if [ "$SKIP_PDP" = false ]; then

  if [ -d "$PANDELOS_PLUS_INPUTDIR" ]; then
    confirm_remove "$PANDELOS_PLUS_INPUTDIR"
  fi
  mkdir -p "${PANDELOS_PLUS_INPUTDIR}"

  if [ -d "$PANDELOS_PLUS_OUTPUTDIR" ]; then
    confirm_remove "$PANDELOS_PLUS_OUTPUTDIR"
  fi

  mkdir -p "${PANDELOS_PLUS_OUTPUTDIR}"

  # copy panprova gbff files to pandelos-plus input directory
  cp "${PANPROVA_OUTDIR}/${NGENOMES}/ogenomes/"*.gbff "${PANDELOS_PLUS_INPUTDIR}/"

  echo "PanDelos-plus input files copied to ${PANDELOS_PLUS_INPUTDIR}"

  cd "$CURRENT_DIR/PanDelos-plus" || exit 1

  conda activate pdp

  echo "Running PanDelos-plus..."
  bash pandelosp.sh -g "${PANDELOS_PLUS_INPUTDIR}" -i "${PANDELOS_PLUS_INPUTDIR}/${NGENOMES}.pdi" -o "${PANDELOS_PLUS_OUTPUTDIR}/${NGENOMES}"

  echo "PanDelos-plus run completed. Output is in ${PANDELOS_PLUS_OUTPUTDIR}"

  conda deactivate

  cd "$CURRENT_DIR" || exit 1

else
  echo "[SKIPPED] PanDelos-plus (--no-pdp)"
fi



# ============================================================================
# PPanGGOLiN
# ============================================================================

PPANGGOLIN_OUTPUTDIR="${CURRENT_DIR}/PPanGGOLiN/output/quality_tests_${NGENOMES}"
PANGOLIN_INPUT_FILE="${CURRENT_DIR}/PPanGGOLiN/input/quality_tests_${NGENOMES}.tsv"
PPANGGOLIN_TABLE_DIR="${PPANGGOLIN_OUTPUTDIR}/table"

if [ "$SKIP_PPANGGOLIN" = false ]; then

  if [ -d "$PPANGGOLIN_OUTPUTDIR" ]; then
    confirm_remove "$PPANGGOLIN_OUTPUTDIR"
  fi
  # no because ppanggolin will create the output directory
  # mkdir -p "${PPANGGOLIN_OUTPUTDIR}"

  if [ -f "$PANGOLIN_INPUT_FILE" ]; then
    confirm_remove "$PANGOLIN_INPUT_FILE"
  fi

  mkdir -p "$(dirname "${PANGOLIN_INPUT_FILE}")"


  conda activate ppanggo
  # create tsv
  ls "${PANPROVA_OUTDIR}/${NGENOMES}/ogenomes/"*.gbff | while read f; do
  echo -e "$(basename $f .gbff)\t$f"
  done > ${PANGOLIN_INPUT_FILE}


  echo "Running PPanGGOLiN..."
  ppanggolin workflow --anno ${PANGOLIN_INPUT_FILE} \
    --output ${PPANGGOLIN_OUTPUTDIR} \
    --cpu $(nproc --all)
  
  echo "PPanGGOLiN run completed. Output is in ${PPANGGOLIN_OUTPUTDIR}"
  conda deactivate

  cd "$CURRENT_DIR" || exit 1

else
  echo "[SKIPPED] PPanGGOLiN (--no-ppanggolin)"


fi

# ============================================================================
# PanTools
# ============================================================================

PANTOOLS_OUTPUTDIR="${CURRENT_DIR}/PanTools/output/quality_tests_${NGENOMES}"
PANTOOLS_FASTA_DIR="${PANTOOLS_OUTPUTDIR}/fasta"
PANTOOLS_GFF_DIR="${PANTOOLS_OUTPUTDIR}/gff"
PANTOOLS_GENOMES_FILE="${PANTOOLS_OUTPUTDIR}/genomes.txt"
PANTOOLS_ANNOTATIONS_FILE="${PANTOOLS_OUTPUTDIR}/annotations.txt"
PANTOOLS_DB="${PANTOOLS_OUTPUTDIR}/pantools_db"
PANTOOLS_CSV="${PANTOOLS_DB}/group_info/group_info.csv"


if [ "$SKIP_PANTOOLS" = false ]; then
  if [ -d "$PANTOOLS_OUTPUTDIR" ]; then
    confirm_remove "$PANTOOLS_OUTPUTDIR"
  fi
  mkdir -p "${PANTOOLS_OUTPUTDIR}"
  mkdir -p "${PANTOOLS_FASTA_DIR}"
  mkdir -p "${PANTOOLS_GFF_DIR}"

  conda activate panprova

  # convert gbff to fasta and gff
  python3 extract_fasta.py ${PANPROVA_OUTDIR}/${NGENOMES}/ogenomes/ ${PANTOOLS_FASTA_DIR}
  python3 extract_gff.py ${PANPROVA_OUTDIR}/${NGENOMES}/ogenomes/ ${PANTOOLS_GFF_DIR}

  conda deactivate

  echo "Running preprocessing for PanTools..."

  ls ${PANTOOLS_FASTA_DIR}/*.fasta | while read f; do
    echo "$(realpath $f)"
  done > ${PANTOOLS_GENOMES_FILE}

  counter=1
    ls ${PANTOOLS_GFF_DIR}/*.gff3 | while read f; do
        echo "${counter} $(realpath $f)"
        counter=$((counter + 1))
    done > ${PANTOOLS_ANNOTATIONS_FILE}


  echo "Preprocessing for PanTools completed."

  echo "Running PanTools..."

  conda activate pantools

  pantools -Xmx48g build_pangenome ${PANTOOLS_DB} ${PANTOOLS_GENOMES_FILE} && \
  pantools -Xmx48g add_annotations --connect ${PANTOOLS_DB} ${PANTOOLS_ANNOTATIONS_FILE} && \
  pantools -Xmx48g group --relaxation=4 ${PANTOOLS_DB} && \
  pantools -Xmx48g grouping_overview ${PANTOOLS_DB} && \
  pantools -Xmx48g group_info ${PANTOOLS_DB}

  conda deactivate
else
  echo "[SKIPPED] PanTools (--no-pantools)"
fi

# ============================================================================
# Evaluation
# ============================================================================

if [ "$SKIP_EVAL" = false ]; then
  conda activate das
  # files for evaluation
  PARSE_PANPROVA_GENE_FAMILIES_SCRIPT="${CURRENT_DIR}/parse_panprova_gene_families.py"
  PARSE_PANDELOS_PLUS_GENE_FAMILIES_SCRIPT="${CURRENT_DIR}/parse_pandelos_plus_gene_families.py"
  PARSE_PANDELOS_PLUS_PARSED_FAMILIES_SCRIPT="${CURRENT_DIR}/convert_pandelos_plus_parsed_families.py"
  PARSE_PPANGGOLIN_GENE_FAMILIES_SCRIPT="${CURRENT_DIR}/parse_ppanggolin_gene_families.py"
  PARSE_PANTOOLS_GENE_FAMILIES_SCRIPT="${CURRENT_DIR}/parse_pantools_gene_families.py"

  CURRENT_EVAL_DIR="${CURRENT_DIR}/evaluation/${NGENOMES}"
  # check if evaluation directory exists
  if [ -d "${CURRENT_EVAL_DIR}" ]; then
    confirm_remove "${CURRENT_EVAL_DIR}"
  fi
  mkdir -p "${CURRENT_EVAL_DIR}"

  # -----------------
  # parse PANPROVA gene families
  # -----------------
  python "$PARSE_PANPROVA_GENE_FAMILIES_SCRIPT" "${PANPROVA_OUTDIR}/${NGENOMES}.gene_families" "${CURRENT_EVAL_DIR}/panprova_gene_families.json"

  # -----------------
  # parse PanDelos-plus gene families
  # -----------------
  python "$PARSE_PANDELOS_PLUS_GENE_FAMILIES_SCRIPT" "${PANDELOS_PLUS_OUTPUTDIR}/${NGENOMES}.json" "${CURRENT_EVAL_DIR}/pandelos_plus_gene_families.json"

  python "$PARSE_PANDELOS_PLUS_PARSED_FAMILIES_SCRIPT" "${CURRENT_EVAL_DIR}/pandelos_plus_gene_families.json" "${CURRENT_EVAL_DIR}/pandelos_plus_parsed_families.json"

  python "$EVALUATION_SCRIPT" "${CURRENT_EVAL_DIR}/panprova_gene_families.json" "${CURRENT_EVAL_DIR}/pandelos_plus_parsed_families.json" "${CURRENT_EVAL_DIR}/${NGENOMES}" >> "${CURRENT_EVAL_DIR}/evaluation_results.txt"

  # -----------------
  # parse PPanGGOLiN gene families
  # -----------------
  if [ -d "$PPANGGOLIN_TABLE_DIR" ]; then
    python "$PARSE_PPANGGOLIN_GENE_FAMILIES_SCRIPT" "${PPANGGOLIN_TABLE_DIR}" "${CURRENT_EVAL_DIR}/ppanggolin_gene_families.json"

    python "$EVALUATION_SCRIPT" "${CURRENT_EVAL_DIR}/panprova_gene_families.json" "${CURRENT_EVAL_DIR}/ppanggolin_gene_families.json" "${CURRENT_EVAL_DIR}/${NGENOMES}" >> "${CURRENT_EVAL_DIR}/evaluation_results.txt"
  else
    echo "[SKIPPED] PPanGGOLiN evaluation (table directory not found: ${PPANGGOLIN_TABLE_DIR})"
  fi

  # -----------------
  # parse PanTools gene families
  # -----------------
  if [ -f "$PANTOOLS_CSV" ]; then
    python "$PARSE_PANTOOLS_GENE_FAMILIES_SCRIPT" "${PANTOOLS_CSV}" "${PANTOOLS_GENOMES_FILE}" "${CURRENT_EVAL_DIR}/pantools_gene_families.json"

    python "$EVALUATION_SCRIPT" "${CURRENT_EVAL_DIR}/panprova_gene_families.json" "${CURRENT_EVAL_DIR}/pantools_gene_families.json" "${CURRENT_EVAL_DIR}/${NGENOMES}" >> "${CURRENT_EVAL_DIR}/evaluation_results.txt"
  else
    echo "[SKIPPED] PanTools evaluation (CSV not found: ${PANTOOLS_CSV})"
  fi
  conda deactivate
else
  echo "[SKIPPED] Evaluation (--no-eval)"
fi

echo "All done."