#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int getposX(int X);            // return actual game positions by providing screen coordinates
int getposY(int Y);            //
void drawBackground();         // draws background and instructions
void initializePlayfield();    // initializes the game matrix with the sudokuInitial array
void drawFields(int x, int y); // draws the game fields, highlighting the cursor
int hasEqual(int x, int y);    // checks for an equal number in that line/col;
int generateSolution();        // generates step-by-step recursive solution
void createNewGame();          // creates a new random matrix
int hasBlanks();               // returns true for any blank field left in the game
int currCol = 0;
int currLin = 0;

int sudokuInitial[9][9] = {{8, 0, 5, 0, 0, 1, 0, 2, 0},
                           {0, 0, 0, 0, 0, 4, 0, 9, 0},
                           {0, 0, 0, 0, 0, 0, 8, 0, 5},

                           {1, 3, 0, 0, 0, 8, 0, 0, 0},
                           {5, 0, 0, 0, 3, 0, 0, 0, 9},
                           {0, 0, 0, 9, 0, 0, 0, 1, 7},

                           {6, 0, 7, 0, 0, 0, 0, 0, 0},
                           {0, 4, 0, 6, 0, 0, 0, 0, 0},
                           {0, 1, 0, 2, 0, 0, 9, 0, 6}};

int sudoku[9][9];

int main(void)
{
    srand(time(NULL));

    int y, x;
    int ch;

    /* initialize ncurses */

    initscr();
    clear();
    keypad(stdscr, TRUE); // enable special character usage
    cbreak();
    noecho();

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);   // regular numbers
        init_pair(2, COLOR_CYAN, COLOR_BLACK);    // fixed number
        init_pair(3, COLOR_BLACK, COLOR_CYAN);    // highlighted fixed number
        init_pair(4, COLOR_BLACK, COLOR_GREEN);   // highlighted regular
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // error
        init_pair(6, COLOR_BLACK, COLOR_MAGENTA); // highlighted error
    }
    else
    {
        printw("Colors are unsupported in this terminal!\n");
        getch();
        exit(EXIT_FAILURE);
    }

    drawBackground();
    initializePlayfield();

    y = 0;
    x = 0;
    int isRunning = 1;

    /* main game loop */
    while (isRunning)
    {
        drawFields(x, y);
        refresh();
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
        case 'w':
        case 'W':
            if (y > 0)
            {
                y = y - 1;
            }
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            if (y < 8)
            {
                y = y + 1;
            }
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            if (x > 0)
            {
                x = x - 1;
            }
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            if (x < 8)
            {
                x = x + 1;
            }
            break;
        case 'q':
        case 'Q':
            isRunning = 0;
            break;
        case KEY_BACKSPACE:
        case 127:
        case 'x':
        case 'X':
            if (!sudokuInitial[x][y])
                sudoku[x][y] = '.' - 48;
            break;
        case 'r':
        case 'R':
            initializePlayfield();
            break;
        case 'c':
        case 'C':
            currLin = 0;
            currCol = 0;
            initializePlayfield();
            if (!generateSolution())
            {
                printw("SEM SOLUCAO");
            }
            break;
        case 'n':
        case 'N':
            initializePlayfield();
            createNewGame();
            initializePlayfield();
            refresh();
            break;
        }

        if ((ch > '0' && ch <= '9'))
        {
            if (!sudokuInitial[x][y])   // block writing over fixed numbers
                sudoku[x][y] = ch - 48; // update game matrix
        }
    }

    endwin();
    exit(0);
}

int getposX(int x)
{
    int posx = x * 2 + 3;
    if (x > 2)
        posx += 2;
    if (x > 5)
        posx += 2;
    return posx;
}

int getposY(int y)
{
    int posy = y + 1;
    if (y > 2)
        posy++;
    if (y > 5)
        posy++;
    return posy;
}

void drawBackground()
{
    for (int y = 0; y < LINES; y++)
    {
        mvhline(y, 0, ' ', COLS);
    }
    // vertical lines
    mvvline(1, 1, ACS_VLINE, 11);
    mvvline(1, 9, ACS_VLINE, 11);
    mvvline(1, 17, ACS_VLINE, 11);
    mvvline(1, 25, ACS_VLINE, 11);
    // horizontal lines
    mvhline(0, 1, ACS_HLINE, 24);
    mvhline(4, 1, ACS_HLINE, 24);
    mvhline(8, 1, ACS_HLINE, 24);
    mvhline(12, 1, ACS_HLINE, 24);
    // corners
    mvaddch(0, 1, ACS_ULCORNER);
    mvaddch(0, 25, ACS_URCORNER);
    mvaddch(12, 1, ACS_LLCORNER);
    mvaddch(12, 25, ACS_LRCORNER);
    mvaddch(4, 1, ACS_LTEE);
    mvaddch(8, 1, ACS_LTEE);
    mvaddch(4, 25, ACS_RTEE);
    mvaddch(8, 25, ACS_RTEE);
    // instructions
    mvaddstr(3, 30, "Arrows or WASD keys to move");
    mvaddstr(4, 30, "Input numbers as usual");
    mvaddstr(5, 30, "X or Backspace to erase a move");
    mvaddstr(6, 30, "N to generate a new game");
    mvaddstr(7, 30, "C to generate a solution");
    mvaddstr(8, 30, "R to restart game");
    mvaddstr(9, 30, "Q to quit");
    return;
}

void createNewGame()
{
    // zero out the fixed game matrix
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            sudokuInitial[i][j] = 0;
        }
    }

    // generate random numbers checking if they are part of a valid solution
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (rand() % 10 >= 8)
            {
                int new = rand() % 10;
                sudokuInitial[i][j] = new;
                sudoku[i][j] = new;
                if (hasEqual(i, j))
                {
                    sudokuInitial[i][j] = 0;
                    sudoku[i][j] = '.' - 48; // blank space
                }
            }
        }
    }
    setbuf(stdin, NULL);
    return;
}

void drawFields(int x, int y)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (i == y && j == x)
            {
                // change character color accordingly
                attron(COLOR_PAIR(4));
                if (sudokuInitial[j][i])
                    attron(COLOR_PAIR(3));
                else if (hasEqual(j, i))
                {
                    attron(COLOR_PAIR(6));
                }
                mvaddch(getposY(i), getposX(j), sudoku[j][i] + 48);
            }
            else // regular numbers
            {
                attron(COLOR_PAIR(1));
                if (sudokuInitial[j][i])
                    attron(COLOR_PAIR(2));
                else if (hasEqual(j, i))
                {
                    attron(COLOR_PAIR(5));
                }
                mvaddch(getposY(i), getposX(j), sudoku[j][i] + 48);
            }
        }
    }
    // debug info
    attron(COLOR_PAIR(3));
    mvaddstr(1, 30, "x: ");
    mvaddch(1, 33, x + 48);
    mvaddstr(1, 34, ", y: ");
    mvaddch(1, 39, y + 48);
}

void initializePlayfield()
{
    memcpy(sudoku, sudokuInitial, 9 * 9 * sizeof(int)); // copies initial matrix into play matrix
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] == 0)
                sudoku[i][j] = '.' - 48;
        }
    }
}

int hasEqual(int x, int y)
{
    // line
    for (int i = 0; i < 9; i++)
    {
        if (i != x &&
            sudoku[x][y] == sudoku[i][y] &&
            sudoku[x][y] != '.' - 48)
            return 1;
    }
    // collumn
    for (int i = 0; i < 9; i++)
    {
        if (i != y &&
            sudoku[x][y] == sudoku[x][i] &&
            sudoku[x][y] != '.' - 48)
            return 1;
    }
    // block
    for (int i = (y / 3) * 3; i < (y / 3) * 3 + 3; i++)
    {
        for (int j = (x / 3) * 3; j < (x / 3) * 3 + 3; j++)
        {
            if (j != x &&
                i != y &&
                sudoku[x][y] == sudoku[j][i] &&
                sudoku[x][y] != '.' - 48)
                return 1;
        }
    }

    return 0;
}

int generateSolution()
{

    int number;           // number being tested
    int origLin, origCol; // original line and column

    if (!hasBlanks())
    {
        return 1;
    }
    for (number = 1; number <= 9; number++)
    {
        int backup = sudoku[currLin][currCol];
        sudoku[currLin][currCol] = number;
        drawFields(currLin, currCol);
        refresh();
        if (!hasEqual(currLin, currCol))
        {
            origLin = currLin;
            origCol = currCol;
            if (generateSolution())
                return 1;
            // update previous values for backtracking
            currLin = origLin;
            currCol = origCol;

            sudoku[currLin][currCol] = '.' - 48; // blank out
            drawFields(currLin, currCol);
            refresh();
        }

        sudoku[currLin][currCol] = backup;
    }

    return 0;
}

int hasBlanks()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[j][i] == '.' - 48)
            {
                currLin = j;
                currCol = i;
                return 1;
            }
        }
    }
    return 0;
}