//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

var ourShow = new SceneTester(arguments);
const TEXTURE_FILENAME = "tex/kugel.png";

try {
    var Base = [];
    var myFrameCounter = 0;

    Base.setup = ourShow.setup;
    ourShow.setup = function() {
        Base.setup();
        
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, TEXTURE_FILENAME);
        var mySize = 5;
        myMaterial.properties.pointsize = new Vector3f(mySize,1,mySize);
        myMaterial.transparent = true;

        // first texture unit
        var myTextureUnit = myMaterial.find(".//textureunit");
        myTextureUnit.sprite = "1";

        var myViewport = window.canvas.find("viewport");
        myViewport.glow = false;

        var myCountour =  new Array();        
        myCountour.push(new Vector2f(1149.25,615.917));
        myCountour.push(new Vector2f(1148.58,614.25));
        myCountour.push(new Vector2f(1148.08,612.5));
        myCountour.push(new Vector2f(1147.08,610.833));
        myCountour.push(new Vector2f(1146.92,609.667));
        myCountour.push(new Vector2f(1147.92,608.583));
        myCountour.push(new Vector2f(1147.42,607.75));
        myCountour.push(new Vector2f(1146.08,607.25));
        myCountour.push(new Vector2f(1145.08,605.5));
        myCountour.push(new Vector2f(1146.67,604.667));
        myCountour.push(new Vector2f(1147.25,603.167));
        myCountour.push(new Vector2f(1146.58,601.75));
        myCountour.push(new Vector2f(1147.,599.667));
        myCountour.push(new Vector2f(1148.92,599.417));
        myCountour.push(new Vector2f(1149.08,598.417));
        myCountour.push(new Vector2f(1150.42,598.75));
        myCountour.push(new Vector2f(1153.08,598.083));
        myCountour.push(new Vector2f(1153.92,597.));
        myCountour.push(new Vector2f(1154.25,595.333));
        myCountour.push(new Vector2f(1155.33,594.75));
        myCountour.push(new Vector2f(1156.92,594.667));
        myCountour.push(new Vector2f(1157.08,596.667));
        myCountour.push(new Vector2f(1157.92,598.333));
        myCountour.push(new Vector2f(1158.75,599.333));
        myCountour.push(new Vector2f(1159.75,598.833));
        myCountour.push(new Vector2f(1159.5,597.083));
        myCountour.push(new Vector2f(1160.,596.583));
        myCountour.push(new Vector2f(1160.42,595.417));
        myCountour.push(new Vector2f(1160.67,594.417));
        myCountour.push(new Vector2f(1161.67,595.083));
        myCountour.push(new Vector2f(1164.42,596.167));
        myCountour.push(new Vector2f(1166.42,597.417));
        myCountour.push(new Vector2f(1166.5,596.25));
        myCountour.push(new Vector2f(1166.08,595.417));
        myCountour.push(new Vector2f(1165.83,594.083));
        myCountour.push(new Vector2f(1166.5,592.667));
        myCountour.push(new Vector2f(1166.75,591.75));
        myCountour.push(new Vector2f(1165.92,590.333));
        myCountour.push(new Vector2f(1164.75,589.667));
        myCountour.push(new Vector2f(1166.17,588.667));
        myCountour.push(new Vector2f(1167.17,589.667));
        myCountour.push(new Vector2f(1168.33,590.417));
        myCountour.push(new Vector2f(1168.17,591.583));
        myCountour.push(new Vector2f(1169.42,592.083));
        myCountour.push(new Vector2f(1171.33,591.083));
        myCountour.push(new Vector2f(1172.08,591.583));
        myCountour.push(new Vector2f(1172.92,591.583));
        myCountour.push(new Vector2f(1175.,591.417));
        myCountour.push(new Vector2f(1176.25,590.917));
        myCountour.push(new Vector2f(1178.58,590.833));
        myCountour.push(new Vector2f(1179.67,590.417));
        myCountour.push(new Vector2f(1182.33,590.75));
        myCountour.push(new Vector2f(1183.67,591.833));
        myCountour.push(new Vector2f(1186.,593.5));
        myCountour.push(new Vector2f(1187.,594.667));
        myCountour.push(new Vector2f(1188.58,596.333));
        myCountour.push(new Vector2f(1190.33,598.));
        myCountour.push(new Vector2f(1190.83,599.333));
        myCountour.push(new Vector2f(1191.75,601.333));
        myCountour.push(new Vector2f(1191.42,602.333));
        myCountour.push(new Vector2f(1190.42,603.333));
        myCountour.push(new Vector2f(1190.42,604.5));
        myCountour.push(new Vector2f(1190.75,605.583));
        myCountour.push(new Vector2f(1190.25,607.083));
        myCountour.push(new Vector2f(1189.5,608.5));
        myCountour.push(new Vector2f(1188.25,609.167));
        myCountour.push(new Vector2f(1186.83,608.417));
        myCountour.push(new Vector2f(1186.75,606.917));
        myCountour.push(new Vector2f(1185.75,605.083));
        myCountour.push(new Vector2f(1185.67,603.333));
        myCountour.push(new Vector2f(1187.33,602.917));
        myCountour.push(new Vector2f(1187.67,601.583));
        myCountour.push(new Vector2f(1187.25,600.667));
        myCountour.push(new Vector2f(1186.5,600.333));
        myCountour.push(new Vector2f(1185.08,600.75));
        myCountour.push(new Vector2f(1183.67,601.));
        myCountour.push(new Vector2f(1183.08,601.583));
        myCountour.push(new Vector2f(1182.17,601.083));
        myCountour.push(new Vector2f(1181.75,601.833));
        myCountour.push(new Vector2f(1181.5,602.333));
        myCountour.push(new Vector2f(1180.08,602.75));
        myCountour.push(new Vector2f(1179.33,602.417));
        myCountour.push(new Vector2f(1178.33,602.833));
        myCountour.push(new Vector2f(1178.33,603.25));
        myCountour.push(new Vector2f(1178.75,604.667));
        myCountour.push(new Vector2f(1179.17,605.833));
        myCountour.push(new Vector2f(1179.67,607.417));
        myCountour.push(new Vector2f(1179.17,608.417));
        myCountour.push(new Vector2f(1178.42,608.833));
        myCountour.push(new Vector2f(1178.67,610.));
        myCountour.push(new Vector2f(1178.17,611.333));
        myCountour.push(new Vector2f(1178.17,612.));
        myCountour.push(new Vector2f(1179.,613.083));
        myCountour.push(new Vector2f(1179.92,614.25));
        myCountour.push(new Vector2f(1180.,614.917));
        myCountour.push(new Vector2f(1181.5,616.));
        myCountour.push(new Vector2f(1181.,617.417));
        myCountour.push(new Vector2f(1181.58,618.083));
        myCountour.push(new Vector2f(1182.67,617.833));
        myCountour.push(new Vector2f(1183.5,617.25));
        myCountour.push(new Vector2f(1184.58,617.667));
        myCountour.push(new Vector2f(1185.92,618.833));
        myCountour.push(new Vector2f(1187.,619.667));
        myCountour.push(new Vector2f(1187.,622.25));
        myCountour.push(new Vector2f(1188.08,624.167));
        myCountour.push(new Vector2f(1188.58,626.083));
        myCountour.push(new Vector2f(1189.75,627.083));
        myCountour.push(new Vector2f(1190.75,628.583));
        myCountour.push(new Vector2f(1191.5,629.083));
        myCountour.push(new Vector2f(1192.83,630.583));
        myCountour.push(new Vector2f(1194.42,632.));
        myCountour.push(new Vector2f(1196.25,632.333));
        myCountour.push(new Vector2f(1197.67,632.417));
        myCountour.push(new Vector2f(1199.58,632.5));
        myCountour.push(new Vector2f(1201.,632.833));
        myCountour.push(new Vector2f(1202.,633.));
        myCountour.push(new Vector2f(1202.25,633.5));
        myCountour.push(new Vector2f(1202.58,634.5));
        myCountour.push(new Vector2f(1200.92,635.));
        myCountour.push(new Vector2f(1200.5,635.833));
        myCountour.push(new Vector2f(1201.08,636.667));
        myCountour.push(new Vector2f(1202.5,637.333));
        myCountour.push(new Vector2f(1204.58,637.833));
        myCountour.push(new Vector2f(1206.25,638.75));
        myCountour.push(new Vector2f(1207.92,639.));
        myCountour.push(new Vector2f(1208.83,640.583));
        myCountour.push(new Vector2f(1209.5,641.083));
        myCountour.push(new Vector2f(1210.83,642.));
        myCountour.push(new Vector2f(1211.,642.583));
        myCountour.push(new Vector2f(1212.42,642.833));
        myCountour.push(new Vector2f(1213.5,644.167));
        myCountour.push(new Vector2f(1213.83,645.167));
        myCountour.push(new Vector2f(1214.75,645.75));
        myCountour.push(new Vector2f(1215.58,646.417));
        myCountour.push(new Vector2f(1214.92,647.083));
        myCountour.push(new Vector2f(1213.33,647.667));
        myCountour.push(new Vector2f(1212.33,646.417));
        myCountour.push(new Vector2f(1211.25,645.667));
        myCountour.push(new Vector2f(1210.08,645.417));
        myCountour.push(new Vector2f(1209.33,645.583));
        myCountour.push(new Vector2f(1208.08,645.25));
        myCountour.push(new Vector2f(1207.83,644.5));
        myCountour.push(new Vector2f(1206.33,644.5));
        myCountour.push(new Vector2f(1205.08,645.167));
        myCountour.push(new Vector2f(1204.58,645.917));
        myCountour.push(new Vector2f(1203.67,647.833));
        myCountour.push(new Vector2f(1203.83,649.25));
        myCountour.push(new Vector2f(1204.83,650.417));
        myCountour.push(new Vector2f(1206.25,651.75));
        myCountour.push(new Vector2f(1207.33,652.583));
        myCountour.push(new Vector2f(1207.,654.));
        myCountour.push(new Vector2f(1206.83,655.25));
        myCountour.push(new Vector2f(1206.58,655.833));
        myCountour.push(new Vector2f(1205.58,655.75));
        myCountour.push(new Vector2f(1203.5,656.667));
        myCountour.push(new Vector2f(1204.,657.583));
        myCountour.push(new Vector2f(1203.17,659.5));
        myCountour.push(new Vector2f(1201.58,660.5));
        myCountour.push(new Vector2f(1201.,661.417));
        myCountour.push(new Vector2f(1200.42,662.583));
        myCountour.push(new Vector2f(1199.5,663.25));
        myCountour.push(new Vector2f(1197.67,662.667));
        myCountour.push(new Vector2f(1197.33,661.917));
        myCountour.push(new Vector2f(1197.83,660.667));
        myCountour.push(new Vector2f(1198.75,659.583));
        myCountour.push(new Vector2f(1199.,658.083));
        myCountour.push(new Vector2f(1199.83,657.667));
        myCountour.push(new Vector2f(1199.92,655.917));
        myCountour.push(new Vector2f(1199.83,653.333));
        myCountour.push(new Vector2f(1198.92,651.167));
        myCountour.push(new Vector2f(1198.5,649.75));
        myCountour.push(new Vector2f(1198.67,648.417));
        myCountour.push(new Vector2f(1198.17,647.417));
        myCountour.push(new Vector2f(1196.33,647.25));
        myCountour.push(new Vector2f(1194.83,646.917));
        myCountour.push(new Vector2f(1193.58,646.));
        myCountour.push(new Vector2f(1193.33,644.583));
        myCountour.push(new Vector2f(1193.58,643.5));
        myCountour.push(new Vector2f(1193.5,642.833));
        myCountour.push(new Vector2f(1191.75,642.333));
        myCountour.push(new Vector2f(1190.33,642.333));
        myCountour.push(new Vector2f(1191.08,640.917));
        myCountour.push(new Vector2f(1190.83,639.));
        myCountour.push(new Vector2f(1189.75,638.583));
        myCountour.push(new Vector2f(1188.83,639.417));
        myCountour.push(new Vector2f(1188.5,641.083));
        myCountour.push(new Vector2f(1187.5,641.583));
        myCountour.push(new Vector2f(1186.92,641.583));
        myCountour.push(new Vector2f(1186.83,640.417));
        myCountour.push(new Vector2f(1187.33,639.25));
        myCountour.push(new Vector2f(1187.25,638.333));
        myCountour.push(new Vector2f(1186.25,638.083));
        myCountour.push(new Vector2f(1184.25,637.833));
        myCountour.push(new Vector2f(1183.08,637.833));
        myCountour.push(new Vector2f(1181.92,637.));
        myCountour.push(new Vector2f(1180.75,636.5));
        myCountour.push(new Vector2f(1180.67,635.));
        myCountour.push(new Vector2f(1180.25,633.667));
        myCountour.push(new Vector2f(1179.,632.5));
        myCountour.push(new Vector2f(1177.92,632.25));
        myCountour.push(new Vector2f(1176.83,631.75));
        myCountour.push(new Vector2f(1176.08,631.));
        myCountour.push(new Vector2f(1174.92,629.833));
        myCountour.push(new Vector2f(1174.17,628.833));
        myCountour.push(new Vector2f(1172.17,628.5));
        myCountour.push(new Vector2f(1171.33,627.25));
        myCountour.push(new Vector2f(1170.83,626.25));
        myCountour.push(new Vector2f(1170.33,624.833));
        myCountour.push(new Vector2f(1169.08,624.333));
        myCountour.push(new Vector2f(1167.5,623.417));
        myCountour.push(new Vector2f(1167.42,622.25));
        myCountour.push(new Vector2f(1167.17,621.417));
        myCountour.push(new Vector2f(1167.08,619.833));
        myCountour.push(new Vector2f(1166.5,619.25));
        myCountour.push(new Vector2f(1166.17,618.833));
        myCountour.push(new Vector2f(1166.,617.667));
        myCountour.push(new Vector2f(1166.08,616.333));
        myCountour.push(new Vector2f(1166.5,614.5));
        myCountour.push(new Vector2f(1165.25,614.083));
        myCountour.push(new Vector2f(1164.17,613.75));
        myCountour.push(new Vector2f(1163.17,613.));
        myCountour.push(new Vector2f(1162.08,613.));
        myCountour.push(new Vector2f(1161.5,612.833));
        myCountour.push(new Vector2f(1160.58,612.167));
        myCountour.push(new Vector2f(1159.83,612.75));
        myCountour.push(new Vector2f(1158.58,612.333));
        myCountour.push(new Vector2f(1158.17,611.583));
        myCountour.push(new Vector2f(1157.5,611.167));
        myCountour.push(new Vector2f(1156.25,611.25));
        myCountour.push(new Vector2f(1154.5,613.));
        myCountour.push(new Vector2f(1154.58,614.167));
        myCountour.push(new Vector2f(1154.5,615.167));
        myCountour.push(new Vector2f(1153.5,615.75));
        myCountour.push(new Vector2f(1152.08,616.333));
        myCountour.push(new Vector2f(1151.33,616.75));

        var myShapeBuilder = new ShapeBuilder();
        var myElement = myShapeBuilder.appendElement("points", myMaterial.id);
        for (var myIndex = 0; myIndex < myCountour.length; myIndex++) {
            myShapeBuilder.appendVertex(myElement, new Vector3f(myCountour[myIndex][0], 
                                                                myCountour[myIndex][1], -10.0));
            myShapeBuilder.appendNormal(myElement, new Vector3f(0,1,0));
            myShapeBuilder.appendTexCoord(myElement, new Vector2i(0,0));
        }
        var myShape = myShapeBuilder.buildNode();
        myShape.name = "Italy";
        window.scene.shapes.appendChild(myShape);
        
        var myBody = Modelling.createBody(window.scene.world, myShape.id);
        window.camera.position = new Vector3f(1213.66,664.938,113.814);
        window.camera.orientation = new Quaternionf(0.140085,0.156472,0.977441,0.0223896);
    }

    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    ourShow.setup();
    ourShow.setTestDurationInFrames(75);
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
