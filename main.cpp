#include <iostream>
#include "board.cpp"
#include "d_matrix.h"
#include "d_except.h"


void findBlankCell(board &b1, int &i, int &j) {
    int mostConflicted = 0; //holds the number of conflicted
    int holder;
    for (int y = 1; y <= BoardSize; y++) {
        for (int x = 1; x <= BoardSize; x++) {
            holder = 0;
            if(b1.isBlank(y,x)){
                for (int k = 1; k <= BoardSize; k++) {
                    holder = holder + b1.getConflict(y-1,x-1,k-1);
                }
                if (holder>mostConflicted){
                    i = y;
                    j = x;
                    mostConflicted = holder;
                }
                //cout << "Most Conflicted: " << mostConflicted << " i: "<< i << " j: "<< j<< endl;
            }
        }
    }
}

void solve(board &b1){
    b1.updateConflicts();
    reCursiveCalls++;
    if(b1.solved()){
        b1.print();
        cout << "Amount of Recursive Calls: "<< reCursiveCalls << endl;
    }
    else{
        int i;
        int j;
        findBlankCell(b1,i,j);
        //cout << "FIND BLANK CELL: i: "<< i << " j: "<< j<< endl;
        for(int dig = 1; dig<=BoardSize; dig++){
            //b1.updateConflicts();
            //cout << b1.getConflict(i-1,j-1,dig) << endl;
            if(b1.getConflict(i-1,j-1,dig-1)==0){
                b1.setCell(i,j,dig);
                //b1.print();
                solve(b1);
                b1.updateConflicts();
                b1.clearCell(i,j);
                //b1.print();
            }
        }
    }
}

int main() {
    ifstream fin;

    // Read the sample grid from the file.
    string fileName = "sudoku1.txt";

    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }

    try
    {
        board b1(SquareSize);
        while (fin && fin.peek() != 'Z')
        {
            b1.initialize(fin);
            b1.print();
            int t;
            solve(b1,t);
            b1.print();
        }
    }
    catch  (indexRangeError &ex)
    {
        cout << ex.what() << endl;
        exit(1);
    }

}
