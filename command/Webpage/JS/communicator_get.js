
serverIP = "https://18.117.12.54:3000"

// Get Requests \\

/* Get credentials 
*/

function checkCredentials(encoded){
    console.log("enteredfunction")  
    checkCredRequest = {
            headers: {
            "Content-Type": "application/json; charset=UTF-8"
             },
            method: "GET"
            };
            checkCredRequest.headers.Authorization = encoded;

         fetch(serverIP + '/isAuthorised', checkCredRequest)
        .then(request => request.json())
        .then( data => {
            validCredentials = data
            console.log("creds :", validCredentials)
        })
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
        });
}



/* getData:
*   - General get request for reciving and updating a section of the webpage 
*   - Used for testing 
*   - Arguments:
*       - location: the address of the text section of the HTML website that will be updated with the data from the server
*       - address: the https address used in combination with the serverIP to send get request and recive corrosponding JSON data 
*/

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




function loadMap(){
    fetch(serverIP + '/map/history/load')
    .then(request => request.json())
    .then(data => {
        if(data != null){
            loadedMap.cols = data.cols;
            loadedMap.rows = data.rows
            loadedMap.layers[0] = data.layout;
            loadedMap.layers[1][data.roverIndx] = getVal(data.roverRotation);
            //console.log(data.driveinstructions[0].instruction)

            var i = 0
            while(data.driveinstructions[i].instruction != null)
            {
                var line = data.driveinstructions[i].instruction + ":" + data.driveinstructions[i].value
                printToInstructionFeed(line)
                i++
            }
           
        }
    })

}

function getFeed(){
    fetch(serverIP + '/feed')
    .then(request => request.json())
    .then(data => {
        if(data != null){
           // console.log(data)
            printToFeedback(data, 1)
        }
    })

}

function getEnergy(){
    fetch(serverIP + '/energy/values')
    .then(request => request.json())
    .then(data => {
        if(data != null){
            stateOfCharge.style.width = data.stateOfCharge + '%'
            stateOfCharge.innerHTML = data.stateOfCharge + '%'

            stateOfHealth.style.width = data.stateOfHealth + '%'
            stateOfHealth.innerHTML = data.stateOfHealth + '%'

            if (data.errorInCells == 1){
                alert("Error in cells!");

            }
            
        }
    })
}