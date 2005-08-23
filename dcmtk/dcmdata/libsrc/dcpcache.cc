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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: general purpose 32-bit CRC in C++
 *           Code is based on the CRC32 implementation (C)1986 Gary S. Brown
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:31:58 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcpcache.h"
#include "dcelem.h"      /* for DcmElement, DcmObject */

DcmPrivateTagCacheEntry::DcmPrivateTagCacheEntry(const DcmTagKey& tk, const char *pc)
: tagKey(tk)
, privateCreator(pc)
{
}


DcmPrivateTagCacheEntry::~DcmPrivateTagCacheEntry()
{
}


const char *DcmPrivateTagCacheEntry::getPrivateCreator() const
{
  return privateCreator.c_str();
}


OFBool DcmPrivateTagCacheEntry::isPrivateCreatorFor(const DcmTagKey& tk) const
{
  return (tagKey.getGroup() == tk.getGroup()) &&
         ((tagKey.getElement() << 8) == (tk.getElement() & 0xff00));
}

/* ======================================================================= */

DcmPrivateTagCache::DcmPrivateTagCache()
: OFList<DcmPrivateTagCacheEntry *>()
{
}


DcmPrivateTagCache::~DcmPrivateTagCache()
{
  clear();
}


void DcmPrivateTagCache::clear()
{
  OFListIterator(DcmPrivateTagCacheEntry *) first = begin();
  OFListIterator(DcmPrivateTagCacheEntry *) last = end();
  while (first != last)
  {
    delete (*first);
    first = erase(first);
  }
}


const char *DcmPrivateTagCache::findPrivateCreator(const DcmTagKey& tk) const
{
  OFListConstIterator(DcmPrivateTagCacheEntry *) first = begin();
  OFListConstIterator(DcmPrivateTagCacheEntry *) last = end();
  while (first != last)
  {
    if ((*first)->isPrivateCreatorFor(tk)) return (*first)->getPrivateCreator();
    ++first;
  }
  return NULL;
}


void DcmPrivateTagCache::updateCache(DcmObject *dobj)
{
  if (dobj)
  {
    const DcmTag& tag = dobj->getTag();
    if (dobj->isLeaf() && (tag.getGTag() & 1) && (tag.getETag() <= 0xff) && (tag.getETag() >= 0x10))
    {
      // dobj is DcmElement containing private creator
      char *c = NULL;
      if ((OFstatic_cast(DcmElement *, dobj)->getString(c)).good() && c)
      {
        push_back(new DcmPrivateTagCacheEntry(tag, c));
      }
    }
  }
}


/*
 * CVS/RCS Log:
 * $Log: dcpcache.cc,v $
 * Revision 1.1  2005/08/23 19:31:58  braindead
 * - initial savannah import
 *
 * Revision 1.1  2005/06/26 19:25:55  pipelka
 * - added dcmtk
 *
 * Revision 1.3  2004/02/04 16:40:48  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.2  2003/06/12 18:22:23  joergr
 * Modified code to use const_iterators where appropriate (required for STL).
 *
 * Revision 1.1  2002/07/23 14:21:34  meichel
 * Added support for private tag data dictionaries to dcmdata
 *
 *
 */
