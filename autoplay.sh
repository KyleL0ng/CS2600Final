#!/bin/bash

GAME=("e" "e" "e" "e" "e" "e" "e" "e" "e")

FIRSTPASS=1

mosquitto_sub -h localhost -t 'game/move' | while read LINE; do 
    echo "Recieved"
    LETTER=${LINE:0:1}
    NUM=$((${LINE:1:1}))
    GAME[NUM]=$LETTER
    if [ $LETTER == "X" ] || [ $FIRSTPASS == 1 ]; then
        FIRSTPASS=0
        for i in {0..8}; do
            echo ${GAME[i]}
            if [ ${GAME[i]} == "e" ]; then
                echo "Setting Move"
                MOVE=$i
            fi
        done
        sleep 2
        mosquitto_pub -h localhost -t game/move -m "O$MOVE"
    elif [ $LETTER == "Q" ] || [ $LETTER == "F" ]; then
        exit 0
    fi
done
