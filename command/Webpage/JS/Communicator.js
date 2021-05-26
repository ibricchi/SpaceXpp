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



function sendTargetIndex(indx){
    
    val = parseInt(indx)
    
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(val)
    };

    fetch(serverIP + "/map/targetIndex", options);

}