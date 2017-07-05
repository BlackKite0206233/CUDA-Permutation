#pragma once
#include<string>
#include<fstream>
#include<vector>

using namespace std;

class InputCSV {
private:
    int _permutationLength;
	int _permutationRowSize;

	int _permutationReelSetCount;
	int* _permutationReelSets;

    int _permutationElementsCount;
    string* _permutationElements;

	int _permutationWiningSetCount;
	string* _permutationWiningSets;

	
	vector<string> OpenWiningSetFile(vector<string>&);

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

