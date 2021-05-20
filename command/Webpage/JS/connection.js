function connect(){
    var mode = document.getElementById("status");

    fetch("http://172.17.251.225:3000/connect")
        .then(request => request.json())
        .then(data => {
            if(data != null){
                mode.innerHTML = data;
            }else{
                mode.innerHTML = " Disconnected: Issue between wepage and server";
            }})
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            mode.innerHTML = " Disconnected: Issue between wepage and server";
        });


}

function battery(){
    var val = document.getElementById("batteryVal")

    fetch("http://172.17.251.225:3000/battery")
        .then(request => request.json())
        .then(data => {val.innerHTML = data;})
        .catch(err => {
            console.warn(err);
            console.warn("Communicator: unable to fetch data from server");
            mode.innerHTML = " Error: unable to fetch battery data";
        });

}