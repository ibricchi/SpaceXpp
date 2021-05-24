

const driveButton = document.getElementById("drive");

driveButton.addEventListener('click', drive , false);

setInterval(function(){
    status();
}, 3000);





function connect(){
    getData('status', '/connect' );
}

function drive(){
    sendData('angle', '/drive/angle');
    sendData('distance', '/drive/distance');
}


