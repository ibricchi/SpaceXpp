

const drived = document.getElementById("driveD");
const drivea = document.getElementById("driveA");
const resetMap = document.getElementById("resetMap");
const mapSave = document.getElementById("mapSave");

const feed = document.getElementById("feedback")


drived.addEventListener('click', driveD , false);
drivea.addEventListener('click', driveA , false);
resetMap.addEventListener('click', mapReset, false);
mapSave.addEventListener('click', saveMap, false )
document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"

setInterval(function(){
    status();
    updateMap();
    updateRover();
    getFeed();
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


function printToFeedback(line, option){

    if (option == 0) {
        feed.innerHTML = line
    } else if (option == 1){
    feed.innerHTML = feed.innerHTML + "<br>" + line
    }

}
  