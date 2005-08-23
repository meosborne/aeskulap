/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportLayer
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:01 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmlayer.h"
#include "dcmtrans.h"

DcmTransportLayer::~DcmTransportLayer()
{
}

DcmTransportConnection * DcmTransportLayer::createConnection(int openSocket, OFBool useSecureLayer)
{
  if (useSecureLayer) return NULL;  /* secure layer connections not supported */
  else return new DcmTCPConnection(openSocket);
}


/*
 *  $Log: dcmlayer.cc,v $
 *  Revision 1.1  2005/08/23 19:32:01  braindead
 *  - initial savannah import
 *
 *  Revision 1.1  2005/06/26 19:26:10  pipelka
 *  - added dcmtk
 *
 *  Revision 1.2  2001/06/01 15:50:05  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/08/10 14:50:56  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

