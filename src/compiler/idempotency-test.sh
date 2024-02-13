#!/bin/bash

# Prepare the compiler
make clean && make > /dev/null 2>&1


# Read flag value
# credits to: https://stackoverflow.com/questions/14447406/bash-shell-script-check-for-a-flag-and-grab-its-value

STEP="mips"

if [ $# -ne 2 ]; then
   echo "Provide flag (s) with appropriate value (scanner, parser, symbol, ir, mips)";
fi


if [ "$1" != "-s" ]; then
  echo "Provide correct flag (s) with appropriate value (scanner, parser, symbol, ir, mips)";
fi

STEP=$2

echo "**** Running the compiler in $STEP mode ****"


# # Directory containing the files
input_dir="tests/$STEP/input"

command="./compiler -s $STEP"

# Create a temporary file for the command output
temp_output="temp_output.txt"

# Loop through files in the input directory
for file in "$input_dir"/*; do
  if [ -f "$file" ]; then

    if [ "$STEP" == "parser" ]; then
      # Apply the command to the current file and store the output in temp_output
      $command "$file" > "$temp_output"
      if [ "$file" == "tests/parser/input/decl-order.c" ]; then
        # We want this file to give an error
        continue
      fi
      
      # Copy the output to test.c
      cp "$temp_output" "test.c"
      
      # Execute the command on test.c and store the output in a variable
      output=$(eval "$command test.c")

      # Compile output with gcc -pedantic flag and check if you get any errors
      gcc -pedantic test.c > /dev/null 2>&1

      if [ $? -ne 0 ]; then
        echo "Error with pendantic flag: $file 🛑"
      fi

      # Compare the output with the content of test.c
      if [ "$output" != "$(cat test.c)" ]; then
        echo  "Output does not match for file: $file ❌"
      else
        echo "Output matches for file: $file ✅"
      fi

    fi

    
    if [ "$STEP" == "symbol" ]; then
      $command "$file" > "$temp_output"

      expected_file=${file/"input"/"expected"}

      if [ "$(cat $expected_file)" != "$(cat $temp_output)" ]; then
        echo  "Output does not match for file: $file ❌"
      else
        echo "Output matches for file: $file ✅"
      fi
    fi

    
    if [ "$STEP" == "type" ]; then
      $command "$file" > "$temp_output"

      expected_file=${file/"input"/"expected"}

      if [ "$(cat $expected_file)" != "$(cat $temp_output)" ]; then
        echo  "Output does not match for file: $file ❌"
      else
        echo "Output matches for file: $file ✅"
      fi
    fi


    if [ "$STEP" == "ir" ]; then
      $command "$file" > "$temp_output"

      expected_file=${file/"input"/"expected"}

      if [ "$(cat $expected_file)" != "$(cat $temp_output)" ]; then
        echo  "Output does not match for file: $file ❌"
      else
        echo "Output matches for file: $file ✅"
      fi
    fi

    if [ "$STEP" == "mips" ]; then
      $command "$file" > "$temp_output"

      expected_file=${file/"input"/"expected"}

      if [ "$(cat $expected_file)" != "$(cat $temp_output)" ]; then
        echo  "Output does not match for file: $file ❌"
      else
        echo "Output matches for file: $file ✅"
      fi
    fi

    # Clean up the temporary output file
    rm "$temp_output"

  fi
done
