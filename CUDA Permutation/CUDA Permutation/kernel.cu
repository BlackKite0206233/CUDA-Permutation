#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <curand.h>
#include <curand_kernel.h>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <string>
#include<iostream>

#include "InputCSV.h"
#include "OutputCSV.h"


using namespace std;

__device__ bool Compare(int* set, int* winningSet, int size) {
    int Any = 0;
    for (int i = 0; i < size; i++) {
        if (winningSet[i] > 0) {
            // ordinary compare
            if (set[i] != winningSet[i]) {
                return false;
            }
        } else if (winningSet[i] == -1) {
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

// �]�w�C��kernel���üƺؤl
__global__ void SetupCurand(curandState *state, unsigned long seed) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    curand_init(seed, idx, 0, &state[idx]);
}
// �]����
__global__ void Simulate(curandState *states, int colunmSize, int rowSize, int* reelSets, int reelSetSize, int* winningSets, int winningSetSize, size_t runTimes, size_t* winningSetCount) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    curandState localState = states[idx];
    int* set;
    set = (int*) malloc(colunmSize*rowSize * sizeof(int));
    for (int t = 0; t < runTimes; t++) {
        for (int col = 0; col < colunmSize; col++) {
            unsigned int rand = curand(&localState) % reelSetSize;
            for (int row = 0; row < rowSize; row++) {
                set[row + col*rowSize] = reelSets[(rand + row) % reelSetSize];
            }
        }
        for (int n = 0; n < winningSetSize; n++) {
            if (Compare(set, (winningSets + colunmSize*rowSize*n), colunmSize*rowSize)) {
                atomicAdd(winningSetCount + n, 1);
            }
        }
        // Validate run time.
        //atomicAdd(realRunTimes, 1);
        states[idx] = localState;
    }
    free(set);
};

int main(int argc, char** argv) {
    // �[�J�Ѽ�
    if(argc != 3){ printf(".exe [input file] [output file]\n"); return 1; }
    string intputPath = argv[1];
    string outputPath = argv[2];

    unsigned long cStart = clock();
    InputCSV inputFile(intputPath);
    OutputCSV outputFile(outputPath);
    const unsigned int RUN_TIMES = 100000000;
    const int COLUMN_SIZE = inputFile.getPermutationColumnSize();
    const int REEL_ROW_SIZE = inputFile.getReelRowSize();

    const string *ELEMENTS = inputFile.getPermutationElements();
    const int ELEMENTS_SIZE = inputFile.getPermutationElementsCount();

    const int* REEL_SETS = inputFile.getReelSet();
    const int REEL_SET_SIZE = inputFile.getReelSetSize();

    const int* WINNING_SETS = inputFile.getWinningSets();
    // Size of winning sets(Element count).
    const int WINNING_SETS_SIZE = inputFile.getWinningSetsSize();

    // = sizeof(WINNING_SETS)/sizeof(int)
    const int WINNING_SET_REAL_SIZE = WINNING_SETS_SIZE * COLUMN_SIZE * REEL_ROW_SIZE;

    //---------------------Begin of cuda-----------------------------
    size_t *winningSetCount;
    size_t *dev_winningSetCount;

    int* dev_reelSets;
    int* dev_winningSets;


    // �]�w thread & block.
    unsigned int threads = 10;
    unsigned int blocks = 1000;

    unsigned int NumOfThread = blocks * threads, kernelRunTimes = ceil(RUN_TIMES / NumOfThread);
    printf("Total times: %d\nBlock count: %d\nThread count: %d\nKernelRunTimes: %d\n", RUN_TIMES, blocks, threads, kernelRunTimes);

    // �t�mHost memory.
    winningSetCount = (size_t*) malloc(WINNING_SETS_SIZE * sizeof(size_t));


    // �t�mDevice memory.
    cudaMalloc((void**) &dev_winningSetCount, WINNING_SETS_SIZE * sizeof(size_t));


    // Declare reel sets.
    cudaMalloc((void**) &dev_reelSets, REEL_SET_SIZE * sizeof(int));
    cudaMemcpy(dev_reelSets, REEL_SETS, REEL_SET_SIZE * sizeof(int), cudaMemcpyHostToDevice);

    // Declare winning sets.
    cudaMalloc((void**) &dev_winningSets, WINNING_SET_REAL_SIZE * sizeof(int));
    cudaMemcpy(dev_winningSets, WINNING_SETS, WINNING_SET_REAL_SIZE * sizeof(int), cudaMemcpyHostToDevice);

    // Setup random seed for each threads.
    curandState* devStates;
    cudaMalloc(&devStates, NumOfThread * sizeof(curandState));
    SetupCurand << <blocks, threads >> > (devStates, time(NULL));

    // Simulate.
    Simulate <<<blocks, threads >> > (devStates, COLUMN_SIZE, REEL_ROW_SIZE, dev_reelSets, REEL_SET_SIZE, dev_winningSets, WINNING_SETS_SIZE, kernelRunTimes, dev_winningSetCount);

    // Copy device memory to host.
    cudaMemcpy(winningSetCount, dev_winningSetCount, WINNING_SETS_SIZE * sizeof(size_t), cudaMemcpyDeviceToHost);


    //����Memory.
    cudaFree(dev_reelSets);
    cudaFree(dev_winningSets);
    cudaFree(dev_winningSetCount);

    //---------------------End of cuda-----------------------------

    unsigned long cEnd = clock();
    printf("CUDA run %lu ms.\n", cEnd - cStart);

    printf("Output to %s... \n", outputPath.c_str());

    // ��X
    outputFile.WriteTitle(blocks, threads, RUN_TIMES, RUN_TIMES, cEnd - cStart, ELEMENTS_SIZE, COLUMN_SIZE, REEL_ROW_SIZE);

    //output winning rate ot csv file.
    for (int i = 0; i < WINNING_SETS_SIZE; i++) {
        //[TEMP]
        outputFile.WriteWinningRate(inputFile.getWinningSetName(i), winningSetCount[i], ((double) winningSetCount[i] / RUN_TIMES));
    }



    outputFile.Close();

    delete[] winningSetCount;

    printf("Finish.\n");
    system("PAUSE");
    return 0;
}