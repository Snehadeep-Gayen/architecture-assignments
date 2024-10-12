#!/bin/bash

# Define the paths to the trace files and expected output files
trace_dir="../../Assignment_files"
output_dir="output.txt"
expected_dir="$trace_dir/val_outputs"

# Array of test configurations: (sim_type param1 param2 trace expected_output)
declare -a tests=(
    "bimodal 6 gcc_trace.txt $expected_dir/bimodal_gcc_val0.txt"
    "bimodal 12 gcc_trace.txt $expected_dir/bimodal_gcc_val1.txt"
    "bimodal 4 jpeg_trace.txt $expected_dir/bimodal_jpeg_val2.txt"
    "gshare 11 5 jpeg_trace.txt $expected_dir/gshare_jpeg_val2.txt"
    "gshare 9 3 gcc_trace.txt $expected_dir/gshare_gcc_val0.txt"
    "gshare 14 8 gcc_trace.txt $expected_dir/gshare_gcc_val1.txt"
)

# Loop through each test case
for test in "${tests[@]}"; do
    # Split the test configuration into an array
    test_config=($test)

    # Determine if the test is bimodal or gshare and adjust parameters accordingly
    if [ "${test_config[0]}" == "bimodal" ]; then
        # Run the simulation for bimodal
        ./bpsim bimodal ${test_config[1]} ${test_config[2]} > $output_dir
        
        # Compare the output with the expected file (4th index)
        expected_output=${test_config[3]}
    else
        # Run the simulation for gshare
        ./bpsim gshare ${test_config[1]} ${test_config[2]} ${test_config[3]} > $output_dir
        
        # Compare the output with the expected file (5th index)
        expected_output=${test_config[4]}
    fi

    # Compare output with expected
    diff -iw $output_dir $expected_output
    if [ $? -eq 0 ]; then
        echo "Test passed: ${test_config[@]}"
    else
        echo "Test failed: ${test_config[@]}"
    fi
done
