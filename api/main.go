package main

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

const PATH_NAME = "../data"

type fsResponse struct {
	IsDir  bool
	IsFile bool
	Path   string
}

// used to turn the file name into the full relative path
// you can also change this into absolute path
func fullPath(filename string) string {
	return "/" + filename
}
func searchAndReturnFiles(folder string) ([]fsResponse, error) {
	result, err := os.ReadDir(folder)
	if err != nil {
		return []fsResponse{}, err

	}
	names := make([]fsResponse, 0, len(result)+1)
	names = append(names, fsResponse{true, false, PATH_NAME})

	for _, file := range result {
		fullPath := filepath.Join(folder, file.Name())
		obj := fsResponse{
			IsDir:  file.IsDir(),
			IsFile: !file.IsDir(),
			Path:   fullPath,
		}
		names = append(names, obj)
	}
	return names, nil
}

// check if the folder exist, if not create one
// true for successfully creating folder OR folder already exist
func OpInit(path string) (bool, error) {
	info, err := os.Stat(path)
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("Folder Doesn't exist and Creating One.")
			os.Mkdir(path, 0755)
			return true, nil
		} else {
			fmt.Println("Error :", err)
			return false, err
		}
	}
	result := info.IsDir()

	if result == true {
		fmt.Println("Folder already exists")
		return true, nil
	} else {
		//case : the result is not a dir but a file
		os.Mkdir(path, 0755)
		return true, nil
	}
}

func getTheDirList(c *gin.Context) {
	names, err := searchAndReturnFiles(PATH_NAME)
	if err != nil {
		c.IndentedJSON(http.StatusInternalServerError, gin.H{
			"message": "Something Went Wrong in getting the files",
		})
	}
	c.IndentedJSON(http.StatusOK, names)
}
func fileUpload(c *gin.Context) {
	now := time.Now()
	date := now.Format("01-06-2006")

	file, err := c.FormFile("file")
	if err != nil {
		c.IndentedJSON(500, gin.H{
			"message": err,
		})
		return
	}
	// ../data/07-04-2026example.txt
	subFolder, OK := c.Params.Get("path")
	// PathToSave := "../data" + name + date + file.Filename
	PathToSave := filepath.Join(PATH_NAME, subFolder, date+file.Filename)
	if !OK { //if the url was /file/ without any parameters
		PathToSave = filepath.Join(PATH_NAME, date+file.Filename)
	}

	saveErr := c.SaveUploadedFile(file, PathToSave)
	if saveErr != nil {
		c.IndentedJSON(500, gin.H{
			"message": "Error in Saving the file",
		})
		return
	}
	c.IndentedJSON(http.StatusOK, gin.H{
		"message": subFolder,
	})

}

// this will be force download
func downloadFile(c *gin.Context) {
	name, OK := c.Params.Get("path")
	if !OK {
		c.IndentedJSON(http.StatusNoContent, gin.H{
			"message": "Couldn't Find " + name,
		})
	}
	//extract only the file name from the pathname
	tmp := strings.Split(name, "/")
	fileName := tmp[len(tmp)-1] //the last element for ["path","to","data","example.txt"]
	c.FileAttachment(filepath.Join(PATH_NAME, name), fileName)
}
func landingPage(c *gin.Context) {
	c.File("../frontend/index.html") //ik it;s messy. this is temp
}
func jsFile(c *gin.Context) {
	c.File("../frontend/script.js") //ik it;s messy. this is temp
}
func styleCSS(c *gin.Context) {
	c.File("../frontend/style.css") //ik it;s messy. this is temp
}
func getFiles(c *gin.Context) {
	query := c.Query("path")
	names, err := searchAndReturnFiles(filepath.Join(PATH_NAME, query))
	if err != nil {
		c.IndentedJSON(404, gin.H{
			"message": err,
		})
		return
	}
	c.IndentedJSON(http.StatusOK, names)

}

func getLocalIP() net.IP {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err != nil {
		log.Fatal("Could not determine local IP. Check your network connection.")
	}
	defer conn.Close()
	return conn.LocalAddr().(*net.UDPAddr).IP
}

func main() {
	result, err := OpInit(PATH_NAME)
	if err != nil || result == false {
		fmt.Println("Couldn't Initiate The Folder.")
		return
	}

	fmt.Println("Hello World")

	server := gin.Default()

	server.Use(cors.Default()) // for dev only
	//for the frontend
	server.GET("/", landingPage)
	server.GET("/script.js", jsFile)
	server.GET("/style.css", styleCSS)
	//server functions
	server.POST("/file", fileUpload)
	server.POST("/file/*path", fileUpload)
	server.GET("/list", getFiles)
	server.GET("/download/*path", downloadFile)
	server.GET("all-file", getTheDirList)

	server.Run(getLocalIP().String() + ":" + "3000")
}
