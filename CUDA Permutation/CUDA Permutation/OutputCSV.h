#pragma once
#include<string>
using namespace std;

class OutputCSV {
private:
    string _path;
public:
    OutputCSV();
    OutputCSV(string path);
    ~OutputCSV();

    // ��X���(�ѼƦۤv�[)
    void WriteData();
};

