#!/bin/bash

# this is a bash script that executes the
# plot_mpki_ipc.py script for all the
# files in the outputs/spec_execs_train_inputs/${folder} folder
# and saves the plots in the outputs/spec_execs_train_plots/${folder} folder

# the script takes as input the folder name
# and executes the plot_mpki_ipc.py script for all the files in the
# outputs/spec_execs_train_inputs/${folder} folder
# and saves the plots in the outputs/spec_execs_train_plots/${folder} folder

for file in $1/*;
do
    python plot_mpki_ipc.py "$file" $1/$(basename "$file") <<< $(basename "$file").png
done

mkdir -p $1/plots
mv *.png $1/plots/.