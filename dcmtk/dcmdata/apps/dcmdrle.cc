/*
 *
 *  Copyright (C) 2002-2004, OFFIS
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
 *  Purpose: Decompress RLE-compressed DICOM file
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:00 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"     /* for dcmtk version name */
#include "dcrledrg.h"  /* for DcmRLEDecoderRegistration */

#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmdrle"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  SetDebugLevel(( 0 ));

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  int opt_debugMode = 0;
  OFBool opt_verbose = OFFalse;
  OFBool opt_oDataset = OFFalse;
  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;

  // RLE parameters
  OFBool opt_uidcreation = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Decode RLE-compressed DICOM file", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--version",                                "print version information and exit", OFTrue /* exclusive */);
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

  cmd.addGroup("processing options:");

    cmd.addSubGroup("SOP Instance UID options:");
     cmd.addOption("--uid-default",        "+ud",     "keep same SOP Instance UID (default)");
     cmd.addOption("--uid-always",         "+ua",     "always assign new UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-little",      "+te",       "write with explicit VR little endian (default)");
      cmd.addOption("--write-xfer-big",         "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",       "write with implicit VR little endian TS");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc",    "+g=",       "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",        "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",        "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",        "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-retain",         "-p=",       "do not change padding\n(default if not --write-dataset)");
      cmd.addOption("--padding-off",            "-p",        "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer",
                                                             "align file on multiple of f bytes\nand items on multiple of i bytes");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check exclusive options first */

      if (cmd.getParamCount() == 0)
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#ifdef WITH_ZLIB
            CERR << endl << "- ZLIB, Version " << zlibVersion() << endl;
#else
            CERR << " none" << endl;
#endif
            return 0;
         }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
      if (cmd.findOption("--debug")) opt_debugMode = 5;

      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-default")) opt_uidcreation = OFFalse;
      if (cmd.findOption("--uid-always")) opt_uidcreation = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_oDataset = OFFalse;
      if (cmd.findOption("--write-dataset")) opt_oDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) opt_oglenc = EGL_recalcGL;
      if (cmd.findOption("--group-length-create")) opt_oglenc = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) opt_oglenc = EGL_withoutGL;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) opt_oenctype = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) opt_oenctype = EET_UndefinedLength;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-retain"))
      {
        if (opt_oDataset) app.printError("--padding-retain not allowed with --write-dataset");
        opt_opadenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
          if (opt_oDataset) app.printError("--padding-create not allowed with --write-dataset");
          app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
          app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
          opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();

    }

    SetDebugLevel((opt_debugMode));

    // register global decompression codecs
    DcmRLEDecoderRegistration::registerCodecs(opt_uidcreation, opt_verbose);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        CERR << "Error: invalid filename: <empty string>" << endl;
        return 1;
    }

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();

    if (opt_verbose)
        COUT << "open input file " << opt_ifname << endl;

    OFCondition error = fileformat.loadFile(opt_ifname);

    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": reading file: " <<  opt_ifname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "decompressing file" << endl;

    DcmXfer opt_oxferSyn(opt_oxfer);

    error = dataset->chooseRepresentation(opt_oxfer, NULL);
    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": decompressing file: " <<  opt_ifname << endl;
        return 1;
    }

    if (! dataset->canWriteXfer(opt_oxfer))
    {
        CERR << "Error: no conversion to transfer syntax " << opt_oxferSyn.getXferName()
             << " possible" << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "create output file " << opt_ofname << endl;

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_oDataset);

    if (error.bad())
    {
        CERR << "Error: "
             << error.text()
             << ": writing file: " <<  opt_ofname << endl;
        return 1;
    }

    if (opt_verbose)
        COUT << "conversion successful" << endl;

    // deregister RLE codec
    DcmRLEDecoderRegistration::cleanup();

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmdrle.cc,v $
 * Revision 1.1  2005/08/23 19:32:00  braindead
 * - initial savannah import
 *
 * Revision 1.1  2005/06/26 19:25:57  pipelka
 * - added dcmtk
 *
 * Revision 1.7  2004/01/16 10:53:53  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.6  2002/11/27 12:07:16  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.5  2002/11/26 08:43:00  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.4  2002/09/23 17:52:02  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.3  2002/09/23 13:50:40  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.2  2002/08/21 10:14:14  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.1  2002/06/06 14:52:32  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
