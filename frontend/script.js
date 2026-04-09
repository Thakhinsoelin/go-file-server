const btn = document.getElementById("get-files");
const container = document.getElementById("download-container");
const form = document.querySelector("form")
const uploadModalBtn = document.getElementById("upload-modal-btn")

var dir = ""
const URI = {
    base: "/",//stands for same origin
    public : "/public",
    all :  "/list/all-file",
    file : "/upload/file",
    list :  "/list",
    download: "/download"

}
//return an anchor link with the download url attached
function DownloadLink(obj){
    let {IsFile,Path} = obj;
    let currentUrl = new URL(window.location.href)
    let pathname = currentUrl.searchParams.get("path") 
    if(pathname == null){
        pathname = ""
    }
    url = IsFile ? extractNameForDownload(pathname,Path) : extractNameForFolder(pathname,Path)
    const a = document.createElement('a');
    const img = document.createElement('img')
    const small = document.createElement('small')
    // from child to parent order
    small.textContent = Path.slice(dir.length + 1)//+1 is for the backward slash
    img.src = `${URI.public}${IsFile ? "/FileIcon.png" : "/DirectoryIcon.png"}`
    img.alt = IsFile ? "FileIcon" : "DirectoryIcon";
    a.href= url
    a.classList.add(IsFile ? "file" : "folder")
    a.appendChild(img)
    a.appendChild(small)
    return a;

}

//a helper function to get the file name from the relative file system name
//for the direct downloads of files
function extractNameForDownload(path,name){
    if(dir == ""){
        console.error("The directory is empty string. There might be an error")
    }
    let edited = name.slice(dir.length)//had to use slice because \ and / can be different in each system
    let urlToServer = URI.download + path + edited
    return urlToServer
}
//this will make the link for the folder navigation in frontend
function extractNameForFolder(path,name){
    if(dir == ""){
        console.error("The directory is empty string. There might be an error")
    }
    let edited = name.slice(dir.length)//had to use slice because \ and / can be different in each system
    let urlToFolder = URI.base + "?path="+ path +  edited //this will take the user to another folder url in frontend
    return urlToFolder
}
function populateDownloadLinks(ary){
    //the first item of the result will be the directory of the data stored
    container.innerHTML = ""
    for(let i=0;i<ary.length;i++){
        if(i===0){
            dir = ary[0].Path
            continue
        }
        if(ary[i].IsDir){
            container.append(DownloadLink(ary[i]))
            container.append(document.createElement("br"))      
        }else if (ary[i].IsFile){
            container.append(DownloadLink(ary[i]))   
            container.append(document.createElement("br"))      
        }
    }
}

const handleFormSubmit = async (event) =>{
    event.preventDefault();
    let currentUrl = new URL(window.location.href)
    let path = currentUrl.searchParams.get("path")
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
    input.value = ''


}
//takes in the path of the dir
const requestToLocation = async() =>{
    let currentUrl = new URL(window.location.href)
    let pathname = currentUrl.searchParams.get("path")
    let response;
    if(pathname == "/" || pathname == null){//this will be send to default route
        response = await fetch(URI.all);
    }else{
        response = await fetch(URI.list + "?path="+ pathname);
    }
    const result = await response.json();
    populateDownloadLinks(result)
}
const openUploadModal = () =>{
    const modal = document.getElementById("modal")
    modal.classList.remove("d-none")


    const handleCloseModal = (event) =>{
        if(event.target === modal){
            modal.classList.add("d-none")
            //this is to remove unncessary events tracking outside of the Modal
            window.removeEventListener('click',handleCloseModal)
        }
    }

    //once the modal is opened, this event listener will be set up
    window.addEventListener("click",handleCloseModal)
}



btn.addEventListener('click',requestToLocation)
form.addEventListener("submit",handleFormSubmit)
uploadModalBtn.addEventListener("click",openUploadModal)