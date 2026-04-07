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
     
    let urlToServer = URI.download + dir + edited[1]
    return urlToServer
}
function populateDownloadLinks(obj){
    //the first item of the result will be the directory of the data stored
    for(let i=0;i<obj.length;i++){
        if(i===0){
            dir = obj[0]
            continue
        }
        container.append(DownloadLink(extractName(obj[i])))   
        container.append(document.createElement("br"))      
    }
}


const requestAllFile = async () => {
    const response = await fetch(URI.all)
    const result = await response.json();//should be an array
    populateDownloadLinks(result)
    
}
const handleFormSubmit = async (event) =>{
    event.preventDefault();
    const input = form.querySelector("input");
    const formData = new FormData()
    formData.append("file",input.files[0])
    const response = await fetch(URI.file,{
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
    const response = await fetch(URI.list + "?path="+ pathname);
    const result = await response.json();
    populateDownloadLinks(result)
}

window.addEventListener("DOMContentLoaded",()=>{
    let currentUrl = new URL(window.location.href)
    let path = currentUrl.searchParams.get("path")
    if(path == null){
        removeEveryListener()
        btn.addEventListener('click',requestAllFile)
        form.addEventListener("submit",handleFormSubmit)
    }else{
        removeEveryListener()
        form.classList.add('d-none')
        btn.addEventListener('click',()=> requestToLocation(path))

    }

})


