#pragma once
#include<string>
#include<fstream>
#include<vector>

using namespace std;

class InputCSV {
private:
    int _permutationColumnSize;
	int _permutationRowSize;

	int _permutationReelSetCount;
	int* _permutationReelSets;

    int _permutationElementsCount;
    string* _permutationElements;

	int _permutationWiningSetsCount;
	int* _permutationWiningSets;
	string* _winningSetNames;

	
	bool  OpenWinningSetFile(vector<string>&,vector<int>&);

public:
    InputCSV();
    InputCSV(string path);
    ~InputCSV();
    
    // ���o�ƦC�զX���������`��(int)
    int getPermutationElementsCount();
    // ���o�ƦC�զX������(string array)
    string* getPermutationElements();

    // get column size.
    int getPermutationColumnSize();
	// get row size.
	int getReelRowSize();

	// get reel set size
	int getReelSetSize();
	// get reel set.
	int* getReelSet();

	// get winning sets size
	int getWinningSetsSize();
	// get wining sets name
	string getWinningSetName(int);
	// get winning sets
	int* getWinningSets();

};

