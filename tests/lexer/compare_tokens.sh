#!/bin/bash

# Find all .jack files in the current directory
for jackfile in *.jack; do
    # Check if file exists (in case no .jack files were found)
    if [ -f "$jackfile" ]; then
        output_file="${jackfile}_tokens_2.txt"
        reference_file="${jackfile}_tokens.txt"
        
        echo "Processing $jackfile -> $output_file"
        
        # Run lexer and redirect output to the corresponding output file
        ./lexer "$jackfile" > "$output_file"
        
        # Check if the lexer command succeeded
        if [ $? -eq 0 ]; then
            echo "Successfully processed $jackfile"
            
            # Check if reference file exists for comparison
            if [ -f "$reference_file" ]; then
                echo "Comparing with reference file $reference_file"
                
                # Compare the files and display differences
                diff_output=$(diff -u "$reference_file" "$output_file")
                
                if [ -z "$diff_output" ]; then
                    echo "✅ Files are identical!"
                else
                    echo "❌ Differences found:"
                    echo "$diff_output"
                fi
            else
                echo "No reference file found for comparison."
            fi
        else
            echo "Error processing $jackfile"
        fi
        
        echo "------------------------"
    fi
done

echo "All .jack files have been processed."
