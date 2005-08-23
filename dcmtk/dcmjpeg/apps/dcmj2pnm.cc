/*
 *
 *  Copyright (C) 2001-2004, OFFIS
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
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: Convert DICOM Images to PPM or PGM using the dcmimage/dcmjpeg library.
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:09 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


// compile "dcm2pnm" with dcmjpeg support
#define BUILD_DCM2PNM_AS_DCMJ2PNM

// include full implementation of "dcm2pnm"
#include "../../dcmimage/apps/dcm2pnm.cc"


/*
 * CVS/RCS Log:
 * $Log: dcmj2pnm.cc,v $
 * Revision 1.1  2005/08/23 19:32:09  braindead
 * - initial savannah import
 *
 * Revision 1.1  2005/06/26 19:26:12  pipelka
 * - added dcmtk
 *
 * Revision 1.2  2004/01/16 14:28:01  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2001/11/19 13:08:37  joergr
 * Added new tool dcmj2pnm which supports all features of dcm2pnm and in
 * addition also JPEG compressed images.
 *
 *
 */
