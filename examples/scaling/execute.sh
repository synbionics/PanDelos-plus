#! bin/bash
calculate_k_path="./../../tools/calculate_k.py"


inFile=""
outFile=""
path=""
genomeNumber=""

function usage() {
    echo "Usage: $0 [-i input_file] [-o output_file] [-m]"
    echo "Options:"
    echo "  -i: Input file path"
    echo "  -o: Output file path"
    echo "  -p: Output path prefix"
    echo "  ?: Display this help message"
}

while getopts ":i:o:p:g:m" opt; do
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
        g )
            genomeNumber=$OPTARG
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
proc=$(nproc)

mainCommand="../../main -i $inFile -k $k -o $outFile -t $proc"
echo "$mainCommand" >> $tmp
/usr/bin/time -f "-> {0,${genomeNumber},%e,%M}" $mainCommand >> $tmp 2>&1
mainCommand="../../main -i $inFile -k $k -o ${outFile}_m -t $proc -m"
echo "$mainCommand" >> $tmp
/usr/bin/time -f "-> {1,${genomeNumber},%e,%M}" $mainCommand >> $tmp 2>&1

