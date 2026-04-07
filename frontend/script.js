const btn = document.getElementById("get-all-files");
const container = document.getElementById("download-container");
const form = document.querySelector("form")

var dir = ""
const URI = {
    //for manual testing
    all : "http://192.168.100.114:3000/all-file",
    file : "http://192.168.100.114:3000/file",
    list : "http://192.168.100.114:3000/list",
    download:"http://192.168.100.114:3000/download"

}
//return an anchor link with the download url attached
function DownloadLink(url,download = true){
    const a = document.createElement("a");
    a.textContent =  url
    a.setAttribute("href", url)
    download && a.setAttribute("download",true)
    return a;
}
//a helper function to get the file name from the relative file system name
function extractNameForDownload(name){
    if(dir == ""){
        console.error("The directory is empty string. There might be an error")
    }
    let edited = name.slice(dir.length)//had to use slice because \ and / can be different in each system
    let urlToServer = URI.download + edited
    return urlToServer
}
function extractNameForFolder(name){
    if(dir == ""){
        console.error("The directory is empty string. There might be an error")
    }
    let edited = name.slice(dir.length)//had to use slice because \ and / can be different in each system
    let urlToServer = URI.list + "?path="+ edited
    return urlToServer
}
function populateDownloadLinks(ary){
    //the first item of the result will be the directory of the data stored
    for(let i=0;i<ary.length;i++){
        if(i===0){
            dir = ary[0].Path
            continue
        }
        if(ary[i].IsDir){
            container.append(DownloadLink(extractNameForFolder(ary[i].Path),false))
            container.append(document.createElement("br"))      
        }else if (ary[i].IsFile){
            container.append(DownloadLink(extractNameForDownload(ary[i].Path)))   
            container.append(document.createElement("br"))      
        }
    }
}

const handleFormSubmit = async (event,path) =>{
    event.preventDefault();
    const input = form.querySelector("input");
    const formData = new FormData()
    formData.append("file",input.files[0])
    let fullPath = ""
    if(path == "/" || path == null){
        fullPath = URI.file
    }else{
        fullPath = URI.file + path
    }
    const response = await fetch(
            fullPath,
        {
            method:"POST",
            body:formData
        })
    const result = await response.json();
    console.log(result)

}
function removeEveryListener(){
    //main
    btn.removeEventListener('click',requestAllFile)
    form.removeEventListener("submit",handleFormSubmit)
}
//takes in the path of the dir
const requestToLocation = async(pathname) =>{
    let response;
    if(pathname == "/" || pathname == null){//this will be send to default route
        response = await fetch(URI.all);
    }else{
        response = await fetch(URI.list + "?path="+ pathname);
    }
    const result = await response.json();
    populateDownloadLinks(result)
}

// window.addEventListener("DOMContentLoaded",()=>{
//     let currentUrl = new URL(window.location.href)
//     let path = currentUrl.searchParams.get("path")
//     if(path == null){
//         removeEveryListener()
//         btn.addEventListener('click',requestAllFile)
//         form.addEventListener("submit",handleFormSubmit)
//     }else{
//         removeEveryListener()
//         form.classList.add('d-none')
//         btn.addEventListener('click',()=> requestToLocation(path))

//     }

// })
let currentUrl = new URL(window.location.href)
let path = currentUrl.searchParams.get("path")
btn.addEventListener('click',()=> requestToLocation(path))
form.addEventListener("submit",(e)=>handleFormSubmit(e,path))


