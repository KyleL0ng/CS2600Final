#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <mosquitto.h>
#include <unistd.h>
#include <poll.h>

// author: Kyle Long
// CS 2600 Final Project
// 16 December 2022
// Tic Tac Toe game in between 2 players (ESP32 and Laptop/Script). This code holds the logic for the game and the laptop player, and communicates with the ESP32 through mqtt.

void intro();                                                                                     // prints the intro for the game
bool connectToServer();                                                                           // connects to mosquitto and subscribes to the topic
void printBoard(char state[]);                                                                    // prints the current status of the board
char determineXO(int turns);                                                                      // determines whether the correct game token is an X or an O
bool checkRange(int sel);                                                                         // checks whether the number is within 0-8
bool checkValidity(int selection, char state[]);                                                  // checks whether the specified space is empty
bool getMove(char state[]);                                                                       // gets a move from the mqtt server
bool makeMove();                                                                                  // prompts the player to make a move, and publishes that move to the server
bool kbdIn();                                                                                     // gets the move from the keyboard, or is interrupted by the script
bool winTest(char state[]);                                                                       // tests whether there is a winning configuration on the board
void messageRecieved(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message); // called when a message is recieved, and stores the message proprely

struct mosquitto *mosq;                                  // required for mosquitto
struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI}; // used to poll the user for their move
bool recieved;                                           // tracks whether a message was recieved or not
int sel;                                                 // stores the number of the square when a move is made
char letter;                                             // stores the letter from a message
bool waiting = false;                                    // tracks whether we are waiting for input from Player2
char move[3];                                            // stores the move from Player2
bool gameOver = false;                                   // tracks whether the game is over

int main()
{
    char state[9]; // array used to store the game state
    state[0] = ' ';
    state[1] = ' ';
    state[2] = ' ';
    state[3] = ' ';
    state[4] = ' ';
    state[5] = ' ';
    state[6] = ' ';
    state[7] = ' ';
    state[8] = ' ';
    int turns;      // stores the number of turns
    bool connected; // tracks whether mosquitto connected

    mosquitto_lib_init();                                  // initial method
    connected = connectToServer();                         // attempt to connect to the mosquitto server, and stores whether it was successful
    mosquitto_message_callback_set(mosq, messageRecieved); // sets the callback to messageRecieved()

    if (!connected) // if not connected
    {
        return EXIT_FAILURE; // terminate program and return EXIT_FAILURE
    }                        // end if

    intro(); // print intro

    printBoard(state); // print current status of the board
    printf("Waiting for Player1 to start the game\n");
    turns = 0;            // there have been no turns yet
    gameOver = false;     // the game is not over
    bool player2 = false; // tracks whether it is Player2's turn
    bool pending = false; // tracks whether we are waiting for a move from Player2

    // Plays out game for 9 turns, and stops if the game is over
    while (turns < 9 && !gameOver)
    {

        if (player2 && !pending) // if it is Player2's turn and we are still waiting for a move from them
        {
            pending = makeMove(); // Player2 makes their move (either with keyboard or script)
        } //end if

        if (getMove(state)) // if a move is recieved from mqtt
        {
            state[sel] = letter; // Sets token in gameboard space
            printBoard(state);   // prints board
            turns++;             // adds 1 to the turns, since another turn has been made
            if (player2)         // if it was just Player2's turn
            {
                player2 = false; // it is not Player2's turn next
            }
            else // if it was not just Player2's turn
            {
                player2 = true; // it is Player2's turn next
            } //end if
            pending = false;           // the program is not waiting for a move from Player2
            gameOver = winTest(state); // checks to see if there is a winner
            if (gameOver)              // if the game is over
            {
                mosquitto_publish(mosq, NULL, "game/move", 2, "F", 0, false); // publish an F for finished
                if (determineXO(turns - 1) == 'X') // if X won
                {
                    printf("Player 1 wins!\n");
                }
                else // if O won
                {
                    printf("Player 2 wins!\n");
                } // end if
            }
            if (turns == 9 && !gameOver) // if there was no winner, and 9 moves have been made (board is full)
            {
                mosquitto_publish(mosq, NULL, "game/move", 2, "F", 0, false); // publish an F for finished
                printf("Tie game!\n");
            } // end if
            recieved = false;        // false because the program is going to recieve another message
        } // end if
        mosquitto_loop(mosq, -1, 1); // makes sure mosquitto can have some time to recieve messages
    } // end while

    printf("Game Over!");

    return EXIT_SUCCESS;
} // end main

void intro()
{
    printf("===========================\n");
    printf("WELCOME TO TIC TAC TOE!\n");
    printf("===========================\n");
} // end intro

bool connectToServer()
{
    mosq = mosquitto_new(NULL, true, NULL); // initializes the instance of mosquitto

    int rtn = mosquitto_connect(mosq, "localhost", 1883, 30); // stores whether the connection to mosquitto was successful

    if (rtn != MOSQ_ERR_SUCCESS) // if the connection was not successful
    {
        printf("Error connecting");
        return false;
    } // end if

    rtn = mosquitto_subscribe(mosq, NULL, "game/move", 0); // stores whether the subscription to the topic was successful

    if (rtn != MOSQ_ERR_SUCCESS) // if the subscription was not successful
    {
        printf("Error subscribing");
        return false;
    } // end if

    return true;
} // end connectToServer

void printBoard(char state[])
{
    printf("The current status is:\n");
    printf("+-------------+\n | %c | %c | %c |\n+-------------+\n | %c | %c | %c |\n+-------------+\n | %c | %c | %c |\n+-------------+\n", state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7], state[8]);
} // end printBoard

char determineXO(int turns)
{
    char player; // tracks whether to add an X or O

    if (turns % 2 == 0) // if the number of turns is even
    {
        player = 'X';
    }
    else // if it is odd
    {
        player = 'O';
    } // end if

    return player;
} // end determineXO

bool checkRange(int sel)
{
    bool test = false; // tracks whether the number of the space is within the bounds of the array

    if (sel >= 0 && sel <= 8) // if the number is within 0-8
    {
        test = true;
    } // end if

    return test;
} // end checkRange

bool checkValidity(int selection, char state[])
{
    bool test = false; // tracks whether the space is empty

    if (state[selection] == ' ') // if the specified space is empty
    {
        test = true;
    } // end if

    return test;
} // end checkValdidity

bool getMove(char state[])
{
    if (!recieved) // if a message was not recieved
    {
        usleep(1000000); // wait 1 second
        return false;
    } // end if

    if (!checkRange(sel)) // makes sure that the selection is within 0-8
    {
        printf("invalid move, not in range");
        return false;
    } // end if

    if (!checkValidity(sel, state)) // makes sure the desired space is empty before replacing the current token
    {
        printf("invalid move, space already taken");
        return false;
    } // end if

    if (letter != 'X' && letter != 'O') // if the letter in the message was not an X or an O
    {
        return false;
    } // end if

    return true;
} // end getMove

bool makeMove()
{
    int result; // tracks whether the mosquitto publish was successful

    waiting = true; // true because we are waiting from a move from Player2

    printf("Player2: make your move (1-9):\n");
    if (kbdIn()) // if there was input from the keyboard (might not be because of the script)
    {
        fflush(stdin);

        if (move[1] == 'Q')
        {                                                                          // if the player wants to quit
            result = mosquitto_publish(mosq, NULL, "game/move", 2, "Q", 0, false); // publish a Q for quit, and store whether it was successful

            return (result == MOSQ_ERR_SUCCESS); // return true if successful
        }                                        // end if

        move[1] = move[1] & 0x0f; // take the string Player2 entered and convert it to a number
        move[1]--;                // subtract 1 so it is in bounds for the array
        move[1] = move[1] | 0x30; // change it back to a string

        move[0] = 'O'; // since the keyboard move is for Player2, it must be an O

        result = mosquitto_publish(mosq, NULL, "game/move", 2, move, 0, false); // publish the move, and store whether it was successful

        return (result == MOSQ_ERR_SUCCESS); // return true if successful
    } // end if

    return false; // otherwise, return false
} // end makeMove

bool kbdIn()
{
    waiting = true; // true because the program is waiting for a move from Player2

    while (waiting)
    {
        if (poll(&mypoll, 1, 500)) // polls the user for data, but also is able to wait for a message from the script
        {
            scanf("%c", &move[1]); // sets any input on the keyboard (should be number) into the move array
            waiting = false;       // false because the program is not waiting for a move from Player2
            return true;
        } // end if
        mosquitto_loop(mosq, -1, 1); // makes sure messages can come in from mosquitto
    } // end while

    return false;
} // end kbdIn

bool winTest(char state[])
{
    bool gameOver = false;

    // X
    if (state[0] == 'X' && state[1] == 'X' && state[2] == 'X')
    { // Top row X
        gameOver = true;
    }
    else if (state[3] == 'X' && state[4] == 'X' && state[5] == 'X')
    { // Mid row X
        gameOver = true;
    }
    else if (state[6] == 'X' && state[7] == 'X' && state[8] == 'X')
    { // Bot row X
        gameOver = true;
    }
    else if (state[0] == 'X' && state[3] == 'X' && state[6] == 'X')
    { // L column X
        gameOver = true;
    }
    else if (state[1] == 'X' && state[4] == 'X' && state[7] == 'X')
    { // Mid column X
        gameOver = true;
    }
    else if (state[2] == 'X' && state[5] == 'X' && state[8] == 'X')
    { // R column X
        gameOver = true;
    }
    else if (state[0] == 'X' && state[4] == 'X' && state[8] == 'X')
    { // Diag 1 X
        gameOver = true;
    }
    else if (state[2] == 'X' && state[4] == 'X' && state[6] == 'X')
    { // Diag 2 X
        gameOver = true;
    } // end if

    // O
    if (state[0] == 'O' && state[1] == 'O' && state[2] == 'O')
    { // Top row O
        gameOver = true;
    }
    else if (state[3] == 'O' && state[4] == 'O' && state[5] == 'O')
    { // Mid row O
        gameOver = true;
    }
    else if (state[6] == 'O' && state[7] == 'O' && state[8] == 'O')
    { // Bot row O
        gameOver = true;
    }
    else if (state[0] == 'O' && state[3] == 'O' && state[6] == 'O')
    { // L column O
        gameOver = true;
    }
    else if (state[1] == 'O' && state[4] == 'O' && state[7] == 'O')
    { // Mid column O
        gameOver = true;
    }
    else if (state[2] == 'O' && state[5] == 'O' && state[8] == 'O')
    { // R column O
        gameOver = true;
    }
    else if (state[0] == 'O' && state[4] == 'O' && state[8] == 'O')
    { // Diag 1 O
        gameOver = true;
    }
    else if (state[2] == 'O' && state[4] == 'O' && state[6] == 'O')
    { // Diag 2 O
        gameOver = true;
    } // end if

    return gameOver;
} // end winTest

void messageRecieved(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    letter = *(char *)message->payload; // stores the letter from the mqtt message

    sel = atoi((char *)(message->payload + 1)); // takes the number from the mqtt message, turns it to an integer, and stores it

    if (letter == 'O')
    { // if this gets a letter from the autoplay script, the program is no longer waiting for a move from Player2
        waiting = false;
    }
    else if (letter == 'Q')
    { // if somebody quit (sent a Q), the game is over
        gameOver = true;
    } //end if

    recieved = true;
} // end messageRecieved