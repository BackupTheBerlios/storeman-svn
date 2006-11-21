#include "graphicsAnalyzer.h"
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

tGraphicsAnalyzer *tGraphicsAnalyzer::me = 0;
CvFont *tGraphicsAnalyzer::font = 0;
bool tGraphicsAnalyzer::verticalFlip = false;
tGraphicsAnalyzer::tGraphicsAnalyzer(){
	me = this;
	dst = img = img_bw = dst_bw = 0;
	capture = 0;
	writer = 0;
	
	angleDetector = new tAngleDetector();
	perspective = new tPerspective();
	canny = new tCanny();
	hough = new tHough();
	surroundCutter = new tSurraundCutter();
	colorDetector = new tColorDetector();
	chessboardFinder = new tChessboardFinder();

	/* 
	 *	mozem si prednastavit par premennych, specifickych pre kameru,
	 *	aby som nehladal pociatocne nastavenie rucne.
	 */
	perspective->cx = 260*perspective->cy/100000;
	perspective->horizont = 139 - 200;
	perspective->cs = 253;
	perspective->scale = perspective->cs / perspective->cy;
	canny->threshold2 = 180;


	//log = new tLogger("debug.log");

	font = new CvFont();
	cvInitFont(font,CV_FONT_HERSHEY_PLAIN,1,1,0,1,CV_AA);
}

tGraphicsAnalyzer::~tGraphicsAnalyzer(){
	delete angleDetector;
	delete perspective;
	delete canny;
	delete hough;
	delete surroundCutter;
	delete colorDetector;
	delete chessboardFinder;
	
}

void tGraphicsAnalyzer::on_trackBar1(int v){
	me->perspective->cx =  v*(me->perspective->cy/100000);
	if (!me->isVideo) me->refresh();
}
void tGraphicsAnalyzer::on_trackBar2(int v){
	me->perspective->horizont =  v - 200;
	if (!me->isVideo) me->refresh();
}
void tGraphicsAnalyzer::on_trackBar3(int v){
	me->canny->threshold2 =  v;
	if (!me->isVideo) me->refresh();
}
void tGraphicsAnalyzer::on_trackBar4(int v){
	me->perspective->scale =  v/me->perspective->cy;
	if (!me->isVideo) me->refresh();
}


void tGraphicsAnalyzer::refresh(){
	analyze();
	cvShowImage("result",me->dst);
}

void tGraphicsAnalyzer::analyze(){
	
	stopwatch[COLORDETECTOR].start();
//	colorDetector->make(img);
	stopwatch[COLORDETECTOR].stop();

	stopwatch[SURROUNDCUTTER].start();
	//surroundCutter->make(img,dst,colorDetector);
	//cvCopyImage(dst, img);	
	stopwatch[SURROUNDCUTTER].stop();

	stopwatch[CANNY].start();
	canny->make(img, dst_bw, img_bw);  //zdroj, ciel, tmp
	if (verticalFlip) cvFlip(dst_bw);  //ak treba tak prevratim vertikalne
	stopwatch[CANNY].stop();

	stopwatch[PERSPECTIVE].start();
	perspective->make(dst_bw, img_bw);
	stopwatch[PERSPECTIVE].stop();

	stopwatch[CHESSBOARDFINDER].start();
	chessboardFinder->make(img_bw, dst_bw, dst);
	stopwatch[CHESSBOARDFINDER].stop();
	
	//cvCopyImage(dst_bw, img_bw);
	stopwatch[HOUGH].start();
	hough->make(img_bw);
	stopwatch[HOUGH].stop();
	
	stopwatch[ANGLEDETECTOR].start();
	angleDetector->make(hough->lineStorage);
	stopwatch[ANGLEDETECTOR].stop();
	
	cvCvtColor(img_bw, dst, CV_GRAY2BGR);

	hough->drawLines(dst);
	colorDetector->drawSample(dst);
	angleDetector->drawAngle(dst);
	drawTimes(dst);
}
void tGraphicsAnalyzer::drawTimes(IplImage *img){
	char text[32];
	sprintf_s(text, 31, "CD: %.0f", stopwatch[COLORDETECTOR].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+COLORDETECTOR*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, "SC: %.0f", stopwatch[SURROUNDCUTTER].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+SURROUNDCUTTER*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, " C: %.0f", stopwatch[CANNY].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+CANNY*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, " P: %.0f", stopwatch[PERSPECTIVE].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+PERSPECTIVE*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, " H: %.0f", stopwatch[HOUGH].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+HOUGH*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, "AD: %.0f", stopwatch[ANGLEDETECTOR].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+ANGLEDETECTOR*12), GA::font, CV_RGB(255,255,255));

	sprintf_s(text, 31, "cf: %.0f", stopwatch[CHESSBOARDFINDER].getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+CHESSBOARDFINDER*12), GA::font, CV_RGB(255,255,255));
	
	sprintf_s(text, 31, " m: %.0f", mainStopwatch.getKlzavyAverageTime());
	cvPutText(img, text, cvPoint(5,100+(TOOLSNUM+1)*12), GA::font, CV_RGB(255,255,255));
}

void tGraphicsAnalyzer::makePhoto(std::string filename, bool verbose){
	isVideo = false;
	img = cvLoadImage(filename.data());
	if(!img) throw bad_inputFile(filename);
	if(!dst) dst = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
	if(!img_bw) img_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	if(!dst_bw) dst_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);

	if (verbose) {
		//zobrazim obrazok

		cvNamedWindow("settings",1);
		int px = (int)(perspective->cx/(perspective->cy/100000));
		int ph = perspective->horizont + 200;
		int pc = canny->threshold2;
		int ps = perspective->cs;
		cvCreateTrackbar("cx", "settings", &px, 1000, (&tGraphicsAnalyzer::on_trackBar1));
		cvCreateTrackbar("horizont", "settings", &ph, 400, (&tGraphicsAnalyzer::on_trackBar2));
		cvCreateTrackbar("canny-thresh1", "settings", &pc, 200, (&tGraphicsAnalyzer::on_trackBar3));
		cvCreateTrackbar("scale", "settings", &ps, 1000, (&tGraphicsAnalyzer::on_trackBar4));


		analyze();

		cvNamedWindow("source",1);
		cvShowImage("source",img);

		cvNamedWindow("result",1);
		cvShowImage("result",dst);

		cvWaitKey();
		
		cvDestroyWindow("result");
		cvDestroyWindow("result");
	} else {
		//vyrobim nazov suboru a ulozim
		string savefile = filename;
		savefile.erase(0,savefile.find_last_of("\\")+1);
		savefile.erase(savefile.find_last_of("."));
		savefile = "out\\"+savefile+".png";

		analyze();
		cvSaveImage(savefile.data(), dst);
		cout << savefile << endl;
	}

	cvReleaseImage(&img);
	cvReleaseImage(&dst);
	cvReleaseImage(&img_bw);
	cvReleaseImage(&dst_bw);	
}

void tGraphicsAnalyzer::makeVideo(){
	isVideo = true;
#if 1	
	capture = cvCaptureFromCAM(0);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
	printf("width: %f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT));
#else
	capture = cvCaptureFromFile( "..\\in\\v_sach04.divx.avi");
#endif
	if (!capture) throw bad_inputCam();
	
	img = cvQueryFrame(capture);
	if (!img) throw bad_inputCam();
	if (!dst) dst = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
	if (!img_bw) img_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	if (!dst_bw) dst_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	if (img->origin) verticalFlip = true;

	//writer = cvCreateVideoWriter( "out\\v_sach04.avi", -1, 20, cvGetSize(img), 1);
	
	cvNamedWindow("source",1);
	cvNamedWindow("test",1);
	cvNamedWindow("result",1);

	cvNamedWindow("settings",1);
	int px = (int)(perspective->cx/(perspective->cy/100000));
	int ph = perspective->horizont + 200;
	int pc = canny->threshold2;
	int ps = perspective->cs;
	cvCreateTrackbar("cx", "settings", &px, 1000, (&tGraphicsAnalyzer::on_trackBar1));
	cvCreateTrackbar("horizont", "settings", &ph, 400, (&tGraphicsAnalyzer::on_trackBar2));
	cvCreateTrackbar("canny: thresh1", "settings", &pc, 200, (&tGraphicsAnalyzer::on_trackBar3));
	cvCreateTrackbar("scale", "settings", &ps, 1000, (&tGraphicsAnalyzer::on_trackBar4));

	while(1){
		mainStopwatch.start();
		analyze();
		cvShowImage("source",img);
		cvShowImage("result",dst);
		//cvWriteFrame(writer, img);

		int c = cvWaitKey(1);
        if (c == 27) break;
		
		img = cvQueryFrame(capture);
		if (!img) break;
		mainStopwatch.stop();
		//printf("%.2f\n",mainStopwatch.getKlzavyAverageTime());
	}
	
	cvReleaseVideoWriter(&writer);
	cvReleaseCapture(&capture);
	cvReleaseImage(&dst);
	cvReleaseImage(&img_bw);
	cvReleaseImage(&dst_bw);
}

uchar * tGraphicsAnalyzer::getPixel_8U3(IplImage *img, int x, int y){
	return &(((uchar*)(img->imageData + img->widthStep*y))[x*3]);
}

uchar * tGraphicsAnalyzer::getPixel_8U1(IplImage *img_bw, int x, int y){
	return &(((uchar*)(img_bw->imageData + img_bw->widthStep*y))[x]);
}

int tGraphicsAnalyzer::random(int hodnota){ //vrati nahodne cislo c, 0 <= c <= hodnota-1
	return int((double(rand())/RAND_MAX-0.000001)*hodnota);
}

bool tGraphicsAnalyzer::bounds(int x, int min, int max){
	return (x >= min && x < max);
}

void tGraphicsAnalyzer::swapImages(IplImage **a, IplImage **b){
	IplImage *tmp = *a;
	*a = *b;
	*b = tmp;
}

bool tGraphicsAnalyzer::linesIntersection(CvPoint &intersection, double theta1, double rho1, double theta2, double rho2){
	if (theta1 == theta2) {
		return false;
	} else if (theta1 == 0) {
		double a = tan(CV_PI/2 + theta2);
		double b = rho2 / sin(theta2);
		intersection.x = cvRound(rho1);
        intersection.y = cvRound(intersection.x*a + b);
		return true;
	} else if (theta2 == 0){
		double a = tan(CV_PI/2 + theta1);
		double b = rho1 / sin(theta1);
		intersection.x = cvRound(rho2);
        intersection.y = cvRound(intersection.x*a + b);
		return true;
	} else {
		double a1 = tan(CV_PI/2 + theta1);
		double b1 = rho1 / sin(theta1);

		double a2 = tan(CV_PI/2 + theta2);
		double b2 = rho2 / sin(theta2);

		intersection.x = cvRound((b1 - b2)/(a2 - a1));
        intersection.y = cvRound(intersection.x*a1 + b1);
		return true;
	}
}