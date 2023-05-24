// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include <map>
#include <utility>
#include <vector>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    int **aiBoard;
    int **frontier;
    //int *frontier[50]; //the uncovered tiles which have unmarked and covered tiles adjacent to them (50 temporarily)
    queue<pair<int,int>> uncovNext; //uncover these next queue
    queue<pair<int,int>> flagNext; //uncover these next queue
    // [(y, x, flagged = 0), (y, x, flagged = 1), (y, x, flagged = 0), (y, x, flagged = 1)]
    // [(y, x, flagged = 1), (y, x, flagged = 1), (y, x, flagged = 1), (y, x, flagged = 0)]
    // [(y, x, #flagsuc= 1), (y, x, #flagsuc= 2), (y, x, #flagsuc= 1), (y, x, #flagsuc= 1)]

    //map<pair<int,int>, int> uncovFront; //the uncovered frontier (uncovered nodes adjacent to a covered node)
    // vector<array<int, 3>> uncovAdjVect; //a vector for the uncovered adjacent tiles
    vector<vector<array<int, 3>>> treeSolutions;


    int colDim;
    int rowDim;
    int xLast;
    int yLast; //might not need these

    //initialize board, called when constructor is called
    void boardInit(int _rowDimension, int _colDimension, int _agentX, int _agentY);
    //make a destructor to get rid of it at the end
    ~MyAI ();
    bool isDuplicateVect(vector<array<int, 3>>& vect, int size);
    bool adjacentUncovered(int y, int x);
    void treeMethod(vector<array<int, 3>>& vect, int size);
    void storeSuccess(vector<array<int, 3>>& vect, int size, vector<array<int, 3>>& solution);
    bool isFlag(int y, int x);
    int numFlagsAdj(int y, int x);
    void updateCoverCount(int y, int x);
    bool numEqUncov(int y, int x);
    void printBoard(int **arr);
    void addAdjacentToFlag(int y, int x);
    void addAdjacentToUncover(int y, int x);
    bool isCovered(int y, int x);
    void flagAdjUncovDec(int y, int x);
    void fakeFlag(int y, int x);
    void unFakeFlag(int y, int x);
    int vectAllZerosOrNeg(vector<array<int, 3>>& vect, int size);
    int allZerosOrNeg(int y, int x);
    //int getY(int y);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
