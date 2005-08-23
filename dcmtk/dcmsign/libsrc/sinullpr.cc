/*
 *
 *  Copyright (C) 1998-2002, OFFIS
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
 *  Module: dcmsign
 *
 *  Author:Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiNullProfile
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:08 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sinullpr.h"

OFBool SiNullProfile::isAllowableMACType(E_MACType /* macType */) const
{
  return OFTrue;
}

OFBool SiNullProfile::isAllowableAlgorithmType(E_KeyType keyType) const
{
  if (keyType == EKT_none) return OFFalse;
  return OFTrue;
}

OFBool SiNullProfile::attributeRequired(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiNullProfile::attributeForbidden(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiNullProfile::isAllowableTransferSyntax(E_TransferSyntax xfer) const
{
  if ((xfer == EXS_LittleEndianImplicit)||(xfer == EXS_BigEndianExplicit)) return OFFalse;
  return OFTrue;
}

#else /* WITH_OPENSSL */

int sinullpr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sinullpr.cc,v $
 *  Revision 1.1  2005/08/23 19:32:08  braindead
 *  - initial savannah import
 *
 *  Revision 1.1  2005/06/26 19:25:59  pipelka
 *  - added dcmtk
 *
 *  Revision 1.4  2002/12/16 12:57:52  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.3  2001/11/16 15:50:54  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.2  2001/06/01 15:50:54  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:06  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

