#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Tic Tac Toe game in between 2 players or player vs computer

int intro();
void printBoard(char state[]);
char determineXO(int turns);
bool checkRange(int A, int B);
int getSelection(int A, int B);
bool checkValidity(int selection, char state[]);
bool winTest(char state[]);

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
    bool gameOver;

    int choice = intro(); // Prompts user and stores game choice

    if (choice == 1)
    {
        // print current status of the board
        printBoard(state);
        turns = 0;
        gameOver = false;

        // Plays out game for 9 turns
        while (turns < 9 && !gameOver)
        {
            int A = 0; // tracks row
            int B = 0; // tracks column
            int player = 0;

            if (determineXO(turns) == 'X')
            {
                player = 1;
            }
            else
            {
                player = 2;
            }
            printf("Player%d: make your move", player);
            scanf("%d %d", &A, &B);

            while (!checkRange(A, B))
            { // Makes sure that the moves correspond to spaces on the board
                printf("invalid move");
                scanf("%d %d", &A, &B);
            }

            while (!checkValidity(getSelection(A, B), state))
            { // Makes sure the desired space is empty before replacing the current token
                printf("invalid move");
                scanf("%d %d", &A, &B);
            }

            state[getSelection(A, B)] = determineXO(turns); // Sets token in gameboard space
            printBoard(state);                              // prints board
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
    }
    if (choice == 2)
    {
        // print current status of the board
        printBoard(state);
        turns = 0;
        gameOver = false;
        time_t t;
        srand((unsigned)time(&t)); // generates random sequence based on system time

        // Plays out game for 9 turns
        while (turns < 9 && !gameOver)
        {
            int A = 0; // tracks row
            int B = 0; // tracks column
            int player = 0;

            if (determineXO(turns) == 'X')
            {
                player = 1;
            }
            else
            {
                player = 2;
            }

            if (player == 1) // Normal player move
            {
                printf("Player%d: make your move", player);
                scanf("%d %d", &A, &B);

                while (!checkRange(A, B))
                { // Makes sure that the moves correspond to spaces on the board
                    printf("invalid move");
                    scanf("%d %d", &A, &B);
                }

                while (!checkValidity(getSelection(A, B), state))
                { // Makes sure the desired space is empty before replacing the current token
                    printf("invalid move");
                    scanf("%d %d", &A, &B);
                }
            }
            else
            { // random computer move
                A = rand() % 4;
                B = rand() % 4;

                while (!checkRange(A, B))
                { // Makes sure that the moves correspond to spaces on the board
                   A = rand() % 4;
                   B = rand() % 4;
                }

                while (!checkValidity(getSelection(A, B), state))
                { // Makes sure the desired space is empty before replacing the current token
                    A = rand() % 4;
                    B = rand() % 4;
                }
            }

            state[getSelection(A, B)] = determineXO(turns); // Sets token in gameboard space
            printBoard(state);                              // prints board
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
                    printf("Computer wins!\n");
                }
            }
            if (turns == 9 && !gameOver)
            { // checks to see if tie
                printf("Tie game!\n");
            }
        }
    }

    printf("Do you want to play again?\n");
    printf("1 --- play again\n");
    printf("2 --- quit\n");
    printf("Enter your choice (1 or 2):\n");
    scanf("%d", &choice);

    if (choice == 1) {
        main();
    }

    return 0;
}

int intro()
{
    int choice; // tracks whether user wants to play vs computer or other player
    printf("===========================\n");
    printf("WELCOME TO TIC TAC TOE!\n");
    printf("1 --- person vs. person\n");
    printf("2 --- person vs. random computer\n");
    printf("Enter your choice (1 or 2):\n===========================\n");
    scanf("%d", &choice);
    printf("You have entered choice %d\n", choice);
    return choice;
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

bool checkRange(int A, int B)
{
    bool test = false;

    if (!(A > 3) && !(A < 1))
    {
        if (!(B > 3) && !(B < 1))
        {
            test = true;
        }
    }

    return test;
}

int getSelection(int A, int B)
{
    int selection = -1;
    if (A == 1)
    {
        if (B == 1)
        {
            selection = 0;
        }
        else if (B == 2)
        {
            selection = 1;
        }
        else if (B == 3)
        {
            selection = 2;
        }
    }
    else if (A == 2)
    {
        if (B == 1)
        {
            selection = 3;
        }
        else if (B == 2)
        {
            selection = 4;
        }
        else if (B == 3)
        {
            selection = 5;
        }
    }
    else if (A == 3)
    {
        if (B == 1)
        {
            selection = 6;
        }
        else if (B == 2)
        {
            selection = 7;
        }
        else if (B == 3)
        {
            selection = 8;
        }
    }
    return selection;
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