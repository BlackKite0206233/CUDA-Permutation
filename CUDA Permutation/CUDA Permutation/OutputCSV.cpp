#include "OutputCSV.h"



OutputCSV::OutputCSV() {
}

OutputCSV::OutputCSV(string path) {
    fout.open(path);
}

OutputCSV::~OutputCSV() {
}

void OutputCSV::WriteTitle(int blockCount, int threadCount, unsigned int targetRunTimes, unsigned int realRunTimes, unsigned long clock, int elementCount, int length,int reelRowSize) {
    this->fout << "Block�ƶq," << blockCount << endl;
    this->fout << "Thread�ƶq," << threadCount << endl;
    this->fout << "�w����������," << targetRunTimes << endl;
    this->fout << "��ڼ�������," << realRunTimes << endl;
    this->fout << "����ɶ�(ms)," << clock << endl;
    this->fout << "�����Ӽ�," << elementCount << endl;
    this->fout << "��L�Ӽ�," << length << endl;
	this->fout << "�X�{�Ӽ�," << reelRowSize << endl;
    this->fout << "�����զX,�X�{����,�X�{���v" << endl;
}

void OutputCSV::WriteWinningRate(string name,size_t count,double percentage){
	this->fout << name << "," << count << "," << percentage << endl;
}

void OutputCSV::WriteRowData(string permutation, int count, double percentage) {
    this->fout << permutation << "," << count << "," << percentage << "%" << endl;
}

void OutputCSV::Close() {
    if (fout.is_open()) {
        fout.close();
    }
}
