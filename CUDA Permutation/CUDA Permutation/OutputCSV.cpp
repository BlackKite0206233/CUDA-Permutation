#include "OutputCSV.h"



OutputCSV::OutputCSV() {
}

OutputCSV::OutputCSV(string path) {
    fout.open(path);
}

OutputCSV::~OutputCSV() {
}

void OutputCSV::WriteTitle(int blockCount, int threadCount, int targetRunTimes, int realRunTimes, int clock, int elementCount, int length, int permutationCount) {
    this->fout << "Block�ƶq," << blockCount << endl;
    this->fout << "Thread�ƶq," << threadCount << endl;
    this->fout << "�w����������," << targetRunTimes << endl;
    this->fout << "��ڼ�������," << realRunTimes << endl;
    this->fout << "����ɶ�(ms)," << clock << endl;
    this->fout << "�����Ӽ�," << elementCount << endl;
    this->fout << "��L�Ӽ�," << length << endl;
    this->fout << "�ƦC�զX�`��," << permutationCount << endl;
    this->fout << "�ƦC�զX,�X�{����,�X�{���v" << endl;
}

void OutputCSV::WriteRowData(string permutation, int count, double percentage) {
    this->fout << permutation << "," << count << "," << percentage << "%" << endl;
}

void OutputCSV::Close() {
    if (fout.is_open()) {
        fout.close();
    }
}
