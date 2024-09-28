#!/bin/bash

# L1 cache sizes in bytes
l1_sizes=(1024 2048 4096 8192 16384 32768)

# Configurations
configs=(
    "Direct-mapped L1 cache with no Victim Cache"
    "Direct-mapped L1 cache with 4-entry Victim Cache"
    "Direct-mapped L1 cache with 8-entry Victim Cache"
    "Direct-mapped L1 cache with 16-entry Victim Cache"
    "2-way set-associative L1 cache with no Victim Cache"
    "4-way set-associative L1 cache with no Victim Cache"
)

# Victim cache entries for configurations that use it
victim_cache_entries=(0 4 8 16 0 0)

# L1 associativity for each configuration
l1_assoc=(1 1 1 1 2 4)

# Output CSV file
output_file="data.csv"

# Write header to CSV file
echo "l1_size,configuration,aat" > $output_file

# Paths (adjust these paths according to your setup)
cachesim_path="../../../build/cachesim"
trace_file="/home/snehadeep/Documents/comp-arch/architecture-assignments/project1_cachesim/Assignment1/solutions/build/gcc_trace.txt"

# Block size in bytes (assuming 32-byte blocks)
block_size=32

# Loop over L1 sizes
for l1_size in "${l1_sizes[@]}"
do
    # Loop over each configuration
    for idx in "${!configs[@]}"
    do
        config="${configs[$idx]}"
        victim_entries="${victim_cache_entries[$idx]}"
        assoc="${l1_assoc[$idx]}"
        
        # Build the command
        cmd="$cachesim_path $l1_size $assoc $block_size $victim_entries 0 0 $trace_file"

        # echo $cmd
        
        # Run the command and extract the AAT
        aat=$($cmd | grep 'average access time' | awk '{print $(NF)}')
        
        # Append data to CSV
        echo -n "$aat,"
    done
    echo ''
done
