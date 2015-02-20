package main

// localhost:8080

import (
		"fmt"
		"io/ioutil"
		"html/template"
		"net"
		"net/http"
		"os"
		"strings"
		"strconv"
)

var (
	tmpl *template.Template

	sendChan = make(chan string, 5)
	recvChan = make(chan string, 5)

)

type File struct {
	Title string
	Body  []string
}

type Constant struct {
	Key string
	Value float64
}

type ConstantsFile struct {
	Title string
	List []Constant
}

var constantList ConstantsFile
var tempConstants ConstantsFile

func indexHandler(w http.ResponseWriter, r *http.Request) {
	tmpl, err := template.ParseFiles("templates/index.html")
	if err != nil {
		fmt.Println("can't parse template", err)
		os.Exit(1)
	}
	tmpl.Execute(w,nil)
}

func constantsHandler(w http.ResponseWriter, r *http.Request) {
	tmpl, err := template.ParseFiles("templates/goserv.html")
	if err != nil {
		fmt.Println("can't parse template", err)
		os.Exit(1)
	}

	for n := 0; n<len(constantList.List);n++ {
		req := r.FormValue(tempConstants.List[n].Key)

		 if req != "" {
			 fmt.Println(req)
			 convReq, err := strconv.ParseFloat(strings.Trim(req,""), 64)
			 if err != nil {
				 fmt.Println(err)
				 break
			 }
			 if convReq == tempConstants.List[n].Value {
				 break
			 }
			 tempConstants.List[n].Value = convReq
		 }
	}

	req := r.FormValue("reset")
	if req != "" {
		tempConstants.List = constantList.List
	}

	req = r.FormValue("save")
	if req != "" {
		err := os.Remove("constants.txt")
		if err != nil {
			fmt.Println("Error deleting file: ",err)
		}
		saveConstants()
	}

	tmpl.Execute(w, tempConstants)
}

func handleConnection(conn net.Conn) {
	for {
		_, err := conn.Write([]byte(<-sendChan));
		if err != nil {
			fmt.Printf("error writing to connection", err)
			return
		}
	}
}

func connect(port string) {
	ln, err := net.Listen("tcp", "localhost:" + port)
	if err != nil {
		fmt.Println("unable to listen on port " + port, err)
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("unable to accept connections on port " + port, err)
		}

		go handleConnection(conn)
	}
}

func socketHandler(w http.ResponseWriter, r *http.Request) {
	tmpl, err := template.ParseFiles("templates/socket.html")
	if err != nil {
		fmt.Println("can't parse template", err)
		os.Exit(1)
	}

	val := r.FormValue("val")
	sendChan <- val
	tmpl.Execute(w, nil)
}

func main() {
	go connect("8081")

	err := loadConstants()
	if err != nil {
		fmt.Println("can't load file", err)
	}
	tempConstants = constantList

	http.HandleFunc("/", indexHandler)
	http.HandleFunc("/constants/", constantsHandler)
	http.HandleFunc("/socket/", socketHandler)

	// Start Web server
	http.ListenAndServe(":8080", nil)
}

func loadConstants() (error) {

	file, err := loadFileAsString("constants")
	if err != nil {
		return err
	}

	data := strings.Split(file, "\n")
	for k, v := range data {
		strings.Trim(v, "\n")
		if (v == "") {
			data = append(data[:k], data[k+1:]...)
		}
	}

	for _,v := range data {

		var constant Constant

		splitLine := strings.Split(v, "=")

		constant.Key = strings.Trim(splitLine[0], " ")

		value, err := strconv.ParseFloat(strings.Trim(splitLine[1], " "), 64)
		if err != nil {
			fmt.Println(err)
		}
		constant.Value = value
		constantList.List = append(constantList.List, constant)
	}

	return nil
}

func saveConstants() {
	constantList = tempConstants
	var saveFile string
	for n:=0;n<len(constantList.List);n++ {
		 saveFile += constantList.List[n].Key + " = " + strconv.FormatFloat(constantList.List[n].Value, 'f', -1, 64) + "\n"
	}
	err := writeStringToFile("constants",saveFile)
	if err != nil {
	fmt.Printf("error writing to constants file: ", err);
    }
}

func loadFileAsString(title string) (string, error) {
	fileName := title + ".txt"
	contents, err := ioutil.ReadFile(fileName);
	if err != nil {
		return "nil", err
	}
	return string(contents), nil
}

func writeStringToFile(fileName, data string) error {
	fileName = fileName + ".txt"
	err := ioutil.WriteFile(fileName, []byte(data), 0600)
	return err
}
