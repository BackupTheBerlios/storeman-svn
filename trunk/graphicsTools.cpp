#include "graphicsTools.h"
#include "graphicsAnalyzer.h"

/*
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;
*/

tColorDetector::tColorDetector(){
	for (int i = 0; i < 3; i++) commonColor[0][i] = commonColor[1][i] = 0;
}

void tColorDetector::drawSample(IplImage* img){
	//printf("prva farba: %d, %d, %d\ndruha farba: %d, %d, %d\n",commonColor[0][0],commonColor[0][1],commonColor[0][2],commonColor[1][0],commonColor[1][1],commonColor[1][2]);
	//nakresilm dva stvorce danych farieb do obrazu
	cvRectangle(img,cvPoint(0,18),cvPoint(80,34),cvScalar(commonColor[0][0],commonColor[0][1],commonColor[0][2]),CV_FILLED);
	cvRectangle(img,cvPoint(0,36),cvPoint(80,52),cvScalar(commonColor[1][0],commonColor[1][1],commonColor[1][2]),CV_FILLED);
}

void tColorDetector::make(IplImage* img){
	find2color(img);
}

uchar * tColorDetector::find2color(IplImage* img, int depth, int rMin, int rMax, int gMin, int gMax, int bMin, int bMax){
	int bodov = 500;
	int iteracii = 2; //zmysel ma 1 a viac
	int dist = 2;
	int rozklad;
	if (depth == 0) rozklad = 32;
	else if (depth == 1) rozklad = 16;
	else rozklad = 1; //dalsie iteracie uz nerobia upresnenie
	
	if (depth == iteracii){
		uchar * u = new uchar[3];
		u[0] = int((rMin+rMax)/2);
		u[1] = int((gMin+gMax)/2);
		u[2] = int((bMin+bMax)/2);
		return u;
	}

	//pripravim si pole kam budem ukladat farby
	int ***a = (int***) new int**[rozklad];
	for(int x = 0; x < rozklad; x++){
		a[x] = (int**) new int*[rozklad];
		for(int y = 0; y < rozklad; y++){
			a[x][y] = (int*) new int[rozklad];
			for(int z = 0; z < rozklad; z++) a[x][y][z] = 0;
		}
	}

	uchar * px;
	//vyberem nahodnych #bodov bodov a zapisem ich do matice
	int i = 0;
	int R,G,B;
	while (i < bodov){
		//printf("%d\n",i);
		px = GA::getPixel_8U3(img, GA::random(img->width),GA::random(img->height));
		if (GA::bounds(px[0],rMin,rMax) && GA::bounds(px[1],gMin,gMax) && GA::bounds(px[2],bMin,bMax)){
			R = (px[0]-rMin)/((rMax-rMin)/rozklad);
			G = (px[1]-gMin)/((gMax-gMin)/rozklad);
			B = (px[2]-bMin)/((bMax-bMin)/rozklad);
			a[R][G][B]++;
		}
		else i--;
		i++;
	}	

	int max1[3] = {0,0,0};
	int max2[3] = {0,0,0};
	//kontrolujem vzdy rovno 8 susednych policok
	for(int r = 0; r < rozklad -1; r++){
		for(int g = 0; g < rozklad -1; g++){
			for(int b = 0; b < rozklad -1; b++){
				if (spolu8Policok(a,r,g,b) > spolu8Policok(a,max1[0],max1[1],max1[2])){					
					if (dostDaleko(r,g,b,max1[0],max1[1],max1[2],dist)){
						max2[0] = max1[0];	max2[1] = max1[1];	max2[2] = max1[2];
					} else;
					max1[0] = r;	max1[1] = g;	max1[2] = b;					
				} else if (spolu8Policok(a,r,g,b) > spolu8Policok(a,max2[0],max2[1],max2[2]) && dostDaleko(r,g,b,max1[0],max1[1],max1[2],dist)){
					max2[0] = r;		max2[1] = g;		max2[2] = b;
				}
			}
		}
	}

	for(int x = 0; x < rozklad; x++){
		for (int y = 0; y < rozklad; y++)
			delete [] (a[x][y]);
		delete [] (a[x]);
	}
	delete [] a;

	int diel = (rMax-rMin)/rozklad;
	if (depth == 0){
		uchar * u0 = find2color(img, depth+1, rMin+max1[0]*diel, rMin+(max1[0]+2)*diel, gMin+max1[1]*diel, gMin+(max1[1]+2)*diel, bMin+max1[2]*diel, bMin+(max1[2]+2)*diel);
		uchar * u1 = find2color(img, depth+1, rMin+max2[0]*diel, rMin+(max2[0]+2)*diel, gMin+max2[1]*diel, gMin+(max2[1]+2)*diel, bMin+max2[2]*diel, bMin+(max2[2]+2)*diel);
		for (int i = 0; i < 3; i++){
			commonColor[0][i] = u0[i];
			commonColor[1][i] = u1[i];
		}
		delete [] u0;
		delete [] u1;
		return 0;
	} else return find2color(img, depth+1, rMin+max1[0]*diel, rMin+(max1[0]+2)*diel, gMin+max1[1]*diel, gMin+(max1[1]+2)*diel, bMin+max1[2]*diel, bMin+(max1[2]+2)*diel);
}

int tColorDetector::spolu8Policok(int ***a, int r, int g, int b){
	return	a[r][g][b] + a[r+1][g][b] + a[r][g+1][b] + a[r][g][b+1] + a[r+1][g+1][b] + a[r+1][g][b+1] + a[r][g+1][b+1] + a[r+1][g+1][b+1];
}

bool tColorDetector::dostDaleko(int r1, int g1, int b1, int r2, int g2, int b2, int dist){
	//return true;
	bool b = (abs(r1 - r2) > dist || abs(g1 - g2) > dist || abs(b1 - b2) > dist);
	int a = abs(b1 - b2);
	return b;
}

int tColorDetector::distanceFromCommonColor(uchar *color){
	return min(colorDistance(color,commonColor[0]),colorDistance(color,commonColor[1]));
}

int tColorDetector::colorDistance(uchar *c1, uchar *c2){ // (0 az 196608) << posun = 0 az colorDistRes
	return ((c1[0]-c2[0])*(c1[0]-c2[0])+(c1[1]-c2[1])*(c1[1]-c2[1])+(c1[2]-c2[2])*(c1[2]-c2[2])) >> colorDistPosun;
}


tCanny::tCanny(){
	threshold1 = 30;
	threshold2 = 180;	
}

void tCanny::make(IplImage *src_img, IplImage *dst_img_bw, IplImage *tmp_img_bw){
	//TODO: pridat nejake adaptacie tresholdu
	cvCvtColor(src_img, tmp_img_bw, CV_BGR2GRAY);
	cvCanny(tmp_img_bw, dst_img_bw, threshold1, threshold2, 3 ); //ta trojka som uz zabudol co znamena...
}

tHough::tHough(){
	threshold = 280;
	lineNumber = 7;
	lineStorage = cvCreateMat(1, lineNumber + 1, CV_32FC2);
}

tHough::~tHough(){
	cvReleaseMat(&lineStorage);
}

void tHough::drawLines(IplImage *img){
	for (int i = 0; i < lineStorage->cols; i++ ){
		float* line = lineStorage->data.fl + i*2;
        double rho = line[0];
        double theta = line[1];
		if (0.01 > abs(theta - CV_PI/2)) continue; //ciara vyhodnotena ako vodorovna z obrazom. Tie zahadzujem koli moznej chybe.

        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
		cvLine(img, pt1, pt2, CV_RGB(20,20,255), 2, 8 );
		/*
		double aa = tan(CV_PI/2 + theta);
		double bb = rho / sin(theta);
		pt1.x = cvRound(0);
        pt1.y = cvRound(pt1.x*aa + bb);
        pt2.x = cvRound(500);
        pt2.y = cvRound(pt2.x*aa + bb);
		cvLine(img, pt1, pt2, CV_RGB(20,255,0), 1, 8 );
		*/
	}
}

void tHough::make(IplImage *img){
	double adaptacia = 0.02;  //v percentach uprava hough_thresh
	
	lineStorage->cols = lineNumber + 1;
	lineStorage->rows = 1;

	//aplikacia houghovej transformacie
	cvHoughLines2(img, lineStorage, CV_HOUGH_STANDARD, 1, CV_PI/180, threshold, 0, 0 );

	//adaptacia houghovho algoritmu na viditelnost
	if (lineStorage->cols > lineNumber) threshold = int(threshold * (1 + adaptacia));
	else if (lineStorage->cols < lineNumber) threshold = int(threshold * (1 - adaptacia));
}

tAngleDetector::tAngleDetector(){
	//nastavenie pociatocneho uhlu
	currentAngle = CV_PI/2;
	//od minuleho uhlu sa moze tento odchylit o x stupnov
	maxAngleDeviation = CV_PI/4;	
}

void tAngleDetector::drawAngle(IplImage *img){
	char uhol[9];
	cvRectangle(img,cvPoint(0,0),cvPoint(80,16),cvScalar(255,255,255),CV_FILLED);

	sprintf_s(uhol, 9, "%.0f deg", currentAngle * 180 / CV_PI);
	//printf("%s\n", uhol);
	cvPutText(img, uhol, cvPoint(5,14), GA::font, CV_RGB(0,0,0));
}

void tAngleDetector::make(CvMat *lineStorage){

	double oldAngle = currentAngle;
	//while (oldAngle >= CV_PI/2) oldAngle -= CV_PI/2; 
	
	double deviation = maxAngleDeviation;
	bool found = false;
	
	for (int i = 0; i < lineStorage->cols; i++ ){
		float* line = lineStorage->data.fl + i*2;
		double newAnlge = double(line[1]);		
		if (0.01 > abs(newAnlge - CV_PI/2)) continue;
		
		newAnlge -= oldAngle;
		while (newAnlge > CV_PI/4) newAnlge -= CV_PI/2;
		while (newAnlge <= -CV_PI/4) newAnlge += CV_PI/2;
		
		if (abs(newAnlge) < abs(deviation)) {
			deviation = newAnlge;
			found = true;
		}
	}
	if (found) {
		//printf("%f\n",maxAngleDeviation);
		currentAngle += deviation;
		//while (currentAngle < 0) currentAngle += 2 * CV_PI;
	}

}

tSurraundCutter::tSurraundCutter(){
	weightFunciton = 0;
	weightMap = 0;
	createWeightFunction();
}

tSurraundCutter::~tSurraundCutter(){
	cvReleaseMat(&weightMap);
}

void tSurraundCutter::makeWeightMap(IplImage *img){
	weightMap = cvCreateMat(img->height,img->width,CV_32F);
}

void tSurraundCutter::createWeightFunction(){
	weightFunciton = new double[colorDistRes];
	bool okruhli = false;
	for (int x = 0; x < colorDistRes; x++){
		if (okruhli) weightFunciton[x] = 1;
		else {
			weightFunciton[x] = (atan((x - double(colorDistRes)/150)*20*10/colorDistRes)*2/3.14f + 1)/2*atan(double(x)/2)*2/3.14f;
			if (weightFunciton[x] > 0.95) okruhli = true;
		}
//		printf("wF[%d] = %.4f\n",x,weightFunciton[x]);
	}
}

void tSurraundCutter::make(IplImage *img, IplImage *dst, tColorDetector *colorDetector){
	if (!weightMap) makeWeightMap(img);

	//vytvorim si vahovu mapu s vahou blizkou 1 ak je farba blizko jednej z dvoch commonColors
	for (int y = 0; y < img->height; y++)
		for (int x = 0; x < img->width; x++) 
			((double*)(weightMap->data.ptr + weightMap->step * y))[x] = 1 - weightFunciton[colorDetector->distanceFromCommonColor(GA::getPixel_8U3(img,x,y))];
	
	//rozmazem cely obraz ale potom to co bolo "najtazsie vo vahovej mape prekopirujem nerozmazane
	cvSmooth(img,dst,CV_BLUR,23,23); //pomaleeeeeeeeeeeeeee!!!
	copyWeightImage(img,dst);
}

void tSurraundCutter::copyWeightImage(IplImage *src, IplImage *dst){
	if (src->width != dst->width || src->height != dst->height) throw "myCopyImage dostal dva nerovnakovelke obrazky";
	for(int x = 0; x < src->width; x++)
		for(int y = 0; y < src->height; y++){
			double w = ((double*)(weightMap->data.ptr + weightMap->step*y))[x];
			GA::getPixel_8U3(dst,x,y)[0] = int((1-w)*GA::getPixel_8U3(dst,x,y)[0] + w*GA::getPixel_8U3(src,x,y)[0]);
			GA::getPixel_8U3(dst,x,y)[1] = int((1-w)*GA::getPixel_8U3(dst,x,y)[1] + w*GA::getPixel_8U3(src,x,y)[1]);
			GA::getPixel_8U3(dst,x,y)[2] = int((1-w)*GA::getPixel_8U3(dst,x,y)[2] + w*GA::getPixel_8U3(src,x,y)[2]);
		}
}

tPerspective::tPerspective(){
	cy = 1000000000;
	cx = cy * 0.001; //cy * 0.0016; pre martinovu kameru
	cs = 100;
	scale = cs/cy;
	horizont = 0-200+35; //tento nema byt pevne nastaveny... zatial je
}

void tPerspective::detectHorizont(IplImage *img_bw, CvMat *lineStorage){
/*
	vector<CvPoint> points;
	CvPoint tmpPoint;
	for(int i = 0; i < hough_storage->cols; i++){
		float* line1 = hough_storage->data.fl + i*2;
		double rho1 = line1[0];
		double theta1 = line1[1];
		for(int j = i+1; j < hough_storage->cols; j++){
			float* line2 = hough_storage->data.fl + j*2;
			double rho2 = line2[0];
			double theta2 = line2[1];
			if (linesIntersection(tmpPoint,theta1,rho1,theta2,rho2)) points.push_back(tmpPoint);
		}
		printf("%d:  theta = %f\n",i,theta1);
	}	

	for (unsigned int i = 0; i < points.size(); i++){
		for (int j = 0; j < 4; j++) getPixel_8U3(img,j,points[i].y)[2]=255;
		if (points[i].x < 640) for (int j = 0; j < 4; j++) getPixel_8U3(img,points[i].x,points[i].y+j)[2]=255;
		printf("%3d:  x = %4d    y = %4d\n",i,points[i].x,points[i].y);
	}
	for (int j = 0; j < 4; j++) getPixel_8U3(img,5+j,150)[1]=255;
*/
}

void tPerspective::make(IplImage *img_bw, IplImage *dst_bw){
	//detectHorizont(img, lineStorage???);

	// pomocna premenna stred obrazku
	int stred = img_bw->width/2;

	//pre debug namalujem ciaru horizontu (nevime ci funguje na bw image
	//cvLine(img_bw, cvPoint(0,horizont), cvPoint(img_bw->width - 1,horizont), CV_RGB(120,20,255), 1, 8 );

	
	//vymazem ciel
	cvSetZero(dst_bw);

	//printf("cx | cy = %.1f | %.1f --- cx/cy = %.5f\n",perspective_cx, perspective_cy, perspective_cx/perspective_cy);
	for (int y = max(horizont + 1, 0); y < img_bw->height; y++){
		double z = double(y - horizont)/(img_bw->height - horizont - 1);  //z = 0 na horizonte, z = 1 v spodnom riadku
		double posunY = cy / z - cy;
		for (int x = 0; x < img_bw->width; x++) {
			//presuvam iba biele bodky
			if (*(GA::getPixel_8U1(img_bw,x,y)) == 0) continue;

			double posunX = cx * (x - stred) / z;
			int xn = int((x - stred + posunX ) * scale + stred);
			int yn = int(img_bw->height - (img_bw->height - (y - posunY)) * scale);
			if (GA::bounds(xn,0,img_bw->width) && GA::bounds(yn,0,img_bw->height)){
				GA::getPixel_8U1(dst_bw,xn,yn)[0] = GA::getPixel_8U1(img_bw,x,y)[0];
				//getPixel_8U3(dst_bw,xn,yn)[1] = getPixel_8U3(img_bw,x,y)[1];
				//getPixel_8U3(dst_bw,xn,yn)[2] = getPixel_8U3(img_bw,x,y)[2];
			}
		}
	}
}

tChessboardFinder::tChessboardFinder(){
	found = false;
}

void tChessboardFinder::make(IplImage *img_bw, IplImage *tmp_bw){
	CvSeq *aprox, *contours, *result;
	CvMemStorage* storage = cvCreateMemStorage(0); //zatial neviem naco... treba to v cvApprox Poly a v cvFindContours
	cvDilate(img_bw, tmp_bw);
	// find contours and store them all as a list
	cvFindContours( tmp_bw, storage, &contours, sizeof(CvContour),CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	aprox = cvApproxPoly( contours, sizeof(CvContour), storage,CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );


	cvPolyLine( tmp_bw, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );


	cvShowImage("test",tmp_bw);	
}