/*
 *
 *  Copyright (C) 1997-2005, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Codec class for decoding JPEG Progressive (lossy, 8/12-bit)
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2007/04/24 09:53:26 $
 *  Source File:      $Source: /sources/aeskulap/aeskulap/dcmtk/dcmjpeg/libsrc/djdecpro.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdecpro.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djdijg8.h"
#include "dcmtk/dcmjpeg/djdijg12.h"


DJDecoderProgressive::DJDecoderProgressive()
: DJCodecDecoder()
{
}


DJDecoderProgressive::~DJDecoderProgressive()
{
}


E_TransferSyntax DJDecoderProgressive::supportedTransferSyntax() const
{
  return EXS_JPEGProcess10_12TransferSyntax;
}


DJDecoder *DJDecoderProgressive::createDecoderInstance(
    const DcmRepresentationParameter * /* toRepParam */,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    OFBool isYBR) const
{
  if (bitsPerSample > 8) return new DJDecompressIJG12Bit(*cp, isYBR);
  else return new DJDecompressIJG8Bit(*cp, isYBR);
}


/*
 * CVS/RCS Log
 * $Log: djdecpro.cc,v $
 * Revision 1.2  2007/04/24 09:53:26  braindead
 * - updated DCMTK to version 3.5.4
 * - merged Gianluca's WIN32 changes
 *
 * Revision 1.1.1.1  2006/07/19 09:16:41  pipelka
 * - imported dcmtk354 sources
 *
 *
 * Revision 1.2  2005/12/08 15:43:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2001/11/13 15:58:27  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
