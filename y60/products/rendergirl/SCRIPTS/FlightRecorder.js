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
//
//   $RCSfile: FlightRecorder.js,v $
//   $Author: valentin $
//   $Revision: 1.4 $
//   $Date: 2005/04/27 17:35:13 $
//
//
//=============================================================================

use("Y60JSSL.js");
use("GUIUtils.js");
use("SceneViewer.js");
use("FrameRateLimiter.js");


var GLADE_FILE = "../GLADE/flightrecorder.glade";

function CameraKeyframe(theId,theSceneViewer, theCameraNode) {
    this.Constructor(this, theId, theSceneViewer, theCameraNode);
}

CameraKeyframe.prototype.Constructor = function(obj, theId, theSceneViewer, theCameraNode) {
    var _mySceneViewer = theSceneViewer;
    var _myCameraNode  = null;
    var _myWidgetRow   = null;

    // public methods
    obj.getNode = function()  {
        return _myCameraNode;
    }
    obj.setRow = function(theRow)  {
        _myWidgetRow = theRow;
    }
    obj.getRow = function()  {
        return _myWidgetRow;
    }
    // private functions
    function createNewCameraNode(theId) {
        _myCameraNode      = window.camera.cloneNode(false);
        _myCameraNode.id   = window.getNextFreeNodeId("camera");
        _myCameraNode.name = _myCameraNode.name + theId;
    }
    if (theCameraNode == undefined ) {
        createNewCameraNode(theId);
    } else {
        _myCameraNode = theCameraNode;
    }
}



function GTKFlightRecorder(theArguments) {
    this.Constructor(this, theArguments, "GTKFlightRecorder");
};

GTKFlightRecorder.prototype.Constructor = function(obj, theArguments, theName) {

    const FILE_EXTENSION = ".f60";

    var _myKeyframes = new Array();
    var _myHandler = {};
    var _mySceneViewer = null;
    var _myMainWindow = null
    var _mySceneWindow = null;
    var _myMainBox = null;
    var _myTreeView = null;
    var _myFlightToggleWidget = null;
    var _mySaveToDiskToggleWidget = null;
    var _myEditToggleWidget = null;
    //var _myLinearInterpolationToggleWidget = null;
    var _myFlightIdWidget = null;
    var _myDurationWidget = null;
    //var _myInterpolationTextWidget = null;
    var _myEditFrameTextWidget = null;
    var _myListScheme = new ACColumnRecord(3);
    var _myListStore = new ListStore(_myListScheme);

    var _myFlyTimeField = null;
    var _myUniqueCounter = 0;

    var _myCurrentRow             = 0;
    var _myCurrentKeyFrameRuntime = 0;
    var _myCurrentAnimationTime   = -1;
    var _myCurrentRunTime         = 0;
    var _myCurrentKeyFrameindex   = 0;
    var _myBlendingStarted        = false;
    var _myStartPos               = null;
    var _myPosDiff                = null;
    var _myStartOrientation       = null;
    var _myEndOrientation       = null;
    var _myRenderFrame            = 0;
    var _myLastImageStripFrame    = 1000;
    var _myFlightDuration         = 0;
    var _myImageStripPrefix       = "";
    var _myImageStripOverwriteFlag = true;
    var _myFps                     = 25;
    var _myCoordSpline            = null;
    var _myModelName              = null;
    var _myShaderLibrary          = null;
    var _myLastSelectedRowIndex   = null;
    var _myFrameRateLimiter       = new FrameRateLimiter(_myFps);

    parseArguments(theArguments);
    obj.SceneViewer = [];

    obj.onResize = function(theNewWidth, theNewHeight) {
        obj.SceneViewer.onResize(theNewWidth, theNewHeight);
    }


    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        obj.SceneViewer.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
    }
    obj.onMouseMotion = function(theX, theY) {
        if (_myTreeView && _myEditToggleWidget.active){
            var myRow = _myTreeView.selected_row;
            if (myRow) {
                var myKeyFrame = findKeyFrameByName(myRow.get_value(1)).keyframe;
                myKeyFrame.getNode().position = window.camera.position;
                myKeyFrame.getNode().orientation = window.camera.orientation;
            }
        }
        obj.SceneViewer.onMouseMotion(theX, theY);
    }
    obj.onFrame = function(theTime) {
        _myFrameRateLimiter.onFrame(theTime);
        obj.SceneViewer.onFrame(theTime);
        if (_myFlightToggleWidget.active) {
            if (_myCurrentAnimationTime == -1) {
                _myCurrentAnimationTime = theTime;
            }
            var myDeltaTime = 0.04;//theTime - _myCurrentAnimationTime;
            _myCurrentKeyFrameRuntime += myDeltaTime;
            //print(myDeltaTime)
            handleState();
            _myCurrentAnimationTime = theTime;
            _myRenderFrame++;
        }
    }

    obj.run = function() {

        var myGlade = new Glade(GLADE_FILE, "mainWindow");
        _myMainWindow = new Window();
        _myMainWindow.resize(1025,643);

        myGlade.reparent_widget("Outerbox", _myMainWindow);

        myGlade.autoconnect(_myHandler, _myMainWindow);

        _myMainBox = myGlade.get_widget("mainPanel");
        _myTreeView = myGlade.get_widget("treeview");
        _myFlyTimeField = myGlade.get_widget("sbFlyTime");
        _myFlightToggleWidget = myGlade.get_widget("pbPlay");
        _mySaveToDiskToggleWidget = myGlade.get_widget("pbRecord");
        _myEditToggleWidget = myGlade.get_widget("pbEdit");
//        _myInterpolationTextWidget         = myGlade.get_widget("Interpolation");
//        _myInterpolationTextWidget.text = "Linear";
//        _myLinearInterpolationToggleWidget = myGlade.get_widget("pbInterpolation");
        _myEditFrameTextWidget = myGlade.get_widget("EditFrameText");
        _myEditFrameTextWidget.text = "Off";
        _myFlightIdWidget = myGlade.get_widget("FlightId");
        _myDurationWidget = myGlade.get_widget("Duration");
        _myFlightIdWidget.text = "flight";
        GtkMain.run(_myMainWindow);

    };

    obj.onPostRender = function() {
        obj.SceneViewer.onPostRender();
        if (_mySaveToDiskToggleWidget.active && _myFlightToggleWidget.active) {
            saveImage();
        }
    }


    // ************************************************
    //
    // GTK Signal callbacks
    //
    // ************************************************
    _myHandler.on_about1_activate = function() {
        print("(c) A+C 11/2004 Flightrecorder");
    }
    _myHandler.on_pbRecord_toggled = function() {
        _myImageStripPrefix = _myFlightIdWidget.text + "\\frame";
        // does the directory exist ?
        if (getDirectoryEntries(_myFlightIdWidget.text)== null) {
            createDirectory(_myFlightIdWidget.text);
        }
    }

    _myHandler.on_pbEdit_toggled = function() {
        _myEditFrameTextWidget.text = (_myEditToggleWidget.active ? "On" : "Off");
    }
/*    _myHandler.on_pbInterpolation_toggled = function() {
        _myInterpolationTextWidget.text = (_myLinearInterpolationToggleWidget.active ? "Spline" : "Linear");
    }*/

    _myHandler.on_new_activate = function() {
        _myKeyframes = [];
        _myUniqueCounter = 0;
        _myListStore.clear();
    }

    _myHandler.on_load_scene1_activate = function() {

        var myFileChooserDialog = new FileChooserDialog("lade Scene");

        myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
        myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);
        var myRetVal = myFileChooserDialog.run();
        myFileChooserDialog.hide();

        var myFilename = null;
        if (myRetVal == Dialog.RESPONSE_OK) {
            myFilename = myFileChooserDialog.get_filename();
        } else {
            return;
        }

        // set up View
        _myTreeView.set_model(_myListStore);
        _myTreeView.append_column("ID", _myListScheme, 0);
        _myTreeView.append_column_editable("Watchpoint", _myListScheme, 1);
        _myTreeView.append_column("Timestamp", _myListScheme, 2);
        // start Renderer
        window = new RenderArea();
        window.show();
        _myMainBox.add(window);
        _myMainBox.position = 300;


        obj._mySceneViewer = new SceneViewer([myFilename, _myShaderLibrary]);

        obj._mySceneViewer.setup();
        obj._mySceneViewer.setMover(ClassicTrackballMover);


        obj.SceneViewer.onKey = obj._mySceneViewer.onKey;
        obj._mySceneViewer.onKey = obj.onKey;

        obj.SceneViewer.onResize = obj._mySceneViewer.onResize;
        obj._mySceneViewer.onResize = obj.onResize;


        obj.SceneViewer.onMouseMotion = obj._mySceneViewer.onMouseMotion;
        obj._mySceneViewer.onMouseMotion = obj.onMouseMotion;

        obj.SceneViewer.onFrame = obj._mySceneViewer.onFrame;
        obj._mySceneViewer.onFrame = obj.onFrame;

        obj.SceneViewer.onPostRender = obj._mySceneViewer.onPostRender;
        obj._mySceneViewer.onPostRender = obj.onPostRender;

        window.setIdle(true);
        window.fixedDeltaT = 1.0/_myFps;
        calcDuration();
    }
    _myHandler.on_savescene2_activate = function(theMenu) {
        window.saveScene("saved_scene.x60", false);
    }

    _myHandler.on_open1_activate = function(theMenu) {
        var myFileChooserDialog = new FileChooserDialog("lade Scene");

        myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
        myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);
        var myRetVal = myFileChooserDialog.run();
        myFileChooserDialog.hide();

        var myFilename = null;
        if (myRetVal == Dialog.RESPONSE_OK) {
            myFilename = myFileChooserDialog.get_filename();
        } else {
            return;
        }

        _myKeyframes = [];
        _myUniqueCounter = 0;
        _myListStore.clear();
        if (fileExists(myFilename)) {
            var myFlightDom = new Node();
            myFlightDom.parseFile(myFilename);
            var myKeyframes = myFlightDom.childNodes[0];
            for (var i = 0; i < myKeyframes.childNodes.length; ++i) {
                var myKeyframeNode = myKeyframes.childNode(i);
                var myCameraNode = myKeyframeNode.childNodes[0];
                var myNewRow = _myListStore.append();
                myNewRow.set_value(0, _myUniqueCounter);
                myNewRow.set_value(1, myCameraNode.name);
                myNewRow.set_value(2, myKeyframeNode.flyTime);
                var myKeyframe = new CameraKeyframe(_myUniqueCounter , obj._mySceneViewer, myCameraNode);
                myKeyframe.setRow(myNewRow);
                _myKeyframes.push(myKeyframe);
                _myUniqueCounter++;
            }
            calcDuration();
            print("Loaded flight: " + myFilename + " with " + _myKeyframes.length +
                  " frames, duration: " + _myFlightDuration + " total frames " + _myLastImageStripFrame);
        } else {
            print("Sorry, could not load flight: " + _myFlightIdWidget.text+ FILE_EXTENSION);
        }
    }

    _myHandler.on_save1_activate = function(theMenu) {
        var myFileChooserDialog = new FileChooserDialog("save Scene", FileChooserDialog.ACTION_SAVE);

        myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
        myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);
        var myRetVal = myFileChooserDialog.run();
        myFileChooserDialog.hide();

        var myFilename = null;
        if (myRetVal == Dialog.RESPONSE_OK) {
            myFilename = myFileChooserDialog.get_filename();
        } else {
            return;
        }
        if (fileExists(myFilename)) {
            var myDialog = new MessageDialog("<b>File exists.</b>\nDo you want to overwrite it?",
                                             true, MessageDialog.MESSAGE_QUESTION,
                                             MessageDialog.BUTTONS_YES_NO, true);
            var myResponse = myDialog.run();
            // :-( otherwise dialog won't close before mainwindow gets focus
            myDialog.hide();

            if ( myResponse != Dialog.RESPONSE_YES) {
                return;
            }
        }

        var myFlight = '<Flight name="MyCurrentFlight"/>';
        var myFlightDoc = new Node(myFlight);
        for(var i = 0; i < _myKeyframes.length; i++) {
            var myCameraNode = _myKeyframes[i].getNode().cloneNode();
            var myKeyFrame     = _myKeyframes[i];
            var myKeyframeNodeStr = '<Keyframe/>';
            var myKeyframeDoc = new Node(myKeyframeNodeStr);
            var myKeyframeNode = myKeyframeDoc.childNodes[0];
            myKeyframeNode.flyTime = myKeyFrame.getRow().get_value(2);

            myKeyframeNode.appendChild(myCameraNode)
            myFlightDoc.childNodes[0].appendChild(myKeyframeNode);
        }
        if (myFilename.length < 4) {
           myFilename = myFilename + FILE_EXTENSION;
        } else {
           var myFileEnding = myFilename.substring((myFilename.length)-4, myFilename.length);
           if (myFileEnding != FILE_EXTENSION) {
               myFilename = myFilename + FILE_EXTENSION;
            }
        }

        myFlightDoc.saveFile(myFilename);
        print("Saved Keyframes: " + myFilename);
    }

    _myHandler.on_treeview_cursor_changed = function(theTreeWidget) {
        _myEditToggleWidget.active = false;

        var myRow = theTreeWidget.selected_row;
        if (myRow) {
            var myKeyFrameIndex = findKeyFrameByName(myRow.get_value(1)).index;
            if (myKeyFrameIndex == _myLastSelectedRowIndex) {
                theTreeWidget.unselect(myRow);
                _myLastSelectedRowIndex = -1;
                return;
            }
            _myFlyTimeField.value = myRow.get_value(2);
            var myKeyFrame = findKeyFrameByName(myRow.get_value(1)).keyframe;
            setKeyFrame(myKeyFrame);
        }
        _myLastSelectedRowIndex = myKeyFrameIndex;
        calcDuration();
    }

    _myHandler.on_quit_activate = function() {
        GtkMain.quit();
    }

    _myHandler.on_wireframe_activate = function(theSource) {
        renderer.wireframe = theSource.active;
    }

    _myHandler.on_backfaceculling_activate = function(theSource) {
        renderer.backfaceCulling = theSource.active;
    }

    _myHandler.on_lighting_activate = function(theSource) {
        renderer.lighting = theSource.active;
    }

    _myHandler.on_sbFlyTime_changed = function(theSource) {
        var myRow = _myTreeView.selected_row;
        if (myRow) {
            myRow.set_value(2,theSource.value);
            calcDuration();
        }
    }

    _myHandler.on_pbAdd_clicked = function(theSource) {

        var myKeyframe = new CameraKeyframe(_myUniqueCounter , obj._mySceneViewer);
        var mySelectedRow = _myTreeView.selected_row;
        var myNewRow = null;
        if (mySelectedRow) {
            var myKeyFrameIndex = findKeyFrameByName(mySelectedRow.get_value(1)).index;
            _myKeyframes.splice(myKeyFrameIndex+1,0, myKeyframe);
            myNewRow = _myListStore.insert_after(mySelectedRow);
            _myTreeView.selected_row = myNewRow;
        } else {
            myNewRow = _myListStore.append();
            _myKeyframes.push(myKeyframe);
        }
        if (myNewRow) {
            myNewRow.set_value(0, _myUniqueCounter);
            myNewRow.set_value(1, myKeyframe.getNode().name);
            myNewRow.set_value(2,0.0);
            if (_myKeyframes.length == 1) {
                myNewRow.set_value(2,0.0);
            } else {
                myNewRow.set_value(2,1.0);
            }
        }
        myKeyframe.setRow(myNewRow);
        _myUniqueCounter++;
        calcDuration();
    }

    _myHandler.on_pbRemove_clicked = function(theSource) {
        var mySelectedRow = _myTreeView.selected_row;
        if (mySelectedRow) {
            var myKeyFrameIndex = findKeyFrameByName(mySelectedRow.get_value(1)).index;
            _myListStore.erase(mySelectedRow);
            _myKeyframes.splice(myKeyFrameIndex, 1)
       }
        calcDuration();
    }

    _myHandler.on_FlightId_changed = function(theTextEntry) {}
    _myHandler.on_treeview_columns_changed = function(theTextEntry) {}

    _myHandler.on_pbPlay_toggled = function(theSource) {
        if (_myFlightToggleWidget.active) {
            calcDuration();
            if (_myKeyframes.length == 0) {
                _myFlightToggleWidget.active = false;
                return;
            }
            _myRenderFrame = -1;
            _myCurrentRow = _myTreeView.selected_row;
            var myKeyFrame = 0;
            if (_myCurrentRow) {
                myKeyFrame = findKeyFrameByName(_myCurrentRow.get_value(1));
                setKeyFrame(myKeyFrame.keyframe);
                _myCurrentKeyFrameindex = myKeyFrame.index;
            } else {
                if (_myKeyframes.length > 0) {
                    setKeyFrame(_myKeyframes[0]);
                    _myCurrentRow = _myKeyframes[0].getRow();
                    _myCurrentKeyFrameindex = 0;
                } else {
                    print("Sorry, either load or define a flight");
                    return;
                }
            }
            _myTreeView.selected_row = _myCurrentRow;
            _myCurrentKeyFrameRuntime = 0;
            _myCurrentAnimationTime = -1;
        }
        // setup spline
        if (false) { //_myLinearInterpolationToggleWidget.active) {
            var myKeyframes = [];
            var i = 0;
            var myTime = 0;
            var myCameraKeyFrame = 0;
            for(i = 0; i < _myCurrentKeyFrameindex+1; i++) {
                myCameraKeyFrame = _myKeyframes[i];
                myTime = Number(myCameraKeyFrame.getRow().get_value(2));
                _myCurrentKeyFrameRuntime += myTime;
            }
            print("time: " + _myCurrentKeyFrameRuntime);
            for(i = _myCurrentKeyFrameindex; i < _myKeyframes.length; i++) {
                myCameraKeyFrame = _myKeyframes[i];
                myTime = Number(myCameraKeyFrame.getRow().get_value(2));
                if (myCameraKeyFrame.getNode().position.value == undefined ) {
                    var myPos = new Vector3f(stringToArray(myCameraKeyFrame.getNode().position));
                    var myOrient = new Vector3f(stringToArray(myCameraKeyFrame.getNode().orientation));
                    myKeyframes.push(new Keyframe(myPos, myOrient, myTime, 1.0));
                } else {
                    myKeyframes.push(new Keyframe(myCameraKeyFrame.getNode().position, myCameraKeyFrame.getNode().orientation, myTime, 1.0));
                }
            }
            _myCoordSpline  = new CoordSpline(myKeyframes);
            //_myCoordSpline.print();
        }
    }

    function setKeyFrame(theKeyframe) {
        if (theKeyframe) {
            window.camera.position    = theKeyframe.getNode().position;
            window.camera.orientation = theKeyframe.getNode().orientation;
            window.camera.scale       = theKeyframe.getNode().scale;
            window.queue_draw();
        }
    }

    function blendKeyFrame( theKeyframe0, theKeyframe1, theBlendFactor) {
        if (!_myBlendingStarted) {
            var myFrame0 = _myKeyframes[theKeyframe0];
            var myFrame1 = _myKeyframes[theKeyframe1];
            if (myFrame0 && myFrame1) {
                var myEndPos = null;
                var myTemp = null;
                if (myFrame0.getNode().position.value == undefined ) {
                    // keyframes node comes from a non-schema file, convert orien+pos to vector3f
                    _myStartPos               = new Vector3f(stringToArray(myFrame0.getNode().position));
                    myTemp = new Vector4f(stringToArray(myFrame0.getNode().orientation));
                    _myStartOrientation       = new Quaternionf(myTemp[0], myTemp[1], myTemp[2], myTemp[3]);
                } else {
                    // keyframe nodes come directly from x60-dom, so they have the vector3f type info
                    _myStartPos               = myFrame0.getNode().position;
                    _myStartOrientation       = myFrame0.getNode().orientation;
                }
                if (myFrame1.getNode().position.value == undefined ) {
                    myEndPos                  = new Vector3f(stringToArray(myFrame1.getNode().position));
                    myTemp = new Vector4f(stringToArray(myFrame1.getNode().orientation));
                    _myEndOrientation         = new Quaternionf(myTemp[0], myTemp[1], myTemp[2], myTemp[3]);
                } else {
                    _myEndOrientation         = myFrame1.getNode().orientation;
                    myEndPos                  = myFrame1.getNode().position;
                }
                _myPosDiff                = difference(myEndPos, _myStartPos);

                //print("Nachher: " +_myStartOrientation, _myEndOrientation, _myOrienDiff);
                _myBlendingStarted        = true;
            }
        }
        if (_myBlendingStarted) {
            // linear blending
            var myScalar = new Vector3f(theBlendFactor, theBlendFactor ,theBlendFactor);
            var myNewPos = sum (_myStartPos, product( _myPosDiff, myScalar) );

            var myOldPos = window.camera.position;
            var myOldOrientation = window.camera.orientation;
            window.camera.position    = myNewPos;
            window.camera.orientation = Quaternionf.slerp(_myStartOrientation, _myEndOrientation, theBlendFactor);
            window.queue_draw();
        }
    }

    function resetAnimation() {
        _myCurrentAnimationTime   = -1;
        _myCurrentKeyFrameRuntime = 0;
        _myBlendingStarted = false;
        _myFlightToggleWidget.active = false;
        _myTreeView.selected_row = _myKeyframes[_myKeyframes.length-1].getRow();
        print("Flight is done");

    }
    function handleState() {
        if (true) {//!_myLinearInterpolationToggleWidget.active) {
            var myKeyFrameIndex = findKeyFrameByName(_myCurrentRow.get_value(1)).index;
            //print(myKeyFrameIndex)
            if (_myCurrentKeyFrameindex > _myKeyframes.length-2) {
                resetAnimation();
                return;
            }
            var myFlyTime  = Number(_myKeyframes[myKeyFrameIndex+1].getRow().get_value(2));
            if (_myCurrentKeyFrameRuntime > myFlyTime) {
                // the keyframe is done
                blendKeyFrame(_myCurrentKeyFrameindex, _myCurrentKeyFrameindex+1, 1.0);
                _myCurrentKeyFrameindex++;
                _myTreeView.selected_row = _myKeyframes[_myCurrentKeyFrameindex].getRow();
                _myCurrentAnimationTime   = -1;
                _myCurrentKeyFrameRuntime = 0;
                _myBlendingStarted = false;
                _myCurrentRow = _myKeyframes[_myCurrentKeyFrameindex].getRow();
                setKeyFrame( _myKeyframes[_myCurrentKeyFrameindex]);
            } else {
                // blend
                if (myFlyTime != 0) {
                    var myBlendFactor = (_myCurrentKeyFrameRuntime) / myFlyTime;
                    blendKeyFrame( _myCurrentKeyFrameindex, _myCurrentKeyFrameindex+1, myBlendFactor);
                }
            }
        } else {
            if (_myCurrentKeyFrameRuntime > _myFlightDuration) {
                resetAnimation();
                return;
            }
            var myCorrectedTimeStamp = _myCurrentKeyFrameRuntime;//_myCoordSpline.s(_myCurrentKeyFrameRuntime);
            var myOrientation = _myCoordSpline.orientation(myCorrectedTimeStamp);
            myOrientation[0] -= Math.PI;
            myOrientation[1] *= -1.0;
            myOrientation[2] += Math.PI;
            window.camera.orientation = Quaternionf.createFromEuler(myOrientation);
            window.camera.position    = _myCoordSpline.position(myCorrectedTimeStamp);
            window.queue_draw();

        }
    }
    function calcDuration() {
        _myFlightDuration = 0;
        for(var i = 0; i < _myKeyframes.length; i++) {
            var myKeyFrame = _myKeyframes[i];
            _myFlightDuration+= Number(myKeyFrame.getRow().get_value(2));
        }
        _myLastImageStripFrame = _myFlightDuration * _myFps;
        _myDurationWidget.text = _myFlightDuration + " sec.";
    }

    function saveImage() {
        var myDigits = Math.ceil(Math.log(_myLastImageStripFrame+1)/Math.LN10);
        if (_myRenderFrame > 0) {
            for(var i = 1; i < myDigits+1; i++) {
                var myBase = Math.pow(10,i);
                if (_myRenderFrame < myBase) {
                    myDigits = myDigits-i;
                    break;
                }
            }
        }
        var myCounter = "";
        for(i = 0; i < myDigits; i++) {
            myCounter = myCounter + "0";
        }
        if (_myRenderFrame >0) {
            myCounter += _myRenderFrame;
        }
        var myFilename = _myImageStripPrefix + "_" + myCounter + ".png";
        if (fileExists(myFilename) == false || _myImageStripOverwriteFlag) {
            window.saveBuffer(myFilename);
            print("saved Image:" + myFilename);
        } else {
            print("Sorry, " + myFilename + ", does exist and no overwrite flag is set: do nothing!");
        }

    }
    function findKeyFrameByName(theName) {
        for(var i = 0; i < _myKeyframes.length; i++) {
            var myKeyFrame = _myKeyframes[i];
            if (myKeyFrame.getNode().name == theName) {
                return {keyframe:myKeyFrame, index:i};
            }
        }
        return 0;
    }

    function parseArguments(theArguments) {
        for (var i = 0; i < theArguments.length; ++i) {
            var myArgument = theArguments[i];
            if (!_myShaderLibrary && myArgument.search(/xml/) != -1) {
                // Take the first xml-file as shader library
                _myShaderLibrary = myArgument;
            } else if (myArgument.search(/\.[xb]60/) != -1 ||
                       myArgument.search(/\.st./) != -1)
            {
                _myModelName = myArgument;
            }
        }

        if (!_myShaderLibrary) {
            throw new Exception("Missing shaderlibrary argument", "FlightRecorder::parseArguments()");
        }
    }

};

var ourShow = new GTKFlightRecorder(arguments);
ourShow.run();


