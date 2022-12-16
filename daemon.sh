#!/bin/bash

#author: Kyle Long
#CS 2600 Final Project
#16 December 2022
#This script will run as a daemon in the background, and will automatically start autoplay.sh if nobody is at the laptop

RUN=1   #makes sure program continually loops
HUMAN=0 #keeps track of whether a human is at the laptop
LINE="" #holds moves/messages that come in
SEND="" #holds the message to publish for the script to recieve

while [ $RUN == 1 ]; do #always runs
    echo "Waiting for start"
    LINE=$(mosquitto_sub -h localhost -t 'game/move' -C 1) #takes the next line from the topic
    if [ $? == 0 ]; then #if the subscribe was successful
        if [ ${LINE:0:1} == "S" ]; then #S is sent when a game is started
            echo "Game started"
            LINE=$(mosquitto_sub -h localhost -t 'game/move' -C 1) #takes the next line from the topic
            if [ ${LINE:0:1} == "X" ]; then
                SEND=$LINE #holds the first move made to send it to autoplay.sh
                for i in {1..15}; do #do this (1 second) subscribe 15 times
                    LINE=$(mosquitto_sub -h localhost -t 'game/move' -W 1 -C 1) #looks for the next line from the topic (for 1 sec)
                    if [ $? == 0 ]; then #if the subscribe was successful
                        if [ ${LINE:0:1} == "O" ]; then #if an O move was made
                            echo "Human Present"
                            HUMAN=1 #there is a human at the laptop
                            break
                        fi
                    fi
                done
                if [ $HUMAN == 0 ]; then #if there is no human
                    echo "Auto"
                    ./autoplay.sh & #start the autoplay script
                    sleep 1
                    mosquitto_pub -h localhost -t game/move -m "$SEND" #send SEND (the first move made) as a message so the script
                    echo "Autoplay script started"
                fi
            fi
        fi
    fi
done
