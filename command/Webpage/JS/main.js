

const connectButton = document.getElementById("connect");
const driveButton = document.getElementById("drive");

connectButton.addEventListener('click', connect , false);
driveButton.addEventListener('click', drive , false);

setInterval(function(){
    getData('status', '/connect' );
}, 3000);





function connect(){
    getData('status', '/connect' );
}

function drive(){
    sendData('angle', '/drive/angle');
    sendData('distance', '/drive/distance');
}


