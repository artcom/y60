/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

function getSelectedMaterial() {
    var mySelectedMaterialName = ourMaterialComboBox.active_text;
    var mySelectedMaterial = getDescendantByName(window.scene.materials, mySelectedMaterialName, true);

    return mySelectedMaterial;
}

function hideFloatSliders() {
    for (var i=0; i<10; ++i) {
        ourGlade.get_widget("float_slider"+i).hide();
        ourGlade.get_widget("float_label"+i).text = "inactive";
        ourGlade.get_widget("float_label"+i).hide();
        ourGlade.get_widget("float_value"+i).hide();
        eval("ourHandler.on_float_slider"+i+"_value_changed = function() {}");
   }
}

function updateMaterialEditor() {
    var mySelectedMaterial = getSelectedMaterial();
    var myActiveFloatValue = 0;

    hideFloatSliders();

    for (var i=0; i<mySelectedMaterial.properties.childNodesLength(); ++i) {
        var myChildNode = mySelectedMaterial.properties.childNode(i);
        // handle vector4f
        if (myChildNode.nodeName == "vector4f") {
            updateColorElement(myChildNode);
        } else if (myChildNode.nodeName == "float") {
            var mySlider = ourGlade.get_widget("float_slider"+myActiveFloatValue);
            var myLabel  = ourGlade.get_widget("float_label"+myActiveFloatValue);
            var myValue  = ourGlade.get_widget("float_value"+myActiveFloatValue);

            if (myChildNode.name == "shininess") {
                mySlider.setRange(2.0,100.0);
            } else {
                mySlider.setRange(0.0,1.0);
            }

            var myProperty = getDescendantByName(mySelectedMaterial, myChildNode.name, true);
            mySlider.value = myProperty.firstChild.nodeValue;

            mySlider.show();
            myLabel.show();
            myValue.show();

            myValue.text = myProperty.firstChild.nodeValue.toFixed(3);
            myLabel.text = " "+myChildNode.name;

            eval("ourHandler.on_float_slider"+myActiveFloatValue+"_value_changed = function() {\n"+
                 "    var myMaterial = getSelectedMaterial();\n"+
                 "    if (!myMaterial) {\n"+
                 "        return;\n"+
                 "    }\n"+
                 "    var myProperty = getDescendantByName(myMaterial, '"+myChildNode.name+"', true);\n"+
                 "    if (myProperty) {\n"+
                 "        var myValue = ourGlade.get_widget(\"float_slider"+myActiveFloatValue+"\").value;\n"+
                 "        updateMaterial(myProperty.name, myValue);\n"+
                 "        ourGlade.get_widget(\"float_value"+myActiveFloatValue+"\").text = \" \"+(myValue.toFixed(3));"+
                 "    }\n"+
                 "}\n");

            myActiveFloatValue++;
        }
    }

    ourLastMaterial = mySelectedMaterial.name;
    //print("Last material:" + ourLastMaterial);
}

function updateMaterial(thePropertyName, theValue) {
	var isPaused = window.pause;
    window.pause = true;

    var myMaterial = getSelectedMaterial();
    var myPropertyNode = getDescendantByName(myMaterial.childNode("properties"), thePropertyName);

    if (myPropertyNode.nodeName == "vector4f") {
        var myColorPicker = ourGlade.get_widget(thePropertyName+"_color");

        var myLabel = ourGlade.get_widget(thePropertyName+"_value");
        myLabel.text = "["+(myColorPicker.color[0].toFixed(2))+
            ","+(myColorPicker.color[1].toFixed(2))+
            ","+(myColorPicker.color[2].toFixed(2))+
            ","+(myColorPicker.color[3].toFixed(2))+
            "] ";

        myPropertyNode.firstChild.nodeValue = myColorPicker.color;

    } else if (myPropertyNode.nodeName == "float") {
        myPropertyNode.firstChild.nodeValue = theValue;
    }
    window.pause = isPaused;
}

function updateColorElement(theNode) {
    var myColorPicker = ourGlade.get_widget(theNode.name+"_color");
    if ( ! myColorPicker ) {
        return;
    }

    // colorpicker
    myColorPicker.use_alpha = true;
    myColorPicker.signal_color_set.connect(ourHandler, "on_"+theNode.name+"_color_changed");

    var myMaterial = getSelectedMaterial();
    var myPropertyNode = getDescendantByName(myMaterial.childNode("properties"), theNode.name);
    myColorPicker.color = myPropertyNode.firstChild.nodeValue;

    eval("ourHandler.on_"+theNode.name+"_color_changed = function() {"+
         "    updateMaterial(\""+theNode.name+"\");"+
         "}");

    var myLabel = ourGlade.get_widget(theNode.name+"_value");
    myLabel.text = "["+(myColorPicker.color[0].toFixed(2))+
                   ","+(myColorPicker.color[1].toFixed(2))+
                   ","+(myColorPicker.color[2].toFixed(2))+
                   ","+(myColorPicker.color[3].toFixed(2))+
                   "] ";
}
