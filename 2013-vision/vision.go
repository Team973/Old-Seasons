package main

import (
	"bitbucket.org/zombiezen/gocv/cv"
	"fmt"
	"os"
	"time"
)

const (
	inputWindowName  = "Input"
	outputWindowName = "Output"

	satWindowName = "Saturation"
	valWindowName = "Value"
)

func main() {
	go run()
	cv.Main()
}

func run() {
	// Create windows
	cv.NamedWindow(inputWindowName, cv.WINDOW_AUTOSIZE)
	cv.NamedWindow(outputWindowName, cv.WINDOW_AUTOSIZE)

	// Set up camera
	capture, err := cv.CaptureFromCAM(0)
	if err != nil {
		fmt.Fprintln(os.Stderr, "failed to start capture")
		os.Exit(1)
	}

	for {
		// Get a frame
		img, err := capture.QueryFrame()
		if err != nil {
			fmt.Fprintln(os.Stderr, "failed to query frame")
			os.Exit(1)
		}

		// Process image
		out, rects := processImage(img)

		// Display images
		drawRectangles(inputWindowName, img, rects)
		cv.ShowImage(outputWindowName, out)
		out.Release()

		// Wait for input
		key := cv.WaitKey(10 * time.Millisecond)
		if key == 'q' {
			break
		}
	}
	os.Exit(0)
}

type Rect [4]cv.Point

func processImage(input *cv.IplImage) (*cv.IplImage, []Rect) {
	storage := cv.NewMemStorage(0)
	defer storage.Release()

	hsv := cv.NewImage(input.Size(), 8, 3)
	defer hsv.Release()
	sat := cv.NewImage(input.Size(), 8, 1)
	defer sat.Release()
	val := cv.NewImage(input.Size(), 8, 1)
	defer val.Release()
	threshSat := cv.NewImage(input.Size(), 8, 1)
	defer threshSat.Release()
	threshVal := cv.NewImage(input.Size(), 8, 1)
	defer threshVal.Release()
	thresh := cv.NewImage(input.Size(), 8, 1)

	cv.CvtColor(input, hsv, cv.BGR2HSV)
	cv.Split(hsv, nil, sat, nil, nil)
	cv.Split(hsv, nil, nil, val, nil)
	cv.Threshold(sat, threshSat, 70, 255, cv.THRESH_BINARY)
	//cv.ShowImage(satWindowName, threshSat)
	cv.Threshold(val, threshVal, 70, 255, cv.THRESH_BINARY_INV)
	//cv.ShowImage(valWindowName, threshVal)
	cv.And(threshVal, threshSat, thresh, nil)
	cv.Dilate(thresh, thresh, nil, 1)
	cv.Erode(thresh, thresh, nil, 1)

	rects := make([]Rect, 0)
	threshClone := thresh.Clone()
	defer threshClone.Release()
	contour, _ := cv.FindContours(threshClone, storage, cv.RETR_LIST, cv.CHAIN_APPROX_SIMPLE, cv.Point{})
	for ; !contour.IsZero(); contour = contour.Next() {
		result := cv.ApproxPoly(contour, storage, cv.POLY_APPROX_DP, cv.ContourPerimeter(contour)*0.02, 0)

		if result.Len() != 4 || cv.ContourArea(result, cv.WHOLE_SEQ, false) < 1000 || !cv.CheckContourConvexity(result) {
			continue
		}

		var r Rect
		for i := 0; i < 4; i++ {
			r[i] = result.PointAt(i)
		}
		rects = append(rects, r)
	}

	return thresh, rects
}

func drawRectangles(windowName string, img *cv.IplImage, rects []Rect) {
	cpy := img.Clone()
	defer cpy.Release()

	for _, r := range rects {
		points := r[:]
		cv.PolyLine(cpy, [][]cv.Point{points}, true, cv.Scalar{0.0, 255.0, 0.0, 0.0}, 3, cv.AA, 0)
	}

	cv.ShowImage(windowName, cpy)
}
