#!/bin/bash

echo -e "********************** Testing modify.sh script **********************\n"

echo "Creating the directory tree"
cd
mkdir Music_test
cd Music_test
mkdir Radiohead
mkdir TheSmiths
mkdir Muse
mkdir JohnFrusciante
mkdir Radiohead/OKComputer
mkdir Radiohead/InRainbows
touch Radiohead/OKComputer/KarmaPolice.mp3
touch Radiohead/OKComputer/NoSurprises.mp3
touch Radiohead/OKComputer/coverphoto.bmp
touch Radiohead/InRainbows/WeirdFishes.mp3
touch Radiohead/InRainbows/Reckoner.mp3
touch Radiohead/InRainbows/Videotape.mp3
touch TheSmiths/Panic.mp3
touch TheSmiths/Ask.mp3
touch TheSmiths/ThisCharmingMan.mp3
touch TheSmiths/readme
touch Muse/Survival.mp3
touch Muse/Bliss.mp3
touch Muse/lyrics.txt
touch JohnFrusciante/Ramparts.mp3
touch JohnFrusciante/LookOn.mp3
touch JohnFrusciante/ThePastRecedes.mp3
touch JohnFrusciante/lyrics
touch list.txt

ls -R 

echo "========================================== Correct usage examples =========================================="

echo "__________________________________________________ TEST 1 __________________________________________________"
echo -e "\n./modify.sh -u list.txt ~/Music_test/Muse/Survival.mp3 ~/Music_test/TheSmiths/Ask.mp3"

../modify.sh -u list.txt /home/$USER/Music_test/Muse/Survival.mp3 /home/$USER/Music_test/TheSmiths/Ask.mp3

echo -e "\n........................................ Directories after TEST 1 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 2 __________________________________________________"
echo -e "\n./modify.sh -l LIST.txt ~/Music_test/Muse/SURVIVAL.mp3"

../modify.sh -l LIST.txt /home/$USER/Music_test/Muse/SURVIVAL.mp3

echo -e "\n........................................ Directories after TEST 2 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 3 __________________________________________________"
echo -e "\n./modify.sh 'y/t/y/' list.txt"

../modify.sh 'y/t/y/' list.txt

echo -e "\n........................................ Directories after TEST 3 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 4 __________________________________________________"
echo -e "\n./modify.sh 'y/s/p/' ~/Music_test/Muse/Bliss.mp3 ~/Music_test/Muse/Survival.mp3"

../modify.sh 'y/s/p/' /home/$USER/Music_test/Muse/Bliss.mp3 /home/$USER/Music_test/Muse/survival.mp3

echo -e "\n........................................ Directories after TEST 4 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 5 __________________________________________________"
echo -e "\n./modify.sh -r -u ~/Music_test/Radiohead"

../modify.sh -r -u Radiohead

echo -e "\n........................................ Directories after TEST 5 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 6 __________________________________________________"
echo -e "\n./modify.sh -ru TheSmiths JohnFrusciante"

../modify.sh -ru TheSmiths JohnFrusciante

echo -e "\n........................................ Directories after TEST 6 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 7 __________________________________________________"
echo -e "\n./modify.sh -rl Radiohead"

../modify.sh -rl Radiohead

echo -e "\n........................................ Directories after TEST 7 ........................................"
ls -R
echo "Press any key to continue."
read pause

echo "__________________________________________________ TEST 8 __________________________________________________"
echo -e "\n./modify.sh -r 'y/e/o/' Radiohead"

../modify.sh -r 'y/e/o/' Radiohead

echo -e "\n........................................ Directories after TEST 8 ........................................"
ls -R
echo "Press any key to continue."
read pause


echo -e "\n========================================== Incorrect usage examples =========================================="
echo -e "\nAn error message and help is displayed after each incorrect usage situation."

echo -e "\n__________________________________________________ TEST 1 __________________________________________________"
echo -e "\n./modify.sh -u -l list.txt"

../modify.sh -u -l list.txt

echo "Press any key to continue."
read pause

echo -e "\n__________________________________________________ TEST 2 __________________________________________________"
echo -e "\n./modify.sh -l TheSmiths"

../modify.sh -l TheSmiths

echo "Press any key to continue."
read pause

echo -e "\n__________________________________________________ TEST 3 __________________________________________________"
echo -e "\n./modify.sh -lr blep" 

../modify.sh -lr blep

echo "Press any key to continue."
read pause

echo -e "\n__________________________________________________ TEST 4 __________________________________________________"
echo -e "\n./modify.sh -r 'y/i/a/' lisy.txt" 

../modify.sh -r 'y/i/a/' lisy.txt

echo "Press any key to continue."
read pause

echo -e "\n__________________________________________________ TEST 5 __________________________________________________"
echo -e "\n./modify.sh -u 'y/i/a/' lisy.txt" 

../modify.sh -u 'y/i/a/' lisy.txt

echo "Press any key to continue."
read pause




