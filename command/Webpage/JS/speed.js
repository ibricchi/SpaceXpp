/*
*   Developed by Bradley Stanley-Clamp (bradley.stanley-clamp19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/


function speed(){
    var slider = document.getElementById("myRange");
    var output = document.getElementById("speedVal");

    output.innerHTML = slider.value;        // Display the default slider value

// Update the current slider value (each time you drag the slider handle)
    slider.oninput = function() {
    output.innerHTML = this.value;
    }

   
}