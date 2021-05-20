
   function speed(){
        var slider = document.getElementById("myRange");
        var output = document.getElementById("speedVal");

        output.innerHTML = slider.value;        // Display the default slider value

    // Update the current slider value (each time you drag the slider handle)
        slider.oninput = function() {
        output.innerHTML = this.value;
        }

       
   }

function speedSend(){
    var output = document.getElementById("speedVal");
    var val = parseInt(output.innerHTML)

    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(val)
    };

    fetch("http://172.17.251.225:3000/speed", options);


}