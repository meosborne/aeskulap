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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRKeyObjectDocConstraintChecker
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:31:52 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrkeycc.h"


DSRKeyObjectDocConstraintChecker::DSRKeyObjectDocConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRKeyObjectDocConstraintChecker::~DSRKeyObjectDocConstraintChecker()
{
}


OFBool DSRKeyObjectDocConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRKeyObjectDocConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


const char *DSRKeyObjectDocConstraintChecker::getRootTemplateIdentifier() const
{
    return "2010";
}


DSRTypes::E_DocumentType DSRKeyObjectDocConstraintChecker::getDocumentType() const
{
    return DT_KeyObjectDoc;
}


OFBool DSRKeyObjectDocConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                  const E_RelationshipType relationshipType,
                                                                  const E_ValueType targetValueType,
                                                                  const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.4-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Composite) ||
                     (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)   || (targetValueType == VT_Code) ||
                     (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasConceptMod) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Code);
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrkeycc.cc,v $
 *  Revision 1.1  2005/08/23 19:31:52  braindead
 *  - initial savannah import
 *
 *  Revision 1.1  2005/06/26 19:26:05  pipelka
 *  - added dcmtk
 *
 *  Revision 1.2  2003/10/09 13:00:41  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.1  2003/09/15 14:16:50  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 */
