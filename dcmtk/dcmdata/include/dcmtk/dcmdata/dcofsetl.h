/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: definition of class DcmOffsetList
 *
 *  Last Update:      $Author$
 *  Update Date:      $Date$
 *  Source File:      $Source$
 *  CVS/RCS Revision: $Revision$
 *  Status:           $State$
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCOFSETL_H
#define DCOFSETL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oflist.h"      /* for class OFList<T> */

/** list of Uint32 values which are interpreted as the offsets
 *  in a Pixel Data offset table
 */
typedef OFList<Uint32> DcmOffsetList;

#endif

/*
 * CVS/RCS Log
 * $Log$
 * Revision 1.1  2007/04/24 09:53:31  braindead
 * - updated DCMTK to version 3.5.4
 * - merged Gianluca's WIN32 changes
 *
 * Revision 1.1.1.1  2006/07/19 09:16:41  pipelka
 * - imported dcmtk354 sources
 *
 *
 * Revision 1.2  2005/12/08 16:28:23  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/05/24 14:51:43  meichel
 * Moved helper methods that are useful for different compression techniques
 *   from module dcmjpeg to module dcmdata
 *
 *
 */
