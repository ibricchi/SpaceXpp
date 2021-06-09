document.getElementById("mapOpen").addEventListener('click', getNameAndLoad, false);

instructions = document.getElementById("instructionLog")

var openOptions = document.getElementById('openOptions');

getMapNamesAndLoad()



function getMapNamesAndLoad() {
  var names = getMapNames()

  var options =
  [
    {
      "text"  : "Select map name ",
      "value" : "Value 1"
    }
  ];
  
  var i = 0
  
  while (names[i] != null){
    console.log( names[i] )
    var intermediate = {
      "text"  : names[i],
      "value" : names[i]
    };
    
    options.push(intermediate);
  
    i += 1; 
  }
  
  
  
  for(var i = 0, l = options.length; i < l; i++){
    var option = options[i];
    openOptions.options.add( new Option(option.text, option.value, option.selected) );
  }

}



function getNameAndLoad(){
  
  cleanMap()
  
  var name = document.getElementById("open").value
  requestMap(name);
  console.log("loading map")

  setTimeout(loadMap, 3000);

}



function printToInstructionFeed(line){
  instructions.innerHTML = instructions.innerHTML + "<br>" + line 
}

function cleanMap(){
  loadedMap.layers[1] = [
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    loadedMap.layers[0] = [
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    instructions.innerHTML = "" 
}


