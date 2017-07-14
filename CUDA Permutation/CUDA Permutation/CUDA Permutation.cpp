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
/*
__device__ bool Compare(int* set, int* winningSet, int size) {
    int Any = 0;
    for (int i = 0; i < size; i++) {
        if (set[i] != -1 && winningSet[i] > 0) {
            // ordinary compare
            if (set[i] != winningSet[i]) {
                return false;
            }
        } else if (set[i] != -1 && winningSet[i] == -1) {
            // any
            if (Any == 0) {
                Any = set[i];
            } else {
                if (set[i] != Any) {
                    return false;
                }
            }
        }
    }

    return true;
}

// 跑模擬
__global__ void Simulate(curandState *states, const int colunmSize, const int rowSize, int* reelSets, const int reelSetSize, int* payTable, int winningSetSize, size_t runTimes, size_t* hitTimes, size_t* noHitTimes, const size_t NUM_OF_THREAD) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    curandState localState = states[idx];
    int* set;
    set = (int*) malloc(colunmSize*rowSize * sizeof(int));
    for (int t = 0; t < runTimes; t++) {
        for (int col = 0; col < colunmSize; col++) {
            unsigned int rand = curand(&localState) % reelSetSize;
            for (int row = 0; row < rowSize; row++) {
                set[row + col * rowSize] = reelSets[(rand + row) % reelSetSize];
            }
        }
        bool hit = false;
        for (int n = 0; n < winningSetSize; n++) {
            if (Compare(set, (payTable + colunmSize * rowSize * n), colunmSize * rowSize)) {
                hit = true;
                // Hit.
                hitTimes[idx + n * NUM_OF_THREAD] += 1;
            }
        }
        // 紀錄no hit.
        if (!hit) { noHitTimes[idx] += 1; }
        states[idx] = localState;
    }
    free(set);
};

*/

void Simulate(const int colunmSize, const int rowSize, int* reelSets, const int reelSetSize, int* payTable, int winningSetSize, size_t runTimes, size_t* hitTimes, size_t* noHitTimes, const size_t NUM_OF_THREAD) {
    int* set;
    set = new int[colunmSize * rowSize];
    for(int t = 0; t < runTimes; t++) {
        for (int col = 0; col < colunmSize; col++) {
            unsigned int random = rand() % reelSetSize;
            for (int row = 0; row < rowSize; row++) {
                set[row + col * rowSize] = reelSets[(random + row) % reelSetSize];
            }
        }
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));

    const unsigned int RUN_TIMES = 50000000;

    // 加入 Console 參數
    if (argc != 3) { printf(".exe [input file] [output file]\n"); return 1; }
    string intputPath = argv[1];
    string outputPath = argv[2];

    // 設定輸入輸出檔案
    InputCSV inputFile(intputPath);
    OutputCSV outputFile(outputPath);

    // 計時開始
    unsigned long cStart = clock();

    // 模擬 Column*Row 的盤面.
    const int COLUMN_SIZE = inputFile.getPermutationColumnSize();
    const int REEL_ROW_SIZE = inputFile.getReelRowSize();

    // Symbols
    const string *SYMBOLS = inputFile.getPermutationElements();
    const int SYMBOLS_SIZE = inputFile.getPermutationElementsCount();

    // Stops.(停止點 = Reel上的元素)
    const int* STOPS = inputFile.getReelSet();
    const int STOPS_SIZE = inputFile.getReelSetSize();

    // Pay Table
    const int* PAY_TABLE = inputFile.getPayTable();
    // Size of pay table(Element count).
    const int PAY_TABLE_SIZE = inputFile.getPayTableSize();

    // PAY_TABLE 裡變數的數量 = sizeof(PAY_TABLE) / sizeof(int)
    const int PAY_TABLE_REAL_SIZE = PAY_TABLE_SIZE * COLUMN_SIZE * REEL_ROW_SIZE;

    //---------------------Begin of test-----------------------------
    size_t *hitTimes;
    size_t *host_hitTimes;

    size_t *host_noHitTimes;


    int* reelSets;
    int* winningSets;


    // 設定 thread & block.
    size_t threads = 100;
    size_t blocks = 1000;

    size_t NumOfThread = blocks * threads, kernelRunTimes = ceil(RUN_TIMES / NumOfThread);
    printf("Total times: %d\nBlock count: %d\nThread count: %d\nKernelRunTimes: %d\n", RUN_TIMES, blocks, threads, kernelRunTimes);

    // set memory.
    hitTimes = new size_t[PAY_TABLE_SIZE];
    host_hitTimes =  new size_t[NumOfThread * PAY_TABLE_SIZE];
    host_noHitTimes = new size_t[NumOfThread];

    reelSets = new int[STOPS_SIZE];
    memcpy(reelSets, STOPS, STOPS_SIZE);

    winningSets = new int[PAY_TABLE_REAL_SIZE];
    memcpy(winningSets, PAY_TABLE, PAY_TABLE_REAL_SIZE);

    // Simulate.
    Simulate(COLUMN_SIZE, REEL_ROW_SIZE, reelSets, STOPS_SIZE, winningSets, PAY_TABLE_SIZE, kernelRunTimes, host_hitTimes, host_noHitTimes, NumOfThread);


    //---------------------End of test-----------------------------
    // 算 Hit.
    size_t totalHitTimes = 0;
    for (size_t i = 0; i < PAY_TABLE_SIZE; i++) {
        hitTimes[i] = 0;
        for (size_t t = 0; t < NumOfThread; t++) {
            hitTimes[i] += host_hitTimes[i * NumOfThread + t];
        }
        totalHitTimes += hitTimes[i];
    }
    // 算 No Hit.
    size_t noHitTimes = 0;
    for (size_t t = 0; t < NumOfThread; t++) {
        noHitTimes += host_noHitTimes[t];
    }
    // 計時完了
    unsigned long spendTime = clock() - cStart;

    // Console print.
    printf("CUDA run %lu ms.\n", spendTime);
    printf("Output to %s... \n", outputPath.c_str());

    // 輸出
    outputFile.WriteTitle(blocks, threads, RUN_TIMES, spendTime, STOPS_SIZE, COLUMN_SIZE, REEL_ROW_SIZE, totalHitTimes, (double)totalHitTimes / RUN_TIMES);

    // Output No hit frequency.
    outputFile.WriteHitFreq("No Hit", noHitTimes, (double) noHitTimes / RUN_TIMES);

    // Output hit frequency.
    for (int i = 0; i < PAY_TABLE_SIZE; i++) {
        outputFile.WriteHitFreq(inputFile.getPayTableFileName(i), hitTimes[i], (double) hitTimes[i] / RUN_TIMES);
    }



    outputFile.Close();
    delete[] hitTimes;
    printf("Finish.\n");
    return 0;
}
