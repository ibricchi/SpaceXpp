serverIP = "http://18.117.12.54:3000"


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
                console.log("changing layout");
                map.cols = data.cols;
                map.rows = data.rows
                map.layers[0] = data.layout;

            }
        })
}

function updateRover(){
    fetch(serverIP + '/map/getRover')
        .then(request => request.json())
        .then(data => {
            if(data != null){
                console.log("Updating rover location");
                map.layers[1][data.indx] = 3;
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
    
   var coords = {
        x: x, 
        y: y,
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