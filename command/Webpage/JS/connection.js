
serverIP = "http://18.117.12.54:3000"


function getData( location, address ){
    webLocation = document.getElementById(location)

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