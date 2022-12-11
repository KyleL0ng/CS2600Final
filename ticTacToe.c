#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <mosquitto.h>
#include <unistd.h>

// Tic Tac Toe game in between 2 players or player vs computer

void intro();
bool connectToServer();
void printBoard(char state[]);
char determineXO(int turns);
bool checkRange(int sel);
int getSelection(int A, int B);
bool checkValidity(int selection, char state[]);
bool getMove(char state[]);
//int getP2Move(char state[]);
bool winTest(char state[]);
void messageRecieved(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

struct mosquitto *mosq;
bool recieved;
int sel;

int main()
{

    char state[9];
    state[0] = ' ';
    state[1] = ' ';
    state[2] = ' ';
    state[3] = ' ';
    state[4] = ' ';
    state[5] = ' ';
    state[6] = ' ';
    state[7] = ' ';
    state[8] = ' ';
    int turns;
    int x;
    int y;
    int choice;
    bool connected;
    bool gameOver;
    bool waiting;

    // recieved = false;

    mosquitto_lib_init();
    connected = connectToServer();
    mosquitto_message_callback_set(mosq, messageRecieved);

    if (!connected)
    {
        return EXIT_FAILURE;
    }

    intro();

    // print current status of the board
    printBoard(state);
    turns = 0;
    gameOver = false;

    // Plays out game for 9 turns
    while (turns < 9 && !gameOver)
    {
        sel = 0; // tracks selection
        int player = 0;

        if (determineXO(turns) == 'X')
        {
            player = 1;
        }
        else
        {
            player = 2;
        }

        //if (EnvVar != scripttakeover && player == 2) {makeMove()}
        if (player == 2) {
            makeMove();
        }

        while (!getMove(state));

        state[sel] = determineXO(turns); // Sets token in gameboard space
        printBoard(state);               // prints board
        turns++;
        gameOver = winTest(state); // checks to see for winner
        if (gameOver)
        {
            if (determineXO(turns - 1) == 'X')
            {
                printf("Player 1 wins!\n");
            }
            else
            {
                printf("Player 2 wins!\n");
            }
        }
        if (turns == 9 && !gameOver)
        { // checks to see if tie
            printf("Tie game!\n");
        }
    }

    printf("Do you want to play again?\n");
    printf("1 --- play again\n");
    printf("2 --- quit\n");
    printf("Enter your choice (1 or 2):\n");
    scanf("%d", &choice);

    if (choice == 1)
    {
        main();
    }

    return EXIT_SUCCESS;
}

void intro()
{
    int choice; // tracks whether user wants to play vs computer or other player
    printf("===========================\n");
    printf("WELCOME TO TIC TAC TOE!\n");
    printf("Waiting for Player1 to start the game\n");
    printf("===========================\n");
}

bool connectToServer()
{
    mosq = mosquitto_new(NULL, true, NULL);

    int rtn = mosquitto_connect(mosq, "localhost", 1883, 30);

    if (rtn != MOSQ_ERR_SUCCESS)
    {
        printf("Error connecting");
        return false;
    }
    return true;
}

void printBoard(char state[])
{
    printf("The current status is:\n");
    printf("+-------------+\n | %c | %c | %c |\n+-------------+\n | %c | %c | %c |\n+-------------+\n | %c | %c | %c |\n+-------------+\n", state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7], state[8]);
}
char determineXO(int turns)
{
    char player; // tracks whether to add an X or O

    if (turns % 2 == 0)
    {
        player = 'X';
    }
    else
    {
        player = 'O';
    }

    return player;
}

bool checkRange(int sel)
{
    bool test = false;

    if (sel >= 0 && sel <= 8)
    {
        test = true;
    }

    return test;
}

bool checkValidity(int selection, char state[])
{
    bool test = false;

    if (state[selection] == ' ')
    {
        test = true;
    }

    return test;
}

bool getMove(char state[]) {
    recieved = false;

    while (!recieved) {
        usleep(500000);
    }

    while (!checkRange(sel))
    { // Makes sure that the moves correspond to spaces on the board
        printf("invalid move, not in range");
        return false;
    }

    while (!checkValidity(sel, state))
    { // Makes sure the desired space is empty before replacing the current token
        printf("invalid move, space already taken");
        return false;
    }

    return true;
}

bool makeMove() {
    char move[2];
    int result;

    printf("Player2: make your move (1-9):\n");
    scanf("%s", move);

    result = mosquitto_publish(mosq, NULL, "game/move", 1, move, 0, false);

    return (result == MOSQ_ERR_SUCCESS);
}

// int getP2Move(char state[])
// {
//     int sel;

//     printf("Player2: make your move");
//     scanf("%d", &sel);
//     sel = sel - 1;

//     while (!checkRange(sel))
//     { // Makes sure that the moves correspond to spaces on the board
//         printf("invalid move");
//         scanf("%d", &sel);
//         sel = sel - 1;
//     }

//     while (!checkValidity(sel, state))
//     { // Makes sure the desired space is empty before replacing the current token
//         printf("invalid move");
//         scanf("%d", &sel);
//         sel = sel - 1;
//     }

//     return sel;
// }

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
    }

    // O
    if (state[0] == 'O' && state[1] == 'O' && state[2] == 'O')
    { // Top row X
        gameOver = true;
    }
    else if (state[3] == 'O' && state[4] == 'O' && state[5] == 'O')
    { // Mid row X
        gameOver = true;
    }
    else if (state[6] == 'O' && state[7] == 'O' && state[8] == 'O')
    { // Bot row X
        gameOver = true;
    }
    else if (state[0] == 'O' && state[3] == 'O' && state[6] == 'O')
    { // L column X
        gameOver = true;
    }
    else if (state[1] == 'O' && state[4] == 'O' && state[7] == 'O')
    { // Mid column X
        gameOver = true;
    }
    else if (state[2] == 'O' && state[5] == 'O' && state[8] == 'O')
    { // R column X
        gameOver = true;
    }
    else if (state[0] == 'O' && state[4] == 'O' && state[8] == 'O')
    { // Diag 1 X
        gameOver = true;
    }
    else if (state[2] == 'O' && state[4] == 'O' && state[6] == 'O')
    { // Diag 2 X
        gameOver = true;
    }

    return gameOver;
}

void messageRecieved(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    sel = atoi((char *)message->payload);

    recieved = true;
}