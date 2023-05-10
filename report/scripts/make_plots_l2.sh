#!/bin/bash

# Loop over all subdirectories of the current directory
for dir in */; do
    # Extract the directory name without the trailing slash
    dirname=${dir%/}
    # Get a list of all files in the current subdirectory that match the pattern
    #files=$(ls ${dirname}/exp2-${dirname}*.txt | sort -V)
    files=($(find "${dirname}" -maxdepth 1 -type f -name "exp2-${dirname}*.txt" | sort -V))
    # Check if any files match the pattern
    if [ ${#files[@]} -gt 0 ]; then
        # Execute the Python script with all files in the current subdirectory as input
        ./plot_l2.sh "${files[@]}"
        mv L2.png ${dirname}/.
    fi
done

