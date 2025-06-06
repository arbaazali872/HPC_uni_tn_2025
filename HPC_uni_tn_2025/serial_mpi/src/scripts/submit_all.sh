#!/bin/bash

# Define the parent directory containing subdirectories
parent_dir="$(pwd)"

# Iterate over each subdirectory
for subdir in "$parent_dir"/*/; do
    # Check if submit_all.sh exists in the subdirectory
    if [[ -f "$subdir/submit_all.sh" ]]; then
        echo "Submitting job from: $subdir"
        # Submit the job using qsub
        qsub "$subdir/submit_all.sh"
    else
        echo "No submit_all.sh found in: $subdir"
    fi
done

