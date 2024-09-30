sizes=("1024" "2048" "4096" "8192" "16384" "32768" "65536" "131072" "262144" "524288" "1048576")
assocs=("1" "2" "4" "8" "1000000000000")

# sizes=("1024")
# assocs=("2")

for size in "${sizes[@]}"
do
    for assoc in "${assocs[@]}"
    do
        if [ "$size" -lt "$assoc" ]; then
            assoc=$(echo $size / 32 | bc)
        fi
        # echo $assoc

        ../../../build/cachesim $size $assoc 32 0 0 0 /home/snehadeep/Documents/comp-arch/architecture-assignments/project1_cachesim/Assignment1/solutions/build/gcc_trace.txt | grep 'average access time' | awk '{printf "%s", $(NF)}'
        echo -n ','
    done
    echo ''
done