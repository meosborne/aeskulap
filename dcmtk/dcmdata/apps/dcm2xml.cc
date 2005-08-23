/*
 *
 *  Copyright (C) 2002-2003, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Convert the contents of a DICOM file to XML format
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:00 $
 *  Source File:      $Source: /sources/aeskulap/aeskulap/dcmtk/dcmdata/apps/dcm2xml.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctk.h"
#include "cmdlnarg.h"
#include "ofstd.h"
#include "ofstream.h"
#include "ofconapp.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcm2xml"
#define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM file and data set to XML"

#define DOCUMENT_TYPE_DEFINITION_FILE "dcm2xml.dtd"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition writeFile(ostream &out,
                             const char *ifname,
                             const OFBool isDataset,
                             const E_TransferSyntax xfer,
                             const OFBool loadIntoMemory,
                             const size_t writeFlags)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return EC_IllegalParameter;
    }

    /* read DICOM file or data set */
    DcmFileFormat dfile;
    result = dfile.loadFile(ifname, xfer, EGL_noChange, DCM_MaxReadLength, isDataset);

    if (result.bad())
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
             << ") reading file: "<< ifname << endl;
    }
    else
    {
        /* write content to XML format */
        if (loadIntoMemory)
            dfile.getDataset()->loadAllDataIntoMemory();
        /* determine dataset character encoding */
        OFString encString;
        OFString csetString;
        if (dfile.getDataset()->findAndGetOFString(DCM_SpecificCharacterSet, csetString).good())
        {
            if (csetString == "ISO_IR 6")
                encString = "UTF-8";
            else if (csetString == "ISO_IR 100")
                encString = "ISO-8859-1";
            else if (csetString == "ISO_IR 101")
                encString = "ISO-8859-2";
            else if (csetString == "ISO_IR 109")
                encString = "ISO-8859-3";
            else if (csetString == "ISO_IR 110")
                encString = "ISO-8859-4";
            else if (csetString == "ISO_IR 148")
                encString = "ISO-8859-9";
            else if (csetString == "ISO_IR 144")
                encString = "ISO-8859-5";
            else if (csetString == "ISO_IR 127")
                encString = "ISO-8859-6";
            else if (csetString == "ISO_IR 126")
                encString = "ISO-8859-7";
            else if (csetString == "ISO_IR 138")
                encString = "ISO-8859-8";
        }
        /* write XML document header */
        out << "<?xml version=\"1.0\"";
        /* optional character set */
        if (encString.length() > 0)
            out << " encoding=\"" << encString << "\"";
        out << "?>" << endl;
        /* add document type definition (DTD) */
        if (writeFlags & DCMTypes::XF_addDocumentType)
        {
            out << "<!DOCTYPE ";
            if (isDataset)
               out << "data-set";
            else
               out << "file-format";
            /* embed DTD */
            if (writeFlags & DCMTypes::XF_embedDocumentType)
            {
                out << " [" << endl;
                /* copy content from DTD file */
#ifdef HAVE_IOS_NOCREATE
                ifstream dtdFile(DOCUMENT_TYPE_DEFINITION_FILE, ios::in|ios::nocreate);
#else
                ifstream dtdFile(DOCUMENT_TYPE_DEFINITION_FILE, ios::in);
#endif
                if (dtdFile)
                {
                    char c;
                    /* copy all characters */
                    while (dtdFile.get(c))
                        out << c;
                }
                out << "]";
            } else { /* reference DTD */
                out << " SYSTEM \"" << DOCUMENT_TYPE_DEFINITION_FILE << "\"";
            }
            out << ">" << endl;
        }
        /* write XML document content */
        if (isDataset)
            result = dfile.getDataset()->writeXML(out, writeFlags);
        else
            result = dfile.writeXML(out, writeFlags);
    }
    return result;
}


#define SHORTCOL 3
#define LONGCOL 20


int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    size_t opt_writeFlags = 0;
    OFBool isDataset = OFFalse;
    OFBool loadIntoMemory = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",   "DICOM input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("xmlfile-out",  "XML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit");
      cmd.addOption("--version",                         "print version information and exit", OFTrue /* exclusive */);
      cmd.addOption("--debug",                 "-d",     "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",     "read file format or data set (default)");
        cmd.addOption("--read-dataset",        "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",      "-t=",    "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",    "-te",    "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",       "-tb",    "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",  "-ti",    "read with implicit VR little endian TS");
      cmd.addSubGroup("long tag values:");
        cmd.addOption("--load-all",            "+M",     "load very long tag values (e.g. pixel data)");
        cmd.addOption("--load-short",          "-M",     "do not load very long values (default)");
    cmd.addGroup("output options:");
      cmd.addSubGroup("XML structure:");
        cmd.addOption("--add-dtd-reference",   "+Xd",    "add reference to document type definition (DTD)");
        cmd.addOption("--embed-dtd-content",   "+Xe",    "embed document type definition into XML document");
        cmd.addOption("--use-xml-namespace",   "+Xn",    "add XML namespace declaration to root element");
      cmd.addSubGroup("DICOM elements:");
        cmd.addOption("--write-binary-data",   "+Wb",    "write binary data of OB and OW elements\n(default: off, be careful with --load-all)");
        cmd.addOption("--encode-base64",       "+Eb",    "encode binary data as Base64 (RFC 2045, MIME)");

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

        /* options */

        if (cmd.findOption("--debug"))
            opt_debugMode = 5;

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file"))
            isDataset = OFFalse;
        if (cmd.findOption("--read-dataset"))
            isDataset = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
        {
            app.checkDependence("--read-xfer-auto", "--read-dataset", isDataset);
            xfer = EXS_Unknown;
        }
        if (cmd.findOption("--read-xfer-little"))
        {
            app.checkDependence("--read-xfer-little", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-big"))
        {
            app.checkDependence("--read-xfer-big", "--read-dataset", isDataset);
            xfer = EXS_BigEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-implicit"))
        {
            app.checkDependence("--read-xfer-implicit", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianImplicit;
        }
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--load-all"))
            loadIntoMemory = OFTrue;
        if (cmd.findOption("--load-short"))
            loadIntoMemory = OFFalse;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--add-dtd-reference"))
            opt_writeFlags |= DCMTypes::XF_addDocumentType;
        if (cmd.findOption("--embed-dtd-content"))
            opt_writeFlags |= DCMTypes::XF_addDocumentType | DCMTypes::XF_embedDocumentType;
        cmd.endOptionBlock();

        if (cmd.findOption("--use-xml-namespace"))
            opt_writeFlags |= DCMTypes::XF_useDcmtkNamespace;

        if (cmd.findOption("--write-binary-data"))
            opt_writeFlags |= DCMTypes::XF_writeBinaryData;
        if (cmd.findOption("--encode-base64"))
            opt_writeFlags |= DCMTypes::XF_encodeBase64;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    /* make sure document type definition file exists */
    if ((opt_writeFlags & DCMTypes::XF_embedDocumentType) &&
        !OFStandard::fileExists(DOCUMENT_TYPE_DEFINITION_FILE))
    {
        CERR << "Warning: DTD file \"" << DOCUMENT_TYPE_DEFINITION_FILE
             << "\" does not exist ... adding reference instead" << endl;
        opt_writeFlags &= ~DCMTypes::XF_embedDocumentType;
    }

    int result = 0;
    /* first parameter is treated as the input filename */
    const char *ifname = NULL;
    cmd.getParam(1, ifname);
    /* if second parameter is present, it is treated as the output filename ("stdout" otherwise) */
    if (cmd.getParamCount() == 2)
    {
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        ofstream stream(ofname);
        if (stream.good())
        {
            if (writeFile(stream, ifname, isDataset, xfer, loadIntoMemory, opt_writeFlags).bad())
                result = 2;
        } else
            result = 1;
    } else {
        if (writeFile(COUT, ifname, isDataset, xfer, loadIntoMemory, opt_writeFlags).bad())
            result = 3;
    }

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2xml.cc,v $
 * Revision 1.1  2005/08/23 19:32:00  braindead
 * - initial savannah import
 *
 * Revision 1.1  2005/06/26 19:25:57  pipelka
 * - added dcmtk
 *
 * Revision 1.11  2003/04/22 08:23:33  joergr
 * Added new command line option which allows to embed the content of the DTD
 * instead of referencing the DTD file.
 *
 * Revision 1.10  2003/04/01 14:56:14  joergr
 * Added support for XML namespaces.
 *
 * Revision 1.9  2002/11/26 08:42:58  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.8  2002/09/23 17:52:01  joergr
 * Prepared code for future support of 'config.guess' host identifiers.
 *
 * Revision 1.7  2002/09/23 13:50:39  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used.
 *
 * Revision 1.6  2002/08/21 10:14:13  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.5  2002/06/10 17:35:47  joergr
 * Fixed inconsistency regarding spelling of the "file-format" element.
 *
 * Revision 1.4  2002/05/14 08:19:22  joergr
 * Added support for Base64 (MIME) encoded binary data.
 *
 * Revision 1.3  2002/05/07 12:47:41  joergr
 * Fixed bug in an error message.
 *
 * Revision 1.2  2002/04/25 14:56:35  joergr
 * Removed unused function parameter to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/04/25 10:08:35  joergr
 * Added new command line tool to convert DICOM files to XML.
 *
 *
 */
