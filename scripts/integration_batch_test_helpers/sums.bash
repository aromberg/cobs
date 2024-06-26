#!/bin/bash

# Define directories
dir1="linux_results"
dir2="local_mac_results"

# Output file
output_file="sums.txt"

# Clear the output file if it exists
> "$output_file"

# Variable to track if any hashes differ
hashes_differ=0

# Loop through files in dir1
for file in "$dir1"/*; do
  filename=$(basename "$file")

  # Check if the file exists in dir2
  if [ -e "$dir2/$filename" ]; then
    # Calculate SHA1 sums
    shasum1=$(shasum "$file" | awk '{ print $1 }')
    shasum2=$(shasum "$dir2/$filename" | awk '{ print $1 }')

    # Output results to file
    echo "$filename" >> "$output_file"
    echo "  $dir1: $shasum1" >> "$output_file"
    echo "  $dir2: $shasum2" >> "$output_file"

    # Compare hashes
    if [ "$shasum1" != "$shasum2" ]; then
      echo "  WARNING: Hashes do not match!" >> "$output_file"
      hashes_differ=1
    fi

    echo "" >> "$output_file"
  fi
done

# Inform the user if any hashes differ
if [ $hashes_differ -eq 1 ]; then
  echo "Some hashes do not match. See $output_file for details."
else
  echo "All hashes match. See $output_file for details."
fi