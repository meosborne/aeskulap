/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Author:  Michael Onken
 *
 *  Purpose: Commandline-Application to modify tags in DICOM-Files
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:00 $
 *  Source File:      $Source: /sources/aeskulap/aeskulap/dcmtk/dcmdata/apps/dcmodify.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"   // make sure OS specific configuration is included first
#include "mdfconen.h"

#define OFFIS_CONSOLE_APPLICATION "dcmodify"

int main(int argc, char *argv[])
{
    int error_count=0;
    MdfConsoleEngine engine(argc,argv,OFFIS_CONSOLE_APPLICATION);
    error_count=engine.startProvidingService();
    CERR << "There were " << error_count << " error(s)" << endl;
    return(error_count);
}

/*
** CVS/RCS Log:
** $Log: dcmodify.cc,v $
** Revision 1.1  2005/08/23 19:32:00  braindead
** - initial savannah import
**
** Revision 1.1  2005/06/26 19:25:57  pipelka
** - added dcmtk
**
** Revision 1.4  2003/10/13 14:52:59  onken
** error-message adapted to mdfconen.cc
**
** Revision 1.3  2003/09/19 12:47:21  onken
** return-value is now only zero, if no error occurred
**
** Revision 1.2  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
