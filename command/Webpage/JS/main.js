

const driveButton = document.getElementById("drive");
const feed = document.getElementById("feedback")


driveButton.addEventListener('click', drive , false);

document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"
feed.innerHTML="Map is loading"

setInterval(function(){
    status();
    updateMap();
    updateRover();
    feedback();
}, 3000);




function connect(){
    getData('status', '/connect' );
}

function drive(){
    sendData('angle', '/drive/angle');
    sendData('distance', '/drive/distance');
}




