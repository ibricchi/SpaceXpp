

const driveButton = document.getElementById("drive");

driveButton.addEventListener('click', getMap , false);

document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"

console.log("getting map:");

setInterval(function(){
    status();
    updateMap();
}, 3000);





function connect(){
    getData('status', '/connect' );
}

function drive(){
    sendData('angle', '/drive/angle');
    sendData('distance', '/drive/distance');
}




