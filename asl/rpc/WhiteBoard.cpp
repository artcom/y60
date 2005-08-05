
//==============================================================================
// Copyright (c) 2002 ART+COM AG Berlin
// All rights reserved.
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//==============================================================================
//
// $Id: WhiteBoard.cpp,v 1.1.1.1 2003/05/12 14:20:23 uzadow Exp $
// $Author: uzadow $
// $Revision: 1.1.1.1 $
// $Date: 2003/05/12 14:20:23 $
//
// (CVS log at the bottom of this file)
//
//==============================================================================

//#define DEBUG_LEVEL 2
#include "WhiteBoard.h"
#include <asl/Station.h>

#include <asl/XmlTypes.h>
#include <asl/ThreadLock.h>
#include <asl/Auto.h>
#include <asl/os_functions.h>
#include <asl/string_functions.h>
#include <asl/Time.h>
#include "oclient.h"
#include "string_aux.h"
#include "DirectorLink.h"

#include <asl/Debug.h>


using namespace std;  // automatically added!



#define DB(x) //  x
#define DB2(x) // x

double BoardSheet::_defaultMaxAge_ = 0.10;

BoardSheet::BoardSheet(const string & theId, xml::NodePtr theSheetValue, 
            const string & theSetter, int theSetCount = 0) 
        : _myName(theId),
        _myValue(theSheetValue),
        _mySetter(theSetter),
        _setCount(theSetCount),
        _lastModified(asl::Time()),
        _lastCopied(asl::Time()+_defaultMaxAge_),
        _maxAge(_defaultMaxAge_)
{
    DB(
    if (theSheetValue && *theSheetValue) {
    } else {
        cerr << "# WARNING: BoardSheet::BoardSheet empty value for sheet with id '" << theId <<"'"; 
        if (!theSheetValue) cerr << "(theSheetValue is 0)" << endl;
        else cerr << "(*theSheetValue is false)" << endl;
    }
    );
}
    
void
BoardSheet::setSheetValue(xml::NodePtr theSheetValue) {
#ifdef SHEET_LEVEL_LOCKS
    AutoWriteLocker autoLocker(_lock);
#endif
    if (theSheetValue) {
        if (!*theSheetValue) {
            cerr << "# WARNING: BoardSheet::setSheet no value given for sheet with id '" << _myName <<"'"<< endl; 
        }
        _myValue = theSheetValue;
        ++_setCount;
        _lastModified = asl::Time();
        if (_lastCopied > _lastModified) {
            _lastCopied = _lastModified;
        }
    } else {
        throw asl::Exception(JUST_FILE_LINE);
    }
}

const string &
BoardSheet::getSetter() const {
    //AutoReadLocker autoLocker(_lock);
    return _mySetter;
}
const string &
BoardSheet::getName() const {
    //AutoReadLocker autoLocker(_lock);
    return _myName;
}

xml::NodePtr
BoardSheet::getValueNode(const string & theGetter, bool doCount) const 
{
#ifdef SHEET_LEVEL_LOCKS
    AutoReadLocker autoLocker(_lock);
#endif
    xml::NodePtr returnValue;
    returnValue = xml::NodePtr(new xml::Element("Sheet"));
    
    if (_myValue && *_myValue) {
        returnValue->AddChild(_myValue);
    } else {
        DB(cerr << "# WARNING: BoardSheet::getValueNode: returning empty sheet with id '" << _myName << "'"); 
        DB(if (!_myValue) cerr << "(_myValue is 0)" << endl);
        DB(else cerr << "(*_myValue is false)" << endl);
    }

    if (doCount) {
        ++_getterMap[theGetter];
    }
    (*returnValue)["id"] = _myName;
    if (theGetter.size()) {
        (*returnValue)["getter"] = theGetter;
    }
    
    (*returnValue)["gets"] = asl::as_string(_getterMap[theGetter]); 
    (*returnValue)["sets"] = asl::as_string(_setCount);
    if (_mySetter.size()) {
        (*returnValue)["setter"] = _mySetter;
    }
     
    if (_lastModified>=_lastCopied) {
        (*returnValue)["modified"] = "yes";
    }

    int mySumGet = 0;
    for (map<string, int>::iterator it = _getterMap.begin(); it != _getterMap.end(); ++it) {
        mySumGet += it->second;
    }
    (*returnValue)["totalgets"] = asl::as_string(mySumGet);
    return returnValue;
}

const xml::NodePtr
BoardSheet::getValue() const {
#ifdef SHEET_LEVEL_LOCKS
    AutoReadLocker autoLocker(_lock);
#endif
    return _myValue;
}

asl::Ptr<BoardSheet>
BoardSheet::clone(const string & theGetter) const {
    ++_getterMap[theGetter];
    asl::Ptr<BoardSheet> myResult = asl::Ptr<BoardSheet>(new BoardSheet(_myName, _myValue->Clone(), _mySetter, _setCount));
#define MODIFIED_HAVE_PRIORITY
#ifdef MODIFIED_HAVE_PRIORITY
    if (getLastCopied() <= getLastModified()) {
        myResult->setLastModified(getLastModified());    
        myResult->setLastCopied(getLastCopied());    
    }
#endif
    return myResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void
IWhiteBoard::copyInto(IWhiteBoard & theBoard, IWhiteBoard::CopyFilterFunction doCopy) const {
    cerr << "IWhiteBoard::copyInto: not yet implemented" << endl;
};
void
IWhiteBoard::clear() {
    cerr << "IWhiteBoard::clear: not yet implemented" << endl;
};
void
IWhiteBoard::assignSheet(const string & theId, asl::Ptr<BoardSheet> theValue) {
    cerr << "IWhiteBoard::assignSheet: not yet implemented" << endl;
}
void
IWhiteBoard::readlock() {
    cerr << "IWhiteBoard::readLock not yet implemented" << endl;
}
void
IWhiteBoard::readunlock() {
    cerr << "IWhiteBoard::readunLock not yet implemented" << endl;
}
void
IWhiteBoard::writelock() {
    cerr << "IWhiteBoard::readLock not yet implemented" << endl;
}
void
IWhiteBoard::writeunlock() {
    cerr << "IWhiteBoard::readunLock not yet implemented" << endl;
}
asl::Ptr<BoardSheet> 
IWhiteBoard::getSheetFast(const string & theId) const {
    cerr << "IWhiteBoard::getSheetFast not yet implemented" << endl;
    return asl::Ptr<BoardSheet>(0);
}

xml::NodePtr
IWhiteBoard::asNode() const {
    cerr << "IWhiteBoard::asNode() not yet implemented" << endl;
    return xml::NodePtr(0);
}
bool
IWhiteBoard::fromNode(const xml::Node & elem) {
    cerr << "IWhiteBoard::fromNode() not yet implemented" << endl;
    return false;
}

bool IWhiteBoard::update() {
    //cerr << "IWhiteBoard::update() not yet implemented" << endl;
    return false;
}

void IWhiteBoard::broadcast() {
    //cerr << "IWhiteBoard::broadcast() not yet implemented" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////

void
WhiteBoard::getSheetIds(vector<string>& ids) {
    ids.clear();
    AutoReadLocker autoLocker(_lock);
    for (SheetMap::iterator it = _myBoard.begin(); it != _myBoard.end(); ++it) {
        ids.push_back(it->second->getName());
    }
}

void
WhiteBoard::clear() {
    AutoWriteLocker autoLocker(_lock);
    _myBoard.erase(_myBoard.begin(),_myBoard.end());
}

void
WhiteBoard::assignSheet(const string & theId, asl::Ptr<BoardSheet> theValue) {
    _myBoard[theId]=theValue;
};

bool
IWhiteBoard::copyAllFilter(const asl::Ptr<BoardSheet> theSheet,
                                IWhiteBoard & theDestination)
{
    theDestination.assignSheet(theSheet->getName(),theSheet->clone());
    theSheet->setLastCopied(asl::Time());
    return true;
}

bool
IWhiteBoard::mergeAllFilter(const asl::Ptr<BoardSheet> theSheet,
                                  IWhiteBoard & theDestination)
{
    asl::Ptr<BoardSheet> myDestinationSheet = theDestination.getSheetFast(theSheet->getName());
    asl::Ptr<BoardSheet> newSheet(0);

    bool retransmitImmediately = false;

    if (myDestinationSheet) {
        DB(cerr << "mergeAllFilter: destination " << myDestinationSheet->getName() 
                <<" sheet set count: " << myDestinationSheet->getSetCount()
                << ", source " << theSheet->getName() 
                << " sheet set count: " << theSheet->getSetCount() 
                <<" last modified: " << asl::Time(theSheet->getLastModified())
                <<" last copied: " << asl::Time(theSheet->getLastCopied())
                << endl);

        if (theSheet->getLastModified()==theSheet->getLastCopied()) {   // REMOTE MODIFIED 

            // Update incoming setcount when local count is larger and remote modified
            if (myDestinationSheet->getSetCount() >= theSheet->getSetCount()) // LARGER OR EQUAL LOCAL SETCOUNT
            {
                DB(cerr << "mergeAllFilter: setting source set count to: " << theSheet->getSetCount()+1 << endl);
                newSheet = theSheet->clone();
                newSheet->setSetCount(myDestinationSheet->getSetCount()+1);
                retransmitImmediately = true; // we have a new setcount for everybody, so let them know
            }

        } else { // NOT REMOTE MODIFIED 

            // Check setcount on not remote modified sheets, should normally be equal
            if(myDestinationSheet->getSetCount() < theSheet->getSetCount())  // LARGER REMOTE SETCOUNT
            {
                // Update destination setcount when incoming count is larger, just in case we don't make it
                // to the bottom assignSheet
                myDestinationSheet->setSetCount(theSheet->getSetCount());
                DB(cerr << "mergeAllFilter: setting destination set count to: " << theSheet->getSetCount() << " and returning" << endl);
            } else if(myDestinationSheet->getSetCount() > theSheet->getSetCount()) {  // LARGER LOCAL SETCOUNT
                // Ignore outdated unmodified sheets with low setcount
                DB(cerr << "mergeAllFilter: Ignoring outdated unmodified sheet with low setcount" << endl);
                return false;
            }        }

        // Do not update modified and not yet sent sheets
        if (myDestinationSheet->getLastModified() > myDestinationSheet->getLastCopied()) 
        {
            DB(cerr << "mergeAllFilter: last modified > last copied, returning false" << endl);
            return false;
        }
        if (!newSheet) {
            newSheet = theSheet->clone();
        }
        newSheet->setGetterMap(myDestinationSheet->getGetterMap());
    } else {
        newSheet = theSheet->clone();
    }
    DB(cerr << "mergeAllFilter: assigning new sheet" << endl);
    if (!retransmitImmediately) {
        newSheet->setLastCopied(newSheet->getLastModified() + BoardSheet::getDefaultMaxAge());
    }
    theDestination.assignSheet(newSheet->getName(), newSheet);
    theSheet->setLastCopied(asl::Time());
    return true;
}


bool
IWhiteBoard::copyModifiedFilter(const asl::Ptr<BoardSheet> theSheet,
                                IWhiteBoard & theDestination)
{
    if (theSheet->getLastModified() >= theSheet->getLastCopied()) {
        DB2(cerr << "copyModifiedFilter cloning " << theSheet->getName() << endl);    
        theDestination.assignSheet(theSheet->getName(),theSheet->clone());
        theSheet->setLastCopied(asl::Time());
        return true;
    }
    DB2(cerr << "copyModifiedFilter IGNORING " << theSheet->getName() << endl);    
    return false;
}
bool
IWhiteBoard::copyAgedFilter(const asl::Ptr<BoardSheet> theSheet,
                                IWhiteBoard & theDestination)
{
    if (asl::Time() - theSheet->getLastCopied() > theSheet->getMaxAge()) {
        theDestination.assignSheet(theSheet->getName(),theSheet->clone());
        theSheet->setLastCopied(asl::Time());
        return true;
    }
    return false;
}


bool
IWhiteBoard::copyModifiedAndOldestAgedFilter(const asl::Ptr<BoardSheet> theSheet,
                                IWhiteBoard & theDestination)
{
    // HACK: The functionality here is wired into the copy function
    return copyModifiedFilter(theSheet, theDestination);
}

bool
IWhiteBoard::copyModifiedOrAgedFilter(const asl::Ptr<BoardSheet> theSheet,
                                IWhiteBoard & theDestination)
{
    return copyModifiedFilter(theSheet, theDestination) || copyAgedFilter(theSheet, theDestination);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void
WhiteBoard::copyInto(IWhiteBoard & theBoard, IWhiteBoard::CopyFilterFunction doCopy) const {
    AutoReadLocker autoLocker(_lock);
    asl::AutoWriteLocker<IWhiteBoard> autoWriteLocker(theBoard);
    asl::Ptr<BoardSheet> myOldestSheet(0);
    for (SheetMap::const_iterator it = _myBoard.begin(); it != _myBoard.end(); ++it) {
        if (!doCopy(it->second, theBoard) && doCopy == &copyModifiedAndOldestAgedFilter) {
            if (!myOldestSheet || (myOldestSheet->getLastCopied() > it->second->getLastCopied())) {
                if (asl::Time() - it->second->getLastCopied() > it->second->getMaxAge()) {
                    myOldestSheet = it->second;
                }
            }
        }
    }
    if (myOldestSheet) {
        copyAllFilter(myOldestSheet,theBoard);
    }
};

xml::NodePtr
WhiteBoard::asNode() const {
    string dummy = "";
    AutoReadLocker autoLocker(_lock);
    xml::NodePtr myResult = xml::NodePtr(new xml::Element("WhiteBoard"));
    for (SheetMap::const_iterator it = _myBoard.begin(); it != _myBoard.end(); ++it) {
        myResult->AddChild(it->second->getValueNode(dummy,false));
    }
    return myResult;
}

bool
WhiteBoard::fromNode(const xml::Node & elem) {
    DB2(cerr << "WhiteBoard::fromNode" << endl);
    DB2(cerr << "WhiteBoard::fromNode: converting " << elem << endl);
    if (elem.Name() != "WhiteBoard") {
        return false;
    }
    clear();
    AutoWriteLocker autoLocker(_lock);
    for (int i = 0; i < elem.NumberOfChildren();++i) {
        DB2(cerr << "WhiteBoard::fromNode: Inserting myChild nr " << i << endl);
        const xml::NodePtr myChild = elem.GetChild(i);
        if (myChild->Name() == "Sheet") {
            string myName = (*myChild)["id"].Value(); 
            xml::NodePtr myChildValue;
            
            if (myChild->NumberOfChildren() == 1) {
                myChildValue = myChild->GetChild(0);
            }

            int mySetCount = asl::as<int>((*myChild)["sets"].Value());
            asl::Ptr<BoardSheet> mySheet(
                    new BoardSheet(myName, myChildValue, (*myChild)["setter"].Value(), mySetCount));
            DB(cerr << "WhiteBoard::fromNode: Inserting myChild = " << endl << *myChild);
            if ((*myChild)["modified"].Value()=="yes") {
                DB(cerr << "WhiteBoard::fromNode: received modified sheet '" << myName << "'" << endl;)
                mySheet->setLastCopied(mySheet->getLastModified());
            }
            assignSheet(myName,mySheet);
        } else {
            cerr << "#### WARNING: Unknown child in whiteboard; expected 'Sheet', found '"<<*myChild<<endl;
        }
    }
    return true;
}

void
WhiteBoard::createSheet(const string & theId, const string & theSetter, xml::NodePtr theValue) {

    AC_DB2("WhiteBoard::createSheet(): id: " << theId << " theSetter: " << 
            theSetter << " theValue " << endl << theValue);

    if (!theId.size()) {
        throw EmptyId("id must not be an empty string",PLUS_FILE_LINE);
    }
    asl::Ptr<BoardSheet> newSheet = asl::Ptr<BoardSheet>(new BoardSheet(theId, theValue, theSetter)); 

    asl::Ptr<BoardSheet> mySheet(0);
#ifndef SHEET_LEVEL_LOCKS
    AutoWriteLocker autoLocker(_lock);
#endif
    {
#ifdef SHEET_LEVEL_LOCKS
        AutoWriteLocker autoLocker(_lock);
#endif
        SheetMap::iterator it = _myBoard.find(theId);
        if (it != _myBoard.end() ) {
            mySheet = it->second; // use already existing sheet
        } else {
            // use newly created sheet and mark it modified
            newSheet->setLastModified(asl::Time());
            newSheet->setLastCopied(newSheet->getLastModified());
            _myBoard[theId] = newSheet;
            return;
        }
    }

    if (mySheet) {
        if (theSetter != mySheet->getSetter() ) {
            string myReason = "Sheet with id " + theId 
                + " already exists with setter '" 
                + mySheet->getSetter() 
                + "', field not created."; 
            throw DuplicateId(myReason, PLUS_FILE_LINE);
        }
        if (theValue && *theValue) {
            mySheet->setSheetValue(theValue);
        }
    }
}

void WhiteBoard::createSheetFast(const string & theId, const string & theSetter, xml::NodePtr theValue) {

    DB2(cerr << "WhiteBoard::createSheetFast(): id: " << theId << " theSetter: " << 
            theSetter << " theValue "  << theValue.get_nativePtr() << endl);

    if (!theId.size()) {
        throw EmptyId("id must not be an empty string",PLUS_FILE_LINE);
    }
    asl::Ptr<BoardSheet> newSheet = asl::Ptr<BoardSheet>(new BoardSheet(theId, theValue, theSetter)); 
    asl::Ptr<BoardSheet> mySheet(0);

    SheetMap::iterator it = _myBoard.find(theId);
    if (it != _myBoard.end() ) {
        mySheet = it->second;
    } else { 
        _myBoard[theId] = newSheet;
        return;
    }

    if (mySheet) {
        if (theSetter != mySheet->getSetter() ) {
            string myReason = "Sheet with id " + theId 
                + " already exists with setter '" 
                + mySheet->getSetter() 
                + "', field not created."; 
            throw DuplicateId(myReason, PLUS_FILE_LINE);
        }
        if (theValue && *theValue) {
            mySheet->setSheetValue(theValue);
        }
    }
}

void WhiteBoard::destroySheet(const string & theId, const string & theSetter) {
    if (!theId.size()) {
        throw EmptyId("id must not be an empty string", PLUS_FILE_LINE);
    }
    AutoWriteLocker    autoLocker(_lock);
    SheetMap::iterator it = _myBoard.find(theId); 
    if (it != _myBoard.end()) {
        if (theSetter != it->second->getSetter() ) {
            string myReason = "destroySheet id '" + theId + "'" +
                + " as unauthorized setter '" + theSetter + "'" + 
                + " (do not pose as authorized setter '" + it->second->getSetter() + "')";
            throw AccessDenied(myReason, PLUS_FILE_LINE);
        }
        _myBoard.erase(it);
        return;
    }
    throw UnknownId("destroySheet with unknown id", PLUS_FILE_LINE);
} 

asl::Ptr<BoardSheet> 
WhiteBoard::getSheetFast(const string & theName) const {
    SheetMap::const_iterator it = _myBoard.find(theName); 
    if (it != _myBoard.end()) {
        return it->second;
    } else {
        return asl::Ptr<BoardSheet>(0);
    }
}

void 
WhiteBoard::setSheet(const string & theId,
            const string & theSetter, 
            const xml::Node & theSheetValue) 
{
#ifndef SHEET_LEVEL_LOCKS
    AutoWriteLocker    autoLocker(_lock);
#endif 
    AC_DB2("WhiteBoard::setSheet(): id: " << theId << " theSetter: " << 
        theSetter << " theValue: " << endl << theSheetValue);

    asl::Ptr<BoardSheet> mySheet = checkExistingSheetAccess(theId, theSetter, true);
    xml::NodePtr myNode = theSheetValue.Clone();
    mySheet->setSheetValue(myNode);
}

void 
WhiteBoard::setSheetFast(const string & theId,
            const string & theSetter, 
            xml::NodePtr theValue) 
{
    DB2(cerr << "WhiteBoard::setSheetFast(): id: " << theId << " theSetter: " << 
            theSetter << " theValue " << theValue.get_nativePtr() << endl);
    asl::Ptr<BoardSheet> mySheet = checkExistingSheetAccess(theId, theSetter, true);
    mySheet->setSheetValue(theValue);
}

void 
WhiteBoard::getSheet(const string & theId, const string & theGetter, xml::Node & theResultingSheet)    
{
#ifndef SHEET_LEVEL_LOCKS
    AutoReadLocker    autoLocker(_lock);
#endif 

    AC_DB2("WhiteBoard::getSheet(): id: " << theId << " theGetter: " << theGetter);

    asl::Ptr<BoardSheet> mySheet = checkExistingSheetAccess(theId, theGetter /*is just a dummy*/, false);
    xml::NodePtr returnNode = mySheet->getValueNode(theGetter, true);
    theResultingSheet = *returnNode;

    AC_DB2("  theResultingSheet: " << endl << theResultingSheet);
}
    
void 
WhiteBoard::statSheet(const string & theId, const string & theGetter,
            xml::Node & theResultingSheet) 
{
#ifndef SHEET_LEVEL_LOCKS
    AutoReadLocker    autoLocker(_lock);
#endif 
    asl::Ptr<BoardSheet> mySheet = checkExistingSheetAccess(theId, theGetter/*is just a dummy*/, false);
    xml::NodePtr returnNode = mySheet->getValueNode(theGetter, false);
    theResultingSheet = *returnNode;
}
    
void 
WhiteBoard::flush() {
}

asl::Ptr<BoardSheet>
WhiteBoard::findSheet(const string & theName) {
#ifdef SHEET_LEVEL_LOCKS
    AutoReadLocker    autoLocker(_lock);
#endif
    SheetMap::iterator it =_myBoard.find(theName); 
    if (it != _myBoard.end()) {
        return it->second;
    }
    return asl::Ptr<BoardSheet>(0);
}
    
asl::Ptr<BoardSheet>
WhiteBoard::checkExistingSheetAccess(const string & theId, const string & theSetter, bool checkSetter) 
{
    if (theId.size() == 0) {
        throw EmptyId(JUST_FILE_LINE);
    }
    
    asl::Ptr<BoardSheet> mySheet = findSheet(theId);
    
    if (!mySheet) {
        string myReason = "Sheet with id '" + theId 
            + "' does not exist"; 
        throw UnknownId(myReason, PLUS_FILE_LINE);            
    }
 
    if (checkSetter) {
        if (mySheet->getSetter() != theSetter) {
            string myReason = "Sheet with id " + theId 
                + " must not be changed by setter '" 
                + theSetter + "', only '"
                + mySheet->getSetter()
                + "' is allowed to set value";
            throw AccessDenied(myReason, PLUS_FILE_LINE);            
        }
    }
    return mySheet;
}


//////////////////////////////////////////////////////////////////////////

BufferedWhiteBoard::BufferedWhiteBoard() {
    _myKeyStatus = pthread_key_create(&_specificKey,destroyList);
}

void
BufferedWhiteBoard::addNewValue(bool doCreate, const string & theId,
        const string & theSetter, 
        xml::NodePtr theValue)
{
    DB2(cerr << "BufferedWhiteBoard::addNewValue theId='" << theId 
         << "',theSetter='"<<theSetter<<"',theValue='"<<endl<<theValue.get_nativePtr()); 
    NewValueList * myNewValues = reinterpret_cast<NewValueList *>(pthread_getspecific(_specificKey));
    if (myNewValues == 0) {
        myNewValues = new NewValueList;
        int status = pthread_setspecific(_specificKey,reinterpret_cast<void*>(myNewValues));
    }
    myNewValues->push_back(NewValue());
    myNewValues->back().myName = theId;
    myNewValues->back().mySetter = theSetter;
    myNewValues->back().myValue = theValue;
    myNewValues->back().doCreate = doCreate;
    DB2(cerr << "done BufferedWhiteBoard::addNewValue theId='" << theId 
             << "',theSetter='"<<theSetter<<"',theValue='"<<endl<<theValue); 
}

void
BufferedWhiteBoard::setSheet(const string & theId,
        const string & theSetter, 
        const xml::Node & theSheetValue)
{
    DB2(cerr << "BufferedWhiteBoard::setSheet theId='" << theId 
             << "',theSetter='"<<theSetter<<"',theValue='"<<endl<<theSheetValue); 
    addNewValue(false, theId, theSetter, theSheetValue.Clone());
};
void
BufferedWhiteBoard::createSheet(const string & theId, const string & theSetter, xml::NodePtr theValue) {
    DB2(cerr << "BufferedWhiteBoard::createSheet theId='" << theId 
             << "',theSetter='"<<theSetter<<"',theValue='"<<endl<<theValue); 
    addNewValue(true, theId, theSetter, theValue);
}
void
BufferedWhiteBoard::flush() {
    DB2(cerr << "BufferedWhiteBoard::flush:" << endl);
    NewValueList * myNewValues = reinterpret_cast<NewValueList *>(pthread_getspecific(_specificKey));
    if (myNewValues) {
        AutoWriteLocker    autoLocker(_lock);
        for (int i = 0; i < myNewValues->size(); ++i) {
            if ((*myNewValues)[i].doCreate) {
                createSheetFast((*myNewValues)[i].myName,(*myNewValues)[i].mySetter,(*myNewValues)[i].myValue);
            } else {
                setSheetFast((*myNewValues)[i].myName,(*myNewValues)[i].mySetter,(*myNewValues)[i].myValue);
            }
        }
        myNewValues->resize(0);
    }
}


//////////////////////////////////////////////////////////////////////////

WhiteBoardProxy::WhiteBoardProxy(oclient<DirectorLink> * connection) {
    bind(connection);
}

WhiteBoardProxy::~WhiteBoardProxy() {
}

void WhiteBoardProxy::requestSheetIds() {
    xml::Element myNode ("requestSheetIds");
    addRequestNode(myNode);
}

void WhiteBoardProxy::getSheetIds(vector<string>& ids) {
    ids = _myIds;
}

void WhiteBoardProxy::createSheet(const string & id, const string & setter, xml::NodePtr theValue) {
    xml::Element myNode("createSheet");
    myNode["id"] = id;
    myNode["setter"] = setter;
    if (theValue) {
        myNode.AddChild(theValue);
    }
    addRequestNode(myNode);
}

void WhiteBoardProxy::destroySheet(const string & id, const string & setter) { 
    xml::Element myNode("destroySheet");
    myNode["id"] = id;
    myNode["setter"] = setter;
    addRequestNode(myNode);
}

void WhiteBoardProxy::setSheet(const string & id,
                const string & setter, 
                const xml::Node & sheetValue) 
{ 
    xml::Element myNode("setSheet");
    myNode["id"] = id;
    myNode["setter"] = setter;
    myNode.AddChild(sheetValue);
    addRequestNode(myNode);
}

void WhiteBoardProxy::getSheet(const string & id, const string & getter, xml::Node & resultingSheet) {
    xml::Element myNode("getSheet");
    myNode["id"] = id;
    myNode["getter"] = getter;
    _resultStorage.push_back(&resultingSheet);
    addRequestNode(myNode);
}

void WhiteBoardProxy::statSheet(const string & id, const string & getter, xml::Node & resultingSheet){ 
    xml::Element myNode("statSheet");
    myNode["id"] = id;
    myNode["getter"] = getter;
    _resultStorage.push_back(&resultingSheet);
    addRequestNode(myNode);
}
 
WhiteBoardProxy::ResultCleaner::ResultCleaner(vector<xml::Node*> & theResultStorage) 
            : resultStorage(theResultStorage) 
{}

WhiteBoardProxy::ResultCleaner::~ResultCleaner() {
    resultStorage.resize(0);
}

void WhiteBoardProxy::flush() {
    try {
        ResultCleaner myCleaner(_resultStorage);
        if (_nextRequest) {
            AC_DB2("Transmitting: " << _nextRequest);
            string myResult;
            myResult = return_string( _myClient->object->callY50(
                        in_string(asl::as_string(_nextRequest))
                        ));
            const xml::Document resultNode(myResult);           
            AC_DB2("Result: " << resultNode);
            if (resultNode("Reply")("Status")("OK")) {
                // Remember playing ids here.
                const xml::Node dataNode = resultNode("Reply")("Data")("Board");
                int i= 0;
                xml::Node curNode = dataNode ("Sheet", i);
                while (curNode) {
                    AC_DB2("WhiteBoardProxy::flush(): Storing result Data of Sheet " 
                            << i << ":" << curNode);
                    AC_DB2("WhiteBoardProxy::flush(): resultStorage size = " 
                            << i << _resultStorage.size());
                    string myId = curNode["id"].Value();
                    if (i >=_resultStorage.size()) {
                        throw asl::Exception("Internal inconsistency", PLUS_FILE_LINE);
                    }
                    *(_resultStorage[i]) = curNode;
                    i++;
                    curNode = dataNode ("Sheet", i); 
                }
                i=0;
                _myIds.clear();
                curNode = dataNode ("SheetIds")("Sheet", i);
                while (curNode) {
                    _myIds.push_back (curNode("#text").Value());
                    i++;
                    curNode = dataNode ("SheetIds")("Sheet", i); 
                }
            } else {
                _nextRequest = xml::Node();                    
                if (!resultNode) {
                    throw asl::Exception(JUST_FILE_LINE);
                }
                if (resultNode("Reply")("Status")("ERROR")) {
                    xml::Node _statusNode = resultNode("Reply")("Status")("ERROR");
                    throw asl::Exception (_statusNode["code"].Value(), 
                            _statusNode("#text").Value());
                }
                throw asl::Exception(JUST_FILE_LINE);
            }
            _nextRequest = xml::Node();
        }
    }
    catch (asl::Exception & e) {
        AC_ERROR("WhiteBoardProxy::flush(): rethrowing exception: " << endl << e);
        _nextRequest = xml::Node();
        throw;
    }
    catch(CORBA_SystemException& ex) {
        AC_ERROR("WhiteBoardProxy::flush(): a CORBA exception occured:" << endl << OBExceptionToString (ex));
        _nextRequest = xml::Node();
        throw; 
    }    
}

void WhiteBoardProxy::bind(oclient<DirectorLink> * theClient) {
    _myClient = theClient;
}

void WhiteBoardProxy::addRequestNode(const xml::Node & theNode) {
    if (!_nextRequest) {
        _nextRequest = xml::Element("ACMControl");
        _nextRequest("Board");
    }
    _nextRequest("Board").AddChild(theNode);
}


void StationWhiteBoard::broadcast() {
    if (_myStation) {
        //  broadcast our changes
        WhiteBoard outgoingBoard;
        this->copyInto(outgoingBoard, &copyModifiedAndOldestAgedFilter);
        string outgoingString = asl::as_string(outgoingBoard);
        DB2(cerr << "StationWhiteBoard::broadcast():outgoingString:" << endl << outgoingString << endl);
        if (outgoingString.size()) {
            _myStation->broadcast(outgoingString);
        }
    }
}

bool
StationWhiteBoard::update() {
    bool updated = false;
    if (_myStation) {
        // receive new stuff 
        string incomingData;
        unsigned long someSender;
        unsigned long itsStationID;
        while (_myStation->receive(incomingData, someSender, itsStationID)) {
            // Parse the Xml first
            const xml::Document incomingNodes(incomingData);
            if (!incomingNodes) {
                throw BadIncomingXML("Could not parse", PLUS_FILE_LINE);
            }
            // ... and make a whiteboard out of it
            WhiteBoard incomingBoard;
            if (!::fromNode(incomingNodes("WhiteBoard"), incomingBoard)) {
                throw BadIncomingXML("Not a WhiteBoard", PLUS_FILE_LINE);
            }
            // ... and copy the sheets into our board 
            incomingBoard.copyInto(*this, &mergeAllFilter);
            updated = true;
        }

    }
    return updated;
}

//==============================================================================
//
// $Log: WhiteBoard.cpp,v $
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.1.1.1  2002/09/17 15:37:04  wolfger
// initial checkin
//
// Revision 1.2  2002/09/06 18:14:00  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.33  2002/08/30 16:25:41  martin
// more std-ization
//
// Revision 1.1.2.32  2002/08/16 15:50:59  christian
// added return values / removed return parameters from setter functions (to avoid warnings)
//
// Revision 1.1.2.31  2002/06/06 20:17:49  pavel
// fixed broken WhiteBoardStation protocol - first usable version
//
// Revision 1.1.2.30  2002/05/30 04:16:47  david
// bugfixes and setup changes
//
// Revision 1.1.2.29  2002/05/24 09:43:10  david
// -enabled "not implemented" messages
//
// Revision 1.1.2.28  2002/05/16 01:48:26  pavel
// reduced received packet re-broadcast minimum time from 1 sec. to _defaultMaxAge_
//
// Revision 1.1.2.27  2002/05/16 01:26:59  pavel
// improved Station.h dependencies, added station packet payload compression
//
// Revision 1.1.2.26  2002/05/15 22:26:37  david
// another brick in the wall
//
// Revision 1.1.2.25  2002/05/15 13:35:00  david
// another step towards a new great future
//
// Revision 1.1.2.24  2002/05/13 23:17:51  david
// fixed send/receive bug
//
// Revision 1.1.2.23  2002/05/13 19:38:18  david
// added more sophisticated copy strategies
//
// Revision 1.1.2.22  2002/05/13 12:36:21  pavel
// some interface modifications due to first integration
//
// Revision 1.1.2.21  2002/05/13 08:34:43  chris
// Moved StationWhiteBoard::update() to .cpp file
//
// Revision 1.1.2.20  2002/05/12 23:26:33  pavel
// first possibly working version of Station with Whiteboard
//
// Revision 1.1.2.19  2002/05/11 21:45:39  pavel
// added a copy filter function interface for more flexibility
//
// Revision 1.1.2.18  2002/05/10 20:23:09  pavel
// added xml input/output function and a change-copier
//
// Revision 1.1.2.17  2002/05/08 06:51:29  pavel
// added lock sand copying stuff
//
// Revision 1.1.2.16  2002/05/08 03:39:54  pavel
// added copy feature to WhiteBoard
//
// Revision 1.1.2.15  2002/05/07 16:07:41  pavel
// added priority ceiling locks
//
// Revision 1.1.2.14  2002/05/06 22:55:10  pavel
// added BufferedWhiteBoard
//
// Revision 1.1.2.13  2002/05/06 20:58:50  pavel
// made locking configuerabel and configured for the fastest version:
// RW-Board level locking and no Sheet level locking
//
// Revision 1.1.2.12  2002/05/03 19:31:53  pavel
// added tons of sophisticated locking mechanisms without actually improving performance
//
// Revision 1.1.2.11  2002/05/02 09:48:55  pavel
// refactored to use map<string,ptr> for preparation of sheet level locking, but
// still using pessimistic whiteboard-level locking yet
//
// Revision 1.1.2.10  2002/05/01 11:36:39  pavel
// some name changes
//
// Revision 1.1.2.9  2002/04/30 18:02:26  david
// /tmp/cvsY4pYkq
//
// Revision 1.1.2.8  2002/04/30 16:36:25  pavel
// added tests and testoutput, changed createsheet behaviour
//
// Revision 1.1.2.7  2002/04/30 13:21:01  christian
// removed exception from sheet destructor
//
// Revision 1.1.2.6  2002/04/29 17:11:11  david
// debug level back to 1
//
// Revision 1.1.2.5  2002/04/26 17:31:14  david
// fixed sync problems and simplified some code
//
// Revision 1.1.2.4  2002/04/25 16:45:48  christian
// beautified debug output
//
// Revision 1.1.2.3  2002/04/19 11:36:11  david
//  - merged wittestrasse source tree into the repository
//
// Revision 1.1.2.2  2002/04/16 13:42:29  uzadow
// Added DumpWhiteboard tool
//
// Revision 1.1.2.1  2002/04/16 12:07:36  uzadow
// Moved Whiteboard implementation to WhiteBoard.cpp.
//
// Revision 1.1.2.10  2002/04/08 12:22:29  stefan
// made WhiteBoard thread-safe
//
// Revision 1.1.2.9  2002/04/04 20:01:35  christian
// changed DB makro to AC_DB, ERROR to AC_ERROR, WARNING to AC_WARNING
//
// Revision 1.1.2.8  2002/04/04 09:21:02  stefan
// WARNINGS ARE BAD BAD BAD BAD BAD
//
// Revision 1.1.2.7  2002/04/03 13:03:39  christian
// added support for pure xml-nodes
//
// Revision 1.1.2.6  2002/03/28 12:15:03  christian
// added getName() to SheetBase
//
// Revision 1.1.2.5  2002/03/27 16:57:38  uzadow
// *** empty log message ***
//
// Revision 1.1.2.4  2002/03/27 11:19:06  pavel
// Added capability to specify getter id explicitly
//
// Revision 1.1.2.3  2002/03/22 20:14:43  pavel
// ACMControlParser is now using common/xmlcom/WhiteBoard.h
//
// Revision 1.1.2.2  2002/03/22 15:45:44  pavel
// moved some functions to separate os_functions file in asl
//
// Revision 1.1.2.1  2002/03/21 17:55:00  pavel
// initial WhiteBoard Library version
//
//==============================================================================
