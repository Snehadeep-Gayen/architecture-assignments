#!/bin/bash

# Run the cache simulator with different configurations

# Configuration 1
./cache_sim 1024 2 16 0 0 0 gcc_trace.txt > test/output0.txt
diff -iw ref_outputs/gcc.output0.txt test/output0.txt

# Configuration 2
./cache_sim 1024 2 16 16 0 0 gcc_trace.txt > test/output1.txt
diff -iw ref_outputs/gcc.output1.txt test/output1.txt

# Configuration 3
./cache_sim 1024 2 16 0 8192 4 gcc_trace.txt > test/output2.txt
diff -iw ref_outputs/gcc.output2.txt test/output2.txt

# Configuration 4
./cache_sim 1024 2 16 16 8192 4 gcc_trace.txt > test/output3.txt
diff -iw ref_outputs/gcc.output3.txt test/output3.txt

# Configuration 5
./cache_sim 1024 1 16 0 0 0 gcc_trace.txt > test/output4.txt
diff -iw ref_outputs/gcc.output4.txt test/output4.txt

# Configuration 6
./cache_sim 1024 1 16 16 0 0 gcc_trace.txt > test/output5.txt
diff -iw ref_outputs/gcc.output5.txt test/output5.txt

# Configuration 7
./cache_sim 1024 1 16 0 8192 4 gcc_trace.txt > test/output6.txt
diff -iw ref_outputs/gcc.output6.txt test/output6.txt

# Configuration 8
./cache_sim 1024 1 16 16 8192 4 gcc_trace.txt > test/output7.txt
diff -iw ref_outputs/gcc.output7.txt test/output7.txt
