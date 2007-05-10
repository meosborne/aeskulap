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
    Update Date:      $Date: 2007/05/10 14:29:59 $
    Source File:      $Source: /sources/aeskulap/aeskulap/imagepool/netloader.cpp,v $
    CVS/RCS Revision: $Revision: 1.16 $
    Status:           $State: Exp $
*/

#include <gtkmm.h>

#include "imagepool.h"
#include "poolmoveassociation.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "netclient.h"
#include "netloader.h"
#include "aconfiguration.h"

namespace ImagePool {

class DicomMover : public MoveAssociation {
public:

	DicomMover() : responsecount(0) {
	};

	sigc::signal<void, DcmDataset*> signal_response_received;

	int responsecount;

protected:
	void OnResponseReceived(DcmDataset *response) {
		if(response != NULL) {
			signal_response_received(response);
			responsecount++;
		}
	};
};

NetLoader::NetLoader(const std::string& local_aet) :
m_local_aet(local_aet) {
}

bool NetLoader::load(const Glib::RefPtr< ImagePool::Study >& study, const std::string& server) {
	if(busy()) {
		return false;
	}

	m_study = study;
	m_server = server;
	start();
	
	return true;
}

bool NetLoader::run() {

	Aeskulap::Configuration& conf = Aeskulap::Configuration::get_instance();
	std::string studyinstanceuid = m_study->studyinstanceuid();
	 
	if(m_study->studyrelatedinstances() > 0) {
		m_cache[studyinstanceuid].m_instancecount = m_study->studyrelatedinstances(); 
	}
	else {
		m_cache[studyinstanceuid].m_instancecount = query_study_instances(studyinstanceuid, m_server, conf.get_local_aet());
	}

	if(m_study->seriescount() > 0) {
		m_cache[studyinstanceuid].m_seriescount = m_study->seriescount();
	}
	else {
		m_cache[studyinstanceuid].m_seriescount = query_study_series(studyinstanceuid, m_server, conf.get_local_aet());
	}

	NetClient<DicomMover> mover;

	mover.signal_response_received.connect(sigc::mem_fun(*this, &NetLoader::add_image));
	mover.SetMaxResults(5000);

	DcmDataset query;
	DcmElement* e = NULL;
	
	e = newDicomElement(DCM_QueryRetrieveLevel);
	e->putString("STUDY");
	query.insert(e);

	e = newDicomElement(DCM_SOPInstanceUID);
	query.insert(e);

	e = newDicomElement(DCM_InstanceNumber);
	query.insert(e);

	e = newDicomElement(DCM_StudyInstanceUID);
	e->putString(studyinstanceuid.c_str());
	query.insert(e);

	e = newDicomElement(DCM_SeriesInstanceUID);
	query.insert(e);

	if(!mover.QueryServer(&query, m_server, conf.get_local_aet())) {
		std::cerr << "C-MOVE failed !" << std::endl;
		return false;
	}

	std::cout << "C-MOVE: " << mover.responsecount << " responses" << std::endl;
	return (mover.responsecount != 0);
}

} // namespace ImagePool
