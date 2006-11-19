#ifndef _graphicsTools_h_included_
#define _graphicsTools_h_included_

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define ANGLEDETECTOR 0
#define PERSPECTIVE 1
#define CANNY 2
#define HOUGH 3
#define SURROUNDCUTTER 4
#define COLORDETECTOR 5
#define CHESSBOARDFINDER 6

#define TOOLSNUM 7

class tColorDetector{
private:
	int spolu8Policok(int ***a, int r, int g, int b);
	bool dostDaleko(int r1, int g1, int b1, int r2, int g2, int b2, int dist = 1);
	/**
	 * Rekurzivna funkcia, ktora hlada 2 najcastejsie vyskytujuce sa farby. Zapise ich do commonColor. (poradie farieb v IMG je BGR)
	 */
	uchar * find2color(IplImage* img, int depth=0, int rMin=0, int rMax=256, int gMin=0, int gMax=256, int bMin=0, int bMax=256);

public:
	/**
	 * Najdene 2 najcastejsie sa vyskytujuce farby
	 */
	uchar commonColor[2][3];

	/**
	 * Spocita sa log(196608/colorDistRes)/log(2)
	 */
	static const int colorDistPosun = 8;

	/**
	 * Vypocita vzdialenost dvoch farieb.
	 * @return int euklidovska vzdialenost farieb c1 a c2
	 */
	static int colorDistance(uchar *c1, uchar *c2);

	/**
	 * Vypocita vzdialenost farby od najdenych commonColor... vhodne je pred tym spustit dat pred tym make
	 * @return int euklidovska vzdialenost farieb c1 a c2
	 */
	int distanceFromCommonColor(uchar *color);

	void drawSample(IplImage *img);
	tColorDetector();
	void make(IplImage *img);
};

class tCanny{
public:
	int threshold1, threshold2;
	tCanny();
	void make(IplImage *src_img, IplImage *dst_img_bw, IplImage *tmp_img_bw);
};

class tHough{
public:
	int threshold, lineNumber;
	CvMat *lineStorage;
	tHough();
	~tHough();
	void drawLines(IplImage *img);
	void make(IplImage *img);
};

/**
 * Trieda ktora ma metody na zistovania uhlu, a jedho upravovania podla dalsich snimkov.
 * Je zavisla na Houghovom algoritme, z ktoreho potrebuje na vstu najdene ciary.
 */
class tAngleDetector{
public:
	double currentAngle;
	double maxAngleDeviation;
	tAngleDetector();
	void drawAngle(IplImage *img);
	void make(CvMat *lineStorage);
};

/**
 * Odstrani pozadie z obrazku (momentalne rozmaze). Je zavysla na detekovanych commonColor objektu colorDetector.
 */
class tSurraundCutter{
private:
	/**
	 * Farba moze byt od inej vzdialena 0 az colorDistRes-1.
	 * Hodnota musi byt 3*2^x. Urcuje akesi rozlicenie pod akym sa vytvori weight funkcia.
	 */
	static const int colorDistRes = 768; 

	void makeWeightMap(IplImage *img);
	/**
	 * ukazatel na vahovu funkciu
	 */
	double *weightFunciton;

	/**
	 * Vahova mapa. policko typu char
	 */
	CvMat *weightMap;

	/** 
	 * vytvori vahovu funkciu, podla ktorej sa bude neskor vytvarat vahova mapa. Definuje vahu pre "farebnu vzdialenost".
	 */
	void createWeightFunction();

	/**
	 * Skopiruje obrazok zo src do dst z vyuzitim vahovej funkcie. WeightMap musi byt typu uchar a rovnakej velkosti ako src a dst.
	 * Ak je vaha pixelu 1 skopiruje sa presne tak ako je. ak je vaha 0 neskopiruje sa vobec. Pre hodnoty medzi sa kopirovany pixel zamiesa
	 * do povodneho obrazu.
	 */
	void copyWeightImage(IplImage *src, IplImage *dst);
public:
	tSurraundCutter();
	~tSurraundCutter();
	void make(IplImage *img, IplImage *dst, tColorDetector *colorDetector);
};

class tPerspective{
public:
	double cx, cy;
	double scale;
	int horizont;
	int cs;
	tPerspective();
	void detectHorizont(IplImage *img_bw, CvMat *lineStorage);
	void make(IplImage *img_bw, IplImage *dst_bw);
};

class tChessboardFinder{
public:
	CvPoint origin, vector;
	bool found;
	tChessboardFinder();
	void make(IplImage *img_bw, IplImage *tmp_bw);
};

#endif