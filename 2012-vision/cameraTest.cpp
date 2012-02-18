// testAlignmentAndExtraction.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#include <cxmisc.h>
#include <highgui.h>
#include <cvaux.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>

using namespace std;

int findSquares(IplImage* src)
{
	IplImage* dst = cvCreateImage( cvGetSize(src), src->depth, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour = 0;

	IplImage* src2 = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
	cvCvtColor(src, src2, CV_RGB2GRAY);
    cvThreshold( src2, src2, 50, 255, CV_THRESH_BINARY );
    cvNamedWindow( "Source", 1 );
    cvShowImage( "Source", src);
	
    cvFindContours(src2, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    cvZero( dst );

    for( ; contour != 0; contour = contour->h_next )
    {
        CvScalar color = CV_RGB( rand(), rand(), rand() );
        // replace CV_FILLED with 1 to see the outlines 
        cvDrawContours( dst, contour, color, color, -1, CV_FILLED, 8 );
    }

    cvNamedWindow( "Components", 1 );
    cvShowImage( "Components", dst );  
    cvWaitKey(0);
    
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CvCapture *capture = 0;
	IplImage *frame = 0;
	int key = 0;

	capture = cvCaptureFromCAM(0);

	if(!capture)
		printf("error!\n");

//	cvNamedWindow("test", CV_WINDOW_AUTOSIZE);

	frame = cvQueryFrame(capture);

//	cvShowImage("test", frame);

	findSquares(frame);

	//cvWaitKey(4000);

	cvReleaseCapture(&capture);
	
	return 0;
}

