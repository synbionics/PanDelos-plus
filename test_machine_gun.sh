#!/bin/bash

mkdir -p tests_output

run_test() {
    local name="$1"
    local input="$2"
    local output_file="tests_output/${name}_output.txt"

    echo "📌 Avvio test per $name (3 run)" | tee "$output_file"

    for i in 1 2 3; do
        echo "----------------------------------------" | tee -a "$output_file"
        echo "▶️  RUN $i per $name" | tee -a "$output_file"
        echo "----------------------------------------" | tee -a "$output_file"

        bash v2_dev_compare.sh -i "$input" -o "$name" -t 10 2>&1 | tee -a "$output_file"

        echo -e "\n\n" >> "$output_file"
    done

    echo "✅ Test completati per $name"
    echo
}

run_ogenomes_test() {
    local name="$1"
    local input="$2"
    local output_file="tests_output/${name}_output.txt"
    local ogen_directory="./${name}_ogen/test/ogenomes/"

    echo "📌 Avvio test per $name (1 run)" | tee "$output_file"

    echo "----------------------------------------" | tee -a "$output_file"
    echo "▶️  RUN unico per $name" | tee -a "$output_file"
    echo "----------------------------------------" | tee -a "$output_file"

    bash v2_dev_compare.sh \
        -i "$input" \
        -o "$name" \
        -g "$ogen_directory" \
        -t 10 \
        2>&1 | tee -a "$output_file"

    echo -e "\n\n" >> "$output_file"

    echo "✅ Test completato per $name"
    echo
}

# -----------------------
# ESECUZIONE DEI TEST
# -----------------------

run_test "salmonella"   "files/pdi/salmonella.pdi"
run_test "mycoplasma5"  "files/pdi/mycoplasma5.pdi"
run_test "xanthomonas"  "files/pdi/xanthomonas.pdi"
run_test "escherichia" "files/pdi/escherichia.pdi"
run_ogenomes_test "salmonella_70" "files/pdi/salmonella_70.pdi"
