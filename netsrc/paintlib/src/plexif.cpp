/*
/--------------------------------------------------------------------
|
|      $Id: plexif.cpp,v 1.10 2004/09/11 12:41:35 uzadow Exp $
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

#include "config.h"

#include "plexif.h"
#include "pldebug.h"
#include "plexcept.h"

#include <sstream>
#include <cctype>
#include <iomanip>
#include <memory>
#include <cmath>

#include <iostream> 

extern "C"
{
#include "jpeglib.h"
}

using namespace std;

struct _PLExifTranslator
{
  int         Value;
  char *      Desc;
};

struct _PLExifTagValues
{
  PLUINT      Tag;            // The tag #
  char *      ShortName;      // Short name for the tag
  _PLExifTranslator *Trans;    // Translate values / units
  void        (PLExifTag::*Convert)(std::string &);  // convert to common format
  char *      Desc;           // Normal name for the tag
};

struct _PLExifFormatter
{
  PLUINT      Size;                   // Size of each item in this format
  void        (*Swizzle)(PLBYTE *Data);      // Swizzle the data
  size_t      (PLExifTag:: *Render)(PLBYTE * & Buffer);
};


// place all these globals in an anonymous namespace
// so they are only visible (accessible) to this
// compilation unit (source file)
namespace
{

const int VALUE_UNIT         = -1;
const int VALUE_LOWER_STR    = -2;

enum ExifType {etUint8 = 1, etAsciiStr, etUint16, etUint32, etUrat64,
               etInt8, etInt16, etInt32, etRat64};



_PLExifTranslator OffOn[] = {
    0, "Off",
    1, "On",
    0,  NULL
};

_PLExifTranslator OnOff[] = {
    0, "On",
    1, "Off",
    0,  NULL
};

_PLExifTranslator DisableEnable[] = {
    0, "Disable",
    1, "Enable",
    0,  NULL
};

_PLExifTranslator EnableDisable[] = {
    0, "Enable",
    1, "Disable",
    0,  NULL
};

_PLExifTranslator ResUnit[] = {
    1,  "no-unit",
    2,  "inch",
    3,  "cm",
    0,  NULL
};

_PLExifTranslator FPResUnit[] = {
    1,  "inch",
    2,  "inch",     // should be meter, but broke on some cameras?
    3,  "centimeter",
    4,  "milimemeter",
    5,  "micrometer",
    0,  NULL
};

_PLExifTranslator ExpProg[] = {
    1,  "Manual",
    2,  "Program",
    3,  "Aperture priority",
    4,  "Shutter priority",
    5,  "Slow",
    6,  "Action",
    7,  "Portrait",
    8,  "Landscape",
    0,  NULL
};

_PLExifTranslator BpsCompress[] = {
    1,  "Basic",
    2,  "Normal",
    3,  "Fine",
    4,  "Fine",
    0,  NULL
};

_PLExifTranslator MeterMode[] = {
    0,  "Unknown",
    1,  "Average",
    2,  "Center weighted",
    3,  "Spot",
    4,  "Multi-spot",
    5,  "Multi-segment",
    6,  "Partial",
    255,"Other",
    0,  NULL
};

_PLExifTranslator LightSource[] = {
    0,  "Unknown/cloudy",
    1,  "Daylight",
    2,  "Fluorescent",
    3,  "Tungsten",
    10, "Flash",
    17, "Standard light A",
    18, "Standard light B",
    19, "Standard light C",
    20, "D55",
    21, "D65",
    22, "D75",
    0,  NULL
};

_PLExifTranslator FlashUsed[] = {
    0,  "No",
    1,  "Yes",
    5,  "Yes, no return detect",
    7,  "Yes, return detected",
    9,  "Yes",        // maybe should be more than just yes ?
    16, "No forced",    // not sure about this one!
    0,  NULL
};


_PLExifTranslator ColorSpace[] = {
    1,  "sRGB",
    65535, "Uncalibrated",
    0,  NULL
};


_PLExifTranslator SenseMethod[] = {
    2, "1 chip color area",
    0,  NULL
};


_PLExifTranslator FileSource[] = {
    3, "Digital still camera",
    0,  NULL
};


_PLExifTranslator SceneType[] = {
    1, "Directly photographed",
    0,  NULL
};

_PLExifTranslator N2Quality[] = {
    1, "VGA Basic",
    2, "VGA Normal",
    3, "VGA Fine",
    4, "SXGA Basic",
    5, "SXGA Normal",
    6, "SXGA Fine",
    12,"UXGA Fine",
    20,"HI (Uncompressed)",
    0,  NULL
};

_PLExifTranslator N2Color[] = {
    1, "Colour",
    2, "Monochrome",
    0,  NULL
};

_PLExifTranslator N2ImgAdjust[] = {
    0, "Normal",
    1, "Bright+",
    2, "Bright-",
    3, "Contrast+",
    4, "Contrast-",
    0,  NULL
};

_PLExifTranslator N2Iso[] = {
    0, "80",
    2, "160",
    4, "320",
    5, "100",
    0,  NULL
};

_PLExifTranslator N2WhiteBal[] = {
    0, "Auto",
    1, "Preset",
    2, "Daylight",
    3, "Incandescent",
    4, "Flourescent",
    5, "Cloudy",
    6, "SpeedLight",
    0,  NULL
};

_PLExifTranslator N2Converter[] = {
    0, "None",
    1, "Fisheye",
    0,  NULL
};

_PLExifTranslator OlyJpegQ[] = {
    1, "SQ",
    2, "HQ",
    3, "SHQ",
    0,  NULL
};

_PLExifTranslator OlyMacro[] = {
    0, "Normal",
    1, "Macro",
    0,  NULL
};

_PLExifTranslator OlySharp[] = {
    0, "Normal",
    1, "Hard",
    2, "Soft",
    0,  NULL
};

_PLExifTranslator OlyFlash[] = {
    0, "On",
    1, "Red-eye",
    2, "Fill",
    2, "Off",
    0,  NULL
};

_PLExifTranslator CanMacro[] = {
    1, "Macro",
    2, "Off",
    0,  NULL
};

_PLExifTranslator CanFlashMode[] = {
    0,  "Off",
    1,  "Auto",
    2,  "On",
    3,  "Red-eye",
    4,  "Slow",
    5,  "Auto + red-eye",
    6,  "On + red-eye",
    16, "External",
    0,  NULL
};

_PLExifTranslator CanDrvMode[] = {
    0,  "Single/self timer",
    1,  "Continuous",
    0,  NULL
};

_PLExifTranslator CanFocusMode[] = {
    0,  "One-shot",
    1,  "AI servo",
    2,  "AI Focus",
    3,  "Manual",
    4,  "Single",
    5,  "Continuous",
    6,  "Manual",
    0,  NULL
};

_PLExifTranslator CanImgSize[] = {
    0,  "Large",
    1,  "Medium",
    2,  "Small",
    0,  NULL
};

_PLExifTranslator CanShootMode[] = {
    0,  "Full auto",
    1,  "Manual",
    2,  "Landscape",
    3,  "Fast shutter",
    4,  "Slow shutter",
    5,  "Night",
    6,  "B & W",
    7,  "Sepia",
    8,  "Portrait",
    9,  "Sports",
    10, "Macro",
    11, "Pan Focus",
    0,  NULL
};

_PLExifTranslator CanLNH[] = {
    0xFFFF, "Low",
    0,      "Normal",
    1,      "High",
    0,  NULL
};

_PLExifTranslator CanIso[] = {
    0,   "Not supplied use EXIF tag",
    15,  "Auto",
    16,  "50",
    17,  "100",
    18,  "200",
    19,  "400",
    0,  NULL
};

_PLExifTranslator CanMeter[] = {
    3,  "Evaluative",
    4,  "Partial",
    5,  "Center-weighted",
    0,  NULL
};

_PLExifTranslator CanWhiteBal[] = {
    0,  "Auto",
    1,  "Sunny",
    2,  "Cloudy",
    3,  "Tungsten",
    4,  "Flourescent",
    5,  "Flash",
    6,  "Custom",
    0,  NULL
};

_PLExifTranslator CanFlashBias[] = {
    0xFFC0,  "-2 EV",
    0xFFCC,  "-1.67 EV",
    0xFFD0,  "-1.50 EV",
    0xFFD4,  "-1.33 EV",
    0xFFE0,  "-1 EV",
    0xFFEC,  "-0.67 EV",
    0xFFF0,  "-0.50 EV",
    0xFFF4,  "-0.33 EV",
    0x0000,  "0 EV",
    0x000C,  "0.33 EV",
    0x0010,  "0.50 EV",
    0x0014,  "0.67 EV",
    0x0020,  "1 EV",
    0x002C,  "1.33 EV",
    0x0030,  "1.50 EV",
    0x0034,  "1.67 EV",
    0x0040,  "2 EV",
    0,  NULL
};

_PLExifTranslator CanAFPoint[] = {
    0x3000, "MF",
    0x3001, "Auto selected",
    0x3002, "Right",
    0x3003, "Center",
    0x3004, "Left",
    0,  NULL
};

_PLExifTranslator CFnLock[] = {
    0,  "AF/AE Lock",
    1,  "AE Lock/AF",
    2,  "AF/AF Lock",
    3,  "AE+release/AE+AF",
    0,  NULL
};

_PLExifTranslator CFnTVAV[] = {
    0,  "1/2 stop",
    1,  "1/3 stop",
    0,  NULL
};

_PLExifTranslator CFnAFAssist[] = {
    0, "On (auto)",
    1, "Off",
    2, "External only",
    0,  NULL
};

_PLExifTranslator CFnShutAv[] = {
    0,  "Auto",
    1,  "1/200(fixed)",
    0,  NULL
};

_PLExifTranslator CFnAeb[] = {
    0,  "0,-,+/Enabled",
    1,  "0,-,+/Disabled",
    2,  "-,0,+/Enabled",
    3,  "-,0,+/Disabled",
    0,  NULL
};

_PLExifTranslator CFnShutSync[] = {
    0,  "1st-curtain sync",
    1,  "2nd-curtain sync",
    0,  NULL
};

_PLExifTranslator CFnAFStop[] = {
    0,  "AF Stop",
    1,  "Operate AF",
    2,  "Lock AE & start timer",
    0,  NULL
};

_PLExifTranslator CFnMenu[] = {
    0,  "Top",
    1,  "Previous(volatile)",
    2,  "Previous",
    0,  NULL
};

_PLExifTranslator CFnSet[] = {
    0,  "Not assigned",
    1,  "Change quality",
    2,  "Change ISO",
    3,  "Select Params",
    0,  NULL
};

_PLExifTranslator FujiSharp[] = {
    1, "Soft",
    2, "Soft",
    3, "Normal",
    4, "Hard",
    5, "Hard",
    0,  NULL
};

_PLExifTranslator FujiWBal[] = {
    0x0000,  "Auto",
    0x0100,  "Daylight",
    0x0200,  "Cloudy",
    0x0300,  "DaylightColor-fluorescent",
    0x0301,  "DaywhiteColor-fluorescent",
    0x0302,  "White-fluorescent",
    0x0400,  "Incandescent",
    0x0F00,  "Custom",
    0,       NULL
};

_PLExifTranslator FujiColor[] = {
    0x0000,  "Normal",
    0x0100,  "High",
    0x0200,  "Low",
    0x0300,  "Black & White",
    0,    NULL
};

_PLExifTranslator FujiTone[] = {
    0x0000,  "Normal",
    0x0100,  "High",
    0x0200,  "Low",
    0,    NULL
};

_PLExifTranslator FujiFlashMode[] = {
    0, "Auto",
    1, "On",
    2, "Off",
    3, "Red-eye",
    0,  NULL
};

_PLExifTranslator FujiFlashStrg[] = {
    0, "Auto",
    1, "On",
    2, "Off",
    3, "Red-eye",
    0,  NULL
};

_PLExifTranslator FujiMacro[] = {
    0, "Off",
    1, "Macro",
    2, "Super Macro",
    0,  NULL
};

_PLExifTranslator FujiFocus[] = {
    0, "Auto",
    1, "Manual",
    0,  NULL
};

_PLExifTranslator FujiPicture[] = {
    0x0000,   "Auto",
    0x0001,   "Portrait",
    0x0002,   "Landscape",
    0x0004,   "Sports",
    0x0005,   "Night",
    0x0006,   "Program AE",
    0x0100,   "Aperture Prior AE",
    0x0200,   "Shutter Prior AE",
    0x0300,   "Manual exposure",
    0,  NULL
};

_PLExifTranslator FujiBlurFocus[] = {
    0, "OK",
    1, "Warning",
    0,  NULL
};

_PLExifTranslator FujiAE[] = {
    0, "OK",
    1, "Over-exposed",
    0,  NULL
};


_PLExifTranslator CasioRecord[] = {
    1,   "Single Shutter",
    2,   "Panorama",
    3,   "Night Scene",
    4,   "Portrait",
    5,   "Landscape",
    7,   "Panorama",
    10,  "Night Scene",
    15,  "Portrait",
    16,  "Landscape",
    0,  NULL
};

_PLExifTranslator CasioQuality[] = {
    1,   "Economy",
    2,   "Normal",
    3,   "Fine",
    0,  NULL
};

_PLExifTranslator CasioFocus[] = {
    2,   "Macro",
    3,   "Auto Focus",
    4,   "Manual Focus",
    5,   "Infinity",
    0,  NULL
};

_PLExifTranslator CasioFlashMode[] = {
    1,   "Auto",
    2,   "On",
    3,   "Off",
    4,   "Off",   // on QV2000 this is Red Eye Reduction must be fixed as a special case :(
    5,   "Red Eye Reduction",
    0,  NULL
};

_PLExifTranslator CasioFlashInt[] = {
    11,   "Weak",
    13,   "Normal",
    14,   "High",
    15,   "Strong",
    0,  NULL
};

_PLExifTranslator CasioWhite[] = {
    1,   "Auto",
    2,   "Tungsten",
    3,   "Daylight",
    4,   "Fluorescent",
    5,   "Shade",
    9,   "Manual",
    0,  NULL
};

_PLExifTranslator CasioZoom[] = {
    0x10000,   "Off",
    0x10001,   "2X",    // QV2000
    0x20000,   "2X",    // QV8000
    0x40000,   "4X",    // QV8000
    0,  NULL
};

_PLExifTranslator CasioSharp[] = {
    0,   "Normal",
    1,   "Soft",
    2,   "Hard",
    0,  NULL
};

_PLExifTranslator CasioLowHi[] = {
    0,   "Normal",
    1,   "Low",
    2,   "High",
    0,  NULL
};

_PLExifTranslator CasioSens[] = {
    64,   "Normal",   // QV3k
    80,   "Normal",   // QV8k/2k
    100,  "High",     // 8k/2k
    125,  "+1.0",     // 3k
    250,  "+2.0",     // 3k
    244,  "+3.0",     // 3k
    9,   "Manual",
    0,  NULL
};


_PLExifTranslator CasioEnhance[] = {
    1,  "Off",
    2,  "Red",
    3,  "Green",
    4,  "Blue",
    5,  "Flesh Tones",
    0,  NULL
};

_PLExifTranslator CasioFFrmLoc[] = {
    1,  "Centre",
    2,  "Scene S1",
    3,  "Scene S2",
    4,  "Scene S3",
    5,  "Scene S4",
    6,  "Scene S5",
    7,  "Scene S8",
    8,  "Scene S6",
    9,  "Scene S7",
    10, "Top Left",
    11, "Top Middle",
    12, "Top Right",
    13, "Middle Left",
    14, "Middle Right",
    15, "Bottom Left",
    16, "Bottom Middle",
    17, "Bottom Right",
    0,  NULL
};

_PLExifTranslator CasioFilter[] = {
    1,  "Off",
    2,  "Black & White",
    3,  "Sepia",
    4,  "Red",
    5,  "Green",
    6,  "Blue",
    7,  "Yellow",
    8,  "Pink",
    9,  "Purple",
    0,  NULL
};

_PLExifTranslator CasioMeter[] = {
    2,   "Centre",
    3,   "Spot",
    5,   "Multi",
    0,  NULL
};

_PLExifTranslator CasioEVShift[] = {
    -6,  "-2.0 EV",
    -5,  "-1.67 EV",
    -4,  "-1.33 EV",
    -3,  "-1.0 EV",
    -2,  "-0.67 EV",
    -1,  "-0.33 EV",
    0,   "0 EV",
    1,   "+0.33 EV",
    2,   "+0.67 EV",
    3,   "+1.0 EV",
    4,   "+1.33 EV",
    5,   "+1.67 EV",
    6,   "+2.0 EV",
    0,  NULL
};

_PLExifTranslator CasioAperture[] = {
    1,   "f2.0",
    2,   "f2.3",
    3,   "f2.8",
    4,   "f4.0",
    5,   "f5.6",
    6,   "f8.0",
    15,  "f8.0",
    0,  NULL
};

_PLExifTranslator CasioRecord2[] = {
    0,   "Normal",
    1,   "Portrait",
    2,   "Landscape",
    3,   "Night Scene",
    4,   "Slow Shutter",
    0,  NULL
};

_PLExifTranslator CasioComp[] = {
    1,  "Off",
    2,  "Scene S1",
    3,  "Scene S2",
    4,  "Scene S3",
    5,  "Scene S4",
    6,  "Scene S5",
    7,  "Scene S8",
    8,  "Scene S6",
    9,  "Scene S7",
    0,  NULL
};

_PLExifTranslator CasioExp[] = {
    1,  "Manual",
    2,  "Program AE",
    3,  "Aperture Priority",
    4,  "Shutter Priority",
    0,  NULL
};




// Units
_PLExifTranslator UnitsSec[] = { VALUE_UNIT, "s" };
_PLExifTranslator UnitsM[] =   { VALUE_UNIT, "m" };
_PLExifTranslator UnitsMM[] =  { VALUE_UNIT, "mm" };
_PLExifTranslator UnitsX[] =   { VALUE_UNIT, "x" };

_PLExifTranslator LowerStr[] = { VALUE_LOWER_STR, NULL };


PLUINT rgMask[] = { 0, 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFF };

static string Nullstring;


// Internal EXIF exception

struct PLExifException
{
  PLExifException(const string & m) : m_message(m) {}
  string m_message;
};

struct PLExifFormatException : public PLExifException
{
  PLExifFormatException(const string & m) : PLExifException(m) {}
};

struct PLExifTagNoException : public PLExifException
{
  PLExifTagNoException(const string & m) : PLExifException(m) {}
};

struct PLExifNoCompException : public PLExifException
{
  PLExifNoCompException(const string & m) : PLExifException(m) {}
};


} // end of anonymous namespace (see comment at top)


// tag arrays contain details of all known tags
// declared as static members in PLExifTag so they have access
// to the private conversion functions declared in PLExifTag

_PLExifTagValues   PLExifTag::MainTags[] = {          // Prefix Main
    0x010E, "Desc",         NULL,       NULL,     "Image Description",
    0x010F, "Make",         NULL,       NULL,     "Camera Make",
    0x0110, "Model",        NULL,       NULL,     "Camera Model",
    0x0112, "Ori",          NULL,       NULL,     "Orientation",
    0x011A, "XRes",         NULL,       NULL,     "X Resolution",
    0x011B, "YRes",         NULL,       NULL,     "Y Resolution",
    0x0128, "ResUnit",      ResUnit,    NULL,     "Resolution Unit",
    0x0131, "Software",     NULL,       NULL,     "Camera Software",
    0x0132, "ModTime",      NULL,       NULL,     "Last Modification",
    0x013E, "WPoint",       NULL,       NULL,     "White Point",
    0x013F, "PrimChr",      NULL,       NULL,     "Primary Chromaticities",
    0x0211, "YCbCrCoef",    NULL,       NULL,     "YCbCrCoefficients",
    0x0213, "YCbCrPos",     NULL,       NULL,     "YCbCrPositioning",
    0x0214, "RefBW",        NULL,       NULL,     "Reference Black/White point",
    0x8298, "Copy",         NULL,       NULL,     "Copyright",
    0x8769, "ExifOffset",   NULL,       NULL,     "Sub IFD Offset",
    0, NULL, NULL, NULL, NULL
};


_PLExifTagValues   PLExifTag::SubTags[] = {        // Prefix Sub
    0x829A, "Shutter",      UnitsSec,   &PLExifTag::CnvFrac,    "Exposure Time",
    0x829D, "FStop",        NULL,       &PLExifTag::CnvRatAp,   "F-Stop",
    0x8822, "Prog",         ExpProg,    NULL,                   "Program",
    0x8827, "Iso",          NULL,       NULL,                   "Equivalent ISO speed",
    0x9000, "ExifVer",      NULL,       NULL,                   "Exif Version",
    0x9003, "OrigTime",     NULL,       NULL,                   "Original Time",
    0x9004, "DigTime",      NULL,       NULL,                   "Digitized Time",
    0x9101, "CompConfig",   NULL,       &PLExifTag::CnvCompCfg, "Components Configuration",
    0x9102, "Bpp",          BpsCompress,NULL,                   "Average compression ratio",
    0x9201, "Shuttera",     NULL,       &PLExifTag::CnvApexShutter, "Shutter Speed",
    0x9202, "Aperturea",    NULL,       &PLExifTag::CnvApexAp,   "Aperture",
    0x9203, "Brighta",      NULL,       &PLExifTag::CnvRat,      "Brightness APEX",
    0x9204, "Expbiasa",     NULL,       &PLExifTag::CnvRat,      "Exposure Bias APEX",
    0x9205, "Maxapa",       NULL,       &PLExifTag::CnvApexAp,   "Maximum Aperture APEX",
    0x9206, "Dist",         UnitsM,     &PLExifTag::CnvRat,      "Subject Distance",
    0x9207, "Meter",        MeterMode,  NULL,                    "Metering Mode",
    0x9208, "Lights",       LightSource,NULL,                    "Light Source",
    0x9209, "Flash",        FlashUsed,  NULL,                    "Flash Used",
    0x920a, "Focal",        UnitsMM,    &PLExifTag::CnvRat,      "Focal Length",
    0x927c, "Maker",        NULL,       NULL,                    "Maker Note",
    0x9286, "User",         NULL,       NULL,                    "User Comment",
    0x9290, "STime",        NULL,       NULL,                    "Subsecond Time",
    0x9291, "SOrigTime",    NULL,       NULL,                    "Subsecond Original Time",
    0x9292, "SDigTime",     NULL,       NULL,                    "Subsecond Digitized Time",
    0xA000, "Flashpix",     NULL,       NULL,                    "Flash Pix Version",
    0xA001, "ColorSpace",   ColorSpace, NULL,                    "Color Space",
    0xA002, "Width",        NULL,       NULL,                    "Image Width",
    0xA003, "Height",       NULL,       NULL,                    "Image Height",
    0xA004, "SndFile",      NULL,       NULL,                    "Sound File",
    0xA005, "ExifIntOff",   NULL,       NULL,                    "Exif Interoperability Offset",
    0xA20E, "FPXRes",       NULL,       &PLExifTag::CnvRat,      "Focal Plane X Resolution",
    0xA20F, "FPYRes",       NULL,       &PLExifTag::CnvRat,      "Focal Plane Y Resolution",
    0xA210, "FPResUnit",    FPResUnit,  NULL,                    "Focal Plane Unit",
    0xA215, "ExpIndex",     NULL,       &PLExifTag::CnvRat,      "Exposure Index",
    0xA217, "SenseMethod",  SenseMethod,NULL,                    "Sensing Method",
    0xA300, "FileSource",   FileSource, NULL,                    "File Source",
    0xA301, "SceneType",    SceneType,  NULL,                    "Scene Type",
    0xA302, "CFAPat",       NULL,       NULL,                    "CFA Pattern",
    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::NikonTags[] = {         // Prefix is Nikon
    0x0002, "ISO",          NULL,       NULL,               "Nikon ISO Setting",
    0x0003, "Color",        LowerStr,   NULL,               "Nikon Color Mode",
    0x0004, "Quality",      LowerStr,   NULL,               "Nikon Quality",
    0x0005, "WhiteBal",     LowerStr,   NULL,               "Nikon White Balance",
    0x0006, "Sharp",        LowerStr,   NULL,               "Nikon Image Sharpening",
    0x0007, "Focus",        LowerStr,   NULL,               "Nikon Focus Mode",
    0x0008, "Flash",        LowerStr,   NULL,               "Nikon Flash",
    0x0009, "FlashMode",    LowerStr,   NULL,               "Nikon Flash Mode",
    0x000F, "ISOSel",       LowerStr,   NULL,               "Nikon ISO Selection",
    0x0080, "ImgAdjust",    LowerStr,   NULL,               "Nikon Image Adjustment",
    0x0082, "Adapter",      LowerStr,   NULL,               "Nikon Adapter Setting",
    0x0084, "Lens",         LowerStr,   NULL,               "Nikon Lens",
    0x0085, "ManFocus",     UnitsM,     &PLExifTag::CnvRat, "Nikon Manual Focus Distance",
    0x0086, "DigZoom",      UnitsX,     &PLExifTag::CnvRat, "Nikon Digital Zoom",
    0x0088, "AFPos",        NULL,       NULL,               "Nikon Auto Focus Position",
    //0x0090, "FlashType?",   LowerStr,   "Nikon Flash Type",
    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::Nikon2Tags[] = {        // Prefix is Nikon2
    0x0002, "Unknown0002",  NULL,       NULL,               "Nikon Unknown (0002)",
    0x0003, "Quality",      N2Quality,  NULL,               "Nikon Quality",
    0x0004, "Color",        N2Color,    NULL,               "Nikon Color Mode",
    0x0005, "ImgAdjust",    N2ImgAdjust,NULL,               "Nikon Image Adjustment",
    0x0006, "CCDSens" ,     N2Iso,      NULL,               "Nikon ISO Setting",
    0x0007, "WhiteBal",     N2WhiteBal, NULL,               "Nikon White Balance",
    0x0008, "Focus",        NULL,       &PLExifTag::CnvRat, "Nikon Focus",
    0x0009, "Unknown0009",  NULL,       NULL,               "Nikon Unknown (0009)",
    0x000A, "DigZoom",      UnitsX,     &PLExifTag::CnvRat, "Nikon Digital Zoom",
    0x000B, "Converter",    N2Converter,NULL,               "Nikon Converter",
    0x0F00, "Unknown0F00",  NULL,       NULL,               "Nikon Unknown (0F00)",
    0, NULL, NULL, NULL, NULL
};


_PLExifTagValues   PLExifTag::OlympusTags[] = {       // Prefix is Oly
    0x0200, "SpcMode",      NULL,       NULL,     "Olympus Special Mode",
    0x0201, "Quality",      OlyJpegQ,   NULL,     "Olympus JPG Quality",
    0x0202, "Macro",        OlyMacro,   NULL,     "Olympus Macro",
    0x0204, "DigZoom",      UnitsX,     &PLExifTag::CnvRat,   "Olympus Digital Zoom",
    0x0207, "Software",     NULL,       NULL,     "Olympus Software",
    0x0209, "CameraID",     NULL,       NULL,     "Olympus Camera ID",

    // E-10 fields (from dougho@niceties.com)
    0x1004, "Flash",        OlyFlash,   NULL,     "Olympus Flash Mode",
    0x100F, "Sharp",        OlySharp,   NULL,     "Olympus Sharpness Mode",
    0x102a, "SharpScale",   NULL,       NULL,     "Olympus Sharpness",

    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::CanonTags[] = {         // Prefix is Canon
    0x0001, "CnSet1",       NULL,       NULL,                 "Canon Settings 1",
    0x0004, "CnSet2",       NULL,       NULL,                 "Canon Settings 2",
    0x0006, "ImageType",    NULL,       NULL,                 "Canon Image Type",
    0x0007, "Software",     NULL,       NULL,                 "Canon Firmware Version",
    0x0008, "ImageNo",      NULL,       &PLExifTag::CnvCanINo,"Canon Image Number",
    0x0009, "Owner",        NULL,       NULL,                 "Canon Owner Name",
    0x000C, "SerialNo",     NULL,       &PLExifTag::CnvCanSNo,"Canon Serial Number",
    0x000F, "CustomFnc",    NULL,       NULL,                 "Canon Custom Functions",
    0, NULL, NULL, NULL, NULL
};


_PLExifTagValues   PLExifTag::CanonSet1[] = {
    1,      "Macro",        CanMacro,       NULL,                    "Canon Macro Mode",
    4,      "Flash",        CanFlashMode,   NULL,                    "Canon Flash Mode",
    5,      "Drive",        CanDrvMode,     NULL,                    "Canon Drive Mode",
    7,      "Focus",        CanFocusMode,   NULL,                    "Canon Focus Mode",
    10,     "ImgSize",      CanImgSize,     NULL,                    "Canon Image Size",
    11,     "Shoot",        CanShootMode,   NULL,                    "Canon Easy Shooting Mode",
    13,     "Contrast",     CanLNH,         NULL,                    "Canon Contrast Setting",
    14,     "Saturation",   CanLNH,         NULL,                    "Canon Saturation Setting",
    15,     "Sharpness",    CanLNH,         NULL,                    "Canon Sharpness Setting",
    16,     "ISO",          CanIso,         NULL,                    "Canon ISO",
    17,     "Metering",     CanMeter,       NULL,                    "Canon Metering mode",
    19,     "AFPoint",      CanAFPoint,     NULL,                    "Canon AutoFocus Point",
    20,     "ExpMode",      NULL,           NULL,                    "Canon Exposure Mode",
    23,     "LongFocal",    NULL,           NULL,                    "Canon Long Focal Length",
    24,     "ShortFocal",   NULL,           NULL,                    "Canon Short Focal Length",
    25,     "FocalUnits",   NULL,           NULL,                    "Canon Focal Units per mm",
    29,     "FlashDet",     NULL,           &PLExifTag::CnvCanFlash, "Canon Flash Details",
    32,     "FocusMode",    NULL,           NULL,                    "Canon Focus Mode",
    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::CanonSet2[] = {
    7,      "WhiteBal",     CanWhiteBal,    NULL,                    "Canon White Balance",
    9,      "SeqNo",        NULL,           NULL,                    "Canon Continuous Frame",
    14,     "AF Point",     NULL,           &PLExifTag::CnvCanAFPnt, "Canon Focus Point",
    15,     "FlashBias",    CanFlashBias,   NULL,                    "Canon Flash Bias",
    19,     "SubjectDist",  NULL,           NULL,                    "Canon Subject Distance (0.01m or 0.001m)",
    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::CanonCFn[] = {
    0x01,     "NoiseRed",     OffOn,          NULL,     "Canon CFn 01 Long exp noise reduction",
    0x02,     "Lock",         CFnLock,        NULL,     "Canon CFn 02 Shutter/AE lock buttons",
    0x03,     "MirrorLock",   DisableEnable,  NULL,     "Canon CFn 03 Mirror lockup   ",
    0x04,     "TVAVexp",      CFnTVAV,        NULL,     "Canon CFn 04 Tv/Av & exposure level",
    0x05,     "AFassist",     CFnAFAssist,    NULL,     "Canon CFn 05 AF-assist light",
    0x06,     "ShuttAv",      CFnShutAv,      NULL,     "Canon CFn 06 Shutter speed in Av mode",
    0x07,     "AEBSeq",       CFnAeb,         NULL,     "Canon CFn 07 AEB Sequence/auto cancel",
    0x08,     "ShuttSync",    CFnShutSync,    NULL,     "Canon CFn 08 Shutter curtain sync",
    0x09,     "AFStop",       CFnAFStop,      NULL,     "Canon CFn 09 Lens AF stop button switch",
    0x0A,     "FillReduce",   EnableDisable,  NULL,     "Canon CFn 10 Fill flash auto reduction",
    0x0B,     "MenuButt",     CFnMenu,        NULL,     "Canon CFn 11 Menu button return pos",
    0x0C,     "SetButt",      CFnSet,         NULL,     "Canon CFn 12 SET button when shooting",
    0x0D,     "SensClean",    DisableEnable,  NULL,     "Canon CFn 13 Sensor cleaning   ",
    0, NULL, NULL, NULL, NULL
};

_PLExifTagValues   PLExifTag::FujifilmTags[] = {         // Prefix is Fuji
    0x0000, "Version",      NULL,           NULL,               "Fuji Version",
    0x1000, "Quality",      LowerStr,       NULL,               "Fuji Quality",
    0x1001, "Sharpness",    FujiSharp,      NULL,               "Fuji Sharpness",
    0x1002, "WhiteBal",     FujiWBal,       NULL,               "Fuji White Balance",
    0x1003, "Color",        FujiColor,      NULL,               "Fuji Color",
    0x1004, "Contrast",     FujiTone,       NULL,               "Fuji Tone",
    0x1010, "FlashMode",    FujiFlashMode,  NULL,               "Fuji Flash Mode",
    0x1011, "FlashStrg",    NULL,           &PLExifTag::CnvRat, "Fuji Flash Strength APEX",
    0x1020, "Macro",        OffOn,          NULL,               "Fuji Macro Mode",
    0x1021, "Focus",        FujiFocus,      NULL,               "Fuji Focus Mode",
    0x1022, "Unknown1022",  NULL,           NULL,               "Fuji Unknown (1022)",
    0x1023, "Unknown1023",  NULL,           NULL,               "Fuji Unknown (1023)",
    0x1030, "SlowSnyc",     OffOn,          NULL,               "Fuji Slow Sync",
    0x1031, "Picture",      FujiPicture,    NULL,               "Fuji Exposure Mode",
    0x1032, "Unknown1032",  NULL,           NULL,               "Fuji Unknown (1032)",
    0x1100, "Sequence",     OffOn,          NULL,               "Fuji Sequence Mode",
    0x1101, "Unknown1101",  NULL,           NULL,               "Fuji Unknown (1101)",
    0x1200, "Unknown1200",  NULL,           NULL,               "Fuji Unknown (1200)",
    0x1300, "BlurWarn",     FujiBlurFocus,  NULL,               "Fuji Blur Warning",
    0x1301, "FocusWarn",    FujiBlurFocus,  NULL,               "Fuji Focus Warning",
    0x1302, "AEWarn",       FujiAE,         NULL,               "Fuji Exposure Warning",
    0, NULL, NULL, NULL, NULL
};


_PLExifTagValues   PLExifTag::CasioTags[] = {         // Prefix is Casio
    0x0001, "RecordMode",   CasioRecord,    NULL,     "Casio Recording Mode",
    0x0002, "Quality",      CasioQuality,   NULL,     "Casio Quality  ",
    0x0003, "FocusMode",    CasioFocus,     NULL,     "Casio Focusing Mode",
    0x0004, "FlashMode",    CasioFlashMode, NULL,     "Casio Flash Mode",
    0x0005, "FlasIntense",  CasioFlashInt,  NULL,     "Casio Flash Intensity",
    0x0006, "ObjDist",      UnitsMM,        NULL,     "Casio Object Distance",
    0x0007, "WhiteBal",     CasioWhite,     NULL,     "Casio White Balance",
    0x0008, "Unknown0008",  NULL,           NULL,     "Casio Unknown (0008)",
    0x0009, "Unknown0009",  NULL,           NULL,     "Casio Unknown (0009)",
    0x000A, "DigZoom",      CasioZoom,      NULL,     "Casio Digital Zoom",
    0x000B, "Sharpness",    CasioSharp,     NULL,     "Casio Sharpness",
    0x000C, "Contrast",     CasioLowHi,     NULL,     "Casio Contrast ",
    0x000D, "Saturation",   CasioLowHi,     NULL,     "Casio Saturation",
    0x000E, "Unknown000E",  NULL,           NULL,     "Casio Unknown (000E)",
    0x000F, "Unknown000F",  NULL,           NULL,     "Casio Unknown (000F)",
    0x0010, "Unknown0010",  NULL,           NULL,     "Casio Unknown (0010)",
    0x0011, "Unknown0011",  NULL,           NULL,     "Casio Unknown (0011)",
    0x0012, "Unknown0012",  NULL,           NULL,     "Casio Unknown (0012)",
    0x0013, "Unknown0013",  NULL,           NULL,     "Casio Unknown (0013)",
    0x0014, "CCDSense",     FujiAE,         NULL,     "Casio CCD Sensitivity",
    0x0015, "Unknown0015",  NULL,           NULL,     "Casio Unknown (0015)",
    0x0016, "Enhancement",  CasioEnhance,   NULL,     "Casio Enhancement",
    0x0017, "Filter",       CasioFilter,    NULL,     "Casio Filter   ",
    0x0018, "FocFrmLoc",    CasioFFrmLoc,   NULL,     "Casio Focus Frame Locator",
    0x0019, "CCDSense",     CasioSens,      NULL,     "Casio CCD Sensitivity",
    0x001A, "Unknown001A",  NULL,           NULL,     "Casio Unknown (001A)",
    0x0100, "MeterMode",    CasioMeter,     NULL,     "Casio Metering Mode",
    0x0101, "EVShift",      CasioEVShift,   NULL,     "Casio EV Shift",
    0x0102, "Unknown0102",  NULL,           NULL,     "Casio Unknown (0102)",
    0x0103, "Aperture",     CasioAperture,  NULL,     "Casio Apertue",
    0x0104, "RecordMode2",  CasioRecord2,   NULL,     "Casio Recording Mode",
    0x0105, "Composition",  CasioComp,      NULL,     "Casio Composition Frame",
    0x0106, "ExpMode",      CasioExp,       NULL,     "Casio Exposure Mode",
    0x0107, "Unknown0107",  NULL,           NULL,     "Casio Unknown (0107)",
    0, NULL, NULL, NULL, NULL
};






//
// Format types
//

static void Swiz16(PLBYTE *);
static void Swiz32(PLBYTE *);
static void SwizR64(PLBYTE *);


_PLExifFormatter PLExifTag::rgExifFormat[] = {
    1,  NULL,   &PLExifTag::RenUndef,  // 0. undefined
    1,  NULL,   &PLExifTag::RenUDef,   // 1. unsigned byte
    1,  NULL,   &PLExifTag::RenStr,    // 2. ascii string
    2,  Swiz16, &PLExifTag::RenUDef,   // 3. unsigned short
    4,  Swiz32, &PLExifTag::RenUDef,   // 4. unsigned long
    8,  SwizR64,&PLExifTag::RenURat,   // 5. unsigned rational
    1,  NULL,   &PLExifTag::RenDef,    // 6. signed byte
    1,  NULL,   &PLExifTag::RenUndef,  // 7. undefined
    2,  Swiz16, &PLExifTag::RenDef,    // 8. signed short
    4,  Swiz32, &PLExifTag::RenDef,    // 9. signed long
    8,  SwizR64,&PLExifTag::RenRat,    // 10. signed rational
    4,  NULL,   &PLExifTag::RenUndef,  // 11. signed float (not used)
    8,  NULL,   &PLExifTag::RenUndef,  // 12. signed double (not used)
};


// Swizzle functions to deal with different endian formats

static void Swiz16(PLBYTE *Buffer)
{
    PLBYTE       l0;

    l0 = Buffer[0];
    Buffer[0] = Buffer[1];
    Buffer[1] = l0;
}

static void Swiz32(PLBYTE *Buffer)
{
    PLBYTE       l0, l1, l2;

    l0 = Buffer[0];
    l1 = Buffer[1];
    l2 = Buffer[2];

    Buffer[0] = Buffer[3];
    Buffer[1] = l2;
    Buffer[2] = l1;
    Buffer[3] = l0;
}

static void SwizR64(PLBYTE *Buffer)
// Rationals are 2 32bit values in a row
{
    Swiz32(Buffer);
    Swiz32(Buffer + 4);
}


// simple utility functions

static void MakeLower(string & s)
{
  for (string::iterator iter = s.begin(); iter != s.end(); ++iter)
    *iter = tolower(*iter);
}

static void TrimRight(string & s, char c)
{
  for (string::reverse_iterator iter = s.rbegin(); iter != s.rend() && *iter == c; )
    s.erase((++iter).base());
}

static void TrimLeft(string & s, char c)
{
  for (string::iterator iter = s.begin(); iter != s.end() && *iter == c; )
    s.erase(iter++);
}

static double round(double value, unsigned int precision)
{
  double factor = 1.0f;
  for (unsigned int i = 0; i < precision; ++i)
    factor *= 10.0f;
  value = value * factor;
  value += 0.5f;
  value = floor(value) / factor;
  return value;
}

/***************************************************************
 *
 *  PLExif implementation
 *
 ***************************************************************/

PLExif::PLExif()
  : m_Pos(0)
  , m_Endian(false)
  , m_IdfOffset(0)
{
}


PLExif::~PLExif()
{
  Clear();
}

void PLExif::Clear()
{
  m_Data = PLByteCPtr(0);
  m_DataSize = 0;
  // Free any tags we may have allocated
  while (!m_AllTags.empty())
      m_AllTags.pop_back();
  m_Tags.clear();

  // Re-Initialize values
  m_IdfOffset = 0;
  m_Pos = 0;
  m_Endian = false;
}

void PLExif::ReadData(const jpeg_decompress_struct * pcinfo)
{
  Clear();

  jpeg_saved_marker_ptr marker;
  for (marker = pcinfo->marker_list; marker != NULL; marker = marker->next)
  {
    if (marker->marker == JPEG_APP0 + 1)  // EXIF marker
    {
      m_DataSize = marker->data_length;
      m_Data = PLByteCPtr(new PLBYTE[m_DataSize]);
      PLBYTE * pSrc = marker->data;
      PLBYTE * pEnd = pSrc + marker->data_length;
      PLBYTE * pDst = m_Data.get();
      while (pSrc != pEnd)                // copy the data
        *pDst++ = *pSrc++;
    }
/*  code removed should probably be in PLBmpInfo
    else if (marker->marker == JPEG_COM)
    {
      m_Comment.reserve(marker->data_length);
      PLBYTE * pSrc = marker->data;
      PLBYTE * pEnd = pSrc + marker->data_length;
      while (pSrc != pEnd)                // copy the data
        m_Comment += (char)*pSrc++;
    }
*/
  }
  if (m_DataSize) // else there's no data to decode
  {
    // verify this is an Exif file
    PLBYTE  text[6];
    Read(&text, 6);
    if (memcmp(text, "Exif\0\0", 6) != 0) {
      Clear();
      return;
    }
    // Determine the endian type
    PLWORD type;
    Read((PLBYTE *)&type, sizeof(type));
    if (type == 256*'I'+'I')
#ifdef WORDS_BIGENDIAN
      m_Endian = TRUE;
#else
      m_Endian = FALSE;
#endif
    else if (type == 256*'M'+'M')
#ifdef WORDS_BIGENDIAN
      m_Endian = FALSE;
#else
      m_Endian = TRUE;
#endif
    else
      PLASSERT (FALSE); // Illegal data in file - should throw an exception.      
    // Check for valid len
    PLWORD Len = GetU16();
    if (Len != 0x2a)
      PLASSERT(false); // should maybe replace this with an exception later

    // all seems ok so lets decode
    decode();
  }
}

void PLExif::WriteData(jpeg_compress_struct * pcinfo)
{
  // this code copied from transupp.c from the JPEG library utilities
#ifdef NEED_FAR_POINTERS
  /* We could use jpeg_write_marker if the data weren't FAR... */
  unsigned int i;
  jpeg_write_m_header(pcinfo, JPEG_APP0 + 1, m_Data.size());
  for (i = 0; i < m_Data.size(); ++i)
    jpeg_write_m_byte(pcinfo, m_Data[i]);
/*  code removed should probably be in PLBmpInfo
  jpeg_write_m_header(pcinfo, JPEG_COM, m_Comment.size());
  for (i = 0; i < m_Comment.size(); ++i)
    jpeg_write_m_byte(pcinfo, m_Comment[i]);
*/
#else
  jpeg_write_marker(pcinfo, JPEG_APP0 + 1, m_Data.get(), m_DataSize);
/*  code removed should probably be in PLBmpInfo
  jpeg_write_marker(pcinfo, JPEG_COM, (PLBYTE *)&m_Comment[0], m_Comment.size());
*/
#endif
}


/*  code removed should probably be in PLBmpInfo
const string & PLExif::GetComment() const
{
  return m_Comment;
}

const char * PLExif::GetCommmentCStr() const
{
  return m_Comment.c_str();
}

void PLExif::SetComment(const std::string & s)
{
  m_Comment = s;
}

void PLExif::SetComment(const char * s)
{
  m_Comment = s;
}
*/

PLBYTE * PLExif::GetRawData()
{
  return m_Data.get();
}

const PLBYTE * PLExif::GetRawData() const
{
  return m_Data.get();
}

size_t PLExif::GetRawDataSize() const
{
  return m_DataSize;
}

const PLExifTagList & PLExif::GetAllTags() const
{
  return m_AllTags;
}

const PLExifTagList & PLExif::GetMainTags() const
{
  return m_MainTags;
}

const PLExifTagList & PLExif::GetSubTags() const
{
  return m_SubTags;
}

const PLExifTagList & PLExif::GetManufacturerTags() const
{
  return m_ManufacturerTags;
}

const PLExifTagCPtr * PLExif::GetAllTagsC(size_t & size) const
{
  size = m_AllTags.size();
  return &m_AllTags[0];
}

const PLExifTagCPtr * PLExif::GetMainTagsC(size_t & size) const
{
  size = m_AllTags.size();
  return &m_MainTags[0];
}

const PLExifTagCPtr * PLExif::GetSubTagsC(size_t & size) const
{
  size = m_AllTags.size();
  return &m_SubTags[0];
}

const PLExifTagCPtr * PLExif::GetManufacturerTagsC(size_t & size) const
{
  size = m_AllTags.size();
  return &m_ManufacturerTags[0];
}

PLExifTag * PLExif::GetTag(const char * TagName) const
{
  string     Str = TagName;

  MakeLower(Str);

  TagMap::const_iterator iter = m_Tags.find(Str);
  return iter != m_Tags.end() ? iter->second.get() : NULL;
}

PLExifTag * PLExif::GetTag(const char * TagName, string & Value) const
{
  PLExifTag *   Tag = GetTag(TagName);
  Value.erase();
  if (Tag)
    Value = Tag->m_Value;
  return Tag;
}

PLExifTag * PLExif::GetTagCommon(const char * TagName, std::string & Value) const
{
  PLExifTag *   Tag = GetTag(TagName);
  Value.erase();
  if (Tag)
    Value = Tag->m_Common;
  return Tag;
}

PLExifTag * PLExif::GetTag(const char * TagName, double & Value) const
{
  PLExifTag *   Tag = GetTag(TagName);
  Value = 0.0;
  if (Tag)
    Value = Tag->m_Double;
  return Tag;
}

const char * PLExif::TagCStr(const char * TagName) const
{
  PLExifTag *    Tag = GetTag(TagName);
  return Tag ? Tag->m_Value.c_str() : Nullstring.c_str();
}

const char * PLExif::TagCStrCommon(const char * TagName) const
{
  PLExifTag *    Tag = GetTag(TagName);
  return Tag ? Tag->m_Common.c_str() : Nullstring.c_str();
}


string & PLExif::TagStr(const char * TagName) const
{
  PLExifTag *    Tag = GetTag(TagName);
  return Tag ? Tag->m_Value : Nullstring;
}

string & PLExif::TagStrCommon(const char * TagName) const
{
  PLExifTag *    Tag = GetTag(TagName);
  return Tag ? Tag->m_Common : Nullstring;
}



//
// private interface
//
// main EXIF decoding routine
//

void PLExif::decode()
{
  try
  {
    PLExifTag *   Tag;
    // crack the main Exif directory
    //
    m_IdfOffset = m_IdfOffset + 6; // 0xC;
    GetU32();       // junk
    ReadIFD(PLExifTag::MainTags, "Main.", m_MainTags);

    //
    // Crack the sub-Exif directory
    //
    Tag = GetTag("Main.ExifOffset");
    if (Tag)
    {
      SetPos(Tag->m_UInt);
      ReadIFD(PLExifTag::SubTags, "Sub.", m_SubTags);
    }

    //
    // Crack camera specific information
    //
    string Str;
    GetTag("Main.Make", Str);
    Tag = GetTag("Sub.Maker");
    MakeLower(Str);

    try     // still sometimes get make notes that make no sense so just ignore the exception
    {       // and at least we have the main and sub tags
      if (Tag)
      {
        if (Str.find("nikon") != string::npos)          // Nikon
        {
          SetPos(Tag->m_Pos);
          char name[6];
          Read((PLBYTE *)name, 5);
          name[5] = '\0';
          if (strcmp(name, "Nikon"))  // if not starting with string Nikon
          {       // nikon format 1
            SetPos(Tag->m_Pos);     // reset to the start
            ReadIFD(PLExifTag::NikonTags, "Nikon.", m_ManufacturerTags);
  /*
            GetTag("Main.Model", Str);
            if (Str == "E990" || Str == "E880")
                m_FocalConvert = 0.209;
            if (Str.find("D1X") != string::npos)
                m_FocalConvert = 1 / 1.5;
  */
          }
          else
          {
            SetPos(Tag->m_Pos + 8);
            ReadIFD(PLExifTag::Nikon2Tags, "Nikon2.", m_ManufacturerTags);
          }
        }
        else if (Str.find("olympus") != string::npos)   // Olympus
        {
          SetPos(Tag->m_Pos + 8);
          ReadIFD(PLExifTag::OlympusTags, "Oly.", m_ManufacturerTags);
  /*
          GetTag("Main.Model", Str);
          if (Str == "E-10")
              m_FocalConvert = 0.2571;
  */
        }
        else if (Str.find("canon") != string::npos)    // Canon
        {
          SetPos(Tag->m_Pos);
          ReadIFD(PLExifTag::CanonTags, "Canon.", m_ManufacturerTags);
          ExpandBinaryTag("Canon.CnSet1", PLExifTag::CanonSet1, etUint16, m_ManufacturerTags);
          ExpandBinaryTag("Canon.CnSet2", PLExifTag::CanonSet2, etUint16, m_ManufacturerTags);
          if (PLExifTag * tagp = GetTag("Canon.CustomFnc"))
            DecodeCanCustomFncs(*tagp, PLExifTag::CanonCFn, m_ManufacturerTags);
        }
        else if (Str.find("fujifilm") != string::npos)
        {
          m_Endian = false;     // for some bizarre reason Fuji switch to Intel for their maker data
          m_IdfOffset = Tag->m_Pos + 6;    // All Fuji offsets from start of Fuji notes not EXIF
          SetPos(0x0C);
          ReadIFD(PLExifTag::FujifilmTags, "Fuji.", m_ManufacturerTags);
        }
        else if (Str.find("casio") != string::npos)          // Nikon
        {
          SetPos(Tag->m_Pos);
          ReadIFD(PLExifTag::CasioTags, "Casio.", m_ManufacturerTags);
          // Now sort the Casio Flash Mode mess
          PLExifTag * model = GetTag("Main.Model");
          PLExifTag * flash = GetTag("Casio.FlashMode");
          if (flash && model && model->m_Common == "QV-2000UX")
          {
            if (flash->m_Int == 4)
            {
              flash->m_Value = "Red Eye Reduction";
              flash->m_Common = "Red Eye Reduction";
            }
          }
        }
      }
    }
    catch(...)  // catch all and ignore (see note above on try)
    {}

    //
    // OK, we've cracked everything we know about.  Now we will
    // build our default tags
    //
    //BuildDefaultTags();

    PLExifTagList::iterator iter;
    for (iter = m_AllTags.begin(); iter != m_AllTags.end(); ++iter)
      (*iter)->CleanWorkingArea();
  }
  catch(PLExifException & p)
  {
    throw PLTextException(PL_ERRBAD_EXIF, p.m_message.c_str());
  }
}


void PLExif::CopyTag(const char * Src, const char * Dst)
// N.B. this only copies the string value of the tag
{
  PLExifTag    *Tag = GetTag(Src);
  if (Tag)
    SetTag(Dst, Tag->m_Value);
}


void PLExif::SetTag(const char * Dst, const char * Value)
{
  PLExifTag    *Tag = GetTag(Dst);
  if (!Tag)
  {
    PLExifTagCPtr Tagcp(new PLExifTag(0, 2, 0));
    Tag = Tagcp.get();

    // Add to our list of tags
    Tag->m_ShortName = Dst;
    Tag->m_Lookup = Tag->m_ShortName;
    MakeLower(Tag->m_Lookup);

    m_AllTags.push_back(Tagcp);
    m_Tags[Tag->m_Lookup] = Tagcp;
  }

  Tag->m_Fmt = &PLExifTag::rgExifFormat[2];
  Tag->m_Value = Value;
}

void PLExif::AddTag(const char * DstTag, const char * SrcTag, const char * Skip, const char * Sep)
// Add SrcTag to DstTag if SrcTag is present and is not Skip
// Put the string Sep between the tags
{
  string     Str;

  GetTag(SrcTag, Str);
  AddStr(DstTag, Str, Skip, Sep);
}

void PLExif::AddStr(const char * DstTag, const string & SrcStr, const char * Skip, const char * Sep)
{
  string     Str(SrcStr);
  string     Str2;

  Str = SrcStr;
  MakeLower(Str);
  if (!(SrcStr.empty() || (Skip && Str == Skip)))
  {
    GetTag(DstTag, Str2);
    if (!Sep)
      Sep = " / ";
    if (Str2.empty())
        Sep = "";

    Str = Str2 + Sep + SrcStr;
    SetTag(DstTag, Str);
  }
}

void PLExif::FormatRange(double Low, double High, string & Str)
{
  ostringstream oss;

  oss << ios::fixed << setprecision(1) << setw(4) << Low;
  string LStr(oss.str());
  TrimRight(LStr, '0');
  TrimRight(LStr, '.');
  TrimLeft(LStr, ' ');

  oss.str("");
  oss << setw(4) << High;
  string HStr(oss.str());
  TrimRight(HStr, '0');
  TrimRight(HStr, '.');
  TrimLeft(HStr, ' ');

  Str = LStr;
  if (LStr != HStr)
    Str = LStr + '-' + HStr;
}

void PLExif::ReadIFD(const _PLExifTagValues *Tags, char * Prefix, PLExifTagList & sectionList)
{
  size_t      NoDir, DirPos, Index;
  size_t      TagNo, Format, NoComp, Offset;

  NoDir = GetU16();
  DirPos = GetPos();

  while (NoDir)
  {
    ostringstream oss;
    //
    // Allocate a new tag
    //

    SetPos(DirPos);
    TagNo  = GetU16();
    Format = GetU16();
    NoComp = GetU32();
    if (NoComp != 0)    // otherwise no components so no data
    {
      PLExifTagCPtr Tag(new PLExifTag(TagNo, Format, NoComp));

      //
      // Create the tag's shortname (and initialize it's m_Tag)
      //

      oss << Prefix << setfill('0') << setw(4) << hex << Tag->m_TagNo;
      Tag->m_ShortName = oss.str();
      for(Index=0; Tags[Index].ShortName; Index++)
      {
        if (Tags[Index].Tag == Tag->m_TagNo)
        {
          Tag->m_Tag = &Tags[Index];
          Tag->m_ShortName = Prefix;
          Tag->m_ShortName += Tag->m_Tag->ShortName;
          break;
        }
      }

      //
      // Read the tag's data into its buffer
      //

      // If the data size is larger then 4, then we have a pointer to it
      if (Tag->m_Size > 4)
      {
        Offset = GetU32();
        SetPos(Offset);
      }

      Tag->m_Pos = GetPos();
      Read(Tag->m_Buffer.get(), Tag->m_Size);

      //
      // If we are the wrong endian, then swizzle the data buffer
      //

      if (m_Endian)
        Tag->Swizzle();

      //
      // Print the value into a string
      //

      Tag->Render();

      //
      // Add this tag to the database
      //
      Tag->m_Lookup = Tag->m_ShortName;
      MakeLower(Tag->m_Lookup);

      m_AllTags.push_back(Tag);       // add to all tags list
      m_Tags[Tag->m_Lookup] = Tag;    // add to the look up map
      sectionList.push_back(Tag);     // add to the section list
    }
    //
    // Next DirEntry
    //

    NoDir = NoDir - 1;
    DirPos = DirPos + 12;
  }
}


/*
void PLExif::BuildDefaultTags()
{
  string          Str;//, Str1, Str2, Model;
  string          Model;
  PLExifTag *     Tag;
  double          w, d1, d2;
  PLBYTE          c;
  ostringstream   oss;
  oss << fixed;         // set stream to fixed precision

  //
  // Once all the other tags are read in, we create a common set
  // of tags built up from the others.  This allows the callers to
  // read the common stuff without needing to worry about the camera
  // specific fields, but if they want they can read those as well.
  //
  //  Make        - Camera Make
  //  Model       - Camera Model
  //  Software    - Camera Software
  //  Date        - Original Photo date/time
  //  Res         - X x Y size of original
  //  Flash       - Flash used & setting
  //  Focal       - Focal length
  //  s           - Exposure time
  //  f           - Aperture f/
  //  ISO         - ISO Equiv
  //  WhiteBal    - WhiteBalance
  //  Meter       - Metering mode
  //  ExpProg     - Exposure program
  //  ExpBias     - Exposure Bias
  //  Focus       - Focus
  //  qual        - Compression quality
  //  Lens        - Info on the lens
  //  ImageAdj    - Image adjustment
  //  Sharpness   - Sharpness setting
  //
  //

  Model = TagStr("Main.Model");

  // Many tags are just copies.. so make them
  CopyTag("Main.Make",        "Make");
  CopyTag("Main.Model",       "Model");
  CopyTag("Sub.Meter",        "Meter");
  CopyTag("Sub.Prog",         "ExpProg");

  // For Date use the last found of these time fields
  CopyTag("Sub.ModTime",      "Date");
  CopyTag("Sub.OrigTime",     "Date");
  CopyTag("Sub.DigTime",      "Date");

  CopyTag("Main.Software",    "Software");
  CopyTag("Canon.Software",   "Software");
  AddTag ("Software",         "Oly.Software");

  // Get the width & height for Res
  Str = TagStr("Sub.Width") + " x " + TagStr("Sub.Height");
  SetTag("Res", Str);

  // Set the flash mode - append the camera specific setting if the flash fired
  GetTag("Sub.Flash", Str);
  SetTag("Flash", Str);
  if (Str.find("yes") != string::npos)
  {
    AddTag("Flash", "Nikon.Flash");
    AddTag("Flash", "Canon.Flash");
    AddTag("Flash", "Oly.Flash");
  }

  // The D1x doesn't set Sub.Flash
  if (Model.find("D1X") != string::npos)
  {
    AddTag("Flash", "Nikon.Flash");
    AddTag("Flash", "Nikon.FlashMode");
  }

  // CCD width
  GetTag("Sub.Width", w);
  GetTag("Sub.FPXRes", d1);
  Tag = GetTag("Sub.FPResUnit");
  d2 = 0;
  if (Tag)
  {
    switch(Tag->m_UInt)
    {
    case 1:     d2 = 25.4;      break;      // inch
    case 2:     d2 = 25.4;      break;      // inch
    case 3:     d2 = 10;        break;      // centimeter
    case 4:     d2 = 1;         break;      // milimeter
    case 5:     d2 = 0.001;     break;      // micrometer
    }
  }
  if (w && d1 && d2)
  {
    d1 = w * d2 / d1;
    oss.str("");
    oss << setprecision(2) << setw(4) << d1 << "mm";
//    Str.Format("%4.2fmm", d1);
    SetTag("CCDWidth", oss.str());

    // Set conversion
    if (!m_FocalConvert)
      m_FocalConvert = d1 / 35;
  }

  // Focal length
  Tag = GetTag("Sub.Focal");
  if (Tag)
  {
    oss.str("");
    oss << setprecision(1) << setw(4) << Tag->m_Double << "mm";
    //    Str.Format("%4.1fmm", );
    if (m_FocalConvert > 0)
    {
      oss.str("");
      oss << setprecision(1) << setw(4) << Tag->m_Double << "mm  (35mm equivalent: "
          << (PLUINT) (Tag->m_Double / m_FocalConvert + 0.5) << "mm";
      //       Str.Format("%4.1fmm  (35mm equivalent: %dmm)", );
    }
    SetTag("Focal", oss.str());
  }

  // shutter speed
  Tag = GetTag("Sub.s");
  if (Tag)
  {
    oss.str("");
    oss << setprecision(3) << setw(6) << Tag->m_Double << "fs";
    //    Str.Format("%6.3fs", );
    if (Tag->m_Double <= 0.5)
    {
      oss.str("");
      oss << setprecision(3) << setw(6) << Tag->m_Double
          << "fs (1/" << (PLUINT) (1 / Tag->m_Double + 0.5) << ')';
      //    Str.Format("%6.3fs (1/%d)", );
    }
    SetTag("s", oss.str());
  }

  // aperture
  Tag = GetTag("Sub.f");
  if (Tag)
  {
    oss.str("");
    oss << "f/" << setprecision(1) << setw(3) << Tag->m_Double;
    //      Str.Format("f/%3.1f", );
    SetTag("f", oss.str());
  }

  // ISO
  CopyTag("Sub.ISO", "ISO");
  if (!GetTag("ISO"))
  {                                   // ISO set?
    Tag = GetTag("Nikon.ISO");        // No, get the Nikon.ISO setting
    if (Tag)
    {
      oss.str("");
      oss << (int)Tag->m_Int;
      SetTag("ISO", oss.str());             // to the ISO info
    }
  }

  // Exposure Bias
  GetTag("Sub.eba", d1);
  if (d1 != 0)
  {
    c = '+';
    if (d1 < 0)
    {
      d1 = -d1;
      c = '-';
    }
    oss.str("");
    oss << c << setprecision(2) << setw(3) << d1;
    //    Str.Format("%c%3.2f", c, d1);
    SetTag("ExpBias", oss.str());
  }

  // White balance
  CopyTag("Sub.ls", "WhiteBal");
  CopyTag("Nikon.WhiteBal", "WhiteBal");

  //
  // Focus
  //

  AddTag("Focus", "Nikon.Focus");
  AddTag("Focus", "Oly.Macro", "normal");

  Tag = GetTag("Sub.Dist", d1);

  // Kodak is expressing -1 as an unsigned in the numerator. So check for it.
  if (Tag && Tag->m_Num == 0xFFFFFFFF)
    d1 = -1;

  Tag = GetTag("Nikon.ManFocus");
  if (Tag && Tag->m_Num != 0)
  {
    d1 = Tag->m_Double;
    // Nikon expresses INF as 1/0
    if (Tag->m_Num == 1 && Tag->m_Den == 0)
      d1 = -1;
  }

  oss.str("");
  if (d1 > 0)
    oss << setprecision(2) << setw(5) << d1 << "fm";
    //      Str1.Format("%5.2fm", d1);
  else if (d1 < 0)
    oss << "Infinite";
  AddStr("Focus", oss.str());

  //
  // Compression Quality
  //

  CopyTag("Sub.bpp", "Qual");
  CopyTag("Nikon.Quality", "Qual");
  CopyTag("Oly.Quality", "Qual");

  // Lens, ImageAdj, Sharpness
  Tag = GetTag("Nikon.Lens");
  if (Tag)
  {
    double      Low, High;
    string      Focal, Aperture;

    Low = Tag->GetDouble(0);
    High = Tag->GetDouble(1);
    FormatRange(Low, High, Focal);

    Low = Tag->GetDouble(2);
    High = Tag->GetDouble(3);
    FormatRange(Low, High, Aperture);

    oss.str("");
    oss << Focal << "mm f/" << Aperture;
    //    Str.Format("%smm f/%s", Focal, Aperture);
    SetTag("Lens", oss.str());
  }

  AddTag("Lens", "Nikon.Adapter",  "off");

  // ImageAdj, Sharpness
  AddTag("ImageAdj", "Nikon.ImgAdjust", "auto");

  AddTag("Sharpness","Nikon.Sharp",     "auto");
  AddTag("Sharpness","Oly.Sharp",       "normal");
}

*/

void PLExif::ExpandBinaryTag(const string & Src,
                            const _PLExifTagValues *Tags,
                            PLUINT Type,
                            PLExifTagList & sectionList)
{
  PLUINT      ElmSize, ElmFormat;
  PLExifTag * SrcTag;
  string      Prefix;
  size_t      Index;
  size_t      Pos;
  ostringstream oss;

  SrcTag = GetTag(Src);
  if (!SrcTag || SrcTag->m_Fmt != &PLExifTag::rgExifFormat[Type]) {
      return ;
  }

  // Prefix is the original prefix
  Prefix = Src;
  Pos = Prefix.find(".");
  if (Pos != string::npos)
  {
    Prefix = Prefix.substr(Pos+1);
    Prefix += '.';
  }

  // Tag should be items of all one size.  For now we are assuming they
  // are type 3 (unsigned short)

  ElmSize = PLExifTag::rgExifFormat[Type].Size;
  ElmFormat = Type;

  for (Index=0; Tags[Index].ShortName && Tags[Index].Tag < SrcTag->m_NoComp; Index++)
  {
    //
    // Allocate a tag for this item
    //

    PLExifTagCPtr Tag(new PLExifTag(Tags[Index].Tag, ElmFormat, 1));
    Tag->m_Tag = &Tags[Index];
    Tag->m_ShortName = Prefix + Tag->m_Tag->ShortName;
    memcpy (Tag->m_Buffer.get(), SrcTag->m_Buffer.get() + Tag->m_TagNo * ElmSize, ElmSize);

    //
    // If we are the wrong endian, then swizzle the data buffer
    //

    if (m_Endian) {
        Tag->Swizzle();
    }

    //
    // Print the value into a string
    //

    Tag->Render();

    //
    // Add this tag to the database
    //
    Tag->m_Lookup = Tag->m_ShortName;
    MakeLower(Tag->m_Lookup);

    m_AllTags.push_back(Tag);       // add to all tags list
    m_Tags[Tag->m_Lookup] = Tag;    // add to the look up map
    sectionList.push_back(Tag);     // add to the section list
  }
}

void PLExif::DecodeCanCustomFncs(const PLExifTag & rootTag, const _PLExifTagValues *Tags, PLExifTagList & sectionList)
{
  string Prefix("CanonCFn.");
  const unsigned short * datap = (const unsigned short *)rootTag.m_Buffer.get();
  unsigned short count = *datap++ / 2;   // step over the count
  for (unsigned short i = 0; i < count; ++i, ++datap)
  {
    if (*datap & 0xFF00)   // ignore a zero tag
    {
      ostringstream oss;

      PLExifTagCPtr Tag(new PLExifTag((*datap & 0xFF00) >> 8, 1, 0));  // use auto_ptr for exception safety

      //
      // Create the tag's shortname (and initialize it's m_Tag)
      //

      oss << Prefix << setfill('0') << setw(2) << Tag->m_TagNo;
      Tag->m_ShortName = oss.str();
      for(size_t i = 0; Tags[i].ShortName; ++i)
      {
        if (Tags[i].Tag == Tag->m_TagNo)
        {
          Tag->m_Tag = &Tags[i];
          Tag->m_ShortName = Prefix;
          Tag->m_ShortName += Tag->m_Tag->ShortName;
          break;
        }
      }

      Tag->m_Int = *datap & 0x00FF;
      Tag->m_UInt = Tag->m_Int;
      Tag->DoTranslation();

      //
      // Add this tag to the database
      //
      Tag->m_Lookup = Tag->m_ShortName;
      MakeLower(Tag->m_Lookup);

      m_AllTags.push_back(Tag);       // add to all tags list
      m_Tags[Tag->m_Lookup] = Tag;    // add to the look up map
      sectionList.push_back(Tag);     // add to the section list
    }
  }
}


  // these functions were originally designed to read from a file
  // they have been kept but are now "reading" from the vector m_Data
  // I may remove at least the first couple later

void PLExif::SetPos(size_t pos)
{
    m_Pos = pos + m_IdfOffset;
}


size_t PLExif::GetPos()
{
    return m_Pos - m_IdfOffset;
}

void PLExif::Read(void * buffer, size_t size)
{
    memcpy(buffer, m_Data.get() + m_Pos, size);
    m_Pos += size;
}


PLWORD PLExif::GetU16()
{
    PLWORD      u16;

    Read(&u16, sizeof(u16));
    if (m_Endian) {
        Swiz16((PLBYTE *) &u16);
    }

    return u16;
}


PLLONG PLExif::GetU32()
{
    PLLONG      u32;

    Read(&u32, sizeof(u32));
    if (m_Endian) {
        Swiz32((PLBYTE *) &u32);
    }

    return u32;
}


/***************************************************************
 *
 *  PLExifTag implementation
 *
 ***************************************************************/




// true public interface

// C Interface - returning const char *

const char * PLExifTag::GetShortNameCStr() const
{
  return m_ShortName.c_str();
}

const char * PLExifTag::GetDescriptionCStr() const
{
  return m_Tag ? m_Tag->Desc : Nullstring.c_str();
}

  // Format is basic as stored in the raw EXIF data
  // eg f4.0 is likely to be 40/10
const char * PLExifTag::GetValueCStr() const
{
  return m_Value.c_str();
}

  // format is common form eg f4.0 is 4.0 rather than 40/10
const char * PLExifTag::GetValueCommonCStr() const
{
  return m_Common.c_str();
}


// STL Interface - returning const string &

const string & PLExifTag::GetShortName() const
{
  return m_ShortName;
}

const string PLExifTag::GetDescription() const
{
  return m_Tag ? m_Tag->Desc : Nullstring;
}

  // Format is basic as stored in the raw EXIF data
  // eg f4.0 is likely to be 40/10
const string & PLExifTag::GetValue() const
{
  return m_Value;
}

  // format is common form eg f4.0 is 4.0 rather than 40/10
const string & PLExifTag::GetValueCommon() const
{
  return m_Common;
}


// interface only used by PLExif


PLExifTag::PLExifTag(PLUINT TagNo, PLUINT Format, PLUINT NoComp)
  : m_Tag(NULL)
  , m_Fmt(NULL)
  , m_Buffer(NULL)
  , m_Size(0)
  , m_Num(0)
  , m_Den(1)
  , m_Int(0)
  , m_UInt(0)
  , m_Double(0)
  , m_TagNo(TagNo)
  , m_Format(Format)
  , m_NoComp(NoComp)
{
    if (Format < 1 || Format > 12)
        throw PLExifException("EXIF Tag format field not understood");

    // Allocate a buffer for this tags data
  m_Fmt = &rgExifFormat[Format];
  m_Size = m_Fmt->Size * m_NoComp;
  if (m_Size > 64*1024 || m_NoComp > 64*1024)
      throw PLExifException("EXIF NoComp field not understood");

  if (m_Size)
  {
    m_Buffer = PLByteCPtr(new PLBYTE [m_Size]);
  }
}


PLExifTag::~PLExifTag()
{
  m_Buffer = PLByteCPtr(0);
}


void PLExifTag::CleanWorkingArea()
{
  m_Buffer = PLByteCPtr(0);
}

void PLExifTag::Swizzle()
// Data needs swizzled
{

  if (m_Fmt->Swizzle)
  {
    PLBYTE * Buffer = m_Buffer.get();
    for (size_t Index=0; Index < m_NoComp; ++Index)
    {
        m_Fmt->Swizzle(Buffer);
        Buffer = Buffer + m_Fmt->Size;
    }
  }
}

void PLExifTag::Render()
{
  size_t        Index, Count;
  PLBYTE *      Buffer = m_Buffer.get();

  m_Value.erase();
  Index = 0;
  Count = 0;

  for (; ;)
  {
    Index += (this->*m_Fmt->Render)(Buffer);
    if (Index >= m_NoComp)
      break;

    Count += 1;
    if (Count >= 16)
    {
      m_Value += " ...";
      break;
    }

    // Add a space between the array of values
    m_Value += " ";
  }

  // Normalize the value into different formats
  if (m_Den != 0)
  {
    m_Double = (float) m_Num / (float) m_Den;
    m_Int = (PLULONG) (m_Double + 0.5);
    m_UInt = (PLUINT) m_Int;
  }

  // set up the common string
  if (m_Tag)
  {
    if (m_Tag->Convert)
      (this->*m_Tag->Convert)(m_Common);
    else
      m_Common = m_Value;
  }

  DoTranslation();

  // Trim trailing spaces from output
  TrimRight(m_Value, ' ');
}

void PLExifTag::DoTranslation()
{
  // See if there is any translation
  if (m_Tag && m_Tag->Trans)
  {
    _PLExifTranslator     *Trans;

    Trans = m_Tag->Trans;
    if (Trans[0].Value == VALUE_UNIT)
    {
      // Translator supplies units of value
      m_Value += " ";
      m_Common += " ";
      m_Value += Trans[0].Desc;
      m_Common += Trans[0].Desc;
    }
    else if (Trans[0].Value == VALUE_LOWER_STR)
    {
      MakeLower(m_Value);
      m_Value[0] = toupper(m_Value[0]);
      MakeLower(m_Common);
      m_Common[0] = toupper(m_Common[0]);
    }
    else
    {
      // Lookup value's verbose name and use that
      for (size_t Index=0; Trans[Index].Desc; Index++)
      {
        if (Trans[Index].Value == m_UInt)
        {
          m_Value = Trans[Index].Desc;
          m_Common = Trans[Index].Desc;
          break;
        }
      }
    }
  }
}
double PLExifTag::GetDouble(size_t Index)
{
    Value(Index);
    return m_Double;
}

void PLExifTag::Value(size_t Index)
{
  string      HoldValue = m_Value;

  m_Num = 0;
  m_Den = 0;
  m_Int = 0;
  m_UInt = 0;
  m_Double = 0;

  // If Index is in range, let's get it
  if (Index <= m_NoComp)
  {
    PLBYTE *    Buffer = m_Buffer.get();
    // Render each element and stop at the one we want
    size_t Count = 0;
    while (Count <= Index)
    {
      m_Value = "";
      Count += (this->*m_Fmt->Render)(Buffer);
    }
  }

  // Normalize the value into different formats
  if (m_Den != 0)
  {
    m_Double = (double) m_Num / (double) m_Den;
    m_Int = (PLULONG) (m_Double + 0.5);
    m_UInt = (PLUINT) m_Int;
  }

  // restore original string
  m_Value = HoldValue;
}


//
// Render functions
//

size_t PLExifTag::RenUDef(PLBYTE * & Buffer)
// Default unsigned number of m_Fmt->Size
{
  PLULONG       Accum, Size;
  ostringstream oss;

  // Pull the value from the stream
  Size = m_Fmt->Size;
  memcpy (&Accum, Buffer, Size);
  Accum = Accum & rgMask[Size];

  // Append this value to the output
  oss << Accum;
  m_Value += oss.str();

  // Remember last value
  m_Num = Accum;

  // Done - we handled 1 value of this type
  Buffer += Size;
  return 1;
}


size_t PLExifTag::RenDef(PLBYTE * &Buffer)
// Default signed number of m_Fmt->Size
{
  PLULONG     Size, Mask;
  PLLONG      Accum;
  ostringstream oss;

  // Pull the value from the stream
  Size = m_Fmt->Size;
  Mask = rgMask[Size];
  memcpy (&Accum, Buffer, Size);
  Accum = Accum & Mask;

  // If the sign bit is set, sign extend the value
  if (Accum & (1 << (Size * 8 - 1)))
      Accum = Accum | (-1L ^ Mask);

  // Append this value to the output
  oss << Accum;
  m_Value += oss.str();
  m_Num = Accum;

  // Done - we handled 1 value of this type
  Buffer += Size;
  return 1;
}


size_t PLExifTag::RenRat(PLBYTE * &Buffer)
// Signed Rational
{
  PLLONG      Num, Den;
  ostringstream oss;

  memcpy (&Num, Buffer, 4);
  memcpy (&Den, Buffer+4, 4);

  // Append this value to the output
  oss << Num << '/' << Den;
  m_Value += oss.str();
  m_Num = Num;
  m_Den = Den;

  // Done - we handle 1 value of this type
  Buffer += 8;
  return 1;
}

size_t PLExifTag::RenURat(PLBYTE * &Buffer)
// Unsigned Rational
{
  PLULONG       Num, Den;
  ostringstream oss;

  memcpy (&Num, Buffer, 4);
  memcpy (&Den, Buffer+4, 4);

  // Append this value to the output
  oss << Num << '/' << Den;
  m_Value += oss.str();
  m_Num = Num;
  m_Den = Den;

  // Done - we handle 1 value of this type
  Buffer += 8;
  return 1;
}

size_t PLExifTag::RenStr(PLBYTE * &Buffer)
{
  size_t        Index;
  PLBYTE        c;
  char          Str[20];
  ostringstream oss;

  Index = 0;
  while (*Buffer && Index < m_Size)
  {
    c = *Buffer;
    if (c >= ' ' && c <= 127)
    {
      m_Value += c;
    }
    else
    {
      switch (c)
      {
      case '\n':
        strcpy(Str, "\\n");
        break;
      case '\r':
        strcpy(Str, "\\r");
        break;
      case '\t':
        strcpy(Str, "\\t");
        break;
      case '\b':
        strcpy(Str, "\\b");
        break;
      default:
        sprintf(Str, "\\0x%02x", c);
      }
      m_Value += Str;
    }

    // Next char
    Buffer += 1;
    Index += 1;
  }

  // Done - we handled the whole value
  return m_NoComp;
}

size_t PLExifTag::RenUndef(PLBYTE * &Buffer)
{
  bool      AscStr;
  size_t    Index, Size;
  PLBYTE    c;
  char      Str[20];

  AscStr = true;
  for(Index=0; Index < m_Size; Index++)
  {
    c = Buffer[Index];
    if ((c < ' '|| c > 127) && c != 0 && c != '\n' && c != '\r' && c != '\t' && c != '\b')
    {
      AscStr = false;
      break;
    }
  }

  if (AscStr)
  {
    return RenStr(Buffer);
  }
  else
  {
    // Dump the leading bytes
    Size = m_Size;
    if (Size > 16)
      Size = 16;

    m_Value += "{ ";
    for(Index=0; Index < Size; Index++)
    {
      sprintf(Str, "%02x ", Buffer[Index]);
      m_Value += Str;
    }

    if (Size != m_Size)
        m_Value += "... ";

    m_Value += "}";

    // Take the last byte as our default value
    m_Num = Buffer[m_Size-1];

    // Done - we handled the whole value
    return m_NoComp;
  }
}


// evaluate nominator/denominator
void PLExifTag::CnvRat(std::string & result)
{
  if (m_Den)
  {
    ostringstream oss;
    oss << double(m_Num) / double(m_Den);
    result = oss.str();
  }
  else
    result = m_Value;
}

// evaluate nominator/denominator for fstop
void PLExifTag::CnvRatAp(std::string & result)
{
  if (m_Den)
  {
    ostringstream oss;
    oss << 'f' << round(double(m_Num) / double(m_Den), 1);
    result = oss.str();
  }
  else
    result = m_Value;
}

// evaluate nominator/denominator if greater than 1
// and reduce to 1/x
void PLExifTag::CnvFrac(std::string & result)
{
  ostringstream oss;
  if (m_Den && m_Den <= m_Num)
  {
    oss << double(m_Num) / double(m_Den);
    result = oss.str();
  }
  else if (m_Num != 1 && m_Num != 0)
  {
    oss << "1/" << m_Den / m_Num;
    result = oss.str();
  }
  else
    result = m_Value;
}

void PLExifTag::CnvApexShutter(std::string & result)
{
  if (m_Den)
  {
    ostringstream oss;
    double d = 0.0;
    d = double(m_Num) / double(m_Den);
    d = pow(2.0, d);
    oss << 1 << '/' << round(d, 0) << " s";
    result = oss.str();
  }
  else
    result = m_Value;
}

void PLExifTag::CnvApexAp(std::string & result)
{
  if (m_Den)
  {
    ostringstream oss;
    double d = 0.0;
    d = double(m_Num) / double(m_Den);
    d = pow(1.4142, d);
    oss << 'f' << round(d, 1);
    result = oss.str();
  }
  else
    result = m_Value;
}

void PLExifTag::CnvCompCfg(std::string & result)
{
  result.erase();
  istringstream iss(m_Value.c_str());
  string val;
  iss >> val;   // reject leading {
  for (int i = 0; i < 4; ++i) // read four values
  {
    iss >> val;
    if (val == "00")
      ;   // ignore
    else if (val == "01")
      result += 'Y';
    else if (val == "02")
      result += "Cb";
    else if (val == "03")
      result += "Cr";
    else if (val == "04")
      result += 'R';
    else if (val == "05")
      result += 'G';
    else if (val == "06")
      result += 'B';
    else
    {
      result += ' ';
      result += val;
    }
  }
}

// decode canon image number
void PLExifTag::CnvCanINo(std::string & result)
{
  ostringstream oss;
  oss << setfill('0') << setw(3) << m_Int / 10000 << '-' << setw(4) << m_Int % 10000;
  result = oss.str();
}

// decode canon serial number
void PLExifTag::CnvCanSNo(std::string & result)
{
  ostringstream oss;
  unsigned short * low = (unsigned short *)&m_Int;
  unsigned short * high = low + 1;
  oss << setfill('0') << setw(4) << hex << *high;
  oss << setw(5) << dec << *low;
  result = oss.str();
}

// decode canon flash details
void PLExifTag::CnvCanFlash(std::string & result)
{
  result.erase();
  if (0x4000 & m_Int)
    result += "External E-TTL ";
  if (0x2000 & m_Int)
    result += "Internal Flash ";
  if (0x0800 & m_Int)
    result += "FP sync used ";
  if (0x0010 & m_Int)
    result += "FP sync enabled ";
}

// decode canon flash details
void PLExifTag::CnvCanAFPnt(string & result)
{
  result.erase();
  if (0xF000 & m_Int)
  {
    switch(0x0FFF & m_Int)
    {
    case 0:
      result = "Right";
      break;
    case 1:
      result = "Centre";
      break;
    case 2:
      result = "Left";
      break;
    }
  }
}

