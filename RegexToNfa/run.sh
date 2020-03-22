#!/bin/bash
g++ -std=c++17 main.cpp -o main
./main.exe
if [ "$?" -eq "0" ]
then
    echo "Generating graph.."
    dot -Tpng main.dot > graph.png
    echo "Open graph.png file from the current directory!"
else
    echo "Error.. graph could not be generated!"
fi

while true; do
    echo "Press [CTRL + C] to close..."
    sleep 10000
done