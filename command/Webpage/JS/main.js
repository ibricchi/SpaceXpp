

const drived = document.getElementById("driveD");
const drivea = document.getElementById("driveA");

const feed = document.getElementById("feedback")


drived.addEventListener('click', driveD , false);
drivea.addEventListener('click', driveA , false);


document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"
feed.innerHTML="Map is loading"

setInterval(function(){
    status();
    updateMap();
    updateRover();
}, 3000);




function connect(){
    getData('status', '/connect' );
}

function driveD(){
    sendData('distance', '/drive/distance');
}

function driveA(){
sendData('angle', '/drive/angle');
}


