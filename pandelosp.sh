#! bin/bash

sdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
sdir=`dirname $sdir`

scripts_path="$sdir/scripts/"

# echo "Using scripts at path: $scripts_path"


gbff2gbk_path="${scripts_path}gbff2gbk.py"
gbk_checker_path="${scripts_path}gbk_checker.py"
faa_checker_path="${scripts_path}faa_checker.py"
calculate_k_path="${scripts_path}calculate_k.py"
genes_distribution_path="${scripts_path}genesDistribution.py"
net_clug_path="${scripts_path}netclu_ng.py"
clus2json_path="${scripts_path}clus2json.py"
gbk2ig_path="${scripts_path}gbk2ig.py"


threadNum=""
inFile=""
outFile=""
mode=""
frags=""
discard=""
path2gbff=""
path2gbks=""
similarityParameter=""

function usage() {
    echo "Usage: $0 [-i input_file] [-o output_file] [-t thread_num] [-m] [-d discard_value] [-g path to gbff folder][-h]"
    echo "Options:"
    echo "  -i: Input file path"
    echo "  -o: Output file path"
    echo "  -t: Number of threads"
    echo "  -m: Enable a different mode"
    echo "  -d: Discard value (0 <= d <= 1, default 0.5)"
    echo "  -g: Path to gbff folder"
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
            path2gbff=$OPTARG
            path2gbks="$path2gbff/gbk/"
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

# Original check for input file
if [ -z "$inFile" ]; then
    echo "Missing input file path"
    usage
    exit 1
fi

# if -g is passed execute a set of operation, if not use the following check
if [ -n "$path2gbff" ]; then

    # check if the directory exists
    if [ ! -d "$path2gbff" ]; then
        echo "Input directory does not exist: $path2gbff"
        exit 1
    fi
    # check if $path2gbff ends with a slash
    if [[ ! $path2gbff =~ /$ ]]; then
    path2gbff="$path2gbff/"
    fi

    # Check if the directory is empty
    if [ -z "$(ls -A $path2gbff)" ]; then
        echo "Input directory is empty: $path2gbff"
        exit 1
    fi

    # GBK processing
    echo "Using files contained in: $path2gbff"

    echo "Converting gbff to gbk"

    # create a directory to store the converted files

    mkdir -p $path2gbks

    python3 "$gbff2gbk_path" "$path2gbff" "$path2gbks"

    if [ $? -ne 0 ]; then
        echo "Error running $gbff2gbk.py"
        exit 1
    fi

    echo "Checking gbk files"
    echo "This may take a while"

    python3 "$gbk_checker_path" "$path2gbks"

    if [ $? -ne 0 ]; then
        echo "Error running $gbk_checker_path.py"
        exit 1
    fi

    echo "Generating pdi input file"
    echo "This may take a while"
    python3 $gbk2ig_path $path2gbks $inFile
fi

echo "$inFile"


if [ -z "$outFile" ]; then
    outFile="$(echo "$(basename $inFile)" | sed 's/\.pdi//').net" 
fi

k=$(python3 $calculate_k_path $inFile)

if [ $? -ne 0 ]; then
    echo "Error running calculate_k.py"
    exit 1
fi

echo "k = $k";


echo "Checking input file (.pdi)"

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

# cat $tmp

echo "" >> $tmp;
python3 "$net_clug_path" "$inFile" "$outFile.net" >> $tmp

if [ $? -ne 0 ]; then
    echo "Error running netclu_ng.py"
    cat $tmp
    exit 1
fi

clus="$outFile.clus"

grep "F{ " $tmp | sed s/F{\ //g | sed s/}//g | sed s/\ \;//g | sort | uniq > "$clus"

json="$outFile.json"

jsonCommand="$clus2json_path $clus $inFile $json"


if [ -n "$path2gbks" ]; then
    echo "Converting clusters to json with GeneBank information"
    jsonCommand="$jsonCommand $path2gbks"
else
    echo "Converting generating json file"
fi


python3 $jsonCommand >> $tmp

if [ $? -ne 0 ]; then
    echo "Error running clus2json.py"
    #cat $tmp
    exit 1
fi

rm "$outFile.net"
rm $tmp

date
