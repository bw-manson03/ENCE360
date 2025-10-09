#!/bin/bash

PROGRAMS=("serial" "thread" "process" "processThread")
TESTFILES=("1inputLight.txt" "10inputLight.txt" "1inputHeavy.txt" "10inputHeavy.txt")
OUTPUT="results_avg.csv"

# Header
echo "Program,TestFile,ElapsedTime(s)" > $OUTPUT

for prog in "${PROGRAMS[@]}"; do
    for file in "${TESTFILES[@]}"; do
        total=0
        runs=10
        for ((i=1; i<=runs; i++)); do
            # Use /usr/bin/time to get elapsed seconds only
            t=$(/usr/bin/time -f "%e" ./$prog < testCaseFiles/$file 2>&1 >/dev/null)
            total=$(echo "$total + $t" | bc -l)
        done
        avg=$(echo "scale=5; $total / $runs" | bc -l)
        echo "$prog,$file,$avg" >> $OUTPUT
    done
done
