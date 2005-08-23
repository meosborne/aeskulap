/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Basic work list management service class provider based on a
 *           set of files as data source.
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:09 $
 *  Source File:      $Source: /sources/aeskulap/aeskulap/dcmtk/dcmwlm/apps/wlmscpfs.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dicom.h"
#include "ofcmdln.h"
#include "wltypdef.h"
#include "dcxfer.h"
#include "ofconapp.h"
#include "wlcefs.h"
#include "wlds.h"
#include "wldsfs.h"

#define OFFIS_CONSOLE_APPLICATION "wlmscpfs"

int main( int argc, char *argv[] )
{
  // Initialize object which provides a connection to the data source
  WlmDataSourceFileSystem *dataSource = new WlmDataSourceFileSystem();

  // Initialize and provide service. After having terminated free memory.
  WlmConsoleEngineFileSystem *consoleEngine = new WlmConsoleEngineFileSystem( argc, argv, OFFIS_CONSOLE_APPLICATION, dataSource );
  int result = consoleEngine->StartProvidingService();

  // Free memory
  delete consoleEngine;
  delete dataSource;

  return( result );
}

/*
** CVS Log
** $Log: wlmscpfs.cc,v $
** Revision 1.1  2005/08/23 19:32:09  braindead
** - initial savannah import
**
** Revision 1.1  2005/06/26 19:26:15  pipelka
** - added dcmtk
**
** Revision 1.4  2002/08/05 09:09:29  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.3  2002/04/18 14:19:57  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.2  2002/01/08 17:44:45  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.1  2002/01/08 16:29:51  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
