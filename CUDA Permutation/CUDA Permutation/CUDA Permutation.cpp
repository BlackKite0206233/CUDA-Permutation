#include <cstdio>
#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <time.h>
#include <random>

#include "InputCSV.h"
#include "OutputCSV.h"


using namespace std;

bool Compare(int* set, int* winningSet, int size) {
    int Any = 0;
    for (int i = 0; i < size; i++) {
        if (set[i] != -1 && winningSet[i] > 0) {
            // ordinary compare
            if (set[i] != winningSet[i]) 
                return false;
        } else if (set[i] != -1 && winningSet[i] == -1) {
            // any
            if (Any == 0) 
                Any = set[i];
            else 
                if (set[i] != Any)
                    return false;
        }
    }

    return true;
}

void Simulate(const int colunmSize, const int rowSize, int* reelSets, const int reelSetSize, int* payTable, int winningSetSize, size_t runTimes, size_t* hitTimes, size_t* noHitTimes) {
    int* set;
    set = new int[colunmSize * rowSize];
    for(int t = 0; t < runTimes; t++) {
        for (int col = 0; col < colunmSize; col++) {
            unsigned int random = rand() % reelSetSize;
            for (int row = 0; row < rowSize; row++) {
                set[row + col * rowSize] = reelSets[(random + row) % reelSetSize];
            }
        }

        bool hit = false;
        for (int n = 0; n < winningSetSize; n++) {
            if (Compare(set, (payTable + colunmSize * rowSize * n), colunmSize * rowSize)) {
                hit = true;
                hitTimes[n]++;
            }
        }
        // ����no hit.
        if (!hit) 
            (*noHitTimes)++; 
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));

    const unsigned int RUN_TIMES = 50000000;

    // �[�J Console �Ѽ�
    if (argc != 3) { printf(".exe [input file] [output file]\n"); return 1; }
    string intputPath = argv[1];
    string outputPath = argv[2];

    // �]�w��J��X�ɮ�
    InputCSV inputFile(intputPath);
    OutputCSV outputFile(outputPath);

    // �p�ɶ}�l
    unsigned long cStart = clock();

    // ���� Column*Row ���L��.
    const int COLUMN_SIZE = inputFile.getPermutationColumnSize();
    const int REEL_ROW_SIZE = inputFile.getReelRowSize();

    // Symbols
    const string *SYMBOLS = inputFile.getPermutationElements();
    const int SYMBOLS_SIZE = inputFile.getPermutationElementsCount();

    // Stops.(�����I = Reel�W������)
    const int* STOPS = inputFile.getReelSet();
    const int STOPS_SIZE = inputFile.getReelSetSize();

    // Pay Table
    const int* PAY_TABLE = inputFile.getPayTable();
    // Size of pay table(Element count).
    const int PAY_TABLE_SIZE = inputFile.getPayTableSize();

    // PAY_TABLE ���ܼƪ��ƶq = sizeof(PAY_TABLE) / sizeof(int)
    const int PAY_TABLE_REAL_SIZE = PAY_TABLE_SIZE * COLUMN_SIZE * REEL_ROW_SIZE;

    //---------------------Begin of test-----------------------------
    size_t *hitTimes;
    size_t *host_hitTimes;

    size_t *host_noHitTimes;


    int* reelSets;
    int* winningSets;

    
    // �]�w thread & block.
    size_t threads = 100;
    size_t blocks = 1000;

    size_t NumOfThread = blocks * threads, kernelRunTimes = ceil(RUN_TIMES / NumOfThread);
    printf("Total times: %d\nBlock count: %d\nThread count: %d\nKernelRunTimes: %d\n", RUN_TIMES, blocks, threads, kernelRunTimes);
    

    // set memory.
    host_hitTimes =  new size_t[PAY_TABLE_SIZE];
    host_noHitTimes = new size_t;

    for(int i = 0; i < PAY_TABLE_SIZE; i++)
        host_hitTimes[i] = 0;
    *host_noHitTimes = 0;

    reelSets = new int[STOPS_SIZE];
    memcpy(reelSets, STOPS, STOPS_SIZE);

    winningSets = new int[PAY_TABLE_REAL_SIZE];
    memcpy(winningSets, PAY_TABLE, PAY_TABLE_REAL_SIZE);

    // Simulate.
    Simulate(COLUMN_SIZE, REEL_ROW_SIZE, reelSets, STOPS_SIZE, winningSets, PAY_TABLE_SIZE, 50000000, host_hitTimes, host_noHitTimes);


    //---------------------End of test-----------------------------
    // �� Hit.
    size_t totalHitTimes = 0;
    for (size_t i = 0; i < PAY_TABLE_SIZE; i++) {
        totalHitTimes += host_hitTimes[i];
    }

    cout << totalHitTimes << endl;

    // �p�ɧ��F
    unsigned long spendTime = clock() - cStart;

    // Console print.
    printf("CUDA run %lu ms.\n", spendTime);
    printf("Output to %s... \n", outputPath.c_str());

    // ��X
    outputFile.WriteTitle(blocks, threads, RUN_TIMES, spendTime, STOPS_SIZE, COLUMN_SIZE, REEL_ROW_SIZE, totalHitTimes, (double)totalHitTimes / RUN_TIMES);

    // Output No hit frequency.
    outputFile.WriteHitFreq("No Hit", *host_noHitTimes, (double)*host_noHitTimes / RUN_TIMES);

    // Output hit frequency.
    for (int i = 0; i < PAY_TABLE_SIZE; i++) {
        outputFile.WriteHitFreq(inputFile.getPayTableFileName(i), host_hitTimes[i], (double) host_hitTimes[i] / RUN_TIMES);
    }



    outputFile.Close();
    delete[] host_hitTimes;
    delete[] reelSets;
    delete[] winningSets;
    printf("Finish.\n");
    return 0;
}