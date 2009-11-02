
plug("ASSTuioServer");

ASSTuioServer.sendUDP();

var mySettings = Node.createDocument();
mySettings.parseFile("settings.xml");

ASSTuioServer.onUpdateSettings( mySettings );

while (1) {

    ASSTuioServer.poll();
    msleep( 10 ); // XXX
}

print("=== done ===");
