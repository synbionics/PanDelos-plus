#! bin/bash

if [ -z "$1" ]; then
    echo "Missing input file"
    exit 0
fi

if [ -z "$2" ]; then
	echo "Missing stats file"
    exit 0
fi


infile="$1"
statsFile="$2"

calculate_k_path="./tools/calculate_k.py" 
# echo "$infile"



k=$(python3 $calculate_k_path $infile)
echo "k = $k";

outFile=$(echo "$(basename $infile)" | sed 's/\.faa//')

# outFile+="tmp_not_all2"
# outFile+="_k$k_matrix"
# echo "outfile: $outFile"

/usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" ./main -i "$infile" -o "$outFile" -k "$k" > $statsFile 2>&1
