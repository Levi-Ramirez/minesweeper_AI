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
using namespace std;

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

    frontier = new int*[_rowDimension]; //initialize board
    for(int i = 0; i < _rowDimension; i++)
    {
        frontier[i] = new int[_colDimension];
    }

    for(int i = 0; i < _rowDimension; i++)
    {
        for(int j = 0; j < _colDimension; j++)
        {
            aiBoard[i][j] = -2;
            if(j == 0 || j == _colDimension - 1 || i == 0 || i == _rowDimension - 1)
                frontier[i][j] = 5;
            else   
                frontier[i][j] = 8;
        }
    }

    frontier[0][0] = 3;
    frontier[0][_colDimension - 1] = 3;
    frontier[_rowDimension - 1][0] = 3;
    frontier[_rowDimension - 1][_colDimension - 1] = 3;

    colDim = _colDimension;
    rowDim = _rowDimension;

    xLast = _agentX;
    yLast = _agentY;
    updateCoverCount(yLast, xLast);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

MyAI::~MyAI() {
    for(int i = 0; i < colDimension; ++i)
        delete[] aiBoard[i];
    
    for(int i = 0; i < colDimension; ++i)
        delete[] frontier;
    
    delete [] aiBoard;
    delete [] frontier;
}

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    
    if(number != -1)
    {
        aiBoard[yLast][xLast] = number - numFlagsAdj(yLast, xLast);
        if(aiBoard[yLast][xLast] == 0)
        {
            addAdjacentToUncover(yLast, xLast);
        }
    }
    //printBoard();

    //NOTE: this was before in an if statement, but the !uncovNext.empty() in the while loop should be good enough
    // if(!uncovNext.empty()) 
    // {
    while(!uncovNext.empty()) //when the queue is not empty, these need to uncovered next
    {
        pair<int,int> myP = uncovNext.front();
        uncovNext.pop();
        if(aiBoard[myP.first][myP.second] != -2)
            continue;
        yLast = myP.first;
        xLast = myP.second;
        updateCoverCount(yLast, xLast);
        // printBoard(frontier);
        // printBoard(aiBoard);
        return {UNCOVER, xLast, yLast};
    }
    // }
    while(true) //loops until all of the frontier was checked and nothing was found that needed flags around it
    {
        while(!flagNext.empty())
        {
            pair<int,int> myP = flagNext.front();
            flagNext.pop();
            if(aiBoard[myP.first][myP.second] != -2) //not a covered tile to flag
                continue;
            yLast = myP.first;
            xLast = myP.second;
            flagAdjUncovDec(yLast, xLast);
            // printBoard(frontier);
            // printBoard(aiBoard);
            return {FLAG, xLast, yLast};
        }


        bool needsFlags = false; //checks to see if it was found that something needs flags around it
        for(int i = 0; i < rowDim; i++)
        {
            for(int j = 0; j < colDim; j++)
            {
                if(frontier[i][j] != 0){
                    if(numEqUncov(i, j)) {
                        addAdjacentToFlag(i, j);
                        needsFlags = true;
                    }
                }
                if(needsFlags){break;} //break out of inner 
            }
            if(needsFlags){break;}
        }
        if(needsFlags){continue;}
        else{break;}
    }

//tree method:
    //store uncovAdjVect positions
    vector<array<int, 3>> uncovAdjVect;
    for(int i = 0; i < rowDim; i++)
    {
        for(int j = 0; j < colDim; j++)
        {
            if(adjacentUncovered(i, j)){
                uncovAdjVect.push_back({i,j,0});
            }
        }
    }

    //run the tree method
    //cout << "we hereeee" << endl;
    treeMethod(uncovAdjVect, uncovAdjVect.size());

    int sizeSolu = treeSolutions.size();

    //cout << "treeSolutions.size(): " << sizeSolu << endl;
    //add solutions to a map mapping them to the count
    map<pair<int,int>, float> soluMap;
    pair<int,int> curPair;
    for(int i = 0; i < treeSolutions.size(); i++) {
        // curPair = {treeSolutions[i][0], treeSolutions[i][1]};
        for(int j = 0; j < treeSolutions[i].size(); j++) {
            curPair.first = treeSolutions[i][j][0];
            curPair.second = treeSolutions[i][j][1];
            //cout << "addPair: " << curPair.first << ", " << curPair.second << endl;
            if(soluMap.find(curPair) != soluMap.end()){
                soluMap[curPair] += 1;
            }
            else {
                soluMap[curPair] = 1;
            }
        }
    }
    
    bool guess = true;
    //cout << "sizeSolu: " << sizeSolu << endl;
    for(auto const& item: soluMap) {
        //cout << "item.second: " << item.second << endl;
        soluMap[item.first] = item.second / sizeSolu;
        //cout << "soluMap item:" << soluMap[item.first] << endl;
        if(soluMap[item.first] == 1) { // if its 1, it needs a flag in all solutions
            flagNext.push(item.first);
            //cout << "item added to queue:" << item.first.first << ", " << item.first.second << endl;
            guess = false;
        }
    }
    if(guess) {
        float maxVal = 0;
        pair<int, int> maxPair = {-1, -1};
        for (const auto& item : soluMap) {
            if(item.second > maxVal) {
                maxVal = item.second;
                maxPair = item.first;
            }
        }
        if(maxPair.first != -1) {
            //cout << "need to take an educated guess" << endl;
            flagNext.push(maxPair);
        }
        else {
            //cout << "need to randomly guess" << endl;
        }
        
    }
    //run the flag process
    while(!flagNext.empty())
    {
        //cout << "Enter possible-flag process" << endl;
        pair<int,int> myP = flagNext.front();
        flagNext.pop();
        if(aiBoard[myP.first][myP.second] != -2) //not a covered tile to flag
            continue;
        yLast = myP.first;
        xLast = myP.second;
        flagAdjUncovDec(yLast, xLast);
        // printBoard(frontier);
        // printBoard(aiBoard);
        treeSolutions.clear();
        return {FLAG, xLast, yLast};
    }



    // for(int i = 0; i < treeSolutions.size(); i++) {
    //     cout << "solution " << i << ": ";
    //     for(int j = 0; j < treeSolutions[i].size(); j++) {
    //         cout << "[" << treeSolutions[i][j][0] << " " << treeSolutions[i][j][0] << " " << treeSolutions[i][j][0] << "] ";
    //     }
    //     cout << endl;
    // }
    //do one last dumby test (maybe)

    //printBoard(aiBoard);
    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
}




// ======================================================================
// YOUR CODE BEGINS
// ======================================================================
bool MyAI::isDuplicateVect(vector<array<int, 3>>& vect, int size) {

    for(int i = 0; i < treeSolutions.size(); i++) {
        for(int j = 0; j < size; j++) {
            //see if they are equal (also account for that their size may not be equal, if they aren't then it automatically isn't equal)
            if(treeSolutions[i][j][0] == vect[j][0]) {
                if(treeSolutions[i][j][1] == vect[j][1]) {
                    if(treeSolutions[i][j][2] == vect[j][2]) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
void MyAI::treeMethod(vector<array<int, 3>>& vect, int size){
    int res = 0;
    //cout << "size: " << size << endl;
    //printBoard(aiBoard);
    for(int i = 0; i < size; i++) {
        //cout << "possible loop" << endl;
        if(isFlag(vect[i][0], vect[i][1])){
            //cout << "is flag" << endl;
            continue; //if it already is a flag, go back to the top of the loop and try again
        }
        fakeFlag(vect[i][0], vect[i][1]); //add a flag
        //printBoard(aiBoard);
        res = vectAllZerosOrNeg(vect, size); //check results
        if(res == 0){ //means there has been a solution
            //cout << "is zero" << endl;
            //printBoard(aiBoard);
            vector<array<int, 3>> vectSolution;
            storeSuccess(vect, size, vectSolution); //store the flagged vectors that led to a solution here
            if(!isDuplicateVect(vectSolution, vectSolution.size())) {
                treeSolutions.push_back(vectSolution); //store this vector into the possible solutions
            }
            vectSolution.clear();
            unFakeFlag(vect[i][0], vect[i][1]); //unflag most recent flag before returning
            continue; //return (actually... continue) because you found a solution, no further recursion will be another solution down this branch
        }
        else if(res == -1) {
            //cout << "is neg 1" << endl;
            //printBoard(aiBoard);
            unFakeFlag(vect[i][0], vect[i][1]); //unflag most recent flag before returning
            continue; //return (actually... continue) this is not a solution
        }
        //cout << "recurs" << endl;
        treeMethod(vect, size); // continue recurssing if the above isn't true
        unFakeFlag(vect[i][0], vect[i][1]); // after you recurs down this branch unflag it to go to other options
    }
}

void MyAI::storeSuccess(vector<array<int, 3>>& vect, int size, vector<array<int, 3>>& solution){
    for(int i = 0; i < size; i++) {
        if(isFlag(vect[i][0], vect[i][1]))
            solution.push_back(vect[i]);
    }
}


bool MyAI::adjacentUncovered(int y, int x) {
    if(aiBoard[y][x] != -2) return false;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(!isCovered(y - 1, x) && !isFlag(y - 1, x))
            return true;
    }
    if(y + 1 < rowDim){
        up = true;
        if(!isCovered(y + 1, x) && !isFlag(y + 1, x))
            return true;
    }

    if(x - 1 >= 0){
        if(!isCovered(y, x - 1) && !isFlag(y, x - 1))
            return true;
        if(up){
            if(!isCovered(y + 1, x - 1) && !isFlag(y + 1, x - 1))
            return true;
        }
        if(down){
            if(!isCovered(y - 1, x - 1) && !isFlag(y - 1, x - 1))
            return true;
        }
    }
    if(x + 1 < colDim){
        if(!isCovered(y, x + 1) && !isFlag(y, x + 1))
            return true;
        if(up){
            if(!isCovered(y + 1, x + 1) && !isFlag(y + 1, x + 1))
            return true;
        }
        if(down){
            if(!isCovered(y - 1, x + 1) && !isFlag(y - 1, x + 1))
            return true;
        }
    }

    return false;
}
void MyAI::updateCoverCount(int y, int x) {
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        --frontier[y - 1][x];
    }
    if(y + 1 < rowDim){
        up = true;
        --frontier[y + 1][x];
    }

    if(x - 1 >= 0){
        --frontier[y][x - 1];
        if(up){
            --frontier[y + 1][x - 1];
        }
        if(down){
            --frontier[y - 1][x - 1];
        }
    }
    if(x + 1 < colDim){
        --frontier[y][x + 1];
        if(up){
            --frontier[y + 1][x + 1];
        }
        if(down){
            --frontier[y - 1][x + 1];
        }
    }
}

//returns the number of adjacent flags to a recently uncovered square so you can update its value
int MyAI::numFlagsAdj(int y, int x) {
    int count = 0;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isFlag(y - 1, x))
            ++count;
    }
    if(y + 1 < rowDim){
        up = true;
        if(isFlag(y + 1, x))
            ++count;
    }

    if(x - 1 >= 0){
        if(isFlag(y, x - 1))
            ++count;
        if(up){
            if(isFlag(y + 1, x - 1))
                ++count;
        }
        if(down){
            if(isFlag(y - 1, x - 1))
                ++count;
        }
    }
    if(x + 1 < colDim){
        if(isFlag(y, x + 1))
            ++count;
        if(up){
            if(isFlag(y + 1, x + 1))
                ++count;
        }
        if(down){
            if(isFlag(y - 1, x + 1))
                ++count;
        }
    }
    
    return count;
}



// check if the number in the square equals the number of uncovered tiles
// that is adjacent to it
bool MyAI::numEqUncov(int y, int x) {
    int count = 0;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isCovered(y - 1, x))
            ++count;
    }
    if(y + 1 < rowDim){
        up = true;
        if(isCovered(y + 1, x))
            ++count;
    }

    if(x - 1 >= 0){
        if(isCovered(y, x - 1))
            ++count;
        if(up){
            if(isCovered(y + 1, x - 1))
                ++count;
        }
        if(down){
            if(isCovered(y - 1, x - 1))
                ++count;
        }
    }
    if(x + 1 < colDim){
        if(isCovered(y, x + 1))
            ++count;
        if(up){
            if(isCovered(y + 1, x + 1))
                ++count;
        }
        if(down){
            if(isCovered(y - 1, x + 1))
                ++count;
        }
    }
    if(count == aiBoard[y][x])
        return true;
    
    return false;
}

// flags y,x and decriments the number on adjacent uncovered squares & the covered count
// also, if the new numbers are 0, add them to uncovNext
void MyAI::flagAdjUncovDec(int y, int x) {
    aiBoard[y][x] = -4;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        --frontier[y - 1][x];
        if(!isCovered(y - 1, x) && (aiBoard[y - 1][x] != -4)){
            --aiBoard[y - 1][x];
            if(aiBoard[y - 1][x] == 0)
            {
                addAdjacentToUncover(y - 1, x);
            }
        }
    }
    if(y + 1 < rowDim){
        up = true;
         --frontier[y + 1][x];
        if(!isCovered(y + 1, x) && (aiBoard[y + 1][x] != -4)) {
            --aiBoard[y + 1][x];
            if(aiBoard[y + 1][x] == 0)
            {
                addAdjacentToUncover(y + 1, x);
            }
        }
    }

    if(x - 1 >= 0){
        --frontier[y][x - 1];
        if(!isCovered(y, x - 1) && (aiBoard[y][x - 1] != -4)){
            --aiBoard[y][x - 1];
            if(aiBoard[y][x - 1] == 0)
            {
                addAdjacentToUncover(y, x - 1);
            }
        }
            
        if(up){
            --frontier[y + 1][x - 1];
            if(!isCovered(y + 1, x - 1) && (aiBoard[y + 1][x - 1] != -4)){
                --aiBoard[y + 1][x - 1];
                if(aiBoard[y + 1][x - 1] == 0)
                {
                    addAdjacentToUncover(y + 1, x - 1);
                }
            }
        }
        if(down){
            --frontier[y - 1][x - 1];
            if(!isCovered(y - 1, x - 1) && (aiBoard[y - 1][x - 1] != -4)){
                --aiBoard[y - 1][x - 1];
                if(aiBoard[y - 1][x - 1] == 0)
                {
                    addAdjacentToUncover(y - 1, x - 1);
                }
            }
        }
    }
    if(x + 1 < colDim){
        --frontier[y][x + 1];
        if(!isCovered(y, x + 1) && (aiBoard[y][x + 1] != -4)){
            --aiBoard[y][x + 1];
            if(aiBoard[y][x + 1] == 0)
            {
                addAdjacentToUncover(y, x + 1);
            }
        }
        if(up){
            --frontier[y + 1][x + 1];
            if(!isCovered(y + 1, x + 1) && (aiBoard[y + 1][x + 1] != -4)){
                --aiBoard[y + 1][x + 1];
                if(aiBoard[y + 1][x + 1] == 0)
                {
                    addAdjacentToUncover(y + 1, x + 1);
                }
            }
        }
        if(down){
            --frontier[y - 1][x + 1];
            if(!isCovered(y - 1, x + 1) && (aiBoard[y - 1][x + 1] != -4)){
                --aiBoard[y - 1][x + 1];
                if(aiBoard[y - 1][x + 1] == 0)
                {
                    addAdjacentToUncover(y - 1, x + 1);
                }
            }
        }
    }
}

void MyAI::fakeFlag(int y, int x) {
    aiBoard[y][x] = -4;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        --frontier[y - 1][x];
        if(!isCovered(y - 1, x) && (aiBoard[y - 1][x] != -4)){
            --aiBoard[y - 1][x];
        }
    }
    if(y + 1 < rowDim){
        up = true;
         --frontier[y + 1][x];
        if(!isCovered(y + 1, x) && (aiBoard[y + 1][x] != -4)) {
            --aiBoard[y + 1][x];
        }
    }

    if(x - 1 >= 0){
        --frontier[y][x - 1];
        if(!isCovered(y, x - 1) && (aiBoard[y][x - 1] != -4)){
            --aiBoard[y][x - 1];
        }
            
        if(up){
            --frontier[y + 1][x - 1];
            if(!isCovered(y + 1, x - 1) && (aiBoard[y + 1][x - 1] != -4)){
                --aiBoard[y + 1][x - 1];
            }
        }
        if(down){
            --frontier[y - 1][x - 1];
            if(!isCovered(y - 1, x - 1) && (aiBoard[y - 1][x - 1] != -4)){
                --aiBoard[y - 1][x - 1];
            }
        }
    }
    if(x + 1 < colDim){
        --frontier[y][x + 1];
        if(!isCovered(y, x + 1) && (aiBoard[y][x + 1] != -4)){
            --aiBoard[y][x + 1];
        }
        if(up){
            --frontier[y + 1][x + 1];
            if(!isCovered(y + 1, x + 1) && (aiBoard[y + 1][x + 1] != -4)){
                --aiBoard[y + 1][x + 1];
            }
        }
        if(down){
            --frontier[y - 1][x + 1];
            if(!isCovered(y - 1, x + 1) && (aiBoard[y - 1][x + 1] != -4)){
                --aiBoard[y - 1][x + 1];
            }
        }
    }
}

void MyAI::unFakeFlag(int y, int x) {
    aiBoard[y][x] = -2;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        ++frontier[y - 1][x];
        if(!isCovered(y - 1, x) && (aiBoard[y - 1][x] != -4)){
            ++aiBoard[y - 1][x];
        }
    }
    if(y + 1 < rowDim){
        up = true;
         ++frontier[y + 1][x];
        if(!isCovered(y + 1, x) && (aiBoard[y + 1][x] != -4)) {
            ++aiBoard[y + 1][x];
        }
    }

    if(x - 1 >= 0){
        ++frontier[y][x - 1];
        if(!isCovered(y, x - 1) && (aiBoard[y][x - 1] != -4)){
            ++aiBoard[y][x - 1];
        }
            
        if(up){
            ++frontier[y + 1][x - 1];
            if(!isCovered(y + 1, x - 1) && (aiBoard[y + 1][x - 1] != -4)){
                ++aiBoard[y + 1][x - 1];
            }
        }
        if(down){
            ++frontier[y - 1][x - 1];
            if(!isCovered(y - 1, x - 1) && (aiBoard[y - 1][x - 1] != -4)){
                ++aiBoard[y - 1][x - 1];
            }
        }
    }
    if(x + 1 < colDim){
        ++frontier[y][x + 1];
        if(!isCovered(y, x + 1) && (aiBoard[y][x + 1] != -4)){
            ++aiBoard[y][x + 1];
        }
        if(up){
            ++frontier[y + 1][x + 1];
            if(!isCovered(y + 1, x + 1) && (aiBoard[y + 1][x + 1] != -4)){
                ++aiBoard[y + 1][x + 1];
            }
        }
        if(down){
            ++frontier[y - 1][x + 1];
            if(!isCovered(y - 1, x + 1) && (aiBoard[y - 1][x + 1] != -4)){
                ++aiBoard[y - 1][x + 1];
            }
        }
    }
}

//adds the adjacent squares to the queue to flag them later
void MyAI::addAdjacentToFlag(int y, int x) {
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isCovered(y - 1, x))
            flagNext.push({y - 1, x});
    }
    if(y + 1 < rowDim){
        up = true;
        if(isCovered(y + 1, x))
            flagNext.push({y + 1, x});
    }

    if(x - 1 >= 0){
        if(isCovered(y, x - 1))
            flagNext.push({y, x - 1});
        if(up){
            if(isCovered(y + 1, x - 1))
                flagNext.push({y + 1, x - 1});
        }
        if(down){
            if(isCovered(y - 1, x - 1))
                flagNext.push({y - 1, x - 1});
        }
    }
    if(x + 1 < colDim){
        if(isCovered(y, x + 1))
            flagNext.push({y, x + 1});
        if(up){
            if(isCovered(y + 1, x + 1))
                flagNext.push({y + 1, x + 1});
        }
        if(down){
            if(isCovered(y - 1, x + 1))
                flagNext.push({y - 1, x + 1});
        }
    }
}

// addAdjacentToUncover adds the adjacent elemnts to uncovNext
//  so it will uncover those next
void MyAI::addAdjacentToUncover(int y, int x)
{
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isCovered(y - 1, x))
            uncovNext.push({y - 1, x});
    }
    if(y + 1 < rowDim){
        up = true;
        if(isCovered(y + 1, x))
            uncovNext.push({y + 1, x});
    }

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
            uncovNext.push({y, x + 1});
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


int MyAI::vectAllZerosOrNeg(vector<array<int, 3>>& vect, int size) {
    bool contRecurs = false;
    int res = 0;
    for(int i = 0; i < size; i++) {
        res = allZerosOrNeg(vect[i][0], vect[i][1]);
        if(res == 1){
            //cout << "flagged to continue" << endl;
            contRecurs = true; //flag to continue recursion
        }
        else if(res == -1){
            //cout << "flagged to backtrack in this recursive step, returning -1" << endl;
            return -1;
        }
    }
    if(contRecurs){
        return 1;
    }
    else
        return 0;
}

int MyAI::allZerosOrNeg(int y, int x)
{
    bool posVals = false;
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(!isCovered(y - 1, x) && !isFlag(y - 1, x)){
            if(aiBoard[y - 1][x] == -1){
                return -1;
            }
            else if(aiBoard[y - 1][x] != 0){
                posVals = true;
            }
        }
    }
    if(y + 1 < rowDim){
        up = true;
        if(!isCovered(y + 1, x) && !isFlag(y + 1, x)){
            if(aiBoard[y + 1][x] == -1){
                return -1;
            }
            else if(aiBoard[y + 1][x] != 0){
                posVals = true;
            }
        }
    }

    if(x - 1 >= 0){
        if(!isCovered(y, x - 1) && !isFlag(y, x - 1)){
            if(aiBoard[y][x - 1] == -1){
                return -1;
            }
            else if(aiBoard[y][x - 1] != 0){
                posVals = true;
            }
        }
        if(up){
            if(!isCovered(y + 1, x - 1) && !isFlag(y + 1, x - 1)){
                if(aiBoard[y + 1][x - 1] == -1){
                    return -1;
                }
                else if(aiBoard[y + 1][x - 1] != 0){
                    posVals = true;
                }
            }
        }
        if(down){
            if(!isCovered(y - 1, x - 1) && !isFlag(y - 1, x - 1)){
                if(aiBoard[y - 1][x - 1] == -1){
                    return -1;
                }
                else if(aiBoard[y - 1][x - 1] != 0){
                    posVals = true;
                }
            }
        }
    }
    if(x + 1 < colDim){
        if(!isCovered(y, x + 1) && !isFlag(y, x + 1)){
            if(aiBoard[y][x + 1] == -1){
                return -1;
            }
            else if(aiBoard[y][x + 1] != 0){
                posVals = true;
            }
        }
        if(up){
            if(!isCovered(y + 1, x + 1) && !isFlag(y + 1, x + 1)){
                if(aiBoard[y + 1][x + 1] == -1){
                    return -1;
                }
                else if(aiBoard[y + 1][x + 1] != 0){
                    posVals = true;
                }
            }
        }
        if(down){
            if(!isCovered(y - 1, x + 1) && !isFlag(y - 1, x + 1)){
                if(aiBoard[y - 1][x + 1] == -1){
                    return -1;
                }
                else if(aiBoard[y - 1][x + 1] != 0){
                    posVals = true;
                }
            }
        }
    }
    if(posVals){
        return 1;
    }
    return 0;
}


bool MyAI::isCovered(int y, int x)
{
    if(aiBoard[y][x] == -2)
        return true;

    return false;
}

bool MyAI::isFlag(int y, int x)
{
    if(aiBoard[y][x] == -4)
        return true;

    return false;
}


void MyAI::printBoard(int **arr) {
    for(int i = rowDim - 1; i >= 0; i--) //print the higher rows first
    {
        for(int j = 0; j < colDim; j++)
        {
            std::cout << arr[i][j] << "\t";
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
