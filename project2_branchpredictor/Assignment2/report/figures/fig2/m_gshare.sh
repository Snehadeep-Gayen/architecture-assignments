#!/bin/bash

# Define the range for m and the even values for n
min_m=7
max_m=12

# Traces for benchmarks
gcc_trace="../traces/gcc_trace.txt"
jpeg_trace="../traces/jpeg_trace.txt"

# Output files for the misprediction rates
gcc_output="gcc_misprediction.txt"
jpeg_output="jpeg_misprediction.txt"

# Clear output files before appending results
> $gcc_output
> $jpeg_output

# Function to simulate for a given benchmark
simulate() {
    local benchmark=$1
    local output_file=$2
    
    # Loop over even n values
    for n in 2 4 6 8 10 12; do
        # Loop over valid m values (m >= n)
        for (( m = min_m; m <= max_m; m++ )); do
            # m should be atleast n
            if (( m < n )); then
                continue
            fi
            echo "Simulating for m=$m, n=$n on $benchmark"
            # Run the simulation and extract the misprediction rate
            misprediction_rate=$(../bpsim gshare $m $n $benchmark | grep "misprediction rate" | awk -F': ' '{printf $2}' | awk -F'%' '{printf $1}')
            echo "$m $n $misprediction_rate" >> $output_file
        done
    done
}

# Simulate for gcc_trace
simulate $gcc_trace $gcc_output

# Simulate for jpeg_trace
simulate $jpeg_trace $jpeg_output

echo "Simulation complete! Results stored in gcc_misprediction.txt and jpeg_misprediction.txt"
