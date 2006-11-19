#include "logger.h"

using namespace std;

tLogger::tLogger(std::string filename){
	out.open(filename.data(),ios_base::out);
	bad_outputFile a(filename);
	if (!out.good()) throw bad_outputFile(filename);
}

void tLogger::log(std::string msg, bool time){
	if (time){
		SYSTEMTIME t;
		GetSystemTime(&t);
		char strTime[64];
		sprintf_s(strTime,63,"[%d.%d. %d:%d:%d]",t.wDay,t.wMonth,t.wHour,t.wMinute,t.wSecond);
		out << strTime;
	}
	out << msg.data() << endl;
}