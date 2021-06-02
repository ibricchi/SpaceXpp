
serverIP = "https://18.117.12.54:3000"


function getData( location, address ){
    webLocation = document.getElementById(location);

    fetch(serverIP + address)
        .then(request => request.json())
        .then(data => {
            if(data != null){
                webLocation.innerHTML = data;
            }else{
                webLocation.innerHTML = " Disconnected: Issue between wepage and server";
            }})
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            webLocation.innerHTML = " Disconnected: Issue between wepage and server";
        });
}


function status(){
    server = document.getElementById("server")
    rover = document.getElementById("rover")

    fetch(serverIP + '/connect')
        .then(request => request.json())
        .then(data => {
            if(data != null){
                server.innerHTML = "Connected";
                if(data == 0){
                    rover.innerHTML =  "Connected";
                } else {
                    rover.innerHTML =  "Disconnected";
                }

            }else{
                server.innerHTML = "Disconnected"
                rover.innerHTML = "Disconnected"

            }})
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            server.innerHTML = "Disconnected"
            rover.innerHTML = "Disconnected"
        });
}



function updateMap() {

    fetch(serverIP + '/map/getMap')
        .then(request => request.json())
        .then(data => {
            if(data != null){
                map.cols = data.cols;
                map.rows = data.rows
                map.layers[0] = data.layout;

            }
        })
}

function getVal(angle){
    if(angle == 0){
        return 6;
    } else if (angle == 90){
        return 7;
    } else if (angle == 180){
        return 8;
    } else if (angle == 270){
        return 9;
    }

}

function updateRover(){
    fetch(serverIP + '/map/getRover')
        .then(request => request.json())
        .then(data => {
            if(data != null){
                map.layers[1] = [
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
                indx = data.x + (data.y * map.cols );
                map.layers[1][indx] = getVal(data.rotation);
            }
        })
}



function check(){

    fetch(serverIP + '/check')
        .then(request => request.json())
        .then(data => {
            if(data != null){
               // decoded = JSON.parse(data);
                console.log(data);
            }
        })    
}


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


function speedSend(){
    var output = document.getElementById("speedVal");
    var val = parseInt(output.innerHTML)

    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(val)
    };

    fetch(serverIP + "/speed", options);


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

function loadMap(){
    fetch(serverIP + '/map/history/load')
    .then(request => request.json())
    .then(data => {
        if(data != null){
            loadedMap.cols = data.cols;
            loadedMap.rows = data.rows
            loadedMap.layers[0] = data.layout;
            indx = data.roverX + (data.roverY * map.cols );
            loadedMap.layers[1][indx] = getVal(data.roverRotation);

        }
    })

}