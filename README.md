# EOPSY
Operating Systems projects (6th semester)

The script 'modify' is dedicated to lowerizing, uppercasing file names or modifying them using the sed command.
Changes may be done either with recursion or without it.

Options:
        -r              envokes recursion
        -l|-u           lowercase or uppercase
        <sed pattern>   sed pattern
        -h              shows help

Syntax options:
        modify.sh [-l|-u] <file name>
        modify.sh [-r] [-l|-u] <directory name>
        modify.sh [-rl|-lr|-ru|-ur] <directory name>
        modify.sh <sed pattern> <file name>
        modify.sh [-r] <sed pattern> <directory name>
        modify.sh [-h]
        
The script 'modify_examples' creates a test directory and performs a series of tests presenting the functionalities of the 'modify' script
