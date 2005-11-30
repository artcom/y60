/*
/--------------------------------------------------------------------
|
|      $Id: plexif.h,v 1.5 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2003 Ulrich von Zadow
|
|      Implementation of PLExif class by Mike Franklin
|      Much of the decoding code was adapted from code by Ken Reneris:
|         http://www.reneris.com/tools/exif.asp -
|                                 though it has been heavily modified!
|
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLEXIF
#define INCL_PLEXIF

#ifdef _MSC_VER                 // The microsoft compiler generates masses 
                                // of symbol too long
#pragma warning(disable: 4786)  // for debugger warnings for sets and maps
#endif

#include "plpaintlibdefs.h"
#include "plcountedpointer.h"

#include <cstdio>
#include <vector>
#include <string>
#include <map>

struct jpeg_decompress_struct;
struct jpeg_compress_struct;

// these structures are for internal use only
struct _PLExifTranslator;
struct _PLExifTagValues;
struct _PLExifFormatter;

class PLExifTag;
typedef PLCountedPointer<PLExifTag>           PLExifTagCPtr;  // PLExifTag counted pointer
typedef std::vector<PLExifTagCPtr>            PLExifTagList;  // vector of PLExifTag counted pointers
typedef PLCountedArrayPointer<PLBYTE>         PLByteCPtr;     // utility typedef


class PLExif
{
public:
                        PLExif();
                        ~PLExif();
                        // default copying is safe

                        // reset the internal data
  void                  Clear();

  // Access the raw EXIF data (just a copy of the EXIF header held in memory)
  size_t                GetRawDataSize() const;
  PLBYTE *              GetRawData();
  const PLBYTE *        GetRawData() const;

/*
  // These are not EXIF comments and should probably be moved to PLBmpInfo
  const std::string &   GetComment() const;
  const char *          GetCommmentCStr() const;

  void                  SetComment(const std::string &);
  void                  SetComment(const char *);
*/
  // Access to the Main, Sub and Manufacturer tags
  // also a single list that contains them all
  const PLExifTagList & GetAllTags() const;
  const PLExifTagList & GetMainTags() const;
  const PLExifTagList & GetSubTags() const;
  const PLExifTagList & GetManufacturerTags() const;

  // As above but returning a traditional C style array pointer
  // instead of an STL vector
  // It is an array of pointers to PLExifTag
  const PLExifTagCPtr * GetAllTagsC(size_t & size) const;
  const PLExifTagCPtr * GetMainTagsC(size_t & size) const;
  const PLExifTagCPtr * GetSubTagsC(size_t & size) const;
  const PLExifTagCPtr * GetManufacturerTagsC(size_t & size) const;


  // All the following functions give access to a tag given a tag shortname
  // they almost all return a pointer to the whole tag but mostly this
  // will just be used to check whether the tag was found by testing for
  // a NULL pointer. The useful data - the tag value is generally returned
  // by reference.
  //
  // duplicate copies of the functions allow efficient use whether
  // the source tag string is a std::string or a char *
  // All forms return a pointer to the found tag, 0 if not found

  // Just return the tag pointer
  PLExifTag *           GetTag(const char * TagShortName) const;
  PLExifTag *           GetTag(const std::string & TagShortName) const
                        { return GetTag(TagShortName.c_str()); }

  // place the tag value into the Value string, format is basic
  // as stored in the raw EXIF data eg f4.0 is likely to be 40/10
  PLExifTag *           GetTag(const char * TagShortName, std::string & Value) const;
  PLExifTag *           GetTag(const std::string & TagShortName, std::string & Value) const
                        { return GetTag(TagShortName.c_str(), Value); }

  // place the tag value in the Value string, format is common form
  // eg f4.0 is 4.0 rather than 40/10
  PLExifTag *           GetTagCommon(const char * TagShortName, std::string & Value) const;
  PLExifTag *           GetTagCommon(const std::string & TagShortName, std::string & Value) const
                        { return GetTagCommon(TagShortName.c_str(), Value); }

  // place the tag value in the Value double where appropriate
  // eg 1.234m focus
  PLExifTag *           GetTag(const char * TagShortName, double & Value) const;
  PLExifTag *           GetTag(const std::string & TagShortName, double & Value) const
                        { return GetTag(TagShortName.c_str(), Value); }

  // return tag value as a string (empty string if not found), format is basic
  // as stored in the raw EXIF data eg f4.0 is likely to be 40/10
  std::string &         TagStr(const char * TagShortName) const;
  std::string &         TagStr(const std::string & TagShortName) const
                        { return TagStr(TagShortName.c_str()); }

  // return tag value as a string (empty string if not found)
  // format is common form eg f4.0 is 4.0 rather than 40/10
  std::string &         TagStrCommon(const char * TagShortName) const;
  std::string &         TagStrCommon(const std::string & TagShortName) const
                        { return TagStrCommon(TagShortName.c_str()); }

  // C string return versions of TagStr and TagStrCommon

  // return tag value as a string (empty string if not found), format is basic
  // as stored in the raw EXIF data eg f4.0 is likely to be 40/10
  const char *          TagCStr(const char * TagShortName) const;
  const char *          TagCStr(const std::string & TagShortName) const
                        { return TagCStr(TagShortName.c_str()); }

  // return tag value as a string (empty string if not found)
  // format is common form eg f4.0 is 4.0 rather than 40/10
  const char *          TagCStrCommon(const char * TagShortName) const;
  const char *          TagCStrCommon(const std::string & TagShortName) const
                        { return TagCStrCommon(TagShortName.c_str()); }

  // These two methods are primarily intended for use only by the Paintlib encoders and decoders
                        // load data from cinfo - jpeg_save_markers must have been called first
  void                  ReadData(const jpeg_decompress_struct * pcinfo);

                         // write data to cinfo - after jpeg_start_compress
  void                  WriteData(jpeg_compress_struct * pcinfo);


private:
  void                  decode();   // do the actual decoding of the EXIF data

  void                  ReadIFD(const _PLExifTagValues * Tags, char * Prefix, PLExifTagList & sectionList);
  void                  ExpandBinaryTag(const std::string & Src, const _PLExifTagValues *Tags, PLUINT Type, PLExifTagList & sectionList);
  void                  DecodeCanCustomFncs(const PLExifTag & rootTag, const _PLExifTagValues *Tags, PLExifTagList & sectionList);

  void                  CopyTag(const char * Src, const char * Dst);
  void                  SetTag(const char * Dst, const char * Value);
  void                  SetTag(const char * Dst, const std::string & Value)
                        { SetTag(Dst, Value.c_str()); }
  void                  AddTag(const char * Dst, const char * SrcTag, const char * Skip = NULL, const char * Sep = NULL);
  void                  AddStr(const char * Dst, const std::string & SrcStr,  const char * Skip = NULL, const char * Sep = NULL);
  void                  FormatRange(double Low, double High, std::string & Str);

  // these functions were originally designed to read from a file
  // they have been kept but are now "reading" from the vector m_Data
  // I may remove at least the first couple later
  void                  SetPos(size_t Pos);
  size_t                GetPos();

  PLWORD                GetU16();
  PLLONG                GetU32();
  void                  Read(void * Buffer, size_t Size);



private:

  PLByteCPtr          m_Data;
  size_t              m_DataSize;

  // this should probably be moved to PLBmpInfo
//  std::string         m_Comment;  // include comments even though they're not really EXIF

  size_t              m_Pos;     // offset into data (effectively index into m_Data
  size_t              m_IdfOffset;
  bool                m_Endian;

  PLExifTagList       m_AllTags;      // Tags in the order found
  PLExifTagList       m_MainTags;
  PLExifTagList       m_SubTags;
  PLExifTagList       m_ManufacturerTags;

  typedef std::map<std::string, PLExifTagCPtr>  TagMap;
  TagMap              m_Tags;         // tag look up for searching by short name

};


class PLExifTag
{
public:
                  // default copying is safe
                  ~PLExifTag();

  // C Interface - returning const char *
  const char *          GetShortNameCStr() const;
  const char *          GetDescriptionCStr() const;
  // Format is basic as stored in the raw EXIF data
  // eg f4.0 is likely to be 40/10
  const char *          GetValueCStr() const;
  // format is common form eg f4.0 is 4.0 rather than 40/10
  const char *          GetValueCommonCStr() const;

  // STL Interface - returning const string &
  const std::string &   GetShortName() const;
  const std::string     GetDescription() const;
  // Format is basic as stored in the raw EXIF data
  // eg f4.0 is likely to be 40/10
  const std::string &   GetValue() const;
  // format is common form eg f4.0 is 4.0 rather than 40/10
  const std::string &   GetValueCommon() const;

private:
  // render functions to convert from raw memory data
  size_t          RenDef(PLBYTE * & Buffer);
  size_t          RenUDef(PLBYTE * & Buffer);
  size_t          RenUndef(PLBYTE * & Buffer);
  size_t          RenStr(PLBYTE * & Buffer);
  size_t          RenURat(PLBYTE * & Buffer);
  size_t          RenRat(PLBYTE * & Buffer);

  // conversion to common values
  void            CnvRat(std::string & result);   // evaluate nominator/denominator
  void            CnvRatAp(std::string & result); // evaluate nominator/denominator for fstop
  void            CnvFrac(std::string & result);  // evaluate nominator/denominator if > than 1
  void            CnvApexShutter(std::string & result);
  void            CnvApexAp(std::string & result);
  void            CnvCompCfg(std::string & result);   // component configuration
  void            CnvCanINo(std::string & result);    // decode canon image no
  void            CnvCanSNo(std::string & result);    // decode canon serial no
  void            CnvCanFlash(std::string & result);  // canon flash details
  void            CnvCanAFPnt(std::string & result);  // canon AF focus point used

  // friends to let them have access to the conversion and rendering
  // functions without having to make them public to the whole world
  friend struct _PLExifTagValues;
  friend struct _PLExifFormatter;

private:
                  PLExifTag(PLUINT TagNo, PLUINT Format, PLUINT NoComp);    // only ever created by friend PLExif

  void            CleanWorkingArea();   // call after all decoding done to free mem buffer
  void            Swizzle();
  void            Render();
  void            DoTranslation();
  void            Value(size_t Index);
  double          GetDouble(size_t Index);


  const _PLExifTagValues *   m_Tag;    // The tag for this item - pointer to static data so copy safe
  const _PLExifFormatter *   m_Fmt;    // The format type - pointer to static data so copy safe

  std::string     m_ShortName;    // Copy of the short name
  std::string     m_Lookup;       // Lowercase version of shortname

  size_t          m_TagNo;        // The tag no
  size_t          m_Format;       // Format of this item
  size_t          m_NoComp;       // Number of components
  size_t          m_Size;         // Total size of item
  PLByteCPtr      m_Buffer;       // Copy of the item's data
  size_t          m_Pos;          // Location of tag

  // The rendered value
  std::string     m_Value;        // In printable form
  std::string     m_Common;       // in printable common form
  PLLONG          m_Num;          // Numerator
  PLLONG          m_Den;          // Denominator
  PLLONG          m_Int;          // As int
  PLUINT          m_UInt;         // As unsigned int
  double          m_Double;       // As double

private:
  // not too thrilled with this but I inherited it!
  // however it does avoid having to put stuff into
  // the public interface that only PLExif will need
  // and will not be needed by the library clients
  friend class PLExif;

private:
  // these need to be static members to have access to conversion and render functions
  static _PLExifTagValues   MainTags[];
  static _PLExifTagValues   SubTags[];
  static _PLExifTagValues   NikonTags[];
  static _PLExifTagValues   Nikon2Tags[];
  static _PLExifTagValues   OlympusTags[];
  static _PLExifTagValues   CanonTags[];
  static _PLExifTagValues   CanonSet1[];
  static _PLExifTagValues   CanonSet2[];
  static _PLExifTagValues   CanonCFn[];
  static _PLExifTagValues   FujifilmTags[];
  static _PLExifTagValues   CasioTags[];
  static _PLExifFormatter   rgExifFormat[];
};






#endif
