l1sizes=("1024" "2048" "4096" "8192" "16384" "32768" "65536")
l2sizes=("32768" "65536" "131072" "262144" "524288" "1048576")

for l1sz in "${l1sizes[@]}"
do
    for l2sz in "${l2sizes[@]}"
    do
        if [ "$l1sz" -lt "$l2sz" ]; then
            assoc=$(echo $size / 32 | bc)
            echo -n "$l1sz,$l2sz,"
            ../../../build/cachesim $l1sz 4 32 0 $l2sz 8 /home/snehadeep/Documents/comp-arch/architecture-assignments/project1_cachesim/Assignment1/solutions/build/gcc_trace.txt | grep 'average access time' | awk '{printf "%s", $(NF)}'    
        echo ''
        fi
    done
done