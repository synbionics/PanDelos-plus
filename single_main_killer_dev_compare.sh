#compilazione cpp

sdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
sdir=`dirname $sdir`

scripts_path="$sdir/scripts/"

echo "skipping main with salmonella 100"

#echo "Compiling cpp serial (O3, can be done in advance)"
#g++ -O3 $scripts_path/serial_netclu_ng.cc ./lib/Graph.hh ./lib/Umbrella_algo.hh -o $scripts_path/serial_net

echo "Compiling cpp parallel hybrid (O3, can be done in advance)"
#g++ -O3 $scripts_path/openmp_netclu.cc ./lib/UmbrThreadPool.hh ./lib/Graph.hh ./lib/Umbrella_algo.hh -fopenmp -o $scripts_path/omp_parallel_net
g++ -O3 $scripts_path/hybrid_final_netclu.cc ./lib/UmbrThreadPool.hh ./lib/Graph.hh ./lib/kahan_v4_umb_algo.hh -o $scripts_path/parallel_net

inFile="files/pdi/salmonella_100.pdi"
outFile="salmonella_100"

#echo "Computing clusters (cpp) (serial)"
#start_cpp=$(date +%s.%N)
#"$scripts_path/serial_net" "$inFile" "$outFile.net" > "cpp_result.txt"
#end_cpp=$(date +%s.%N)
#cpp_time=$(echo "$end_cpp - $start_cpp" | bc)
#cpp_time_fmt=$(LC_NUMERIC=C printf "%.2f" "$cpp_time")

#echo "tempo esecuzione c++ (seriale): $cpp_time_fmt secondi"

echo "Computing clusters (cpp) (parallel hybrid)"
start_cpp_parall=$(date +%s.%N)
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes "$scripts_path/parallel_net" "$inFile" "$outFile.net" > parallel_cpp_result.txt
"$scripts_path/parallel_net" "$inFile" "$outFile.net" > parallel_cpp_result.txt
end_cpp_parall=$(date +%s.%N)
cpp_time_parall=$(echo "$end_cpp_parall - $start_cpp_parall" | bc)
cpp_time_fmt_parall=$(LC_NUMERIC=C printf "%.2f" "$cpp_time_parall")

echo "tempo esecuzione c++ (parallelo): $cpp_time_fmt_parall secondi"

echo "✅Clustering eseguito"

#echo "tempo esecuzione python: $python_time_fmt secondi"

#speedup_cpp=$(echo "$cpp_time_fmt / $cpp_time_fmt_parall" | bc -l)

#echo "speedup: $speedup_cpp"