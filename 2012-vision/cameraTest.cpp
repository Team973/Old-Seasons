



#include "stdafx.h"

#include <cv.h>
#include <cxmisc.h>
#include <math.h>
#include <highgui.h>
#include <cvaux.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include <iostream>


using namespace std;

// looking for white surfaces:
int high_thresh = 255;
int low_thresh = 230;
IplImage* img = 0;
IplImage* img0 = 0;
CvMemStorage* storage = 0;
const char* wndname = "Square Detection";

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
CvSeq* findRectangles( IplImage* img, CvMemStorage* storage )
{
    CvSeq* contours;
    int i, c, l, N = 11;
    CvSize sz = cvSize( img->width & -2, img->height & -2 );
    IplImage* timg = cvCloneImage( img ); // make a copy of input image
    IplImage* gray = cvCreateImage( sz, 8, 1 );
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

    // down-scale and upscale the image to filter out the noise
    cvPyrDown( timg, pyr, 7 );
    cvPyrUp( pyr, timg, 7 );
    tgray = cvCreateImage( sz, 8, 1 );

    // find squares in every color plane of the image
    for( c = 0; c < 3; c++ )
    {
        // extract the c-th color plane
        cvSetImageCOI( timg, c+1 );
        cvCopy( timg, tgray, 0 );

        // try several threshold levels (skip for now)
        //for( l = 0; l < N; l++ )
        {
            cvThreshold( tgray, gray, low_thresh, high_thresh, CV_THRESH_BINARY );

			// add some fluff:
			cvDilate(gray, gray, NULL, 10);
			cvErode(gray, gray, NULL, 8);

            // find contours and store them all as a list
            cvFindContours( gray, storage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly( contours, sizeof(CvContour), storage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result->total == 4 &&
                    cvContourArea(result,CV_WHOLE_SEQ,0) > 1000 &&
                    cvCheckContourConvexity(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( result, i ),
                            (CvPoint*)cvGetSeqElem( result, i-2 ),
                            (CvPoint*)cvGetSeqElem( result, i-1 )));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    //if( s < 0.3 )
                    if (s < 0.5) 
						for( i = 0; i < 4; i++ )
                            cvSeqPush( squares, (CvPoint*)cvGetSeqElem( result, i ));
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    }

    // release all the temporary images
    cvReleaseImage( &gray );
    cvReleaseImage( &pyr );
    cvReleaseImage( &tgray );
    cvReleaseImage( &timg );

    return squares;
}

int compareCvPointX(const void* point1, const void* point2)
{
	CvPoint* cPoint1 = (CvPoint*) point1; 
	CvPoint* cPoint2 = (CvPoint*) point2;

	return (cPoint1->x - cPoint2->x);	
}

int compareCvPointY(const void* point1, const void* point2)
{
	CvPoint* cPoint1 = (CvPoint*) point1; 
	CvPoint* cPoint2 = (CvPoint*) point2;

	return (cPoint1->y - cPoint2->y);	
}

// the function draws all the squares in the image
void drawSquares( IplImage* img, CvSeq* squares )
{
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage( img );
    int i;

    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );

    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint pt[4], *rect = pt;
        int count = 4;

        // read 4 vertices
        CV_READ_SEQ_ELEM( pt[0], reader );
        CV_READ_SEQ_ELEM( pt[1], reader );
        CV_READ_SEQ_ELEM( pt[2], reader );
        CV_READ_SEQ_ELEM( pt[3], reader );

        // draw the square as a closed polyline
        cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );

		// find the max and min pixel values of the square
		qsort(pt, count, sizeof(CvPoint), compareCvPointY);
		int distance = abs((pt[0].y+pt[1].y)/2 - (pt[2].y+pt[3].y)/2);
		printf("average height %d \n", distance);
		
    }

    // show the resultant image
    cvShowImage( wndname, cpy );
    cvReleaseImage( &cpy );
}




int main(int argc, char** argv)
{
    int c = 0;
	char *imgName = "Picture 15.jpg";
    // create memory storage that will contain all the dynamic data
    storage = cvCreateMemStorage(0);
	CvSeq *foundSquares;

	bool useStaticImage = false;
	bool drawImageSquare = true;

	if (drawImageSquare)
		cvNamedWindow( wndname, 1 );
	
	if (useStaticImage) // use image listed above
	{
		img = cvLoadImage( imgName, 1 );
		img0 = cvCloneImage( img );
		if( !img0 )
		{
			printf("Couldn't load image from CAM0\n");
			return -1;
		}
		foundSquares = findRectangles(img0, storage);

		if (drawImageSquare)
			drawSquares( img0, foundSquares);
			
		c = cvWaitKey(0);
		cvClearMemStorage( storage );
		cvReleaseImage( &img0 );
	}
	else // import from webcam
	{
		CvCapture *capture = 0;
		capture = cvCaptureFromCAM(0);
		if(!capture)
		{
			printf("error reading from CAM0!\n");
			return -1;
		}

		// exit loop by pressing q 
		while (c != 'q')
		{
			img = cvQueryFrame(capture);
			img0 = cvCloneImage( img );

			if( !img0 )
			{
				printf("Couldn't load image from CAM0\n");
				return -1;
			}
			foundSquares = findRectangles(img0, storage);

			if (drawImageSquare)
			{
				drawSquares( img0, foundSquares);
				c = cvWaitKey(0);
			}
			else 
				c = cvWaitKey(10);

			cvReleaseImage( &img0 );
			cvClearMemStorage( storage );
		}
		cvReleaseCapture(&capture);
	}

    cvDestroyWindow(wndname);

    return 0;
}