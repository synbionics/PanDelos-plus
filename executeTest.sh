#! bin/bash

infile="$1"
statsFile="$2"
scalFile="$3"

calculate_k_path="./tools/calculate_k.py" 

# echo "$infile"

#k=5 
k=$(python3 $calculate_k_path $infile)
echo "k = $k";

outFile=$(echo "$(basename $infile)" | sed 's/\.faa//')


echo "$outFile" >> $scalFile

outFile+="_k$k"


mkdir trash
tmpfile="tmp.txt"

./mainStats -i "$infile"> $statsFile 2>&1



echo "thread,time,memory" >> $scalFile

# legion
for i in {8..8}
do
    #echo "" > $tmpfile
    outFile+="_t$i"

    /usr/bin/time -f "time(seconds): %e user time(seconds): %U memory(KB): %M" ./main -i "$infile" -o trash/"$outFile" -k "$k" -t "$i"> $tmpfile 2>&1
    echo "Test done, threads: $i"

    t=$(awk '/time\(seconds\)/ {print $2}' $tmpfile)
    m=$(awk '/memory\(KB\)/ {print $7}' $tmpfile)
    
    echo "$i,$t,$m" >> $scalFile
    
    echo "" >> $statsFile
    echo "number of threads: $i" >> $statsFile
    cat $tmpfile >> $statsFile
    rm $tmpfile
    sleep 10
done

rm -rf trash

echo "" >> $statsFile
lscpu >> $statsFile
