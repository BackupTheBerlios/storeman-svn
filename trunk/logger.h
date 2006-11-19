#ifndef _logger_h_included_
#define _logger_h_included_

#include <iostream>
#include <fstream>
#include <windows.h>

#include "myExceptions.h"

class tLogger{
private: 
	std::ofstream out;
public:
	tLogger(std::string filename);
	void log(std::string msg, bool time);
};

#endif