ms=("7" "8" "9" "10" "11" "12")
traces=("gcc" "jpeg")

echo m,
for trace in "${traces[@]}"
do 
   echo -n $trace, 
done
echo 

for m in "${ms[@]}"
do
    echo -n $m,
    for trace in "${traces[@]}"
    do
        ../bpsim bimodal $m ../traces/$trace\_trace.txt | grep 'misprediction rate' | awk -F': ' '{printf $2}' | awk -F'%' '{printf $1}'
        echo -n ','
    done
    echo ''
done