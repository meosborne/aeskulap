/*
    Aeskulap ImagePool - DICOM abstraction library
    Copyright (C) 2005  Alexander Pipelka

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    Alexander Pipelka
    pipelka@teleweb.at

    Last Update:      $Author: braindead $
    Update Date:      $Date: 2005/09/12 18:00:51 $
    Source File:      $Source: /sources/aeskulap/aeskulap/imagepool/fileloader.cpp,v $
    CVS/RCS Revision: $Revision: 1.5 $
    Status:           $State: Exp $
*/

#include <gtkmm.h>

#include "dcfilefo.h"
#include "dcdeftag.h"

#include "imagepool.h"
#include "fileloader.h"

namespace ImagePool {

bool FileLoader::load(const Glib::SListHandle< Glib::ustring >& filelist) {
	if(busy()) {
		return false;
	}

	m_filelist = new Glib::SListHandle< Glib::ustring >(filelist);
	
	m_studysize.clear();
	prescan_files(m_filelist);

	if(m_studysize.size() == 0) {
		return false;
	}
	
	start();
	
	return true;
}

void FileLoader::prescan_files(Glib::SListHandle< Glib::ustring >* filelist) {
	std::string studyinstanceuid;
	Glib::SListHandle< Glib::ustring >::iterator i = filelist->begin();

	for(; i != filelist->end(); i++) {
		DcmFileFormat dfile;
	
		OFCondition cond = dfile.loadFile(
							(*i).c_str(),
							EXS_Unknown,
							EGL_noChange,
							DCM_MaxReadLength,
							false);
	
		if(cond.good() && dfile.getDataset()->findAndGetOFString(DCM_StudyInstanceUID, studyinstanceuid).good()) {
			m_studysize[studyinstanceuid]++;
		}
	}
}

void FileLoader::run() {
	Glib::SListHandle< Glib::ustring >* filelist = m_filelist;
	Glib::SListHandle< Glib::ustring >::iterator i = filelist->begin();
	std::string studyinstanceuid;

	for(; i != filelist->end(); i++) {
		DcmFileFormat dfile;
	
		OFCondition cond = dfile.loadFile(
							(*i).c_str(),
							EXS_Unknown,
							EGL_noChange,
							DCM_MaxReadLength,
							false);
		
		if(!cond.good()) {
			std::cout << "unable to open file !!!" << std::endl;
		}
		else {
			dfile.loadAllDataIntoMemory();
			std::cout << "opened file:" << (*i) << std::endl;
		
			DcmDataset* dset = dfile.getDataset();
			if(dset->findAndGetOFString(DCM_StudyInstanceUID, studyinstanceuid).good()) {
				add_image(dset, m_studysize[studyinstanceuid]);
			}
		}
	}
	
	delete filelist;
	m_filelist = NULL;
	m_studysize.clear();
}

} // namespace ImagePool
