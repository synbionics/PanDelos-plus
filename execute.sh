#! bin/bash

if [ -z "$1" ]; then
    echo "Missing input file"
    exit 0
fi

# if [ -z "$2" ]; then
# 	echo "Missing stats file"
#     exit 0
# fi


infile="$1"
# statsFile="$2"

calculate_k_path="./tools/calculate_k.py"
genes_distribution_path="./tools/genesDistribution.py"
net_clug_path="./tools/netclu_ng.py"
# net_clug_path="./tools/netclu_ng_plot.py"
# clus2json_path="./tools/clus2json.py"
# echo "$infile"



k=$(python3 $calculate_k_path $infile)
echo "k = $k";

outFile=$(echo "$(basename $infile)" | sed 's/\.faa//')

# outFile+="tmp_not_all2"
# outFile+="_k$k_matrix"
# echo "outfile: $outFile"

tmp="tmp.txt"

# plot genes distribution
python3 "$genes_distribution_path" "$infile"

/usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" ./main -m -i "$infile" -o "$outFile" -k "$k" > $tmp 2>&1

python3 "$net_clug_path" "$infile" "$outFile.net" >> $tmp

clus="$outFile.clus"

grep "F{ " $tmp | sed s/F{\ //g | sed s/}//g | sed s/\ \;//g | sort | uniq > "$clus"

# json="$outFile.json"
# python3 "$clus2json_path" "$tmp" "$json"

# rm $tmp