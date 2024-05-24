#!/bin/bash
ilist="$1"
folder="$2"

rm -rf "./$folder"
mkdir -p $folder

cat $ilist
echo ""

i=0
python3 ./download.py $ilist | while read line;
do
[ -z "$line" ] && continue
arrLine=(${line//;/ })
url=${arrLine[0]}
prefix=${arrLine[1]}

echo "curl for $url --output $folder/$prefix.gz"
curl "$url" --output "$folder/$prefix.gz"
gzip -d "$folder/$prefix.gz"

((i=i+1))
echo "Converted: $i"
done
