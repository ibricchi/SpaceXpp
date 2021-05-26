

const driveButton = document.getElementById("drive");
const feedback = document.getElementById("feedback")

driveButton.addEventListener('click', drive , false);

document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"
//feedback.innerHTML="Map is loading"

setInterval(function(){
    status();
    updateMap();
    updateRover();
}, 3000);





function connect(){
    getData('status', '/connect' );
}

function drive(){
    sendData('angle', '/drive/angle');
    sendData('distance', '/drive/distance');
}




