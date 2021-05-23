status = document.getElementById("status");
statusButton = document.getElementById("statusButton");

driveDistanceVal = document.getElementById("driveDistanceVal");
driveDistanceButton = document.getElementById("driveDistanceButton");

statusButton.addEventListener("click", getData("status", "/connect"));
driveDistanceButton.addEventListener("click", sendData(5, "/drive/distance"));
