#!/bin/bash

# Prepare the compiler
make clean && make > /dev/null 2>&1

echo "Are you sure you want to overwrite IR tests? (y/n)"
read answer

if [ "$answer" = "y" ]; then
    echo "**** Overwriting tests ****"
    command="./compiler -s ir"
    input_dir="tests/ir/input"
    # Loop through files in the input directory
    for file in "$input_dir"/*; do
      if [ -f "$file" ]; then
      expected_file=${file/"input"/"expected"}
      $command "$file" > "$expected_file"
      fi
    done
else
    echo "Exiting..."
fi
