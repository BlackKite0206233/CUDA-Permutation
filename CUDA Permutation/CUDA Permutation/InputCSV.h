#pragma once
#include<string>
using namespace std;

class InputCSV {
private:
    string _path;
public:
    InputCSV();
    InputCSV(string path);
    ~InputCSV();
    
    // ���o�ƦC�զX������(int)
    int ReadPermutationLength();
    // ���o�ƦC�զX������(string array)
    string* ReadPermutationElements();
};

