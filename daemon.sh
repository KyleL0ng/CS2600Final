#!/bin/bash

RUN=1
HUMAN=0
LINE=""
SEND=""

while [ $RUN == 1 ]; do
    echo "Waiting for start"
    LINE=$(mosquitto_sub -h localhost -t 'game/move' -C 1)
    if [ $? == 0 ]; then
        if [ ${LINE:0:1} == "S" ]; then
            SEND=$LINE
            echo "Game started"
            LINE=$(mosquitto_sub -h localhost -t 'game/move' -C 1)
            if [ ${LINE:0:1} == "X" ]; then
                for i in {1..15}; do
                    LINE=$(mosquitto_sub -h localhost -t 'game/move' -W 1 -C 1)
                    if [ $? == 0 ]; then
                        if [ ${LINE:0:1} == "O" ]; then
                            echo "Human Present"
                            HUMAN=1
                            break
                        fi
                    fi
                done
                if [ $HUMAN == 0 ]; then
                    echo "Auto"
                    ./autoplay.sh &
                    sleep 1
                    mosquitto_pub -h localhost -t game/move -m "$SEND"
                    echo "Autoplay script started"
                fi
            fi
        fi
    fi
done
