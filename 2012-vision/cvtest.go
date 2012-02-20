package main

import (
	"bitbucket.org/zombiezen/gocv/cv"
	"bitbucket.org/zombiezen/gocv/highgui"
	"flag"
	"fmt"
	"math"
	"os"
	"runtime"
	"sort"
	"time"
)

func main() {
	imageName := flag.String("image", "", "Use a static image instead of loading from a webcam")
	flag.Parse()

	runtime.LockOSThread()
	highgui.NamedWindow(windowName, highgui.WINDOW_AUTOSIZE)

	if *imageName != "" {
		img, err := highgui.LoadImage(*imageName, 1)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		defer img.Release()

		rects := findRectangles(img)
		drawRectangles(img, rects)
		highgui.WaitKey(time.Duration(0))
	} else {
		capture, err := highgui.CaptureFromCAM(0)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		for {
			img, err := capture.QueryFrame()
			if err != nil {
				fmt.Println(err)
				os.Exit(1)
			}
			img0 := img.Clone()

			rects := findRectangles(img0)
			drawRectangles(img0, rects)
			if key := highgui.WaitKey(time.Duration(10 * time.Millisecond)) & 0x7f; key == 'q' {
				break
			}

			img0.Release()
			//img.Release()
		}

		capture.Release()
	}
}

const windowName = "Rectangle Detection"

const (
	lowThreshold  = 230
	highThreshold = 255
)

func angle(pt1, pt2, pt0 cv.Point) float64 {
	dx1 := float64(pt1.X - pt0.X)
	dy1 := float64(pt1.Y - pt0.Y)
	dx2 := float64(pt2.X - pt0.X)
	dy2 := float64(pt2.Y - pt0.Y)
	return (dx1*dx2 + dy1*dy2) / math.Sqrt((dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2)+1e-10)
}

func findRectangles(img *cv.IplImage) [][4]cv.Point {
	storage := cv.NewMemStorage(0)
	defer storage.Release()

	sz := cv.Size{img.Width &^ 1, img.Height &^ 1}
	timg := img.Clone()
	defer timg.Release()
	gray := cv.NewImage(sz, 8, 1)
	defer gray.Release()
	pyr := cv.NewImage(cv.Size{sz.Width / 2, sz.Height / 2}, 8, 3)
	defer pyr.Release()

	rects := make([][4]cv.Point, 0)

	timg.SetROI(cv.Rect{0, 0, sz.Width, sz.Height})

	cv.PyrDown(timg, pyr, cv.GAUSSIAN_5x5)
	cv.PyrUp(timg, pyr, cv.GAUSSIAN_5x5)
	tgray := cv.NewImage(sz, 8, 1)
	defer tgray.Release()

	for channel := 1; channel <= 3; channel++ {
		timg.SetCOI(channel)
		cv.Copy(timg, tgray, nil)

		cv.Threshold(tgray, gray, lowThreshold, highThreshold, cv.THRESH_BINARY)
		cv.Dilate(gray, gray, nil, 10)
		cv.Erode(gray, gray, nil, 8)

		contour, _ := cv.FindContours(gray, storage, cv.RETR_LIST, cv.CHAIN_APPROX_SIMPLE, cv.Point{0, 0})

		for ; !contour.IsZero(); contour = contour.Next() {
			result := cv.ApproxPoly(contour, storage, cv.POLY_APPROX_DP, cv.ContourPerimeter(contour)*0.02, 0)

			if result.Len() != 4 || cv.ContourArea(result, cv.WHOLE_SEQ, false) < 1000 || !cv.CheckContourConvexity(result) {
				continue
			}
			var r [4]cv.Point
			for i := 0; i < 4; i++ {
				r[i] = result.PointAt(i)
			}

			s := 0.0
			// TODO: Check this boundary
			for i := 2; i < 5; i++ {
				t := math.Abs(angle(result.PointAt(i%4), result.PointAt((i-2)%4), result.PointAt((i-1)%4)))
				if t > s {
					s = t
				}
			}
		
			// originally 0.3 for strict 90 degrees, may need to increase slightly
			if s < 0.4 {
				points := r
				sort.Sort(ByY(points[:]))
				distance := (points[2].Y+points[3].Y)/2 - (points[0].Y+points[1].Y)/2
				fmt.Println("average height: ", distance)

				rects = append(rects, r)
			}
		}
	}

	return rects
}

type ByY []cv.Point

func (slice ByY) Len() int {
	return len(slice)
}

func (slice ByY) Less(i, j int) bool {
	return slice[i].Y < slice[j].Y
}

func (slice ByY) Swap(i, j int) {
	slice[i], slice[j] = slice[j], slice[i]
}

func drawRectangles(img *cv.IplImage, rects [][4]cv.Point) {
	cpy := img.Clone()
	defer cpy.Release()

	for _, r := range rects {
		points := r[:]
		cv.PolyLine(cpy, [][]cv.Point{points}, true, cv.Scalar{0.0, 255.0, 0.0, 0.0}, 3, cv.AA, 0)
	}

	highgui.ShowImage(windowName, cpy)
}
