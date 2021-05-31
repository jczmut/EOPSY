#!/bin/bash
# shell script for testing the 'copy' program

echo -e "********************** Testing copy.c program **********************\n"

echo -e "\nShowing help information:\n\t./copy -h"
./copy -h

echo -e "\n____________________ Correct copying examples ____________________\n"

echo "Press any key to start the testing."
read pause

echo -e "Copying a small file 'test1' using read & write method to an existing destination file 'test1copy':\n\t./copy tests/test1 test1copy\n"
./copy tests/test1 test1copy
echo "Press any key to continue."
read pause

echo -e "\nCopying a small text file 'test2_suspirium.txt' using memory mapping method to an existing destination file 'test2copy.txt':\n\t./copy tests/test2_suspirium test2copy\n"
./copy -m tests/test2_suspirium.txt test2copy.txt
echo "Press any key to continue."
read pause

echo -e "\nCopying a small text file 'test2_thesmiths.txt' using memory mapping method to a non-existing destination file 'test3copy.txt':\n\t./copy tests/test3_thesmiths.txt test3copy.txt\n"
./copy -m tests/test3_thesmiths.txt test3copy.txt
echo "Press any key to continue."
read pause

echo -e "\nCopying an mp3 file 'red_hot_chili_peppers-hey.mp3' using memory mapping method to a non-existing destination file 'hey_copy.mp3':\n\t./copy tests/red_hot_chilli_peppers/red_hot_chili_peppers-hey.mp3 hey_copy.mp3\n"
./copy -m tests/red_hot_chilli_peppers/red_hot_chili_peppers-hey.mp3 hey_copy.mp3
echo "Press any key to continue."
read pause

echo -e "\nCopying an mp3 file 'red_hot_chili_peppers-stadium_arcadium.mp3' using read & write method to a non-existing destination file 'stadium_arcadium_copy.mp3':\n\t./copy tests/red_hot_chilli_peppers/red_hot_chili_peppers-stadium_arcadium.mp3 stadium_arcadium_copy.mp3\n"
./copy tests/red_hot_chilli_peppers/red_hot_chili_peppers-stadium_arcadium.mp3 stadium_arcadium_copy.mp3
echo "Press any key to continue."
read pause


echo -e "\n____________________ Incorrect copying examples ____________________\n"

echo -e "Trying to copy a non-exiting file using read & write method:\n\t./copy tests/some_file some_file_copy"
./copy tests/some_file some_file_copy
echo "Press any key to continue."
read pause

echo -e "\nTrying to copy a file 'test1 to a non-existing directory using the memory map method:\n\t./copy tests/test1 non_existing_directory/test1_copy"
./copy -m tests/test1 non_existing_directory/test1_copy

echo -e "\nTests finished."
