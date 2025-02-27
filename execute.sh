#! bin/bash

sdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
sdir=`dirname $sdir`

scripts_path="$sdir/scripts/"

echo "Using scripts at path: $scripts_path"



faa_checker_path="${scripts_path}faa_checker.py"
calculate_k_path="${scripts_path}calculate_k.py"
genes_distribution_path="${scripts_path}genesDistribution.py"
net_clug_path="${scripts_path}netclu_ng.py"
clus2json_path="${scripts_path}clus2json.py"



threadNum=""
inFile=""
outFile=""
mode=""
frags=""
discard=""
path2gbks=""
similarityParameter=""

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
    echo "  -p: For a stronger threshold (similarity parameter)"
    echo "  -h: Display this help message"
}

while getopts ":i:o:t:mfpd:g:h" opt; do
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
        p) 
            similarityParameter="1"
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


date

if [ -z "$inFile" ]; then
    echo "Missing input file"
    usage
    exit 1
fi

echo "$inFile"




if [ -z "$outFile" ]; then
    outFile="$(echo "$(basename $inFile)" | sed 's/\.faa//').net" 
fi

k=$(python3 $calculate_k_path $inFile)

if [ $? -ne 0 ]; then
    echo "Error running calculate_k.py"
    exit 1
fi

echo "k = $k";


echo "Checking input file (.faa)"

python3 "$faa_checker_path" "$inFile" "$k"
if [ $? -ne 0 ]; then
    echo "Error running faa_checker.py"
    exit 1
fi

echo "Executing main"


mainCommand="$sdir/./main -i $inFile -k $k -o $outFile"

if [ -n "$threadNum" ]; then
    mainCommand+=" -t $threadNum"
fi
if [ -n "$mode" ]; then
    mainCommand+=" -m"
fi
if [ -n "$frags" ]; then
    mainCommand+=" -f"
fi
if [ -n "$similarityParameter" ]; then
    mainCommand+=" -p"
fi
if [ -n "$discard" ]; then
    mainCommand+=" -d $discard"
fi


tmp="tmp.txt"
echo "$mainCommand" >> $tmp

# plot genes distribution
# python3 "$genes_distribution_path" "$inFile"

/usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" $mainCommand > $tmp 2>&1

if [ $? -ne 0 ]; then
    echo "Error running main command"
    cat $tmp
    exit 1
fi

echo "Computing clusters"

cat $tmp

echo "" >> $tmp;
python3 "$net_clug_path" "$inFile" "$outFile.net" >> $tmp

if [ $? -ne 0 ]; then
    echo "Error running netclu_ng.py"
    cat $tmp
    exit 1
fi

clus="$outFile.clus"

grep "F{ " $tmp | sed s/F{\ //g | sed s/}//g | sed s/\ \;//g | sort | uniq > "$clus"



if [ -n "$path2gbks" ]; then
    echo "Converting clusters to json"

    json="$outFile.json"
    python3 "$clus2json_path" "$path2gbks" "$clus" "$json" >> $tmp
    if [ $? -ne 0 ]; then
        echo "Error running clus2json.py"
        cat $tmp
        exit 1
    fi
else 
    
    echo "Missing gbk folder unable to convert clusters to json"
    # echo "Missing gbk folder unable to convert clusters to json" >> $tmp
    # usage
    # usage >> $tmp
fi


rm $tmp

date