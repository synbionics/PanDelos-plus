#! bin/bash

threadNum=""
inFile=""
outFile=""
mode=""
frags=""
discard=""
path2gbks=""

function usage() {
    echo "Usage: $0 [-i input_file] [-o output_file] [-t thread_num] [-m] [-d discard_value] [-g path to gbks][-h]"
    echo "Options:"
    echo "  -i: Input file path"
    echo "  -o: Output file path"
    echo "  -t: Number of threads"
    echo "  -m: Enable a different mode"
    echo "  -d: Discard value (0 <= d <= 1, default 0.5)"
    echo "  -g: Path to gbk folder"
    echo "  -f: For fragmented genes"
    echo "  -h: Display this help message"
}

while getopts ":i:o:t:mfd:g:h" opt; do
    case ${opt} in
        i )
            inFile=$OPTARG
            ;;
        o )
            outFile=$OPTARG
            ;;
        t )
            threadNum=$OPTARG
            ;;
        m )
            mode="1"
            ;;
        f )
            frags="1"
            ;;
        d )
            discard=$OPTARG
            ;;
        g )
            path2gbks=$OPTARG
            ;;
        
        h )
            usage
            exit 0
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

echo "$inFile"

calculate_k_path="./tools/calculate_k.py"
genes_distribution_path="./tools/genesDistribution.py"
net_clug_path="./tools/netclu_ng.py"
# clus2json_path="./tools/clus2json.py"

if [ -z "$outFile" ]; then
    outFile="$(echo "$(basename $inFile)" | sed 's/\.faa//').net" 
fi
k=$(python3 $calculate_k_path $inFile)
echo "k = $k";

mainCommand="./main -i $inFile -k $k -o $outFile"

if [ -n "$threadNum" ]; then
    mainCommand+=" -t $threadNum"
fi
if [ -n "$mode" ]; then
    mainCommand+=" -m"
fi
if [ -n "$frags" ]; then
    mainCommand+=" -f"
fi
if [ -n "$discard" ]; then
    mainCommand+=" -d $discard"
fi


tmp="tmp.txt"
echo "$mainCommand" >> $tmp

# plot genes distribution
python3 "$genes_distribution_path" "$inFile"

/usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" $mainCommand > $tmp 2>&1

echo "" >> $tmp;
python3 "$net_clug_path" "$inFile" "$outFile.net" >> $tmp

clus="$outFile.clus"

grep "F{ " $tmp | sed s/F{\ //g | sed s/}//g | sed s/\ \;//g | sort | uniq > "$clus"



if [ -z "$path2gbks" ]; then
    echo "Missing gbk folder"
    echo "Missing gbk folder" >> $tmp
    usage
    usage >> $tmp
    exit1
fi


json="$outFile.json"
python3 "$clus2json_path" "$path2gbks" "$clus" "$json" >> $tmp


rm $tmp