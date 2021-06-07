var validCredentuails = false;

while(validCredentuails == false){

var userName=prompt("Username","");
var pass =  prompt("Password","")
var encoded = 'Basic ' + btoa(userName + ":" + pass );

checkCredentials(encoded)
console.log(validCredentuails)

}