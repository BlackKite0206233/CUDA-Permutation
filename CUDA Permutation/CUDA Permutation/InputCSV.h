#pragma once
#include<string>
#include<fstream>
#include<vector>

using namespace std;

class InputCSV {
private:
    int _permutationLength;
    int _permutationElementsCount;
    string* _permutationElements;
public:
    InputCSV();
    InputCSV(string path);
    ~InputCSV();
    
    // ���o�ƦC�զX������(int)
    int getPermutationLength();
    // ���o�ƦC�զX���������`��(int)
    int getPermutationElementsCount();
    // ���o�ƦC�զX������(string array)
    string* getPermutationElements();
};

