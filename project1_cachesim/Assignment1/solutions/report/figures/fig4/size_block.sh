sizes=("1024" "2048" "4096" "8192" "16384" "32768")
blksizes=("16" "32" "64" "128")

for size in "${sizes[@]}"
do
    for blksize in "${blksizes[@]}"
    do
        ../../../build/cachesim $size 4 $blksize 0 0 0 /home/snehadeep/Documents/comp-arch/architecture-assignments/project1_cachesim/Assignment1/solutions/build/gcc_trace.txt | grep 'L1+VC' | awk '{printf "%s", $(NF)}'
        echo -n ','
    done
    echo ''
done