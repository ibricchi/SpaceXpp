#!/bin/bash

coproc esp { python ./test.py ;}

while :
do
    read output <&${esp[0]}
    while [[ "${output}" != "<data>"* ]]
    do
        read output <&${esp[0]}
    done
    read output <&${esp[0]}

    output=$(echo $output | python ./communicator.py)

    echo $output
done