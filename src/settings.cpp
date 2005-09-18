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
    Update Date:      $Date: 2005/09/18 19:52:35 $
    Source File:      $Source: /sources/aeskulap/aeskulap/src/settings.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "settings.h"
#include "imagepool.h"
#include "gettext.h"
#include <vector>

Settings::Settings(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) :
Gtk::Window(cobject),
m_refGlade(refGlade) {
	// Get the client and tell it we want to monitor /apps/aeskulap/preferences
	m_conf_client = Gnome::Conf::Client::get_default_client();
	m_conf_client->add_dir("/apps/aeskulap/preferences");
	
	// dicom settings

	m_refGlade->get_widget("local_aet", m_local_aet);
	
	m_refGlade->get_widget("local_port", m_local_port);

	// server details

	m_refGlade->get_widget("server_detail_server", m_server_detail_server);
	m_server_detail_server->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_apply));
	
	m_refGlade->get_widget("server_detail_aet", m_server_detail_aet);
	m_server_detail_aet->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_apply));
	
	m_refGlade->get_widget("server_detail_port", m_server_detail_port);
	m_server_detail_port->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_apply));
	
	m_refGlade->get_widget("server_detail_group", m_server_detail_group);
	m_server_detail_group->get_entry()->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_apply));

	m_refGlade->get_widget("server_detail_description", m_server_detail_description);
	m_server_detail_description->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_apply));

	m_refGlade->get_widget("server_detail_echo", m_server_detail_echo);
	m_server_detail_echo->signal_clicked().connect(sigc::mem_fun(*this, &Settings::on_echotest));

	m_refGlade->get_widget("server_detail_echostatus", m_server_detail_echostatus);

	// disable server detail
	
	set_server_detail_sensitive(false);

	// connect buttons
	m_refGlade->get_widget("settings_ok", m_settings_ok);
	m_settings_ok->signal_clicked().connect(sigc::mem_fun(*this, &Settings::on_settings_save));

	Gtk::Button* button;
	m_refGlade->get_widget("settings_cancel", button);
	button->signal_clicked().connect(sigc::mem_fun(*this, &Settings::on_settings_cancel));	

	m_refGlade->get_widget("servers_add", button);
	button->signal_clicked().connect(sigc::mem_fun(*this, &Settings::on_server_add));

	m_refGlade->get_widget("servers_remove", button);
	button->signal_clicked().connect(sigc::mem_fun(*this, &Settings::on_server_remove));

	// create server list
	m_refGlade->get_widget("serverlist", m_list_servers);

	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_refTreeModel->set_sort_column(m_Columns.m_description, Gtk::SORT_ASCENDING);

	m_list_servers->set_model(m_refTreeModel);

	m_list_servers->append_column(gettext("Description"), m_Columns.m_description);
	m_list_servers->append_column(gettext("AET"), m_Columns.m_aet);
	m_list_servers->append_column_numeric(gettext("Port"), m_Columns.m_port, "%i");
	m_list_servers->append_column(gettext("Hostname"), m_Columns.m_hostname);
	m_list_servers->append_column(gettext("Group"), m_Columns.m_group);

	m_list_servers->get_column(0)->set_sort_column(m_Columns.m_description);
	m_list_servers->get_column(0)->property_sort_indicator().set_value(true);
	m_list_servers->get_column(0)->set_sort_order(Gtk::SORT_ASCENDING);
	m_list_servers->get_column(1)->set_sort_column(m_Columns.m_aet);
	m_list_servers->get_column(2)->set_sort_column(m_Columns.m_port);
	m_list_servers->get_column(3)->set_sort_column(m_Columns.m_hostname);
	m_list_servers->get_column(3)->set_sort_column(m_Columns.m_group);

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_list_servers->get_selection();
	refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &Settings::on_server_activated));

	restore_settings();
}

Settings::~Settings() {
}

void Settings::on_show() {
	Gtk::Window::on_show();
	restore_settings();
}


void Settings::on_settings_save() {
	save_settings();
	ImagePool::close();
	ImagePool::init(false);
	hide();
}
	
void Settings::on_settings_cancel() {
	restore_settings();
	hide();
}

void Settings::save_settings() {
	Glib::ustring value;

	value = m_local_aet->get_text();
	m_conf_client->set("/apps/aeskulap/preferences/local_aet", value);

	value = m_local_port->get_text();
	m_conf_client->set("/apps/aeskulap/preferences/local_port", atoi(value.c_str()));

	std::vector< Glib::ustring > aet_list;
	std::vector< Glib::ustring > hostname_list;
	std::vector< int > port_list;
	std::vector< Glib::ustring > description_list;
	std::vector< Glib::ustring > group_list;

	Gtk::TreeModel::Children::iterator i = m_refTreeModel->children().begin();
	for(; i != m_refTreeModel->children().end(); i++) {
		aet_list.push_back((*i)[m_Columns.m_aet]);
		hostname_list.push_back((*i)[m_Columns.m_hostname]);
		port_list.push_back((*i)[m_Columns.m_port]);
		description_list.push_back((*i)[m_Columns.m_description]);
		group_list.push_back((*i)[m_Columns.m_group]);
	}

	m_conf_client->set_string_list("/apps/aeskulap/preferences/server_aet", aet_list);
	m_conf_client->set_string_list("/apps/aeskulap/preferences/server_hostname", hostname_list);
	m_conf_client->set_int_list("/apps/aeskulap/preferences/server_port", port_list);
	m_conf_client->set_string_list("/apps/aeskulap/preferences/server_description", description_list);
	m_conf_client->set_string_list("/apps/aeskulap/preferences/server_group", group_list);
}

void Settings::restore_settings() {
	Glib::ustring local_aet_setting = m_conf_client->get_string(
		"/apps/aeskulap/preferences/local_aet");

	if(local_aet_setting.empty()) {
		local_aet_setting = "AESKULAP";
		m_conf_client->set("/apps/aeskulap/preferences/local_aet", local_aet_setting);
	}

	m_local_aet->set_text(local_aet_setting);

	gint local_port_setting = m_conf_client->get_int(
		"/apps/aeskulap/preferences/local_port");

	if(local_port_setting == 0) {
		local_port_setting = 6000;
		m_conf_client->set("/apps/aeskulap/preferences/local_port", local_port_setting);
	}

	char buffer[10];
	sprintf(buffer, "%i", local_port_setting);
	m_local_port->set_text(buffer);

	Gnome::Conf::SListHandle_ValueString aet_list = m_conf_client->get_string_list("/apps/aeskulap/preferences/server_aet");
	Gnome::Conf::SListHandle_ValueInt port_list = m_conf_client->get_int_list("/apps/aeskulap/preferences/server_port");
	Gnome::Conf::SListHandle_ValueString hostname_list = m_conf_client->get_string_list("/apps/aeskulap/preferences/server_hostname");
	Gnome::Conf::SListHandle_ValueString description_list = m_conf_client->get_string_list("/apps/aeskulap/preferences/server_description");
	Gnome::Conf::SListHandle_ValueString group_list = m_conf_client->get_string_list("/apps/aeskulap/preferences/server_group");

	Gnome::Conf::SListHandle_ValueString::iterator a = aet_list.begin();
	Gnome::Conf::SListHandle_ValueInt::iterator p = port_list.begin();
	Gnome::Conf::SListHandle_ValueString::iterator h = hostname_list.begin();
	Gnome::Conf::SListHandle_ValueString::iterator d = description_list.begin();
	Gnome::Conf::SListHandle_ValueString::iterator g = group_list.begin();

	Gtk::TreeModel::Children::iterator i = m_refTreeModel->children().begin();
	for(; i != m_refTreeModel->children().end();) {
		i = m_refTreeModel->erase(i);
	}

	for(; h != hostname_list.end() && a != aet_list.end() && p != port_list.end(); a++, p++, h++) {
		Gtk::TreeModel::Row row = *(m_refTreeModel->append());

		row[m_Columns.m_aet] = *a;
		row[m_Columns.m_port] = *p;
		row[m_Columns.m_hostname] = *h;
		
		if(d != description_list.end()) {
			row[m_Columns.m_description] = *d;
			d++;
		}
		if(g != group_list.end()) {
			row[m_Columns.m_group] = *g;
			g++;
		}
	}
}

void Settings::on_server_add() {
	static int count = m_refTreeModel->children().size();
	set_server_detail_sensitive(false);

	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	Glib::RefPtr<Gtk::TreeSelection> selection = m_list_servers->get_selection();

	char servername[50];
	sprintf(servername, "Server%i", ++count);

	row[m_Columns.m_description] = servername;
	row[m_Columns.m_aet] = "AET";
	row[m_Columns.m_port] = 6100;
	row[m_Columns.m_hostname] = "hostname";

	Gtk::TreePath path(row);
	m_list_servers->row_activated (path, *(m_list_servers->get_column(0)));
	selection->select(row);
}

void Settings::on_server_remove() {
	Glib::RefPtr<Gtk::TreeSelection> selection = m_list_servers->get_selection();
	m_refTreeModel->erase(selection->get_selected());
}

void Settings::set_server_detail_sensitive(bool sensitive) {
	m_server_detail_server->set_sensitive(sensitive);
	m_server_detail_aet->set_sensitive(sensitive);
	m_server_detail_port->set_sensitive(sensitive);
	m_server_detail_group->set_sensitive(sensitive);
	m_server_detail_description->set_sensitive(sensitive);
	m_server_detail_echo->set_sensitive(sensitive);

	if(!sensitive) {
		m_server_detail_server->set_text("");		
		m_server_detail_aet->set_text("");
		m_server_detail_port->set_text("");
		m_server_detail_group->get_entry()->set_text("");
		m_server_detail_description->set_text("");
	}
}

void Settings::on_server_activated() {
	set_server_detail_sensitive(false);
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_list_servers->get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

	if(!iter) {
		return;
	}
	Gtk::TreeModel::Row row = *iter;

	m_server_detail_server->set_text(row[m_Columns.m_hostname]);
	
	m_server_detail_aet->set_text(row[m_Columns.m_aet]);
	
	char buffer[10];
	guint i = row[m_Columns.m_port];
	sprintf(buffer, "%i", i);
	m_server_detail_port->set_text(buffer);
	
	m_server_detail_group->get_entry()->set_text(row[m_Columns.m_group]);
	
	m_server_detail_description->set_text(row[m_Columns.m_description]);

	set_server_detail_sensitive(true);
}

void Settings::on_server_apply() {
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_list_servers->get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

	if(!iter) {
		return;
	}
	Gtk::TreeModel::Row row = *iter;

	if(m_server_detail_server->is_sensitive()) {
		row[m_Columns.m_hostname] = m_server_detail_server->get_text();
	}
	if(m_server_detail_aet->is_sensitive()) {
		row[m_Columns.m_aet] = m_server_detail_aet->get_text();
	}
	if(m_server_detail_port->is_sensitive()) {
		guint i = atoi(m_server_detail_port->get_text().c_str());
		row[m_Columns.m_port] = i;
	}
	if(m_server_detail_group->is_sensitive()) {
		row[m_Columns.m_group] = m_server_detail_group->get_entry()->get_text();
	}
	if(m_server_detail_description->is_sensitive()) {
		Glib::ustring desc = m_server_detail_description->get_text();
		row[m_Columns.m_description] = desc;
		if(desc.empty()) {
			m_settings_ok->set_sensitive(false);
		}
		else {
			m_settings_ok->set_sensitive(true);
		}
	}
	
	// enable echo button
	if(!m_server_detail_aet->get_text().empty() &&
		!m_server_detail_server->get_text().empty() &&
		!m_server_detail_port->get_text().empty()) {
			m_server_detail_echo->set_sensitive(true);
	}
	else {
			m_server_detail_echo->set_sensitive(false);
	}
}

void Settings::on_echotest() {
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_list_servers->get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	Gtk::TreeModel::Row row = *iter;

	m_server_detail_echo->set_sensitive(false);
	while(Gtk::Main::events_pending()) Gtk::Main::iteration(false);

	Glib::ustring hostname = row[m_Columns.m_hostname];
	Glib::ustring aet = row[m_Columns.m_aet];
	unsigned int port = row[m_Columns.m_port];
	std::string status;

	if(!ImagePool::send_echorequest(hostname, aet, port, status)) {
		Gtk::MessageDialog error(
					*this,
					gettext("<span weight=\"bold\" size=\"larger\">Echo test failed!</span>\n\n")
					+ status,
					true,
					Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_OK,
					true);
					
		error.show();
		error.run();
		error.hide();
	}
	else {
		Gtk::MessageDialog msg(
					*this,
					gettext("<span weight=\"bold\" size=\"larger\">Echo succeeded</span>"),
					true,
					Gtk::MESSAGE_INFO,
					Gtk::BUTTONS_OK,
					true);
					
		msg.show();
		msg.run();
		msg.hide();
	}

	m_server_detail_echo->set_sensitive(true);
}
