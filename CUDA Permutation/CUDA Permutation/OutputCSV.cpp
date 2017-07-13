#include "OutputCSV.h"



OutputCSV::OutputCSV() {
}

OutputCSV::OutputCSV(string path) {
    fout.open(path);
}

OutputCSV::~OutputCSV() {
}

void OutputCSV::WriteTitle(int blockCount, int threadCount, size_t realRunTimes, unsigned long clock, int stopsSize, int columnSize, int rowSize, size_t totalHitTimes, double totalHitFreq) {
    this->fout << "Blocks," << blockCount << endl;
    this->fout << "Threads," << threadCount << endl;
    this->fout << "��������," << realRunTimes << endl;
    this->fout << "����ɶ�(ms)," << clock << endl;
    this->fout << "Stops," << stopsSize << endl;
    this->fout << "�L���j�p," << columnSize << "*" << rowSize << endl;
    this->fout << "Total Hit," << totalHitTimes << endl;
    this->fout << "Hit Frequency," << totalHitFreq << endl;
    this->fout << "Pay Table,Hits,Frequency" << endl;
}

void OutputCSV::WriteHitFreq(string name, size_t count, double percentage){
	this->fout << name << "," << count << "," << percentage << endl;
}


void OutputCSV::Close() {
    if (fout.is_open()) {
        fout.close();
    }
}
