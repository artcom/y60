//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function getSelectedMaterial() {
    var mySelectedMaterialName = ourMaterialComboBox.active_text;
    var mySelectedMaterial = getDescendantByName(window.scene.materials, mySelectedMaterialName, true);

    return mySelectedMaterial;
}

function updateMaterialEditor() {
    var mySelectedMaterial = getSelectedMaterial();
    ourFloatPropsComboBox = new ComboBoxText();
    ourGlade.get_widget("float_box").pack_end(ourFloatPropsComboBox, false, false);
    ourFloatPropsComboBox.show();

    ourFloatPropsComboBox.signal_changed.connect(ourFloatPropsComboBox, "on_changed");
    ourFloatPropsComboBox.on_changed = function() {
        var myProperty = getDescendantByName(getSelectedMaterial(), ourFloatPropsComboBox.active_text, true);
        ourGlade.get_widget("float_slider").value = myProperty.firstChild.nodeValue;
    }

    var myFloatValueFound = false;
    for (var i=0; i<mySelectedMaterial.properties.childNodesLength(); ++i) {
        var myChildNode = mySelectedMaterial.properties.childNode(i);
        // handle vector4f  
        if (myChildNode.nodeName == "vector4f") {
            updateColorElement(myChildNode);
        } else if (myChildNode.nodeName == "float") {
            ourFloatPropsComboBox.append_text(myChildNode.name); 
            if (!myFloatValueFound) {
                ourFloatPropsComboBox.active_text = myChildNode.name; 
                var myProperty = getDescendantByName(mySelectedMaterial, myChildNode.name, true);
                ourGlade.get_widget("float_slider").value = myProperty.firstChild.nodeValue;
            }
            myFloatValueFound = true; 
        }
    }
}

function updateMaterial(thePropertyName) {
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
        var myFloatSlider = ourGlade.get_widget("float_slider");
        myPropertyNode.firstChild.nodeValue = myFloatSlider.value;
    }
}

function updateColorElement(theNode) {
    var myColorPicker = ourGlade.get_widget(theNode.name+"_color");
   
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

