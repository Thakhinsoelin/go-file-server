package main

import (
	"fmt"
	"net/http"
	"os"
	"time"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

const PATH_NAME = "../data"

// used to turn the file name into the full relative path
// you can also change this into absolute path
func fullPath(filename string) string {
	return PATH_NAME + "/" + filename
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
	files, err := os.ReadDir(PATH_NAME)
	if err != nil {
		c.IndentedJSON(http.StatusInternalServerError, gin.H{
			"message": "Couldn't get the file list",
		})
	}
	var names = make([]string, 0, len(files))
	names = append(names, PATH_NAME+"/")
	for _, file := range files {
		names = append(names, fullPath(file.Name()))

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
	saveErr := c.SaveUploadedFile(file, fullPath(date+file.Filename))
	if saveErr != nil {
		c.IndentedJSON(500, gin.H{
			"message": "Error in Saving the file",
		})
		return
	}
	c.IndentedJSON(http.StatusOK, gin.H{
		"message": "YESSSSS",
	})

}

// this will be force download
func downloadFile(c *gin.Context) {
	name, OK := c.Params.Get("name")
	// c.File(PATH_NAME + "/" + name)
	if !OK {
		c.IndentedJSON(http.StatusNoContent, gin.H{
			"message": "Couldn't Find " + name,
		})
	}
	c.FileAttachment(PATH_NAME+"/"+name, name)
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
	server.GET("all-file", getTheDirList)
	server.POST("/file", fileUpload)
	//04-26-2026_Shiloh_Dynasty(256k).mp3
	server.GET("/file/:name", downloadFile)

	server.Run("localhost:3000")
}
