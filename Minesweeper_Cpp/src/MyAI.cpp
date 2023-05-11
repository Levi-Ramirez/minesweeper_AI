// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

// flags = -1, covered = -2,
// agent is the parent class of MyAI, so here we initialize it in
// an initialized list
MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    aiBoard = new int*[_rowDimension]; //initialize board
    for(int i = 0; i < _rowDimension; i++)
    {
        aiBoard[i] = new int[_colDimension];
    }

    for(int i = 0; i < _rowDimension; i++)
    {
        for(int j = 0; j < _colDimension; j++)
        {
            aiBoard[i][j] = -2;
        }
    }

    colDim = _colDimension;
    rowDim = _rowDimension;

    xLast = _agentX;
    yLast = _agentY;

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

MyAI::~MyAI() {
    for(int i = 0; i < colDimension; ++i)
        delete[] aiBoard[i];
    
    delete [] aiBoard;
}

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    
    if(number != -1)
    {
        aiBoard[yLast][xLast] = number;
        if(number == 0)
        {
            addAdjacent(yLast, xLast);
        }
    }
    printBoard();
    if(!uncovNext.empty()) //when the queue is not empty, these need to removed next
    {
        pair<int,int> myP = uncovNext.front();
        uncovNext.pop();
        yLast = myP.first;
        xLast = myP.second;
        return {UNCOVER, xLast, yLast};
    }
    //aiBoard[5][5] = number;

    return {FLAG, 1, 1};

    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}




// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

//adds the adjacent squares to the queue to uncover them later
void MyAI::addAdjacent(int y, int x) {
    bool up = false;
    bool down = false;
    if(y - 1 >= 0)
        down = true;
    if(y + 1 < rowDim)
        up = true;

    if(x - 1 >= 0){
        if(isCovered(y, x - 1))
            uncovNext.push({y, x - 1});
        if(up){
            if(isCovered(y + 1, x - 1))
                uncovNext.push({y + 1, x - 1});
        }
        if(down){
            if(isCovered(y - 1, x - 1))
                uncovNext.push({y - 1, x - 1});
        }
    }
    if(x + 1 < colDim){
        if(isCovered(y, x + 1))
            uncovNext.push({y,x + 1});
        if(up){
            if(isCovered(y + 1, x + 1))
                uncovNext.push({y + 1, x + 1});
        }
        if(down){
            if(isCovered(y - 1, x + 1))
                uncovNext.push({y - 1, x + 1});
        }
    }
}


bool MyAI::isCovered(int y, int x)
{
    if(aiBoard[y][x] == -2)
        return true;
    return false;
}


void MyAI::printBoard() {
    for(int i = rowDim - 1; i >= 0; i--) //print the higher rows first
    {
        for(int j = 0; j < colDim; j++)
        {
            std::cout << aiBoard[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


// getY: the reson we use this is because when you go higher in y, you go up the baord
// int MyAI::getY(int y) { 
//     return (rowDim - yLast - 1);
// }

// ======================================================================
// YOUR CODE ENDS
// ======================================================================
