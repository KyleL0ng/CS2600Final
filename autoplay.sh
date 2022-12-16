#!/bin/bash

#author: Kyle Long
#CS 2600 Final Project
#16 December 2022
#This script will automatically make moves into the tic-tac-toe game. It is run by daemon.sh, which is a daemon.

GAME=("e" "e" "e" "e" "e" "e" "e" "e" "e") #array used to store the game and make sure that moves are valid

FIRSTPASS=1 #variable used to make sure the script will make a move after the first move

mosquitto_sub -h localhost -t 'game/move' | while read LINE; do #constantly reads messages from the mosquitto topic
    echo "Recieved"
    LETTER=${LINE:0:1}   #takes the first value in the message
    NUM=$((${LINE:1:1})) #takes the second value in the message as a number
    GAME[NUM]=$LETTER    #sets the correct value of the array to the correct letter (so it does make the same move later)
    if [ $LETTER == "X" ] || [ $FIRSTPASS == 1 ]; then
        FIRSTPASS=0 #makes sure that this if statement is only entered on the first pass
        for i in {0..8}; do
            echo ${GAME[i]}
            if [ ${GAME[i]} == "e" ]; then
                echo "Setting Move"
                MOVE=$i
            fi
        done #loop gets the number of the last open square available
        sleep 2 #wait 2 seconds to make sure that the C program is caught up
        mosquitto_pub -h localhost -t game/move -m "O$MOVE" #publishes the move with an O
    elif [ $LETTER == "Q" ] || [ $LETTER == "F" ]; then
        exit 0 #program will exit if the message's letter is Q (sent when player quits the game)
    fi
done
