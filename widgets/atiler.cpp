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
    Update Date:      $Date: 2005/09/30 16:57:53 $
    Source File:      $Source: /sources/aeskulap/aeskulap/widgets/atiler.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#include "atiler.h"

namespace Aeskulap {

TilerBase::TilerBase(int x, int y) : m_tile_x(x), m_tile_y(y) {
}

TilerBase::~TilerBase() {
}

void TilerBase::set_layout(int x, int y) {
	m_tile_x = x;
	m_tile_y = y;
}

void TilerBase::get_layout(int& tilex, int& tiley) {
	tilex = m_tile_x;
	tiley = m_tile_y;
}

unsigned int TilerBase::max_size() {
	return m_tile_x * m_tile_y;
}

void TilerBase::get_xy_from_pos(int n, int& x, int& y) {
	// find next empty table cell (oh boy, need to do some math)
	int i = 1;
	bool bExit = false;

	for(y=0; y<m_tile_y && !bExit; y++) {
		for(x=0; x<m_tile_x && !bExit; x++) {
			if(i == n) {
				bExit = true;
			}
			i++;
		}
	}
}


} // namespace Aeskulap
