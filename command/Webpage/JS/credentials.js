var validCredentials = false;

while(validCredentials == false){

var userName=prompt("Username","");
var pass =  prompt("Password","")
var encoded = 'Basic ' + btoa(userName + ":" + pass );

checkCredentials(encoded)
console.log(validCredentials)

}