
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

    fetch(serverIP + "/map/targetCoords", options);

}


function mapReset(){
    
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(0)
    };

    fetch(serverIP + "/map/reset", options);

}


function saveMap(){

   var name = document.getElementById("save").value

    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(name)
    };

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

    fetch(serverIP + "/map/history/request", options);

}
