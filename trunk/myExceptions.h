#ifndef _myExceptions_h_included_
#define _myExceptions_h_included_

#include <exception>
#include <string>

class bad_inputFile:public std::exception{
private:
	std::string filename; 
public:
	bad_inputFile(std::string _filename){filename = _filename;}
	const char * what() const {return ("Bad imput file ["+filename+"].").data();}
};

class bad_inputCam:public std::exception{
private:
	std::string filename;
public:
	const char * what() const {return "Could not initialize capturing.";}
};


class bad_outputFile:public std::exception{
private:
	std::string filename;
public:
	bad_outputFile(std::string _filename){filename = _filename;}
	const char * what() const {return ("Bad output file ["+filename+"].").data();}
};

#endif