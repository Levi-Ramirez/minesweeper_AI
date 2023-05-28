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
bool debug = false;
bool printBoards = false;

bool compVects(vector<array<int, 3>> v1, vector<array<int, 3>> v2) {
    return v1.size() < v2.size();
}


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
        // cout << "uncovering yLast, xLast:" << yLast << ", " << xLast << endl;
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
            //cout << "flagging" << yLast << ", " << xLast << endl;
            //printBoard(aiBoard);
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

    //we would need a vector of these vectors
    //make an array of arrays
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

    // vector<vector<array<int,3>>> myPartition;
    // myPartition.push_back(uncovAdjVect);
    vector<vector<array<int,3>>> myPartitionActual = partition();
    sort(myPartitionActual.begin(), myPartitionActual.end(), compVects); //sort partition
    // cout << "fake partition: " << endl;
    // for(auto i: myPartition) {
    //     cout << "size: " << i.size() << endl;
    //     for(auto vect : i) {
    //         cout << vect[0] << ", " << vect[1] << endl;
    //     }
    // }

    //sort partition (don't think I need this)
    for(int i  = 0; i < myPartitionActual.size(); ++i) {
        sort(myPartitionActual[i].begin(), myPartitionActual[i].end());
        sort(myPartitionActual[i].begin(), myPartitionActual[i].end());
    }

//trim down the partitions to 25:
    for(int i = 0; i < myPartitionActual.size(); ++i) {
        while(myPartitionActual[i].size() > 26) {
            myPartitionActual[i].pop_back();
        }
    }

//print actual partition
    if(debug){
        cout << "actual partition: " << endl;
        for(auto i: myPartitionActual) {
            cout << "size: " << i.size() << endl;
            for(auto vect : i) {
                cout << vect[0] << ", " << vect[1] << endl;
            }
        }
    }

    
    // cout << "parition sections: " << myPartition.size() << endl;


    // //run the tree method
    // printBoard(aiBoard);
    bool guess = true;
    int index = 0;
    vector<map<pair<int,int>, float>> soluMapVect; //create a solution map with (y, x): frequency


    for(vector<array<int,3>> curPart : myPartitionActual) {
    //test print:
        // cout << "passed partition: " << endl;
        // cout << "size: " << curPart.size() << endl;
        // for(auto vect : curPart) {
        //     cout << vect[0] << ", " << vect[1] << endl;
        // }
        treeMethod(curPart, curPart.size(), 0);
        map<pair<int,int>, float> emptyMap;
        soluMapVect.push_back(emptyMap);
        // treeMethod(uncovAdjVect, uncovAdjVect.size(), 0);
        //cout << "done treeMethod" << endl;
        int sizeSolu = treeSolutions.size(); //store the treeSolutions.size()

        

    //test print treeSolutions
        if(debug){
            for(auto vect : treeSolutions) {
                cout << "a solution: ";
                for(auto arr : vect) {
                    cout << arr[0] << ": " << arr[1] << ", ";
                }
                cout << endl;
            }
        }

        pair<int,int> curPair;
        //add solutions to a map mapping them to the count
        //loop to get the treeSolutions into the map
        for(int i = 0; i < treeSolutions.size(); i++) {
            for(int j = 0; j < treeSolutions[i].size(); j++) {
                curPair.first = treeSolutions[i][j][0];
                curPair.second = treeSolutions[i][j][1];
                if(soluMapVect[index].find(curPair) != soluMapVect[index].end()){
                    soluMapVect[index][curPair] += 1;
                }
                else {
                    soluMapVect[index][curPair] = 1;
                }
            }
        }

        //loop to calculate the frequency (by dividing by the total number of full solutions)  
        for(auto const& item: soluMapVect[index]) {
            soluMapVect[index][item.first] = item.second / sizeSolu; //gets the ratio of the frequency of the solution for every flagged vertex
            if(soluMapVect[index][item.first] == 1) { // if its 1, it needs a flag in all solutions
                flagNext.push(item.first); //push it to flagNext
                guess = false;
            }
            if(debug){cout << "soluMapVect: " << item.first.first << ", " << item.first.second << ": " << soluMapVect[index][item.first] << endl;}
        }

        if(!guess) {
            break;
        }
        //if it gets here, that means the frequencies for the last partition were calculated but none were found to be 1
        //now, incriment the index and keep going
        ++index;
        treeSolutions.clear();
    }



    if(guess) { //if no frequencies equal 1, then we need an educated guess
        float maxVal = 0;
        pair<int, int> maxPair = {-1, -1};
        for(int i = 0; i < soluMapVect.size(); i++) {
            for (const auto& item : soluMapVect[i]) {
                if(item.second > maxVal) {
                    maxVal = item.second;
                    maxPair = item.first;
                }
            }
        }

        if(maxPair.first != -1) { //need to take an educated guess
            flagNext.push(maxPair);
        }
        else { //if it gets to here, then we need to randomly guess as no solutions were found (to be done later)

        }
    }

    //run the flag process on the new found flags we found via the treeMethod
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
        uncovAdjVect.clear();
        treeSolutions.clear();
        // cout << "flagging" << yLast << ", " << xLast << endl;
        // printBoard(aiBoard);
        return {FLAG, xLast, yLast};
    }
    uncovAdjVect.clear();
    treeSolutions.clear();
    //print treeSolutions
    // for(int i = 0; i < treeSolutions.size(); i++) {
    //     cout << "solution " << i << ": ";
    //     for(int j = 0; j < treeSolutions[i].size(); j++) {
    //         cout << "[" << treeSolutions[i][j][0] << " " << treeSolutions[i][j][0] << " " << treeSolutions[i][j][0] << "] ";
    //     }
    //     cout << endl;
    // }


    //do one last dumby test (maybe)


    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
}




// ======================================================================
// YOUR CODE BEGINS
// ======================================================================


vector<vector<array<int, 3>>> MyAI::partition() // Creates partitions for the map
{
    //initialize adjBoard
    int **adjBoard;
    adjBoard = new int*[rowDim]; //initialize board
    for(int i = 0; i < rowDim; i++)
    {
        adjBoard[i] = new int[colDim];
    }
    for(int i = 0; i < rowDim; i++)
    {
        for(int j = 0; j < colDim; j++)
        {
            adjBoard[i][j] = 0; //means hasn't been accounted for
        }
    }
    
    //create stack and start DFS
    stack<pair<int,int>> dfs;
    vector<vector<array<int, 3>>> ret; //the partition
    int count = -1;
    for(int i = 0; i < rowDim; i++) //mighttt need to switch colDim to rowDim
    {
        for(int j = 0; j < colDim; j++)
        {
            if((aiBoard[i][j] > 0) && adjBoard[i][j] == 0) {
                dfs.push({i, j}); //found a new section
                vector<array<int, 3>> hold;
                ret.push_back(hold);
                ++count;
               // cout << "count: " << count << endl;;
            }
           while(!dfs.empty()) {
                pair<int,int> top = dfs.top();
                dfs.pop();
                if(adjBoard[top.first][top.second] == 1) {
                    continue;
                }
                adjBoard[top.first][top.second] = 1;
                vector<pair<int,int>> coverds = getUnseenCovereds(top, adjBoard);
                for(pair<int,int> p : coverds)
                {
                    ret[count].push_back({{p.first, p.second, 0}});
                    vector<pair<int,int>> nextCovs = adjUncovList(p, adjBoard);
                    for(pair<int,int> nextCovsPair : nextCovs) {
                        dfs.push(nextCovsPair);
                    }
                }
            }
        }
        // ++count;
    }

    for(int i = 0; i < rowDim; ++i) { //changed to rowDim
        delete[] adjBoard[i];
    }
    delete [] adjBoard;
    return ret;
} 
vector<pair<int,int>> MyAI::getUnseenCovereds(pair<int,int> cords, int **arr) {
    vector<pair<int,int>> covereds;
    int y = cords.first;
    int x = cords.second;
    
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isCovered(y - 1, x)){ //make sure its an adjacent covered square
            if((arr[y - 1][x] == 0)){ //make sure it hasn't been seen before
                covereds.push_back({y - 1, x});
                arr[y - 1][x] = 2;
            }
        }
    }
    if(y + 1 < rowDim){
        up = true;
        if(isCovered(y + 1, x)){
            if(arr[y + 1][x] == 0){
                covereds.push_back({y + 1, x});
                arr[y + 1][x] = 2;
            }
        }
    }

    if(x - 1 >= 0){
        if(isCovered(y, x - 1)) {
            if(arr[y][x - 1] == 0){
                covereds.push_back({y, x - 1});
                arr[y][x - 1] = 2;
            }
        }
        if(up){
            if(isCovered(y + 1, x - 1)){
                if(arr[y + 1][x - 1] == 0){
                    covereds.push_back({y + 1, x - 1});
                    arr[y + 1][x - 1] = 2;
                }
            }
        }
        if(down){
            if(isCovered(y - 1, x - 1)){
                if(arr[y - 1][x - 1] == 0){
                    covereds.push_back({y - 1, x - 1});
                    arr[y - 1][x - 1] = 2;
                }
            }
        }
    }
    if(x + 1 < colDim){
        if(isCovered(y, x + 1)){
            if(arr[y][x + 1] == 0){
                covereds.push_back({y, x + 1});
                arr[y][x + 1] = 2;
            }
        }
        if(up){
            if(isCovered(y + 1, x + 1)){
                if(arr[y + 1][x + 1] == 0){
                    covereds.push_back({y + 1, x + 1});
                    arr[y + 1][x + 1] = 2;
                }
            }
        }
        if(down){
            if(isCovered(y - 1, x + 1)){
                if(arr[y - 1][x + 1] == 0){
                    covereds.push_back({y - 1, x + 1});
                    arr[y - 1][x + 1] = 2;
                }
            }
        }
    }

    return covereds;
}

vector<pair<int,int>> MyAI::adjUncovList(pair<int,int> cords, int **arr) {
    vector<pair<int,int>> myList;
    int y = cords.first;
    int x = cords.second;
    if(aiBoard[y][x] != -2){
        cout << "not a covered tile" << endl;
        return myList;
    }
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(!isCovered(y - 1, x) && !isFlag(y - 1, x) && arr[y - 1][x] == 0){
            myList.push_back({y - 1, x});
        }
    }
    if(y + 1 < rowDim){
        up = true;
        if(!isCovered(y + 1, x) && !isFlag(y + 1, x) && arr[y + 1][x] == 0){
            myList.push_back({y + 1, x});
        }
    }

    if(x - 1 >= 0){
        if(!isCovered(y, x - 1) && !isFlag(y, x - 1) && arr[y][x - 1] == 0){
            myList.push_back({y, x - 1});
        }
        if(up){
            if(!isCovered(y + 1, x - 1) && !isFlag(y + 1, x - 1) && arr[y + 1][x - 1] == 0){
                myList.push_back({y + 1, x - 1});
            }
        }
        if(down){
            if(!isCovered(y - 1, x - 1) && !isFlag(y - 1, x - 1) && arr[y - 1][x - 1] == 0) {
                myList.push_back({y - 1, x - 1});
            }
        }
    }
    if(x + 1 < colDim){
        if(!isCovered(y, x + 1) && !isFlag(y, x + 1) && arr[y][x + 1] == 0){
            myList.push_back({y, x + 1});
        }
        if(up){
            if(!isCovered(y + 1, x + 1) && !isFlag(y + 1, x + 1) && arr[y + 1][x + 1] == 0){
                myList.push_back({y + 1, x + 1});
            }
        }
        if(down){
            if(!isCovered(y - 1, x + 1) && !isFlag(y - 1, x + 1) && arr[y - 1][x + 1] == 0){
                myList.push_back({y - 1, x + 1});
            }
        }
    }
    return myList;
}



void MyAI::updateAdjGroupNum(int y, int x, int **arr, int group) {
    if(aiBoard[y][x] != -2){
        cout << "error in updateAjdGroupNum" << endl;
        return;
    }
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(!isCovered(y - 1, x) && !isFlag(y - 1, x))
            arr[y - 1][x] = group;
    }
    if(y + 1 < rowDim){
        up = true;
        if(!isCovered(y + 1, x) && !isFlag(y + 1, x))
            arr[y + 1][x] = group;
    }

    if(x - 1 >= 0){
        if(!isCovered(y, x - 1) && !isFlag(y, x - 1))
            arr[y][x - 1] = group;
        if(up){
            if(!isCovered(y + 1, x - 1) && !isFlag(y + 1, x - 1))
            arr[y + 1][x - 1] = group;
        }
        if(down){
            if(!isCovered(y - 1, x - 1) && !isFlag(y - 1, x - 1))
            arr[y - 1][x - 1] = group;
        }
    }
    if(x + 1 < colDim){
        if(!isCovered(y, x + 1) && !isFlag(y, x + 1))
            arr[y][x + 1] = group;
        if(up){
            if(!isCovered(y + 1, x + 1) && !isFlag(y + 1, x + 1))
            arr[y + 1][x + 1] = group;
        }
        if(down){
            if(!isCovered(y - 1, x + 1) && !isFlag(y - 1, x + 1))
            arr[y - 1][x + 1] = group;
        }
    }

}

int MyAI::influencesThisGroup(int y, int x, int **arr) {
    bool up = false;
    bool down = false;
    if(y - 1 >= 0){
        down = true;
        if(isCovered(y - 1, x)){
            if(arr[y - 1][x] != -1){
                return arr[y - 1][x];
            }
        }
    }
    if(y + 1 < rowDim){
        up = true;
        if(isCovered(y + 1, x)){
            if(arr[y + 1][x] != -1){
                return arr[y + 1][x];
            }
        }
    }

    if(x - 1 >= 0){
        if(isCovered(y, x - 1)) {
            if(arr[y][x - 1] != -1){
                return arr[y][x - 1];
            }
        }
        if(up){
            if(isCovered(y + 1, x - 1)){
                if(arr[y + 1][x - 1] != -1){
                    return arr[y + 1][x - 1];
                }
            }
        }
        if(down){
            if(isCovered(y - 1, x - 1)){
                if(arr[y - 1][x - 1] != -1){
                    return arr[y - 1][x - 1];
                }
            }
        }
    }
    if(x + 1 < colDim){
        if(isCovered(y, x + 1)){
            if(arr[y][x + 1] != -1){
                return arr[y][x + 1];
            }
        }
        if(up){
            if(isCovered(y + 1, x + 1)){
                if(arr[y + 1][x + 1] != -1){
                    return arr[y + 1][x + 1];
                }
            }
        }
        if(down){
            if(isCovered(y - 1, x + 1)){
                if(arr[y - 1][x + 1] != -1){
                    return arr[y - 1][x + 1];
                }
            }
        }
    }

    return -1;
}


bool MyAI::isDuplicate(vector<array<int, 3>>& vect1, int size1, vector<array<int, 3>>& vect2, int size2) {
    if(size1 != size2) {
        return false;
    }
    bool found = false;
    for(int i = 0; i < size1; i++) {
        found = false;
        for(int j = 0; j < size2; j++) {
            if(vect1[i][0] == vect2[j][0] && vect1[i][1] == vect2[j][1]) {
                found = true;
                break;
            }
        }
        if(!found) {
            return false;
        }
    }
    return true;
}

//checks to see if the vertex already exists in treeSolutions
bool MyAI::isDuplicateVect(vector<array<int, 3>>& vect, int size) {
    for(vector<array<int, 3>> curSolution : treeSolutions) {
        if(isDuplicate(curSolution, curSolution.size(), vect, size)) {
            return true;
        }
    }
    return false;
}

void MyAI::treeMethod(vector<array<int, 3>>& vect, int size, int startIndex){
    int res = 0;
    if(debug && printBoards){
        cout << "called treeMethod/recursed" << endl;
        printBoard(aiBoard);
    }

    for(int i = startIndex; i < size; i++) {
        if(isFlag(vect[i][0], vect[i][1])){ //if it already is a flag, go back to the top of the loop and try again
            continue; 
        }
        fakeFlag(vect[i][0], vect[i][1]); //add a flag
        if(debug && printBoards){
            printBoard(aiBoard);
        }
        res = vectAllZerosOrNeg(vect, size); //gets a integer representation the status of the board: 0 = solution found, -1 means invalid flag combination, 1 = need to keep recurring down the tree for a solution

        if(res == 0){ //means there has been a solution
            //consider adding counter for solutions instead of vector of vectors and using the size
            vector<array<int, 3>> vectSolution;
            storeSuccess(vect, size, vectSolution); //store the flagged vectors that led to a solution here
            if(!isDuplicateVect(vectSolution, vectSolution.size())) {//if it is not a duplicate...
                treeSolutions.push_back(vectSolution); //store this vector into the possible solutions
            }
            
            vectSolution.clear();
            unFakeFlag(vect[i][0], vect[i][1]); //unflag most recent flag before going back to the top of the loop
            continue; //continue because you found a solution (aka backtrack)
        }
        else if(res == -1) { // -1 means that it was invalid flag combination
            unFakeFlag(vect[i][0], vect[i][1]); //unflag most recent flag before returning
            continue; //continue, this is not a solution (aka backtrack)
        }
        //cout << "recurs" << endl;
        ++startIndex;
        treeMethod(vect, size, startIndex); // continue recurssing if the above isn't true
        unFakeFlag(vect[i][0], vect[i][1]); // after you recurs down this branch unflag it to go to other options, ex: [-4, -2, -2] --> [-2, -2, -2], so the first one was unflagged and on the next step it will go: [-2, -4, -2]
    }
}

//storeSuccess stores the vectors that are flags solution. This is later stored in treeSolution (if not a duplicate)
void MyAI::storeSuccess(vector<array<int, 3>>& vect, int size, vector<array<int, 3>>& solution){
    for(int i = 0; i < size; i++) {
        if(isFlag(vect[i][0], vect[i][1]))
            solution.push_back(vect[i]);
    }
}

//see if the square is covered and it has adjacent uncovered squares next to it
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

// update values to make it look like a flag
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

// un-fakeFlags it by reseting the values
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


// ======================================================================
// YOUR CODE ENDS
// ======================================================================
