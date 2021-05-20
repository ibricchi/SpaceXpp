
   function speed(){
        var slider = document.getElementById("myRange");
        var output = document.getElementById("speedVal");

        output.innerHTML = slider.value;        // Display the default slider value

    // Update the current slider value (each time you drag the slider handle)
        slider.oninput = function() {
        output.innerHTML = this.value;
        }

        const options = {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(slider.value)
        };

        fetch("http://18.117.12.54:3000/speed", options);

   }