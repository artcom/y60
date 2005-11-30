/*
/--------------------------------------------------------------------
|
|      $Id: pldicomdec.cpp,v 1.3 2004/11/09 15:55:06 artcom Exp $
|      DICOM Decoder Class
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pldicomdec.h"
#include "plexcept.h"

#include <dcfilefo.h>
#include <dcrledrg.h>
#include <djdecode.h>
#include <dcmimage.h>
#include <dcistrma.h>

#include <vector>

/*
#ifdef _WIN32
#pragma comment(lib, "ofstd.lib")
#pragma comment(lib, "dcmdata.lib")  
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "dcmimage.lib")
#pragma comment(lib, "dcmjpeg.lib")
#pragma comment(lib, "dcmimgle.lib")
#pragma comment(lib, "ijg16.lib")
#pragma comment(lib, "ijg8.lib")
#pragma comment(lib, "ijg12.lib")
#endif
*/

using namespace std;

extern const OFCondition EC_Normal;
extern const OFCondition EC_InvalidStream;

class DICOMTK {
public:
  static DICOMTK & get();
  ~DICOMTK();
private:
  DICOMTK();    
};

class PLProducer 
  : public DcmProducer 
{
public:
  PLProducer(PLDataSource * pDatasource);
  OFBool good() const;
  OFCondition status() const;  
  OFBool eos() const;
  Uint32 avail() const;
  Uint32 read(void *buf, Uint32 buflen);
  Uint32 skip(Uint32 skiplen);
  void putback(Uint32 num);

  Uint32 tell() const;
  PLDataSource * GetDataSource() const;
private:
  Uint32 m_currentPos;
  PLDataSource * m_pSrc;
  bool m_bGood;
};

class PLInputStreamFactory
  : public DcmInputStreamFactory
{
public:
  PLInputStreamFactory(PLDataSource *pSrc, Uint32 initialPos);
  DcmInputStream *create() const;    
  DcmInputStreamFactory *clone() const;
private:
  PLDataSource *m_pSrc;
  Uint32 m_initialPos;
};

class PLInputStream 
  : public DcmInputStream
{
public:
  PLInputStream(PLProducer *pProducer);
  DcmInputStreamFactory *newFactory() const;
private:
  PLProducer * m_pProducer;    
};

PLDICOMDecoder::PLDICOMDecoder
    ()
    : PLPicDecoder(), 
      m_di(0),
      m_bInvert(false)
    // Creates a pDecoder
{
  // make sure the DICOM toolkit is initialized
  DICOMTK::get();
}


PLDICOMDecoder::~PLDICOMDecoder
    ()
{
}

void PLDICOMDecoder::Open (PLDataSource * pDataSrc)
{  
  PLPixelFormat pf = PLPixelFormat::DONTCARE;

  DcmFileFormat *dfile = new DcmFileFormat(); // NOTE: this is owned
  // by the Image it is going to construct (sic!)

  m_pProducer = new PLProducer(pDataSrc);
  m_pInputStream = new PLInputStream(m_pProducer);

  OFCondition cond = dfile->read(*m_pInputStream, EXS_Unknown, 
         EGL_withoutGL, DCM_MaxReadLength);

  if (cond.bad())
  {         
      cerr<< cond.text() << endl;   
     throw PLTextException(PL_ERRFORMAT_UNKNOWN, cond.text());
  }

  E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();

  unsigned long opt_compatibilityMode = 
      CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
  
  m_di = new DicomImage(dfile, xfer, opt_compatibilityMode, 0 /*first frame*/, 0 /* all frames */);

  if (m_di->getStatus() != EIS_Normal) {
      cerr << DicomImage::getString(m_di->getStatus()) << endl;      
      throw PLTextException(PL_ERRFORMAT_UNKNOWN, m_di->getString(m_di->getStatus()));
  }

/*
    const char *SOPClassUID = NULL;
    const char *SOPInstanceUID = NULL;
    const char *SOPClassText = NULL;
*/
    /*E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();
    const char *XferText = DcmXfer(xfer).getXferName();
    */ 
/*
    dfile->getDataset()->findAndGetString(DCM_SOPClassUID, SOPClassUID);
    dfile->getDataset()->findAndGetString(DCM_SOPInstanceUID, SOPInstanceUID);

    if (SOPInstanceUID == NULL)
        SOPInstanceUID = "not present";
    if (SOPClassUID == NULL)
        SOPClassText = "not present";
    else
        SOPClassText = dcmFindNameOfUID(SOPClassUID);
    if (SOPClassText == NULL)
        SOPClassText = SOPClassUID;

    char aspectRatio[30];
    OFStandard::ftoa(aspectRatio, sizeof(aspectRatio), m_di->getHeightWidthRatio(), OFStandard::ftoa_format_f, 0, 2);

    cerr<< "transfer syntax     : " << XferText << endl
        << "SOP class           : " << SOPClassText << endl
        << "SOP instance UID    : " << SOPInstanceUID << endl << endl
        << "columns x rows      : " << m_di->getWidth() << " x " << m_di->getHeight() << endl
        << "bits per sample     : " << m_di->getDepth() << endl
        << "color model         : " << colorModel << endl;
    cerr << "pixel aspect ratio  : " << aspectRatio << endl
        << "number of frames    : " << m_di->getFrameCount() << endl << endl;
*/
  int bpp = m_di->getDepth();

bpp = 8;
  
  switch(m_di->getPhotometricInterpretation()) {    
    case EPI_Monochrome1:  // 0==white      
      m_bInvert = true;
    case EPI_Monochrome2:  // 0==black
      pf = PLPixelFormat::FromChannels("L", bpp);
      break;
    case EPI_PaletteColor:
      pf = PLPixelFormat::FromChannels("I", bpp);
      break;
    case EPI_RGB:
      pf = PLPixelFormat::FromChannels("RGB", bpp);
      break;
    case EPI_ARGB:
      pf = PLPixelFormat::FromChannels("ARGB", bpp);
      break;
    default:
      throw PLTextException(PL_ERRFORMAT_NOT_SUPPORTED, m_di->getString(m_di->getPhotometricInterpretation()));
  }
  
  SetBmpInfo (PLPoint (m_di->getWidth(), m_di->getHeight()), 
              PLPoint(0, 0), pf);
}

void PLDICOMDecoder::Close ()
{
  delete m_di;
  delete m_pInputStream;
  delete m_pProducer;

  PLPicDecoder::Close();
}


void PLDICOMDecoder::GetImage (PLBmpBase & Bmp)
{  
  PLBYTE ** pLineArray = Bmp.GetLineArray();
  unsigned long buffer_size = m_di->getOutputDataSize(8);
  vector<char> buffer(buffer_size);
  int data_size = m_di->getOutputData(
      &buffer[0],
      buffer_size, 8, 0
  );

  int y;
  int Height = GetHeight();
  int Width = GetWidth();
  for (y=0; y < Height; y++)
  {    
    memcpy(pLineArray[y], &buffer[y * Width], Width);
  }
  Bmp.SetGrayPalette ();  
}

// --
DICOMTK & DICOMTK :: get() {
  static DICOMTK theDICOMTK;
  return theDICOMTK;
}

DICOMTK :: DICOMTK() {
  DicomImageClass::setDebugLevel(
    DicomImageClass::getDebugLevel() | 
    DicomImageClass::DL_Informationals  | 
    DicomImageClass::DL_DebugMessages);
    
  // register RLE decompression codec
  DcmRLEDecoderRegistration::registerCodecs(OFFalse /*pCreateSOPInstanceUID*/, OFTrue /*debug mode*/);

  // register JPEG decompression codecs
  DJDecoderRegistration::registerCodecs(EDC_always /*always convert YUV to RGB*/, EUC_default, EPC_default, OFTrue/*debug mode*/);
}

DICOMTK :: ~DICOMTK() {
  // deregister RLE decompression codec
  DcmRLEDecoderRegistration::cleanup();

  // deregister JPEG decompression codecs
  DJDecoderRegistration::cleanup();
}

// -- PLDICOMDecoder

PLProducer :: PLProducer(PLDataSource * pDatasource)
  : m_pSrc(pDatasource),
    m_bGood(true),
    m_currentPos(0)
{}

/** returns the status of the producer. Unless the status is good,
  *  the producer will not permit any operation.
  *  @return status, true if good
  */
OFBool PLProducer :: good() const
{
  return m_bGood;
}

/** returns the status of the producer as an OFCondition object.
  *  Unless the status is good, the producer will not permit any operation.
  *  @return status, EC_Normal if good
  */
OFCondition PLProducer :: status() const
{
  return m_bGood ? EC_Normal : EC_InvalidStream;
}

/** returns true if the producer is at the end of stream.
  *  @return true if end of stream, false otherwise
  */
OFBool PLProducer :: eos() const
{
  return m_currentPos >= Uint32(m_pSrc->GetFileSize());
}

/** returns the minimum number of bytes that can be read with the
  *  next call to read(). The DcmObject read methods rely on avail
  *  to return a value > 0 if there is no I/O suspension since certain
  *  data such as tag and length are only read "en bloc", i.e. all
  *  or nothing.
  *  @return minimum of data available in producer
  */
Uint32 PLProducer :: avail() const
{
  return m_pSrc->GetFileSize() - m_currentPos;
}


/** reads as many bytes as possible into the given block.
  *  @param buf pointer to memory block, must not be NULL
  *  @param buflen length of memory block
  *  @return number of bytes actually read. 
  */
Uint32 PLProducer :: read(void *buf, Uint32 buflen)
{
  m_pSrc->Seek(m_currentPos);

  if (buflen > avail()) {
    buflen = avail();
  }
  if (buflen) {
    memcpy(buf, m_pSrc->ReadNBytes(buflen), buflen);
    m_currentPos += buflen;
  }
  return buflen;
}

/** skips over the given number of bytes (or less)
  *  @param skiplen number of bytes to skip
  *  @return number of bytes actually skipped. 
  */
Uint32 PLProducer :: skip(Uint32 skiplen)
{
  if (skiplen > avail()) {
    skiplen = avail();
  } else {
    m_currentPos += skiplen;
    m_pSrc->Seek(m_currentPos);
  }
  return skiplen;
}

/** resets the stream to the position by the given number of bytes.
  *  @param num number of bytes to putback. If the putback operation
  *    fails, the producer status becomes bad. 
  */
void PLProducer :: putback(Uint32 num)
{
  if (m_currentPos < num) {
    m_bGood = false;
  } else {
    m_currentPos -= num;
    m_pSrc->Seek(m_currentPos);
  }
}

Uint32 PLProducer :: tell() const 
{
  return m_currentPos;
}

PLDataSource * PLProducer :: GetDataSource() const {
  return m_pSrc;
}


// -- PLInputStreamFactory

PLInputStreamFactory :: PLInputStreamFactory(PLDataSource *pSrc, Uint32 initialPos) 
    : m_pSrc(pSrc),
      m_initialPos(initialPos)
{}

DcmInputStream * PLInputStreamFactory :: create() const
{
  PLProducer *pProducer = new PLProducer(m_pSrc);
  pProducer->skip(m_initialPos);
  return new PLInputStream(pProducer);
}

DcmInputStreamFactory * PLInputStreamFactory :: clone() const {
  return new PLInputStreamFactory(*this);
}

// --

PLInputStream :: PLInputStream(PLProducer *pProducer) 
  : DcmInputStream(pProducer),
    m_pProducer(pProducer)
{}

DcmInputStreamFactory * PLInputStream :: newFactory() const {
  return new PLInputStreamFactory(m_pProducer->GetDataSource(), m_pProducer->tell());
}
