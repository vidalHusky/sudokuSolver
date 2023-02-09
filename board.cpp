#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>
#include <cmath>

//4 counters-> if index i/3 = 0 and index j/3 = 0 in box 1:
using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  //  The number of cells in a small square
//  (usually 3).  The board has
//  SquareSize^2 rows and SquareSize^2
//  columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

int reCursiveCalls = 0;

class board
// Stores the entire Sudoku board
{
public:
    board(int);

    void clear();

    void initialize(ifstream &fin);

    void print();

    void printConflict();

    bool isBlank(int, int);

    int getCell(int, int);

    void setCell(int, int, int);

    void updateConflicts();

    void clearCell(int, int);

    bool solved();

    void findBlankCell(int &i, int &j);

    void solve(board &b1, int recursiveCalls);

    int getConflict(int i, int j, int k){return conflictValue[i][j][k];}

private:

    // The following matrices go from 1 to BoardSize in each
    // dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)

    matrix<vector<int> > conflictValue; //for loop to resize each at i j to 9, then go through for loop to set all values to 0
    matrix<int> value;
};

board::board(int sqSize) : value(BoardSize + 1, BoardSize + 1)
// Board constructor
{
    clear();
    conflictValue.resize(BoardSize, BoardSize);
//    cout << conflictValue[0][0][0];

//resizing conflictValue vector matrix
    for (int i = 0; i < BoardSize; i++) {
        for (int j = 0; j < BoardSize; j++) {
            conflictValue[i][j].resize(BoardSize);
        }
    }
    //resizing conflictValue vector that holds all conflicts to length 8 with each index being zero
    for (int i = 0; i < BoardSize; i++) {
        for (int j = 0; j < BoardSize; j++) {
            for (int k = 0; k <= 8; k++) {
                conflictValue[i][j][k] = 0;
            }
        }
    }
}

//clear cell of value
void board::clearCell(int i, int j) {
    value[i][j] = Blank;
}

void board::clear()
// Mark all possible values as legal for each board entry
{
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++) {
            value[i][j] = Blank;
        }
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
    char ch;

    clear();

    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++) {
            fin >> ch;

            // If the read char is not Blank
            if (ch != '.')
                setCell(i, j, ch - '0');   // Convert char to int
            else
                setCell(i, j, Blank);
        }
    updateConflicts(); //Update conflicts according to initial sudoku board
}

//set cell to certain value
void board::setCell(int i, int j, int cellValue) {
    value[i][j] = cellValue;
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
    // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
    // coordinates of the square that i,j is in.

    return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
    for (int i = 0; i < v.size(); i++) {
        ostr << v[i] << " ";
    }
    cout << endl;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in setCell");

    return (getCell(i, j) == Blank);
}

//add box conflicts
void board::updateConflicts() {
    int counter;
    for (int i = 1; i <= BoardSize; i++) {
        for (int j = 1; j <= BoardSize; j++) {
            for (int z = 1; z <= BoardSize; z++) {
                counter = 0;
                for (int x = 1; x <= BoardSize; x++) { //updates by checking conflicts for boxes in the same column
                    if (value[x][j] == z) {
                        counter++;
                    }
                }
                for (int y = 1; y <= BoardSize; y++) {//updates by checking conflicts for boxes in the same rows
                    if (value[i][y] == z) {
                        counter++;
                    }
                }
                //get top left square of box
                int rowIndex = floor((i - 1) / SquareSize);
                int colIndex = floor((j - 1) / SquareSize);
                rowIndex = rowIndex * SquareSize + 1;
                colIndex = colIndex * SquareSize + 1;
                //Update conflicts according to the box it is in
                for (int r = rowIndex; r < rowIndex + SquareSize; r++) {
                    for (int c = colIndex; c < colIndex + SquareSize; c++) {
                        if (value[r][c] == z) {
//                            cout << "value at " << r << ", " << c << ": " << z << endl;
                            counter++;
                        }
                    }
                }
                conflictValue[i - 1][j - 1][z - 1] = counter; //updating conflicts to match
            }
        }

    }
}

void board::print()
// Prints the current board.
{
    for (int i = 1; i <= BoardSize; i++) {
        if ((i - 1) % SquareSize == 0) {
            cout << " -";
            for (int j = 1; j <= BoardSize; j++)
                cout << "---";
            cout << "-";
            cout << endl;
        }
        for (int j = 1; j <= BoardSize; j++) {
            if ((j - 1) % SquareSize == 0)
                cout << "|";
            if (!isBlank(i, j))
                cout << " " << getCell(i, j) << " ";
            else
                cout << "   ";
        }
        cout << "|";
        cout << endl;
    }

    cout << " -";
    for (int j = 1; j <= BoardSize; j++)
        cout << "---";
    cout << "-";
    cout << endl;
}

//Prints conflicts by iterating through all squares in board
void board::printConflict() {
    for (int i = 0; i < BoardSize; i++) {
        for (int j = 0; j < BoardSize; j++) {
            cout << i + 1 << "," << j + 1 << ": ";
            for (int k = 0; k < BoardSize; k++) {
                //cout<< i << ": "<< j <<endl;
                cout << conflictValue[i][j][k];
            }
            cout << endl;
        }
    }
}

//Returns true if board is solved ie all squares are filled, false if not
bool board::solved() {
    for (int i = 1; i <= BoardSize; i++) {
        for (int j = 1; j <= BoardSize; j++) {
            if (isBlank(i, j)) {
                return false;
            }
        }
    }
    return true;
}

