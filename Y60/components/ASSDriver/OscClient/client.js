
plug("ASSOscPlugin");
var myClient = new ASSOscClient();

print("client: " + myClient);

var mySettings = Node.createDocument();
mySettings.parseFile("settings.xml");

myClient.onUpdateSettings( mySettings );


while (1) {

    myClient.poll();
    msleep( 10 ); // XXX
}

print("=== done ===");
