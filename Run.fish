#!/bin/fish
echo -e "\033[33m"
echo "AirClassroomDownload  Copyright (C) 2023  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"
cmake -B build
cmake --build build
set Username (cat Username)
set Password (cat Password)
read grade -P "Please input the grade (e.g. 9): "
read semester -P "Please input the semester (0 for first semester, 1 for second semester): "
read subject -P "Please input the subject (e.g. 英语): "
./build/main -u $Username -p $Password -g $grade -s $semester -sj $subject
