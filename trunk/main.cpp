#include "graphicsAnalyzer.h"

#include<iostream>
#include<string>

using namespace std;

int main(int argc, char** argv){
	tGraphicsAnalyzer graphicsAnalyzer;
	string filename;
	bool verbose = false;
	bool video = false;
	if (argc == 3) {
		filename = argv[2];
		if (strcmp(argv[1],"-v") || strcmp(argv[1],"/v")) verbose = true;
	} else if (argc == 2 && strcmp(argv[1],"--video") == 0) {
		video = true;
	} else if (argc == 2) {
		filename = argv[1];
	} else {
		cout << "Storeman, (c) Andrej Mikulik" << endl << "usage:" << endl << "\tstoreman.exe [-v] filename" << endl << "default filename is in\\sach_04.jpg" << endl << endl;
		filename = "in\\sach_04.jpg";
		//filename = "in\\sach_67.jpg";
		//filename = "in\\2farby.jpg";
		verbose = true;
	}

	if (video){
		try {
			graphicsAnalyzer.makeVideo();
		} catch (exception &e){
			cerr << e.what() << endl;
			return 2;
		}
	} else {
		try {
			graphicsAnalyzer.makePhoto(filename, verbose);
		} catch (exception &e){
			cerr << e.what() << endl;
			return 1;
		}
	}

	return 0;
}