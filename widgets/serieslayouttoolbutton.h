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
    Update Date:      $Date: 2005/09/22 15:40:46 $
    Source File:      $Source: /sources/aeskulap/aeskulap/widgets/serieslayouttoolbutton.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#ifndef AESKULAP_SERIESLAYOUTTOOLBUTTON_H
#define AESKULAP_SERIESLAYOUTTOOLBUTTON_H

#include <gtkmm.h>
#include <gtkmm/menutoolbutton.h>

class SeriesLayoutToolButton : public Gtk::MenuToolButton {
public:

	SeriesLayoutToolButton();

	void accelerate(Gtk::Window& window);

	void set_layout(int x, int y);

	sigc::signal<void, int, int> signal_change_layout;

protected:

	void on_clicked();

	void on_change_layout(int index);

	Gtk::Menu m_menu;

private:

	int m_index;
};

#endif // AESKULAP_SERIESLAYOUTTOOLBUTTON_H
