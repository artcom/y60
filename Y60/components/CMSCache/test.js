plug("y60CMSCache");

var myPresentation='<?xml version=\'1.0\' encoding=\'UTF-8\'?>'+ 
'<presentation title="BMW Welt CMS">'+
'    <themepool name="Theme_Pool" lastmodified="Fri, 03 Nov 2006 15:10:38 +0000" server="himmel" trunk="/" backend="OCS" sslport="443" id="aLN/SdlS0UE=" wsdlport="7778">'+
'        <theme lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" type="BMWWeltTheme" id="H9t/SdlS0UE=" name="Designphilosophie">'+
'            <content/>'+
'            <screen lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" type="BMWWeltScreen2VerticalSplit45" id="V+l/SdlS0UE=" variant="normal" name="Designphilosophie_1">'+
'               <content>'+
'                    <externalcontent mimetype="image/png" name="de_leftImage_Designphilosophie_1" language="de" lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" uri="http://himmel.intern.artcom.de/big.b60" field="leftImage" path="big.b60" id="3PV/SdlS0UE="/>'+
'                </content> </screen> </theme> </themepool> </presentation>';


var myCMSCache = new CMSCache("/tmp/cmstest", new Node(myPresentation),
                           "OCS", "", "", "" );
myCMSCache.verbose = true;
Logger.info("start");

myCMSCache.synchronize();
var i = 0;
while (! myCMSCache.isSynchronized()) {
    msleep(1000);
    print(++i, " secs")
}
var mySize = readFileAsBlock("/tmp/cmstest/big.b60").size;
print("Rate: ",mySize / (i*1024*1024),"MByte/s");

Logger.info("done, iterationen="+i);


