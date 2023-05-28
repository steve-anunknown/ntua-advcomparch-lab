#!/bin/bash

# Assuming the file name is passed as an argument to the script
file_name="$1"

# Extract the base name without the directory path
base_name=$(basename "$file_name")

# Split the base name using the dot as the delimiter
string_part=$(echo "$base_name" | cut -d '.' -f 2-)

# Output the string part
echo "$string_part"_base.amd64-m64-gcc42-nn

