#!/bin/bash

MAX_PROCESSES=8

wait_for_processes() {
  while (( $(jobs -p | wc -l) >= MAX_PROCESSES )); do
    sleep 1
  done
}

for file in spec_execs_train_inputs/*;
do
    (./run_branch_predictors.sh "$file") &
    wait_for_processes
done

wait
