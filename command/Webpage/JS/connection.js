function connect(){
    var mode = document.getElementById("Mode");

    if (mode.innerHTML != "Mode: connected")
        {
            // This will call function that will attempt to connect with rover 
            // For now just simple replacement 
            mode.innerHTML = "Mode: Connected";
        }
    
}