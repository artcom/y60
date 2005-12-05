/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  Memory Mapping Classes with asl::Block interface
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : poor
//    documentation          : poor
//    test coverage          : ok
//    names                  : poor
//    style guide conformance: poor
//    technical soundness    : fair
//    dead code              : poor
//    readability            : fair
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : fair
//    integration            : fair
//    dependencies           : fair
//    cheesyness             : fair
//
//    overall review status  : poor
//
//    recommendations:
//       - remove dead code
//       - reformat source code
//       - put OS-dependent i/o functions into separate file
//       - put beef into .cpp file
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_MappedBlock_
#define _included_asl_MappedBlock_

#include "Block.h"
#include "string_functions.h"
#include "file_functions.h"
#include "error_functions.h"
#include "Logger.h"

#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DB(x) x


/*! \addtogroup aslbase */
/* @{ */

#ifndef WIN32
// for some unknown reason, in Linux (kernel 2.4.20) the remap call does not work
// as described so we unmap and map to get the desired result
// DS: The remap() call in Linux is not portable anyway. There is no equivalent
// on BSD style systems like Mac OS X. IMHO we should remove this feature.
#define NO_REMAP
#endif

#ifndef WIN32
# include <sys/mman.h>
# include <unistd.h>
# define FD_t            int
# define FD_INVALID      (-1)

# define OFF_T  off_t // 64-bit offset

# define OPEN   open
# define CLOSE  close
# define LSEEK  lseek
# define WRITE  write
# define READ   read

#endif

#ifdef WIN32

# include <windows.h>
# include <io.h>
# include <map>
# define FD_t               HANDLE
# define FD_INVALID         INVALID_HANDLE_VALUE
# define CALL_HAS_SUCCEEDED ERROR_SUCCESS

# define OFF_T unsigned long // 32-bit offset (should be 64-bit)

# define OPEN  asl_open
# define CLOSE asl_close
/*
  #  define LSEEK  asl_lseek
  #  define WRITE  asl_write
  #  define READ  asl_read
*/

# define O_RDONLY _O_RDONLY
# define O_RDWR   _O_RDWR
# define O_CREAT  _O_CREAT

// from Linux asm-x86_64/mman.h
#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4             /* page can be executed */
#define PROT_NONE       0x0             /* page can not be accessed */

#define MAP_SHARED      0x01            /* Share changes */
#define MAP_PRIVATE     0x02            /* Changes are private */
#define MAP_TYPE        0x0f            /* Mask for type of mapping */
#define MAP_FIXED       0x10            /* Interpret addr exactly */
#define MAP_ANONYMOUS   0x20            /* don't use a file */

#define MREMAP_MAYMOVE  1
#define MREMAP_FIXED    2

//==========================================================================
// we have to maintain a map of existings mappings to be able to
// perform Posix-style remap and unmap by just providing the memory pointer
struct MapInfo {
    FD_t fd;
    HANDLE mappingHandle;
    DWORD dwDesiredAccess;
    OFF_T offset;
    DWORD flProtect;
};

typedef std::map<const void*,MapInfo> MapInfoMap;
inline
MapInfoMap &
getMappingHandleMap() {
    // avoid static destruction order nightmare by creating the object
    // on the heap where it will never be destructed
    static MapInfoMap * ourMappingHandles = new MapInfoMap();
    return *ourMappingHandles;
}

DEFINE_EXCEPTION(MapInfoFailure,asl::Exception)
    DEFINE_EXCEPTION(MapInfoNotFound,MapInfoFailure)
    DEFINE_EXCEPTION(DuplicateMapInfo,MapInfoFailure)

    inline
const MapInfo & getMappingInfo(const void * theAddr) {
    MapInfoMap::iterator it = getMappingHandleMap().find(theAddr);
    if (it != getMappingHandleMap().end()) {
        return it->second;
    }
    throw MapInfoNotFound(JUST_FILE_LINE);
}

inline
void addMappingInfo(const void * theAddr, const MapInfo & theInfo) {
    //AC_TRACE << "addMappingInfo: theAddr = "<< theAddr << std::endl;
    MapInfoMap::iterator it = getMappingHandleMap().find(theAddr);
    if (it == getMappingHandleMap().end()) {
        getMappingHandleMap()[theAddr] = theInfo;
        return;
    }
    throw DuplicateMapInfo(JUST_FILE_LINE);
}

inline
void removeMappingInfo(const void * theAddr) {
    //AC_TRACE << "removeMappingInfo: theAddr = "<< theAddr << std::endl;
    MapInfoMap::iterator it = getMappingHandleMap().find(theAddr);
    if (it != getMappingHandleMap().end()) {
        getMappingHandleMap().erase(it);
        return;
    }
    AC_WARNING << "removeMappingInfo: theAddr = "<< theAddr << " not found" << std::endl;
    // throw MapInfoNotFound(JUST_FILE_LINE);
}
// end of memory mapping functions
//==========================================================================

//==========================================================================
// our own version of posix-style file I/O functions; the original
// windows-builtin ones unfortunately don't combine with memory mapping
// The following functions return or use a WIN-32 File Handle that we
// need to work with the WIN32 memory mapping functions

DEFINE_EXCEPTION(UnsupportedParameters, asl::Exception);

inline
FD_t asl_open(const char *filename, int oflag, int pmode, bool share = true) {
    DB(AC_TRACE << "asl_open('"<<filename<<"', oflag="<<oflag<<",pmode="<<pmode<<",share="<<share<<std::endl);
    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode = 0;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0;
    DWORD dwCreationDisposition = 0;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    HANDLE hTemplateFile = 0;

    if (share) {
        dwShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;
    }

    if (oflag & O_CREAT) {
        //		dwCreationDisposition = CREATE_NEW;
        dwCreationDisposition = OPEN_ALWAYS;
    } else {
        dwCreationDisposition = OPEN_EXISTING;
    }
    if (oflag & O_RDWR) {
        dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;
    } else {
        if (oflag == O_RDONLY) {
            dwDesiredAccess = GENERIC_READ;
        }  else {
            return INVALID_HANDLE_VALUE; // failure
        }
    }

    HANDLE myResult = CreateFile(
                                 filename,
                                 dwDesiredAccess,
                                 dwShareMode,
                                 lpSecurityAttributes,
                                 dwCreationDisposition,
                                 dwFlagsAndAttributes,
                                 hTemplateFile
                                 );

    if (myResult == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }
    DB(AC_TRACE<<"asl_open: return fd="<<myResult<<std::endl);
    return myResult;
}

inline
int asl_close(FD_t fd) {
    DB(AC_TRACE<<"asl_close: fd="<<fd<<std::endl);
    if (CloseHandle(fd) != 0) {
        return 0; // success
    };
    return -1; // failure
}
//
//==================================================================================
// emulate posix mmap call using a Win32-handle as file descriptor
inline
void * mmap(void * start, size_t length, int prot, int flags, FD_t fd, OFF_T offset) {
    DB(AC_TRACE << "mmap: start="<<start<<", length="<<length<<",prot="<< prot <<",flags="<<flags<<",fd="<<fd<<",offset="<<offset<<std::endl);

    MapInfo myMapInfo;
    myMapInfo.fd = fd;
    myMapInfo.flProtect = 0;
    myMapInfo.dwDesiredAccess = 0;
    if ((prot&PROT_READ) && (prot&PROT_WRITE)) {
        if (flags & MAP_PRIVATE) {
            myMapInfo.flProtect = PAGE_WRITECOPY;
            myMapInfo.dwDesiredAccess = FILE_MAP_COPY;
        } else {
            myMapInfo.flProtect = PAGE_READWRITE;
            myMapInfo.dwDesiredAccess = FILE_MAP_WRITE;
        }
    } else if (prot & PROT_READ) {
        myMapInfo.flProtect = PAGE_READONLY;
        myMapInfo.dwDesiredAccess = FILE_MAP_READ;
    } else {
        throw UnsupportedParameters(JUST_FILE_LINE);
    }

    DWORD dwMaximumSizeHigh = 0;
    DWORD dwMaximumSizeLow = length&0xfffffff;
    myMapInfo.mappingHandle = CreateFileMapping(
                                                fd,
                                                0, // LPSECURITY_ATTRIBUTES
                                                myMapInfo.flProtect,
                                                0, //dwMaximumSizeHigh,
                                                0, //dwMaximumSizeLow,
                                                0 // lpname
                                                );
    if (myMapInfo.mappingHandle == NULL) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mmap: CreateFileMapping failed, reason:" << asl::errorDescription(err) << std::endl;
        return (void*)(-1);
    }
    void * myMappedMem = MapViewOfFileEx(
                                         myMapInfo.mappingHandle, // HANDLE hFileMappingObject,
                                         myMapInfo.dwDesiredAccess,
                                         0, // dwFileOffsetHigh,
                                         offset & 0xffffffff, //dwFileOffsetLow,
                                         length, // dwNumberOfBytesToMap
                                         start
                                         );
    if (myMappedMem == 0) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mmap: MapViewOfFileEx failed, reason:" << asl::errorDescription(err) << std::endl;
        CloseHandle(myMapInfo.mappingHandle);
        return (void*)(-1);
    }
    myMapInfo.offset = offset;
    addMappingInfo(myMappedMem, myMapInfo);
    DB(AC_TRACE<<"mmap: returning "<<myMappedMem<<std::endl);
    return myMappedMem;
}

// emulate posix munmap call using a Win32-handle as file descriptor
inline
int munmap(void * start, size_t length) {
    DB(AC_TRACE << "munmap: unmapping "<<start<<", handle = "<< getMappingInfo(start).mappingHandle<<std::endl);
    UnmapViewOfFile(start);
    CloseHandle(getMappingInfo(start).mappingHandle);
    removeMappingInfo(start);
    return 0;
}

// emulate posix mremap call using a Win32-handle as file descriptor
inline
void * mremap(void * old_address, size_t old_size, size_t new_size, unsigned long flags) {

    // unmap and write dirty pages to disk
    DB(AC_TRACE << "mremap: unmapping "<<old_address<<std::endl);
    if (UnmapViewOfFile(old_address) == 0) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mremap: UnmapViewOfFile failed, reason:" << asl::errorDescription(err) << std::endl;
        return (void*)(-1);
    }
    // get rid of old mapping object
    MapInfo oldMappingInfo = getMappingInfo(old_address);
    CloseHandle(oldMappingInfo.mappingHandle);

    // enlarge or shrink file
    // move file pointer to desired position
    DWORD myResult = SetFilePointer(oldMappingInfo.fd, new_size, 0, FILE_BEGIN);
    if (myResult == INVALID_SET_FILE_POINTER && asl::lastError() != CALL_HAS_SUCCEEDED) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mremap: SetFilePointer failed, reason:" << asl::errorDescription(err) << std::endl;
        return (void*)(-1);
    }

    // move eof-pointer to file pointer;
    // the doc says there must be no file mapping (UnmapViewOfFile & CloseHandle)
    // to allow this call to succeed, but even if I have some doubts about it
    // the above code should have done exactly this;
    // however, this would cause this function to fail if another mapping exists
    if (SetEndOfFile(oldMappingInfo.fd) == 0) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mremap: SetEndOfFile (enlarging or shrinking file) failed, reason:" << asl::errorDescription(err) << std::endl;
        return (void*)(-1);
    }

    // create new file mapping object
    DWORD dwMaximumSizeHigh = 0;
    DWORD dwMaximumSizeLow = new_size&0xfffffff;
    HANDLE myMapping = CreateFileMapping(
                                         oldMappingInfo.fd,
                                         0, // LPSECURITY_ATTRIBUTES
                                         oldMappingInfo.flProtect,
                                         0,//dwMaximumSizeHigh,
                                         0,//dwMaximumSizeLow,
                                         0 // lpname
                                         );
    if (myMapping == INVALID_HANDLE_VALUE) {
        LAST_ERROR_TYPE err = asl::lastError();
        AC_ERROR << "mremap: CreateFileMapping failed, reason:" << asl::errorDescription(err) << std::endl;
        return (void*)(-1);
    }

    // now try to map file again to the old memory location
    void * myMappedMem = MapViewOfFileEx(
                                         myMapping, // HANDLE hFileMappingObject,
                                         oldMappingInfo.dwDesiredAccess,
                                         0, // dwFileOffsetHigh,
                                         oldMappingInfo.offset & 0xffffffff, //dwFileOffsetLow,
                                         new_size, // dwNumberOfBytesToMap
                                         old_address // lpBaseAddress
                                         );
    DB(AC_TRACE << "mremap: first try size = "<<new_size<<", result = "<< myMappedMem <<std::endl);

    // if mapping to the old location failed, allow to relocate
    if (myMappedMem == 0) {
        removeMappingInfo(old_address);
        if ((flags&MREMAP_MAYMOVE)) {
            // retry without base address, maybe we have more success at some other location
            myMappedMem = MapViewOfFileEx(
                                          myMapping, // HANDLE hFileMappingObject,
                                          oldMappingInfo.dwDesiredAccess,
                                          0, // dwFileOffsetHigh,
                                          oldMappingInfo.offset & 0xffffffff, //dwFileOffsetLow,
                                          new_size, // dwNumberOfBytesToMap
                                          0 // lpBaseAddress
                                          );
            DB(AC_TRACE << "mremap: 2nd try size = "<<new_size<<", result = "<< myMappedMem <<std::endl);
            if (myMappedMem) {
                oldMappingInfo.mappingHandle = myMapping;
                addMappingInfo(myMappedMem,oldMappingInfo);
                return myMappedMem;
            }
            LAST_ERROR_TYPE err = asl::lastError();
            AC_ERROR << "mremap: MapViewOfFileEx size "<<new_size<<" failed, error = "<<err<<" (" << asl::errorDescription(err) <<")"<< std::endl;
            AC_ERROR << "mremap: map move not possible, giving up"<<std::endl;
            return (void*)(-1);
        }
        AC_ERROR << "mremap: map move not allowed, giving up"<<std::endl;
        return (void*)(-1);
    }

    return myMappedMem;
}

#endif

namespace asl {

    class MappedIO {
    public:
        DEFINE_NESTED_EXCEPTION(MappedIO, Failure, asl::IO_Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, SeekFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, SeekEndFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, WriteFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, OpenReadOnlyFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, OpenReadWriteFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, OpenCreateReadWriteFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, SetFileSizeFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, MMapFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, MRemapFailed, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, FileSizeMismatch, Failure);
        DEFINE_NESTED_EXCEPTION(MappedIO, GetFileSizeFailed, Failure);


#ifdef WIN32
#if 0
        static bool setFileSize(FD_t fd, OFF_T theNewSize) {
            // enlarge or shrink file
            // move file pointer to desired position
	    DWORD myResult = SetFilePointer(fd, theNewSize, 0, FILE_BEGIN);
            if (myResult == INVALID_SET_FILE_POINTER && lastError() != CALL_HAS_SUCCEEDED) {
                LAST_ERROR_TYPE err = lastError();
                AC_ERROR << "setFileSize: SetFilePointer failed, reason:" << asl::errorDescription(err) << std::endl;
                return false;
            }

            // move eof-pointer to file pointer;
            // the doc says there must be no file mapping (UnmapViewOfFile & CloseHandle)
            // to allow this call to succeed
            if (SetEndOfFile(fd) == 0) {
                LAST_ERROR_TYPE err = lastError();
                AC_ERROR << "setFileSize: SetEndOfFile (enlarging or shrinking file) failed, reason:" << asl::errorDescription(err) << std::endl;
                return false;
            }
            return true;
        }
#endif
    static void setFileSize(FD_t fd, AC_SIZE_TYPE theNewSize) {
			AC_SIZE_TYPE mySize = SetFilePointer(fd, theNewSize, 0, FILE_BEGIN);
			bool success = SetEndOfFile(fd);
			if (mySize!=theNewSize) {
				LAST_ERROR_TYPE err = lastError();
				throw MappedIO::SeekFailed(std::string("fd=")+asl::as_string(fd)+", requested size ="
					+as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
			}
			if (SetEndOfFile(fd) == 0) {
				LAST_ERROR_TYPE err = lastError();
				throw MappedIO::SetFileSizeFailed(std::string("fd=")+asl::as_string(fd)+", requested size ="
					+as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
			}
		}
		static void getFileSize(FD_t fd, OFF_T & theSize) {
			theSize = GetFileSize(fd,0);
			if (theSize == INVALID_FILE_SIZE) {
				LAST_ERROR_TYPE err = lastError();
				throw MappedIO::GetFileSizeFailed(std::string("reason:")+asl::errorDescription(err),PLUS_FILE_LINE);
			}
		}
#else

#if 1
        static void setFileSize(FD_t fd, OFF_T theNewSize)
        {
            if (ftruncate(fd, theNewSize) != 0) {
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::SetFileSizeFailed(std::string("fd=")+asl::as_string(fd)+", requested size ="
                                                  +as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);

            }
        }
#else
        static void setFileSize(FD_t fd, OFF_T theNewSize)
        {
            OFF_T mySize = LSEEK(fd, theNewSize, SEEK_SET);
            if (mySize!=theNewSize) {
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::SeekFailed(std::string("fd=")+asl::as_string(fd)+", requested size ="
                                           +as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
            }
            if (theNewSize>0) {
                LSEEK(fd, theNewSize-1, SEEK_SET);
                int dummy = 0;
                if (WRITE(fd, &dummy, 1) != 1) {
                    int err = errno;
                    throw MappedIO::ExtendFileSizeFailed(std::string("fd=")+asl::as_string(fd)+", requested size ="
                                                         +as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
                }
                LSEEK(fd, 0, SEEK_SET);
            }
        }
#endif
        static void getFileSize(FD_t fd, OFF_T & theSize) {
            theSize = LSEEK(fd, 0, SEEK_END);
            if (theSize==(OFF_T)-1) {
                throw MappedIO::SeekEndFailed(std::string("fd=")+asl::as_string(fd),PLUS_FILE_LINE);
            }
        }
#endif

        static void openFileReadOnly(const char * theFileName, FD_t & fd) {
            DB(AC_TRACE << "openFileReadOnly filename='" << theFileName << "'");
            fd = OPEN(theFileName, O_RDONLY, 0);
            if (fd == FD_INVALID) {
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::OpenReadOnlyFailed(std::string("filename=")+theFileName+" ,reason:"+asl::errorDescription(err), PLUS_FILE_LINE);
            }
        }

        static void openFileReadWrite(const char * theFileName, FD_t & fd) {
            fd = OPEN(theFileName, O_RDWR, 0);
            if (fd == FD_INVALID) {
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::OpenReadWriteFailed(std::string("filename=")+theFileName+" ,reason:"+asl::errorDescription(err), PLUS_FILE_LINE);
            }
        }

        static void createFileReadWrite(const char* theFileName, FD_t & fd) {
            fd = OPEN(theFileName, O_RDWR|O_CREAT, 0666);
            if (fd == FD_INVALID) {
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::OpenCreateReadWriteFailed(std::string("filename=")+theFileName+" ,reason:"+asl::errorDescription(err), PLUS_FILE_LINE);
            }
        }

        static void mapFileReadOnly(FD_t fd, OFF_T theSize, OFF_T theOffset, void * & theData) {
            theData = mmap(NULL, theSize, PROT_READ, MAP_SHARED, fd, theOffset);
            if (theData == (void*)-1) {
                theData = 0;
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::MMapFailed(std::string("fd=")+asl::as_string(fd)+",  size ="
                                           +as_string(theSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
            }
        }

        static void mapFileReadWrite(FD_t fd, OFF_T theSize, OFF_T theOffset, void * & theData) {
            DB(AC_TRACE << "mapFileReadWrite: fd = " << fd << ", theSize = "<<theSize<<", theOffset="<<theOffset<<std::endl);
            theData = mmap(NULL, theSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, theOffset);

            if (theData == (void*)-1) {
                theData = 0;
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::MMapFailed(std::string("fd=")+asl::as_string(fd)+",  size ="
                                           +as_string(theSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
            }
            DB(AC_TRACE << "mapFileReadWrite: return theData = " << theData <<std::endl);
        }
        static void mapFilePrivate(FD_t fd, OFF_T theSize, OFF_T theOffset, void * & theData) {
            theData = mmap(NULL, theSize, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, theOffset);

            if (theData == (void*)-1) {
                theData = 0;
				LAST_ERROR_TYPE err = lastError();
				throw MappedIO::MMapFailed(std::string("fd=")+asl::as_string(fd)+",  size ="
					+as_string(theSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
			}
		}
        static void remapFile(void * & theData, OFF_T theSize, OFF_T theNewSize) {
#ifdef OSX 
            throw MappedIO::MRemapFailed(std::string("DS: The Linux mremap() call is *not* portable ") + 
                                         "and its use in portable applications is discouraged.", 
                                         PLUS_FILE_LINE);
#else
            DB(AC_TRACE << "remapFile: theData = "<< theData << ", theSize = "<<theSize<< ", theNewSize = "<<theNewSize<<std::endl);
            theData = mremap(theData, theSize, theNewSize,  MREMAP_MAYMOVE);
            if (theData == (void*)-1) {
                theData = 0;
                LAST_ERROR_TYPE err = lastError();
                throw MappedIO::MRemapFailed(std::string("address=")+as_string(theData)
                                             +",size="+as_string(theSize)+",  new size ="
                                             +as_string(theNewSize)+",reason:"+asl::errorDescription(err),PLUS_FILE_LINE);
            }
            DB(AC_TRACE << "remapFile: return theData = " << theData <<std::endl);
#endif
		}
        bool isGood() const {
            return _myfd !=FD_INVALID && !_myErrorFlag;
        }

    protected:

        MappedIO() : _myData(0), _myfd(FD_INVALID), _myErrorFlag(false) {}
        void * data() {
            return _myData;
        }
        const void * data() const {
            return _myData;
        }
        OFF_T nbytes() const {
            return _mySize;
        }
        OFF_T offset() const {
            return _myOffset;
        }
        const std::string & name() const {
            return _myFileName;
        }
        ~MappedIO() {
            cleanup();
        };
        void cleanup() {
            if (_myData) {
                unmapFile(_myData, _mySize);
                _myData = 0;
                _mySize = 0;
            }
            if (_myfd != FD_INVALID) {
                closeFile(_myfd);
                _myfd = 0;
            }
        }
        void init(const char * theFileName) {
            DB(AC_TRACE << "MappedIO::init(theFileName='"<<theFileName);
            _myFileName = theFileName;
            _myOffset = 0;
            _mySize = 0;
            _myData = 0;
            openFile(theFileName, _myfd);
            getFileSize(_myfd, _mySize);
            if (_mySize == 0) {
                return;
            }
            mapFile(_myfd, _mySize, _myOffset, _myData);
        }

        void init(const char * theFileName, OFF_T theSize, OFF_T theOffset = 0) {
            DB(AC_TRACE << "MappedIO::init(theFileName='"<<theFileName<<"',theSize="<<theSize<<",theOffset="<<theOffset<<")");
            _myFileName = theFileName;
            _myOffset = theOffset;
            _mySize = theSize;
            _myData = 0;
            openFile(theFileName, _myfd);
            //OFF_T myRealSize = 0;
            //getFileSize(_myfd, myRealSize);
            if (theSize == 0) {
                return;
            }
            mapFile(_myfd, _mySize, _myOffset, _myData);
            /*
              if (_myOffset +  _mySize <= myRealSize) {
              mapFile(_myfd, _mySize, _myOffset, _myData);
              return;
              }
              else {
              throw FileSizeMismatch(std::string("Filename:")+theFileName+
              ", size="+as_string(myRealSize)+
              ",requested offset+size"+as_string(_myOffset+_mySize),PLUS_FILE_LINE);
              }
            */
        }

        virtual void openFile(const char * theFileName, FD_t & fd) = 0;

        void closeFile(FD_t fd) {
            if (CLOSE(fd) != 0) {
                AC_ERROR << "MappedIO: closeFile() : close(fd = " << fd << ") failed" << std::endl;
                perror("reason");
            }
        }

        virtual void mapFile(FD_t fd, OFF_T theSize, OFF_T theOffset, void * & theData) = 0;

        void unmapFile(void * theData, OFF_T theSize) {
            if (munmap(theData, theSize) != 0) {
                int err = errno;
                AC_ERROR << "### ERROR: unmapFile(): munmap failed, reason:" << errorDescription(err) << std::endl;
            }
            _myData = 0;
        }
#ifdef NO_REMAP
        void resizeMapping(OFF_T newSize) {
            if (_myData) {
                unmapFile(_myData, _mySize);
                _myData = 0;
                _mySize = 0;
            }
            if (newSize > 0) {
                setFileSize(_myfd, newSize);
                mapFile(_myfd, newSize, _myOffset, _myData);
                _mySize = newSize;
            }
        }
#else
        void resizeMapping(OFF_T newSize) {
            if (_mySize == 0 && newSize > 0) {
                setFileSize(_myfd, newSize);
                mapFile(_myfd, newSize, _myOffset, _myData);
                _mySize = newSize;
                return;
            }
            if (_myData) {
                remapFile(_myData, _mySize, newSize);
            }
            _mySize = newSize;
        }
#endif
    protected:
        void setErrorFlag(bool theFlag) const {
            _myErrorFlag = theFlag;
        }
        bool getErrorFlag() const {
            return _myErrorFlag;
        }
    private:
        void *_myData;
        OFF_T _mySize;
        OFF_T _myOffset;
        FD_t _myfd;
        std::string _myFileName;
        mutable bool _myErrorFlag;
    };


    class ConstMappedBlock : public asl::ReadableBlock, private MappedIO {
    public:
        ConstMappedBlock() {}
        ConstMappedBlock(const ConstMappedBlock & r) {
            cleanup();
            init(r.name().c_str(), r.nbytes());
        }
        ConstMappedBlock & operator=(const ConstMappedBlock & r) {
            cleanup();
            init(r.name().c_str(), r.nbytes());
            return *this;
        }
        explicit ConstMappedBlock(const std::string & filename) {
            init(filename.c_str());
        }
        ConstMappedBlock(const std::string & filename, OFF_T Size)  {
            init(filename.c_str(), Size);
        }
        explicit ConstMappedBlock(const char * filename) {
            init(filename);
        }
        ConstMappedBlock(const char * filename, OFF_T Size)  {
            init(filename, Size);
        }
        const unsigned char * begin() const {
            return static_cast<const unsigned char *>(data());
        }
        const unsigned char * end() const {
            return static_cast<const unsigned char *>(data()) + nbytes();
        }
        AC_SIZE_TYPE size() const {
            return nbytes();
        }
        operator bool() const {
            return isGood();
        }
    private:
        void openFile(const char * theFileName, FD_t & fd) {
            openFileReadOnly(theFileName, fd);
        }
        void mapFile(FD_t fd, OFF_T theSize, OFF_T theOffset, void * & theData) {
            mapFileReadOnly(fd, theSize, theOffset, theData);
        }
    };
    typedef asl::Ptr<ConstMappedBlock> ConstMappedBlockPtr;

    class WriteableMappedBlock : public asl::WriteableBlock, private MappedIO {
    public:
        WriteableMappedBlock() {}
    private:
        WriteableMappedBlock(WriteableMappedBlock & r) {
        }
        WriteableMappedBlock & operator=(const WriteableMappedBlock & r) {
            return *this; // avoid warning
        }
    public:
        WriteableMappedBlock(const char* filename) {
            init(filename);
        }
        WriteableMappedBlock(const char* filename, long Size)  {
            init(filename, Size);
        }
        WriteableMappedBlock(const std::string & filename) {
            init(filename.c_str());
        }
        WriteableMappedBlock(const std::string & filename, OFF_T Size)  {
            init(filename.c_str(), Size);
        }
        const unsigned char * begin() const {
            return static_cast<const unsigned char *>(data());
        }
        const unsigned char * end() const {
            return static_cast<const unsigned char *>(data()) + nbytes();
        }
        unsigned char * begin() {
            return static_cast<unsigned char *>(data());
        }
        unsigned char * end() {
            return static_cast<unsigned char *>(data()) + nbytes();
        }
        AC_SIZE_TYPE size() const {
            return nbytes();
        }
        virtual void assign(const ReadableBlock & theSource) {
            if (theSource.size() != size() )
                throw BlockSizeMismatch (
                                         "Blocks have different theSize",
                                         "WriteableMappedBlock::assign()");
            std::copy(theSource.begin(),theSource.end(), begin());
        }
        operator bool() const {
            return isGood();
        }
    private:
        void openFile(const char * theFileName, FD_t & fd) {
            openFileReadWrite(theFileName, fd);
        }
        void mapFile(FD_t fd, OFF_T theSize, OFF_T theOffset, void *& theData) {
            mapFileReadWrite(fd, theSize, theOffset, theData);
        }
    };

    class NewMappedBlock : public asl::WriteableBlock, private MappedIO {
    public:
        NewMappedBlock() {}
    private:
        NewMappedBlock(NewMappedBlock & r) {
        }
        NewMappedBlock & operator=(const NewMappedBlock & r) {
            return *this; // avoid warning
        }
    public:
        NewMappedBlock(const char* filename, long Size)  {
            init(filename, Size);
        }
        const unsigned char * begin() const {
            return static_cast<const unsigned char *>(data());
        }
        const unsigned char * end() const {
            return static_cast<const unsigned char *>(data()) + nbytes();
        }
        unsigned char * begin() {
            return static_cast<unsigned char *>(data());
        }
        unsigned char * end() {
            return static_cast<unsigned char *>(data()) + nbytes();
        }
        AC_SIZE_TYPE size() const {
            return nbytes();
        }
        virtual void assign(const ReadableBlock & theSource) {
            if (theSource.size() != size() )
                throw BlockSizeMismatch (
                                         "Blocks have different theSize",
                                         "NewMappedBlock::assign()");
            std::copy(theSource.begin(),theSource.end(), begin());
        }
        operator bool() const {
            return isGood();
        }
    private:
        void openFile(const char * theFileName, FD_t & fd) {
            createFileReadWrite(theFileName, fd);
            setFileSize(fd, nbytes());
        }
        void mapFile(FD_t fd, OFF_T theSize, OFF_T theOffset, void *& theData) {
            mapFileReadWrite(fd, theSize, theOffset, theData);
        }
    };

    class MappedBlock : public asl::VariableCapacityBlock, private MappedIO {
    public:
        MappedBlock() {}
        ~MappedBlock() {
            // truncate file to actual size
            reserve(size());
        }
    private:
        MappedBlock(MappedBlock & r) {
        }
        MappedBlock & operator=(const MappedBlock & r) {
            return *this; // avoid warning
        }
    public:
        MappedBlock(const char* filename)  {
            init(filename);
            _mySize = capacity();
        }
        MappedBlock(const char* filename, long Size)  {
            init(filename, Size);
            _mySize = capacity();
        }
        MappedBlock(const std::string & filename) {
            init(filename.c_str());
            _mySize = capacity();
        }
        MappedBlock(const std::string & filename, OFF_T Size)  {
            init(filename.c_str(), Size);
            _mySize = capacity();
        }
        void resize(AC_SIZE_TYPE newSize) {
            DB(AC_TRACE << "MappedBlock::resize("<<newSize<<")"<<std::endl);
            if (newSize == size()) {
                return;
            }
            if (capacity() < newSize) {
                reserve(newSize + newSize/2 + MIN_CAPACITY);
            }
            _mySize = newSize;
        }
        const unsigned char * begin() const {
            return static_cast<const unsigned char *>(data());
        }
        const unsigned char * end() const {
            return static_cast<const unsigned char *>(data()) + size();
        }
        unsigned char * begin() {
            return static_cast<unsigned char *>(data());
        }
        unsigned char * end() {
            return static_cast<unsigned char *>(data()) + size();
        }
        AC_SIZE_TYPE size() const {
            return _mySize;
        }
        AC_SIZE_TYPE capacity() const {
            return nbytes();
        }
        virtual void reserve(AC_SIZE_TYPE theCapacity) {
            DB(AC_TRACE << "MappedBlock::reserve("<<theCapacity<<")"<<std::endl);
            resizeMapping(theCapacity);
        }
        operator bool() const {
            return isGood();
        }
    private:
        enum { MIN_CAPACITY = 4096 };
        OFF_T _mySize;

        void openFile(const char * theFileName, FD_t & fd) {
            createFileReadWrite(theFileName, fd);
            setFileSize(fd, capacity());
        }
        void mapFile(FD_t fd, OFF_T theSize, OFF_T theOffset, void *& theData) {
            mapFileReadWrite(fd, theSize, theOffset, theData);
        }
    };

} // asl

/* @} */


#undef DB

#endif
