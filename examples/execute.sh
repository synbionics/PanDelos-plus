#! bin/bash
calculate_k_path="./../tools/calculate_k.py"


inFile=""
outFile=""
path=""

function usage() {
    echo "Usage: $0 [-i input_file] [-o output_file] [-m]"
    echo "Options:"
    echo "  -i: Input file path"
    echo "  -o: Output file path"
    echo "  -p: Output path prefix"
    echo "  ?: Display this help message"
}

while getopts ":i:o:p:m" opt; do
    case ${opt} in
        i )
            inFile=$OPTARG
            ;;
        o )
            outFile=$OPTARG
            ;;
        p )
            path=$OPTARG
            ;;
        \? )
            echo "Invalid option: $OPTARG" 1>&2
            usage
            exit 1
            ;;
        : )
            echo "Invalid option: $OPTARG requires an argument" 1>&2
            usage
            exit 1
            ;;
    esac
done

if [ -z "$inFile" ]; then
    echo "Missing input file"
    usage
    exit 1
fi

if [ -z "$outFile" ]; then
    echo "Missing output file"
    usage
    exit 1
fi

if [ -z "$path" ]; then
    echo "Missing path"
    usage
    exit 1
fi


k=$(python3 $calculate_k_path $inFile)

tmp="${path}tmp.txt"
stats="${path}stats.csv"

echo "k = $k" >> $tmp;
echo "inFile = $inFile" >> $tmp;
echo "outFile = $outFile" >> $tmp;
maxT=$(nproc)

echo "max threads: $maxT" >>$tmp
for ((t = 1; t <= $maxT; t*=2)); do

    echo "Iterazione $t"
    currentOutName="${outFile}_${t}"
    mainCommand="../main -i $inFile -k $k -o $currentOutName -t $t"
    
    echo "Running $mainCommand"
    echo "$mainCommand" >> $tmp
    echo "" >> $tmp;
    echo "# no mode" >> $tmp;
    # mode, time, memory, threads
    /usr/bin/time -f "-> {0,%e,%M,$t}" $mainCommand >> $tmp 2>&1
    echo "" >> $tmp;
    currentOutName+="_m"

    mainCommand="../main -i $inFile -k $k -o $currentOutName -t $t -m"
    echo "Running $mainCommand"

    echo "# mode" >> $tmp;
    /usr/bin/time -f "-> {1,%e,%M,$t}" $mainCommand >> $tmp 2>&1
    echo "" >> $tmp;
done

python plot.py $tmp $path
