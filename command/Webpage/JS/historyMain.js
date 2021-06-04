document.getElementById("mapOpen").addEventListener('click', getNameAndLoad, false);

instructions = document.getElementById("feedback")


function getNameAndLoad(){
  var name = document.getElementById("open").value
  requestMap(name);
  console.log("loading map")

  setTimeout(loadMap, 3000);

}



function printToInstructionFeed(line){
  instructions.innerHTML = instructions.innerHTML + "<br>" + line 
}

