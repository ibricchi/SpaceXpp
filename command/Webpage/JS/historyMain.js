document.getElementById("mapOpen").addEventListener('click', getNameAndLoad, false);



function getNameAndLoad(){
  var name = document.getElementById("open").value
  requestMap(name)
}