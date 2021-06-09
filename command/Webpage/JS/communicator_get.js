/*
*   File: communiator_get.js
*   Author: Bradley Stanley-Clamp
*   Description: 
*               Contains all HTTPS GET requests for webpage functionality and test functions. 
                Correct authentication is required for sucsessful communication with server 
*/


// Defining variables \\

serverIP = "https://18.117.12.54:3000"

var validCredentials = false;
var encoded

// LogIn function \\

/* 
 *  Used to create popups
 *  Recive username and password
 *  Call GET request to check if credentials are valid
 *  Then loop if not valid credentials
 */

async function LogIn() {
    while (validCredentials == false) {

        var userName = prompt("Username", "");
        var pass = prompt("Password", "")

        encoded = 'Basic ' + btoa(userName + ":" + pass);

        validCredentials = await checkCredentials(encoded)
        console.log(validCredentials)
    }

    var line = "Welcome to Mars " + userName + "!"
    printToFeedback(line, 0)
}

LogIn()


//  GET Requests \\

/* 
 *   Checks if credentials are valid
 *   reutns bool true if valid and false if no  
 */

async function checkCredentials(encoded) {
    console.log("entered function")
    checkCredRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    checkCredRequest.headers.Authorization = encoded;

    return fetch(serverIP + '/isAuthorised', checkCredRequest)
        .then(request => request.json())
        .then(data => { return data })
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            return false
        });
}


// Private requests 






/* getData:
 *   - General get request for reciving and updating a section of the webpage 
 *   - Used for testing 
 *   - Arguments:
 *       - location: the address of the text section of the HTML website that will be updated with the data from the server
 *       - address: the https address used in combination with the serverIP to send get request and recive corrosponding JSON data 
 */

function getData(location, address) {
    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;

    webLocation = document.getElementById(location);

    fetch(serverIP + address, privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                webLocation.innerHTML = data;
            } else {
                webLocation.innerHTML = " Disconnected: Issue between wepage and server";
            }
        })
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            webLocation.innerHTML = " Disconnected: Issue between wepage and server";
        });
}


function status() {

    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;
    server = document.getElementById("server")
    rover = document.getElementById("rover")

    fetch(serverIP + '/connect', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                server.innerHTML = "Connected";
                if (data == true) {
                    rover.innerHTML = "Connected";
                } else {
                    rover.innerHTML = "Disconnected";
                }

            } else {
                server.innerHTML = "Disconnected"
                rover.innerHTML = "Disconnected"

            }
        })
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            server.innerHTML = "Disconnected"
            rover.innerHTML = "Disconnected"
        });
}



function updateMap() {
    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;
    fetch(serverIP + '/map/getMap', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                map.cols = data.cols;
                map.rows = data.rows
                map.layers[0] = data.layout;

            }
        })
}

function getVal(angle) {
    if (angle == 0) {
        return 6;
    } else if (angle == 90) {
        return 7;
    } else if (angle == 180) {
        return 8;
    } else if (angle == 270) {
        return 9;
    }

}

function updateRover() {
    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;
    fetch(serverIP + '/map/getRover', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
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
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                ]
                indx = data.x + (data.y * map.cols);
                map.layers[1][indx] = getVal(data.rotation);
            }
        })
}



function check() {
    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;

    fetch(serverIP + '/check', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                // decoded = JSON.parse(data);
                console.log(data);
            }
        })
}




function loadMap() {

    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;

    fetch(serverIP + '/map/history/load', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                loadedMap.cols = data.cols;
                loadedMap.rows = data.rows
                loadedMap.layers[0] = data.layout;
                loadedMap.layers[1][data.roverIndx] = getVal(data.roverRotation);
                //console.log(data.driveinstructions[0].instruction)

                var i = 0
                while (data.driveinstructions[i].instruction != null) {
                    var line = data.driveinstructions[i].instruction + ":" + data.driveinstructions[i].value
                    printToInstructionFeed(line)
                    i++
                }

            }
        })

}

function getFeed() {

    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;

    fetch(serverIP + '/feed', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                // console.log(data)
                printToFeedback(data, 1)
            }
        })

}

function getEnergy() {

    privateRequest = {
        headers: {
            "Content-Type": "application/json; charset=UTF-8"
        },
        method: "GET"
    };
    privateRequest.headers.Authorization = encoded;

    fetch(serverIP + '/energy/values', privateRequest)
        .then(request => request.json())
        .then(data => {
            if (data != null) {
                stateOfCharge.style.width = data.stateOfCharge + '%'
                stateOfCharge.innerHTML = data.stateOfCharge + '%'

                stateOfHealth.style.width = data.stateOfHealth + '%'
                stateOfHealth.innerHTML = data.stateOfHealth + '%'

                if (data.errorInCells == 1) {
                    alert("Error in cells!");

                }

            }
        })
}