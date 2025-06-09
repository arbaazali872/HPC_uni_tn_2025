#!/bin/bash

for cores in 1 2 4 8 16 32; do
    script="2048_core_${cores}.sh"
    if [[ -f "$script" ]]; then
        echo "Submitting $script..."
        qsub "$script"
    else
        echo "Script $script not found!"
    fi
done
