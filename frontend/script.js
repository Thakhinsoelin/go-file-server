const btn = document.getElementById("get-files");
const container = document.getElementById("download-container");
const form = document.querySelector("form")
const uploadModalBtn = document.getElementById("upload-modal-btn")
const breadCrumb = document.getElementById("bread-crumb")
var dir = ""

const EXTENSION_ICONS = {
    "docx": "microsoft-word-icon.png",
    "doc":  "microsoft-word-icon.png",
    "xlsx": "microsoft-excel-icon.png",
    "xls":  "microsoft-excel-icon.png",
    "pptx": "microsoft-powerpoint-icon.png",
    "ppt":  "microsoft-powerpoint-icon.png",
    // extensions under these are unsupported
    "pdf":  "microsoft-pdf-icon.png",
    "txt":  "text-icon.png",
    "png":  "image-icon.png",
    "jpg":  "image-icon.png",
    "jpeg": "image-icon.png"
};

const BASE_URL = ""
const URI = {
    base    : BASE_URL + "/",//stands for same origin
    public  : BASE_URL + "/public",
    all     : BASE_URL + "/list/all-file",
    file    : BASE_URL + "/upload/file",
    list    : BASE_URL + "/list",
    download: BASE_URL + "/download"

}
//return an anchor link with the download url attached
function DownloadLink(obj) {
    let { IsFile, Path } = obj;
    let currentUrl = new URL(window.location.href);
    let pathname = currentUrl.searchParams.get("path") || "";

    let url = IsFile ? extractNameForDownload(pathname, Path) : extractNameForFolder(pathname, Path);
    
    const a = document.createElement('a');
    const img = document.createElement('img');
    const small = document.createElement('small');

    // Set the filename text
    small.textContent = Path.slice(dir.length + 1);

    // 1. Determine the icon source
    let iconName = "DirectoryIcon.png"; // Default to folder icon

    if (IsFile) {
        
        const extension = Path.split('.').pop().toLowerCase();
        const imageExtensions = ["png", "jpg", "jpeg", "webp", "gif"];
        if (imageExtensions.includes(extension)) {
            // Use the actual image as the thumbnail
            img.src = extractNameForDownload(pathname, Path);
            img.classList.add("thumbnail");
        } else {
            
            let iconName = EXTENSION_ICONS[extension] || "FileIcon.png";
            img.src = `${URI.public}/${iconName}`;
        }
        
    } else {

        img.src = `${URI.public}/${iconName}`;
    }

    // img.alt = IsFile ? "FileIcon" : "DirectoryIcon";
    
    a.href = url;
    a.classList.add(IsFile ? "file" : "folder");
    a.appendChild(img);
    a.appendChild(small);
    
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
    console.log(urlToServer)
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
    for(let i = 0; i<ary.length; i++){
        if(i === 0){
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
    const files = input.files;

    if(files.length === 0){
        alert("Please select a file to upload.")
        return
    }
    const formData = new FormData();
    for(let i = 0; i<files.length; i++){
        formData.append("file", files[i])
    }



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
    init()

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
const updateBreadCrumb = () => {//clanker code
    const currentUrl = new URL(window.location.href);
    let pathname = currentUrl.searchParams.get("path");

    if (!pathname || pathname === "/") return;

    // Decode URL-encoded characters (%5C → \)
    pathname = decodeURIComponent(pathname);

    // Normalize backslashes to forward slashes
    pathname = pathname.replace(/\\/g, "/");

    // Split into folders
    const folders = pathname.split("/").filter(Boolean); // remove empty strings

    // Reset breadcrumb
    breadCrumb.innerHTML = `
            <li class="crumbs">
                <a href=${URI.base}>/</a>
            </li>`;

    let accumulatedPath = "";

    folders.forEach(route => {
        accumulatedPath += `/${encodeURIComponent(route)}`; // build path safely
        breadCrumb.innerHTML += `
            <li class="crumbs">
                <a href="?path=${accumulatedPath}">${route}</a>
            </li>`;
    });
};

function init(){
    updateBreadCrumb()
    requestToLocation()
}
btn.addEventListener('click',requestToLocation)
form.addEventListener("submit",handleFormSubmit)
uploadModalBtn.addEventListener("click",openUploadModal)
window.addEventListener("load",init)

const menu = document.getElementById('custom-menu');

// Listen for the right-click (contextmenu) on the container
container.addEventListener('contextmenu', (e) => {
    e.preventDefault(); // Stop the default browser menu

    // Find if we clicked an anchor (file/folder)
    const target = e.target.closest('a');
    const fileOptions = document.querySelectorAll('.file-only');
    const spaceOptions = document.querySelectorAll('.space-only');
    
    menu.classList.remove('d-none');
    menu.style.left = e.pageX + "px";
    menu.style.top = e.pageY + "px";

    if (target) {

        fileOptions.forEach(opt => opt.classList.remove('d-none'));
        spaceOptions.forEach(opt => opt.classList.add('d-none'));
        menu.dataset.currentPath = target.href;
    } else {
        // Right-clicked the Blank Space
        fileOptions.forEach(opt => opt.classList.add('d-none'));
        spaceOptions.forEach(opt => opt.classList.remove('d-none'));
        delete menu.dataset.currentPath;
    }
});

// 2. Hide menu on left click anywhere else
window.addEventListener('click', () => {
    menu.classList.add('d-none');
});

document.getElementById('menu-copy-path').addEventListener('click', () => {
    const path = menu.dataset.currentPath;
    navigator.clipboard.writeText(path);
    alert("Path copied to clipboard!");
});

document.getElementById('menu-delete').addEventListener('click', async () => {
    const fullUrl = new URL(menu.dataset.currentPath);
    
    // Use .pathname to get "/download/folder/file.ext" 
    // and replace only the /download part
    const deletePath = fullUrl.pathname.replace("/download", "/delete");
    if (!confirm("Are you sure you want to delete this?")) return;

    try {
        const response = await fetch(deletePath, {
            method: "DELETE"
        });

        if (response.ok) {
            // 4. Refresh the UI
            init(); 
        } else {
            const err = await response.json();
            alert("Error: " + err.message);
        }
    } catch (error) {
        console.error("Delete request failed:", error);
    }
});


document.getElementById('menu-rename').addEventListener('click', () => {
    const path = menu.dataset.currentPath;
    const newName = prompt("Enter new name:");
    if (newName) {
        // Here you would send a request to the server to rename the file/folder
        alert(`Renaming ${path} to ${newName} (functionality not implemented)`);
    }
});

document.getElementById('menu-new-folder').addEventListener('click', async () => {
    const folderName = prompt("Enter folder name:");
    if (!folderName) return;

    const currentUrl = new URL(window.location.href);
    const path = currentUrl.searchParams.get("path") || "";
    
    
    const response = await fetch(`/create-folder?path=${path}&name=${folderName}`, {
        method: "POST"
    });

    if (response.ok) {
        init(); 
    } else {
        alert("Failed to create folder");
    }
});