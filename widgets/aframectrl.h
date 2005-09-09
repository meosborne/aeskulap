/*
    Aeskulap - DICOM image viewer and network client
    Copyright (C) 2005  Alexander Pipelka

    This file is part of Aeskulap.

    Aeskulap is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Aeskulap is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Aeskulap; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Alexander Pipelka
    pipelka@teleweb.at

    Last Update:      $Author: braindead $
    Update Date:      $Date: 2005/09/09 16:12:33 $
    Source File:      $Source: /sources/aeskulap/aeskulap/widgets/aframectrl.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#ifndef AESKULAP_FRAMECTRL_H
#define AESKULAP_FRAMECTRL_H

#include <gtkmm.h>

namespace Aeskulap {

class Display;

class FrameCtrl {
public:

	virtual void connect(Display* display) = 0;
	
	virtual void disconnect() = 0;
};

} // namespace Aeskulap

#endif // AESKULAP_FRAMECTRL_H
