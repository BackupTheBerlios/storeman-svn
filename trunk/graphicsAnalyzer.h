#ifndef _graphicsAnalyzer_h_included_
#define _graphicsAnalyzer_h_included_

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <string>

#include "graphicsTools.h"
#include "logger.h"
#include "myExceptions.h"
#include "stopwatch.h"

class tGraphicsAnalyzer{
private:
	IplImage *img, *dst;
	IplImage *img_bw, *dst_bw;
	CvCapture* capture;
	CvVideoWriter* writer;

	tLogger *log;

	tAngleDetector *angleDetector;
	tPerspective *perspective;
	tCanny *canny;
	tHough *hough;
	tSurraundCutter *surroundCutter;
	tColorDetector *colorDetector;
	tChessboardFinder *chessboardFinder;

	tStopwatch stopwatch[TOOLSNUM];
	tStopwatch mainStopwatch;

	bool isVideo;

	static void swapImages(IplImage **a, IplImage **b); //asi staci obycajny swap
	bool linesIntersection(CvPoint &intersection, double theta1, double rho1, double theta2, double rho2);
	void drawTimes(IplImage *img);

	/**
	 * Ukazatel na seba.
	 */
	static tGraphicsAnalyzer *me;

	//void cvShowManyImages(IplImage **dst, int nArgs, ...);

public:
	static CvFont *font;
	static bool verticalFlip;

	/**
	 * @return bool vrati true ak x >= min && x < max
	 */
	static bool bounds(int x, int min, int max);

	/**
	 * Funkcia vrati nahodnu hodnotu v rozmedi 0 az hodnota.
	 * @return int 0 az hodnota-1
	 */
	static int random(int hodnota);

	/**
	 * Funkcia vrati ukazatel na pixel v konkretnom obrazku (3 kanaly po 8bitoch) na konkretnych suradniciach.
	 * @return uchar* funkcia vrati ukazatel na pixel
	 */
	static uchar * getPixel_8U3(IplImage *img, int x, int y);
	
	/**
	 * Funkcia vrati ukazatel na pixel v konkretnom obrazku (1 kanal, 8bitov) na konkretnych suradniciach.
	 * @return uchar* funkcia vrati ukazatel na pixel
	 */
	static uchar * getPixel_8U1(IplImage *img, int x, int y);


	static void on_trackBar1(int v);
	static void on_trackBar2(int v);
	static void on_trackBar3(int v);
	static void on_trackBar4(int v);

	void analyze();
	void makePhoto(std::string filename, bool verbose = false);
	void makeVideo();
	void refresh();

	tGraphicsAnalyzer();
	~tGraphicsAnalyzer();
};

typedef tGraphicsAnalyzer GA;

#endif