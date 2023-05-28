#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include <stack>
#include <map>
#include <utility>

vector<vector<array<int, 3>>> treeSolutions;
bool isDuplicateVect(vector<array<int, 3>>& vect, int size) {
    for(int i = 0; i < treeSolutions.size(); i++) {
        if(vect.size() != treeSolutions[i].size()){
            continue;
        }

        int inc = 0;
        while(inc < size) {
            bool contains = false;
            for(array<int, 3> arr : treeSolutions[i]) {
                if(vect[inc][0] == arr[0] && vect[inc][1] == arr[1]) {
                    contains = true;
                    break;
                }
            }
            if(!contains) {
                return false;
            }

            inc++;
        }
        if(inc == size) {
            return true;
        }
    }
    return false;


int main() {
  vector<array<int, 3>> vect;
  treeSolutions.push_back(vect);
  treeSolutions[0].push_back({0, 1, 3});
  return 0;
}