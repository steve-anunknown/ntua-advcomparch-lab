#!/bin/bash

# specify the output file name
output_file="ipc.txt"   

for subdir in */; do
    cd $subdir
    sorted_files=$(ls *.txt | sort -V)
    for file in $sorted_files; do
        # extracts the sixth line of the file
        ipc_line=$(sed '6q;d' $file)   
        # extracts the floating point number from the sixth line
        number=$(echo $ipc_line| grep -oE '0\.[0-9]+')  
        # appends the number to the output file
        echo $number >> $output_file   
    done
    cd ..
done

