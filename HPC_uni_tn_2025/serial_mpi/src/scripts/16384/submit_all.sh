#!/bin/bash
for cores in 1; do
    script="core_${cores}.sh"
    if [[ -f "$script" ]]; then
        echo "Submitting $script..."
        qsub "$script"
    else
        echo "Script $script not found!"
    fi
done
