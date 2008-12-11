
/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// $Id: WhiteBoard.h,v 1.1.1.1 2003/05/12 14:20:23 uzadow Exp $
// $Author: uzadow $
// $Revision: 1.1.1.1 $
// $Date: 2003/05/12 14:20:23 $
//
// (CVS log at the bottom of this file)
//
//==============================================================================

#ifndef AC_WHITE_BOARD_INCLUDED
#define AC_WHITE_BOARD_INCLUDED

#include <asl/XmlTypes.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/ReadWriteLock.h>
#include <asl/base/Auto.h>
#include <asl/base/os_functions.h>
#include <asl/base/string_functions.h>
#include "oclient.h"
#include "string_aux.h"
#include "DirectorLink.h"
#include <asl/base/Ptr.h>
#include <asl/base/Exception.h>

#include <vector>
#include <string>
#include <map>
#include <set>

#include "pthread.h"

//#define SHEET_LEVEL_LOCKS 1
#ifdef SHEET_LEVEL_LOCKS
    //#define SHEET_LEVEL_READ_WRITE_LOCK
    #ifndef SHEET_LEVEL_READ_WRITE_LOCK 
        #define SHEET_LEVEL_MUTEX 1
    #endif
#endif

//#define BOARD_PRIORITY_CEILING_READ_WRITE_LOCK 1
#define BOARD_READ_WRITE_LOCK 1
//#define BOARD_MUTEX 1

class BoardSheet {
public:
#ifdef SHEET_LEVEL_LOCKS
    #ifdef SHEET_LEVEL_MUTEX
        typedef asl::AutoLocker<ThreadLock>  AutoWriteLocker;
        typedef asl::AutoLocker<ThreadLock>  AutoReadLocker;
    #endif
    #ifdef SHEET_LEVEL_READ_WRITE_LOCK
        typedef asl::AutoWriteLocker<ReadWriteLock>  AutoWriteLocker;
        typedef asl::AutoReadLocker<ReadWriteLock>  AutoReadLocker;
    #endif     
#endif     
     BoardSheet(const std::string & theId,
               xml::NodePtr theSheetValue,
               const std::string & theSetter,
               int theSetCount = 0);
   
    asl::Ptr<BoardSheet> clone(const std::string & theGetter="X") const;

    void setSheetValue(xml::NodePtr theValue);
    const std::string & getSetter() const;
    xml::NodePtr getValueNode(const std::string & theGetter, bool doCount) const;
    const xml::NodePtr getValue() const;
    const std::string & getName() const;
    //void wasCopied() const;

    double getLastModified() const {
        return _lastModified;
    }
    void setLastModified(double theValue) {
        _lastModified=theValue;
    }
    double getLastCopied() const {
        return _lastCopied;
    }
    void setLastCopied(double theValue) const {
        _lastCopied=theValue;
    }
    double getMaxAge() const {
        return _maxAge;
    }
    void setMaxAge(double theValue) {
        _maxAge = theValue;
    }
    const int getSetCount() const {
        return _setCount;
    }
    void setSetCount(int theSetCount) const {
        _setCount = theSetCount;
    }
    static double getDefaultMaxAge() {
        return _defaultMaxAge_;
    }
    const std::map<std::string,int> & getGetterMap() const {
        return _getterMap;
    }
    void setGetterMap(const std::map<std::string,int> & theMap) const {
        _getterMap = theMap;
    }
      
private: 
    const std::string _myName;
    xml::NodePtr _myValue;
    const std::string _mySetter;
    mutable int _setCount; 
    mutable std::map<std::string,int> _getterMap;
    double _lastModified;
    mutable double _lastCopied;
    double _maxAge;
    static double _defaultMaxAge_;
#ifdef SHEET_LEVEL_LOCKS
    #ifdef SHEET_LEVEL_READ_WRITE_LOCK
    mutable ReadWriteLock      _lock;
    #endif
    #ifdef SHEET_LEVEL_MUTEX
    mutable ThreadLock      _lock;
    #endif
#endif     
};

class IWhiteBoard : public asl::ReadWriteLockable,
                    public xml::Representable,
                    public xml::Constructible {
    public:
        
        DEFINE_NESTED_EXCEPTION(IWhiteBoard,Exception,asl::Exception);
        DEFINE_NESTED_EXCEPTION(IWhiteBoard,DuplicateId,Exception);
        DEFINE_NESTED_EXCEPTION(IWhiteBoard,EmptyId,Exception);
        DEFINE_NESTED_EXCEPTION(IWhiteBoard,UnknownId,Exception);
        DEFINE_NESTED_EXCEPTION(IWhiteBoard,AccessDenied,Exception);

        virtual void requestSheetIds() = 0;
        virtual void getSheetIds(std::vector<std::string>& theNames) = 0;
        virtual void createSheet(const std::string & theName,
                                 const std::string & theSetter,
                                 xml::NodePtr theValue=xml::NodePtr(0)) = 0;
        virtual void destroySheet(const std::string & theName, const std::string & theSetter) = 0;
        virtual void setSheet(const std::string & theId,
                const std::string & theSetter, 
                const xml::Node & theValue) = 0;
        virtual void getSheet(const std::string & theName, const std::string & theGetter, xml::Node & theResult) = 0;
        virtual void statSheet(const std::string & theName, const std::string & theGetter, xml::Node & theResult) = 0;
        virtual bool update();
        virtual void broadcast();
        virtual void flush() = 0;
        virtual void clear();
        virtual void assignSheet(const std::string & theId, asl::Ptr<BoardSheet> theValue);
        virtual void readlock();
        virtual void readunlock();
        virtual void writelock();
        virtual void writeunlock();
        virtual xml::NodePtr asNode() const;
        virtual bool fromNode(const xml::Node & elem);

        typedef bool(*CopyFilterFunction)(const asl::Ptr<BoardSheet>, IWhiteBoard &);

        static bool copyAllFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination); 
        static bool copyModifiedFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination);
        static bool copyAgedFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination);
        static bool copyModifiedAndOldestAgedFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination);
        static bool copyModifiedOrAgedFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination);
        static bool mergeAllFilter(const asl::Ptr<BoardSheet> theSheet, IWhiteBoard & theDestination);

        virtual void copyInto(IWhiteBoard & theBoard,
                      IWhiteBoard::CopyFilterFunction doCopy=&copyAllFilter) const;
        virtual asl::Ptr<BoardSheet> getSheetFast(const std::string & theId) const;
};

class WhiteBoard : public IWhiteBoard {
public:
#ifdef BOARD_MUTEX
    typedef asl::AutoLocker<ThreadLock>  AutoWriteLocker;
    typedef asl::AutoLocker<ThreadLock>  AutoReadLocker;
#endif
#ifdef BOARD_READ_WRITE_LOCK
    typedef asl::AutoWriteLocker<asl::ReadWriteLock>  AutoWriteLocker;
    typedef asl::AutoReadLocker<asl::ReadWriteLock>  AutoReadLocker;
#endif     
#ifdef BOARD_PRIORITY_CEILING_READ_WRITE_LOCK
    typedef asl::AutoWriteLocker<asl::PriorityCeilingRWLock>  AutoWriteLocker;
    typedef asl::AutoReadLocker<asl::PriorityCeilingRWLock>  AutoReadLocker;
#endif     
    virtual void requestSheetIds() {}
    virtual void getSheetIds(std::vector<std::string>& ids);
    virtual void createSheet(const std::string & theId, const std::string & theSetter, xml::NodePtr theValue=xml::NodePtr(0));
    virtual void destroySheet(const std::string & theId, const std::string & theSetter);
    virtual void setSheet(const std::string & theId,
            const std::string & theSetter, 
            const xml::Node & theSheetValue);
    virtual void getSheet(const std::string & theId, const std::string & theGetter, xml::Node & theResultingSheet);
    virtual void statSheet(const std::string & theId, const std::string & theGetter,
            xml::Node & theResultingSheet);
    virtual void flush();

    virtual void clear();
   
    virtual void readlock() {
        _lock.readlock();
    }
    virtual void readunlock() {
        _lock.readunlock();
    }
    virtual void writelock() {
        _lock.writelock();
    }
    virtual void writeunlock() {
        _lock.writeunlock();
    }
    virtual xml::NodePtr asNode() const;
    virtual bool fromNode(const xml::Node & elem);
    virtual void copyInto(IWhiteBoard & theBoard,
            IWhiteBoard::CopyFilterFunction doCopy=&copyAllFilter) const;
    virtual asl::Ptr<BoardSheet> getSheetFast(const std::string & theName) const;
protected:
    void assignSheet(const std::string & theId, asl::Ptr<BoardSheet> theValue);
    void setSheetFast(const std::string & theId,
            const std::string & theSetter, 
            xml::NodePtr newSheetValue); 
    void createSheetFast(const std::string & theId,
            const std::string & theSetter, 
            xml::NodePtr newSheetValue); 
#ifdef BOARD_PRIORITY_CEILING_READ_WRITE_LOCK
    mutable asl::PriorityCeilingRWLock      _lock;
#endif
#ifdef BOARD_READ_WRITE_LOCK
    mutable asl::ReadWriteLock      _lock;
#endif
#ifdef BOARD_MUTEX
    mutable ThreadLock      _lock;
#endif
private:
    typedef std::map<std::string,asl::Ptr<BoardSheet> > SheetMap;

    asl::Ptr<BoardSheet> checkExistingSheetAccess(const std::string & theId,
                                                  const std::string & theSetter,
                                                  bool checkSetter);

    asl::Ptr<BoardSheet> findSheet(const std::string & theName);
    
    SheetMap    _myBoard;
};

class BufferedWhiteBoard : public WhiteBoard {
public:
    BufferedWhiteBoard(); 
    void addNewValue(bool doCreate, const std::string & theId,
            const std::string & theSetter, 
            xml::NodePtr theSheetValue);

    virtual void setSheet(const std::string & theId,
            const std::string & theSetter, 
            const xml::Node & theSheetValue);
    
    virtual void createSheet(const std::string & theId,
            const std::string & theSetter,
            xml::NodePtr theValue=xml::NodePtr(0));

    virtual void flush();
   
private:
    struct NewValue {
        std::string myName;
        std::string mySetter;
        xml::NodePtr myValue; 
        bool doCreate;
    };

    static void destroyList(void * theList) {
        NewValueList * myNewValues = reinterpret_cast<NewValueList *>(theList);
        delete myNewValues;
    }

    typedef std::vector<NewValue> NewValueList;
    pthread_key_t _specificKey;
    int _myKeyStatus;
};

class Station;

class StationWhiteBoard : public WhiteBoard {
public:
    DEFINE_NESTED_EXCEPTION(StationWhiteBoard,BadIncomingXML,IWhiteBoard::Exception);

    StationWhiteBoard(Station * theStation = 0) : _myStation(theStation) {
    }
    void bind(Station * theStation) {
        _myStation = theStation;
    }
    virtual bool update();
    //virtual void flush();
    virtual void broadcast(); // TODO: broadcast should be same as flush in the future, but we call
                              // it currently too often 
private:
    Station * _myStation;
};

class WhiteBoardProxy : public IWhiteBoard {
    public:
        WhiteBoardProxy(oclient<DirectorLink> * connection = 0);
        ~WhiteBoardProxy();

        virtual void requestSheetIds();
        virtual void getSheetIds(std::vector<std::string>& ids);
        
        virtual void createSheet(const std::string & id, const std::string & setter, xml::NodePtr theValue=xml::NodePtr(0));
        virtual void destroySheet(const std::string & id, const std::string & setter);
        virtual void setSheet(const std::string & id,
                const std::string & setter, 
                const xml::Node & sheetValue);
        virtual void getSheet(const std::string & id, const std::string & getter, xml::Node & resultingSheet);
        virtual void statSheet(const std::string & id, const std::string & getter, xml::Node & resultingSheet);

        struct ResultCleaner {
           ResultCleaner(std::vector<xml::Node*> & theResultStorage);
           ~ResultCleaner();
           std::vector<xml::Node*> & resultStorage;
        };

        virtual void flush();
        void bind(oclient<DirectorLink> * theClient);

    protected:
        void addRequestNode(const xml::Node & theNode);

    private:
        oclient<DirectorLink> * _myClient;
        xml::Node _nextRequest;
        xml::Node _lastReply;
        std::vector<xml::Node*> _resultStorage;
        std::vector<std::string> _myIds;
};


DEFINE_EXCEPTION(SheetException, asl::Exception);
DEFINE_EXCEPTION(ValueNotAvailable, SheetException);
DEFINE_EXCEPTION(ConversionFailed, SheetException);

template <class VALUE>
class SheetBase {
protected:
    SheetBase(IWhiteBoard & theBoard,
          const std::string & theName, 
          bool doCreate,
          bool doDestroy,
          const std::string & exclusiveSetter, 
          const std::string & getter) 
        : _board(theBoard),
          _name(theName),
          _destroy(doDestroy),
          _setter(exclusiveSetter),
          _getter(getter),
          _lastSetCount(0)
    {
        if (doCreate) {
            _board.createSheet(_name, _setter);
        }
    }
public:
    virtual ~SheetBase() {
        try {
            _board.flush();
        } catch (...) {
            cerr << "WARNING: ~SheetBase(): flush failed!" << endl;
            // We do not want to throw exceptions in the destructor!
        }
        if (_destroy) {
            _board.destroySheet(_name, _setter);
        }
    };

    void setValue(const VALUE & theValue) {
        _value = theValue;
    }
    
    VALUE getValue() const {
        if (!_node) {
            throw ValueNotAvailable("value not yet available; board has not been flushed yet"
                               " or value could not be retrieved from board", PLUS_FILE_LINE);
        }
        if (_node.NumberOfChildren()!=1) {
            throw ConversionFailed(
                std::string("Empty Value: retrieved value node has wrong number of children (")
                    + asl::as_string(_node.NumberOfChildren()) + ":\n"
                    + asl::as_string(_node)+"\n",PLUS_FILE_LINE);
         }
        if (!fromNode(*_node.GetChild(0),_value)) {
            throw ConversionFailed(std::string("could not convert xml node to desired value:\n")
                    + asl::as_string(_node)+"\n",PLUS_FILE_LINE);
        };
        _lastSetCount = getSetCount();
        return _value;
    }

    void readFromBoard(bool immediateFlush = false) {
        _node = xml::Node();
        _board.getSheet(_name,_getter, _node);
    }

    void writeToBoard(bool immediateFlush = false) {
        _board.setSheet(_name, _setter, *asNode(_value));
    }

    int getSetCount() const {
        if (_node) {
            return asl::as<int>(_node["sets"].Value());
        }
        return -1;
    }

    bool hasNewValue() const {
        if (_node) {
            if (getSetCount() > _lastSetCount) {  
                return true;
            }
        }
        return false;
    }

    const std::string & getName() const {
        return _name;
    }
    
private:
   mutable VALUE    _value;
   std::string           _name;
   bool             _destroy;
   std::string           _setter;
   std::string           _getter;
   IWhiteBoard &    _board;
   xml::Node        _node;
   mutable int      _lastSetCount;
};


template <class VALUE>
class NewSheet : public SheetBase<VALUE> {
public:
    NewSheet(IWhiteBoard & theBoard, 
          const std::string & theName, 
          const std::string & exclusiveSetter = asl::hostappid(), 
          const std::string & theGetter = asl::hostappid()) 
        : SheetBase<VALUE>(theBoard, theName, /*doCreate*/ true , /*doDestroy*/ false, exclusiveSetter,theGetter) 
    {
    }
};

template <class VALUE>
class Sheet : public SheetBase<VALUE> {
public:
    Sheet(IWhiteBoard & theBoard,
          const std::string & theName, 
          const std::string & exclusiveSetter="",
          const std::string & theGetter = asl::hostappid()) 
        : SheetBase<VALUE>(theBoard, theName, /*doCreate*/ false, /*doDestroy*/ false, exclusiveSetter,theGetter) 
    {
    }
};

template <class VALUE>
class ConstSheet : public SheetBase<VALUE> {
    public:
        ConstSheet(IWhiteBoard & theBoard, const std::string & theName, const std::string & theGetter = asl::hostappid()) 
            : SheetBase<VALUE>(theBoard, theName, /*doCreate*/ false, /*doDestroy*/ false, "", theGetter) 
            {
            }
};


#endif

