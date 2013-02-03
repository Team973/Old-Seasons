package main

import (
	"bitbucket.org/zombiezen/gocv/cv"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"mime/multipart"
	"net/http"
	"os"
	"time"
)

const (
	inputWindowName  = "Input"
	outputWindowName = "Output"

	satWindowName = "Saturation"
	valWindowName = "Value"
)

var (
	axisHost     string
	axisUsername string
	axisPassword string
)

func main() {
	flag.StringVar(&axisHost, "axishost", "", "Axis camera host")
	flag.StringVar(&axisUsername, "axisuser", "", "Axis camera username")
	flag.StringVar(&axisPassword, "axispass", "", "Axis camera password")
	flag.Parse()

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

type AxisCamera struct {
	closer io.Closer
	mr     *multipart.Reader

	tmpFile   *os.File
	lastImage *cv.IplImage

	frames chan *cv.IplImage
	quit   chan struct{}
}

func NewAxisCamera(host string, username, password string) (*AxisCamera, error) {
	// Open temp file for frames
	f, err := ioutil.TempFile("", "camera_mjpg")
	if err != nil {
		return nil, err
	}

	const path = "/mjpg/video.mjpg"
	req, err := http.NewRequest("GET", "http://"+host+path, nil)
	if err != nil {
		os.Remove(f.Name())
		f.Close()
		return nil, err
	}
	if username != "" || password != "" {
		// TODO: may need stronger authentication
		req.SetBasicAuth(username, password)
	}
	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		os.Remove(f.Name())
		f.Close()
		return nil, err
	}

	// TODO: parse this out from Content-Type
	const boundary = "myboundary"
	r := multipart.NewReader(resp.Body, boundary)

	cam := &AxisCamera{
		closer:  resp.Body,
		mr:      r,
		tmpFile: f,
		frames:  make(chan *cv.IplImage),
		quit:    make(chan struct{}),
	}
	go cam.fetchFrames()
	return cam, nil
}

func (cam *AxisCamera) fetchFrames() {
	defer close(cam.frames)
	for {
		select {
		case <-cam.quit:
			return
		default:
			// don't block
		}

		image, err := cam.frame()
		if err != nil {
			log.Println("axis camera error:", err)
			continue
		}

		select {
		case <-cam.quit:
			return
		case cam.frames <- image:
			// frame sent, receiver controls memory
		default:
			// receiver is not ready, skip the frame and move on
			image.Release()
		}
	}
}

func (cam *AxisCamera) frame() (*cv.IplImage, error) {
	part, err := cam.mr.NextPart()
	if err != nil {
		return nil, err
	}
	if _, err := cam.tmpFile.Seek(0, 0); err != nil {
		return nil, err
	}
	if err := cam.tmpFile.Truncate(0); err != nil {
		return nil, err
	}
	if _, err := io.Copy(cam.tmpFile, part); err != nil {
		return nil, err
	}
	return cv.LoadImage(cam.tmpFile.Name(), cv.LOAD_IMAGE_COLOR)
}

func (cam *AxisCamera) QueryFrame() (*cv.IplImage, error) {
	if cam.lastImage != nil {
		cam.lastImage.Release()
		cam.lastImage = nil
	}
	cam.lastImage = <-cam.frames
	return cam.lastImage, nil
}

func (cam *AxisCamera) Close() error {
	// Wait for fetchFrames to finish
	cam.quit <- struct{}{}
	close(cam.quit)

	// Remove temp file and close up connection
	name := cam.tmpFile.Name()
	err1 := cam.tmpFile.Close()
	err2 := os.Remove(name)
	err3 := cam.closer.Close()
	if err1 != nil {
		return err1
	} else if err2 != nil {
		return err2
	} else if err3 != nil {
		return err3
	}

	// Release last image
	if cam.lastImage != nil {
		cam.lastImage.Release()
		cam.lastImage = nil
	}

	return nil
}
