#!/bin/bash

for cores in 2 4 8 16 32; do
    script="4096_core_${cores}.sh"
    if [[ -f "$script" ]]; then
        echo "Submitting $script..."
        qsub "$script"
    else
        echo "Script $script not found!"
    fi
done
