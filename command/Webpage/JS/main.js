

const drived = document.getElementById("driveD");
const drivea = document.getElementById("driveA");
const resetMap = document.getElementById("resetMap");
const mapSave = document.getElementById("mapSave");
const stateOfCharge = document.getElementById("stateOfCharge")
const stateOfHealth = document.getElementById("stateOfHealth")
const feed = document.getElementById("feedback")
//const stopAutonom = document.getElementById("stopAutonomous")

drived.addEventListener('click', driveD , false);
drivea.addEventListener('click', driveA , false);
resetMap.addEventListener('click', mapReset, false);
mapSave.addEventListener('click', saveMap, false )
//stopAutonom.addEventListener('click', stopAutonomous, false )


document.getElementById("server").innerHTML = "loading"
document.getElementById("rover").innerHTML = "loading"

    setInterval(function(){
        if (validCredentials == true){
            status();
            updateMap();
            updateRover();
            getFeed();
            getEnergy();
        }
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
if (line != "") {


        if (option == 0) {
            feed.innerHTML = line
        } else if (option == 1){
        feed.innerHTML =  line + "<br>" + feed.innerHTML + "<br>"
        }
    }
}

 
  