/*
*   Developed by Bradley Stanley-Clamp (bradley.stanley-clamp19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/


function sendData(location, address){
    webLocation = document.getElementById(location);
    val = parseInt(webLocation.value)
    
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(val)
    };
    options.headers.Authorization = encoded;


    fetch(serverIP + address, options);

}



function sendTargetCoords(x, y){

    mode = parseInt(document.getElementById("mode").value);
    
    
   var coords = {
        x: x, 
        y: y,
        mode: mode,
    };
    
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(coords)
    };
    options.headers.Authorization = encoded;


    fetch(serverIP + "/map/targetCoords", options);

    
    var line = "Target coordinates: " + x + "," + y + ": sent to server "
    console.log(line)
       printToFeedback(line, 1)

}


function mapReset(){
    
    var line = "Resetting map & Clearing feed"
    printToFeedback(line, 0)
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(0)
    };
    options.headers.Authorization = encoded;

    fetch(serverIP + "/map/reset", options);

}


function saveMap(){

   var name = document.getElementById("save").value

   var line = "Map saved as: " + name 
    printToFeedback(line, 1)

    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(name)
    };
    options.headers.Authorization = encoded;

    fetch(serverIP + "/map/history/save", options);

    console.log("name sent", name)
}

function requestMap(mapName){
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(mapName)
    };
    options.headers.Authorization = encoded;

    fetch(serverIP + "/map/history/request", options);

}

function stopAutonomous(){
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(true)
    };
    options.headers.Authorization = encoded;

    fetch(serverIP + "/map/stopAutonomous", options);

}