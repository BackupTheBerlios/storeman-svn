#ifndef _stopky_h_included_
#define _stopky_h_included_

#include <mmsystem.h>   // ::timeGetTime ()
#pragma comment (lib, "winmm.lib")

#define SW_KLZAVYPRIEMER 5

class tStopwatch{
private:
	int totalTime, runs;
	int lastStartTime;
	int history[SW_KLZAVYPRIEMER], pointer;
	int historySum;
public:
	tStopwatch(){
		reset();
	};

	void start(){
		lastStartTime = timeGetTime();
	};

	void stop(){
		historySum -= history[pointer];
		totalTime += history[pointer] = timeGetTime() - lastStartTime;
		historySum += history[pointer];
		pointer = (pointer + 1) % SW_KLZAVYPRIEMER;
		runs++;
	}

	void reset(){
		totalTime = runs = 0;
		lastStartTime = timeGetTime();
		
		for (int i = 1; i < SW_KLZAVYPRIEMER; i++) history[i] = 0;
		pointer = 0;
	};

	double getAverageTime(){
		if (runs == 0) return 0;
		return double(totalTime) / runs;
	}

	double getKlzavyAverageTime(){
		if (runs < SW_KLZAVYPRIEMER) return 0;
		return double(historySum) / SW_KLZAVYPRIEMER;
	}

	int getRunsCount(){
		return runs;
	}
};

#endif