/*
 *
 *  Copyright (C) 2000-2005, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRWaveformTreeNode
 *
 *  Last Update:      $Author$
 *  Update Date:      $Date$
 *  CVS/RCS Revision: $Revision$
 *  Status:           $State$
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVTN_H
#define DSRWAVTN_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrwavvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for content item WAVEFORM
 */
class DSRWaveformTreeNode
  : public DSRDocumentTreeNode,
    public DSRWaveformReferenceValue
{

  public:

    /** constructor
     ** @param  relationshipType  type of relationship to the parent tree node.
     *                            Should not be RT_invalid or RT_isRoot.
     */
    DSRWaveformTreeNode(const E_RelationshipType relationshipType);

    /** destructor
     */
    virtual ~DSRWaveformTreeNode();

    /** clear all member variables.
     *  Please note that the content item might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the content item is valid.
     *  The content item is valid if the two base classes are valid.
     ** @return OFTrue if tree node is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content is short.
     *  The method isShort() from the base class DSRWaveformReferenceValue is called.
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print content item.
     *  A typical output looks like this: has properties WAVEFORM:=(HemodynamicWaveform
     *  Storage,"1.2.3")
     ** @param  stream  output stream to which the content item should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(ostream &stream,
                              const size_t flags) const;

    /** write content item in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream) const;


  protected:

    /** read content item (value) from dataset
     ** @param  dataset    DICOM dataset from which the content item should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readContentItem(DcmItem &dataset,
                                        OFConsole *logStream);

    /** write content item (value) to dataset
     ** @param  dataset    DICOM dataset to which the content item should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeContentItem(DcmItem &dataset,
                                         OFConsole *logStream) const;

    /** read content item specific XML data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXMLContentItem(const DSRXMLDocument &doc,
                                           DSRXMLCursor cursor);

    /** render content item (value) in HTML format
     ** @param  docStream     output stream to which the main HTML document is written
     *  @param  annexStream   output stream to which the HTML document annex is written
     *  @param  nestingLevel  current nesting level.  Used to render section headings.
     *  @param  annexNumber   reference to the variable where the current annex number is stored.
     *                        Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags         flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream     pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTMLContentItem(ostream &docStream,
                                              ostream &annexStream,
                                              const size_t nestingLevel,
                                              size_t &annexNumber,
                                              const size_t flags,
                                              OFConsole *logStream) const;


  private:

// --- declaration of default/copy constructor and assignment operator

    DSRWaveformTreeNode();
    DSRWaveformTreeNode(const DSRWaveformTreeNode &);
    DSRWaveformTreeNode &operator=(const DSRWaveformTreeNode &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log$
 *  Revision 1.1  2007/04/24 09:53:28  braindead
 *  - updated DCMTK to version 3.5.4
 *  - merged Gianluca's WIN32 changes
 *
 *  Revision 1.1.1.1  2006/07/19 09:16:43  pipelka
 *  - imported dcmtk354 sources
 *
 *
 *  Revision 1.10  2005/12/08 16:05:33  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2003/09/15 14:18:54  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.8  2003/08/07 13:07:08  joergr
 *  Added readXML functionality. Added support for Chest CAD SR.
 *
 *  Revision 1.7  2001/11/09 16:10:55  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.6  2001/09/26 13:04:15  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:51:06  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/11/07 18:14:32  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.3  2000/11/01 16:23:28  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.2  2000/10/23 15:13:42  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:37  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
