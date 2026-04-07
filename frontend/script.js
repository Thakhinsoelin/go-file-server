const btn = document.getElementById("get-all-files");
const container = document.getElementById("download-container");
const form = document.querySelector("form")

var dir = ""
var api = "http://localhost:3000/"
//return an anchor link with the download url attached
function DownloadLink(url){
    const a = document.createElement("a");
    a.textContent =  url
    a.setAttribute("href", url)
    a.setAttribute("download",true)
    return a;
}
//a helper function to get the file name from the relative file system name
function extractName(name){
    if(dir == ""){
        console.error("The directory is empty string. There might be an error")
    }
    let edited = name.split(dir) 
    let urlToServer = api+"file/"+edited[1]
    return urlToServer
}

const requestAllFile = async () => {
    const response = await fetch("http://localhost:3000/all-file")
    const result = await response.json();//should be an array
    //the first item of the result will be the directory of the data stored
    for(let i=0;i<result.length;i++){
        if(i===0){
            dir = result[0]
            continue
        }
        container.append(DownloadLink(extractName(result[i])))   
        container.append(document.createElement("br"))      
    }
    
}
const handleFormSubmit = async (event) =>{
    event.preventDefault();
    const input = form.querySelector("input");
    const formData = new FormData()
    formData.append("file",input.files[0])
    const response = await fetch("http://localhost:3000/file",{
        method:"POST",
        body:formData
    })
    const result = response.json();
    console.log(result)

}

btn.addEventListener('click',requestAllFile)
form.addEventListener("submit",handleFormSubmit)