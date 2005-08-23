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
    Update Date:      $Date: 2005/08/23 19:31:54 $
    Source File:      $Source: /sources/aeskulap/aeskulap/imagepool/Attic/DicomAssociation.cpp,v $
    CVS/RCS Revision: $Revision: 1.1 $
    Status:           $State: Exp $
*/

// dcmtk includes
#include "ofstdinc.h"
#include <diutil.h>
#include "dcdeftag.h"

#include "dcuid.h"     /* for dcmtk version name */
#include "djdecode.h"  /* for dcmjpeg decoders */
#include "djencode.h"  /* for dcmjpeg encoders */
#include "djrplol.h"   /* for DJ_RPLossless */
#include "djrploss.h"  /* for DJ_RPLossy */
#include "dipijpeg.h"  /* for dcmimage JPEG plugin */
#include "diregist.h"  /* include to support color images */
#include "ofcmdln.h"

// DicomAssociation class header
#include "DicomAssociation.h"
#include "DicomNetwork.h"

#include "djencode.h"
#include "djdecode.h"
#include "djrplol.h"
#include "djrploss.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

DicomAssociation::DicomAssociation() :
m_abstractSyntax(NULL),
m_timeout(0),
assoc(NULL),
presId(0),
msgId(0),
dcmNet(NULL)
{
	strcpy(sopClass, "");
	strcpy(sopInstance, "");
	m_CompressionQuality = 70;
	m_ProposeCompression = true;
}

DicomAssociation::~DicomAssociation()
{
	// drop an existing association on shutdown
	if(assoc != NULL)
		Drop();
}

CONDITION DicomAssociation::Drop(CONDITION cond) {
	// tear down association
	if(cond == DIMSE_NORMAL) {
		/* release association */
		cond = ASC_releaseAssociation(assoc);
	}
	else if(cond == DIMSE_PEERREQUESTEDRELEASE) {
		cond = ASC_abortAssociation(assoc);
		if (SUCCESS(cond)) {
			return cond;
		}
	}
	else if(cond == DIMSE_PEERABORTEDASSOCIATION) {
		return cond;
	}
	else {
		cond = ASC_abortAssociation(assoc);
		if (SUCCESS(cond)) {
			return cond;
		}
	}

	Destroy();
	return cond;
}

void DicomAssociation::Destroy() {
	CONDITION cond = ASC_destroyAssociation(&assoc);

	dcmNet = NULL;
	assoc = NULL;
	msgId = 0;
	presId = 0;
	strcpy(sopClass, "");
	strcpy(sopInstance, "");

}

CONDITION DicomAssociation::SendObject(DcmDataset *dataset) {

/*  OFCmdUnsignedInt opt_selection_value = 6;
  OFCmdUnsignedInt opt_point_transform = 0;
  OFCmdUnsignedInt opt_quality = 90;
  OFBool           opt_huffmanOptimize = OFTrue;
  OFCmdUnsignedInt opt_smoothing = 0;
  int              opt_compressedBits = 0; // 0=auto, 8/12/16=force
  E_CompressionColorSpaceConversion opt_compCSconversion = ECC_lossyYCbCr;
  E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
  E_SubSampling    opt_sampleFactors = ESS_444;
  OFBool           opt_useYBR422 = OFFalse;*/
// OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
/*  OFBool           opt_createOffsetTable = OFTrue;
  int              opt_windowType = 0;
  OFCmdUnsignedInt opt_windowParameter = 0;
  OFCmdFloat       opt_windowCenter=0.0, opt_windowWidth=0.0;
  E_UIDCreation    opt_uidcreation = EUC_default;
  OFBool           opt_secondarycapture = OFFalse;
  OFCmdUnsignedInt opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
  OFBool           opt_usePixelValues = OFTrue;
  OFBool           opt_useModalityRescale = OFFalse;*/

	CONDITION cond;
	DcmDataset *statusDetail = NULL;

	T_DIMSE_C_StoreRQ req;
	T_DIMSE_C_StoreRSP rsp;

	// check if we SOPClass and SOPInstance in dataset
	if (!DU_findSOPClassAndInstanceInDataSet(dataset, sopClass, sopInstance))
	{
		return DIMSE_BADDATA;
	}

	/* which presentation context should be used */
	presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
	if (presId == 0)
	{
		const char *modalityName = dcmSOPClassUIDToModality(sopClass);
		if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);
		if (!modalityName) modalityName = "unknown SOP class";
		return DIMSE_BADDATA;
	}

	// init store

	bzero((char*)&req, sizeof(req));
	req.MessageID = msgId;
	strcpy(req.AffectedSOPClassUID, sopClass);
	strcpy(req.AffectedSOPInstanceUID, sopInstance);
	req.DataSetType = DIMSE_DATASET_PRESENT;
	req.Priority = DIMSE_PRIORITY_LOW;

	// convert to accepted transfer syntax
	T_ASC_PresentationContext pc;
	cond = ASC_findAcceptedPresentationContext(assoc->params, presId, &pc);
	ASC_dumpPresentationContext(&pc, COUT);

	/*
	DJEncoderRegistration::registerCodecs(
		opt_compCSconversion,
		opt_uidcreation,
		false,
		opt_huffmanOptimize,
		(int) opt_smoothing,
		opt_compressedBits,
		(Uint32) opt_fragmentSize,
		opt_createOffsetTable,
		opt_sampleFactors,
		opt_useYBR422,
		opt_secondarycapture,
		opt_windowType,
		opt_windowParameter,
		opt_windowCenter,
		opt_windowWidth,
		opt_roiLeft,
		opt_roiTop,
		opt_roiWidth,
		opt_roiHeight,
		opt_usePixelValues,
		opt_useModalityRescale);
	*/

	DJEncoderRegistration::registerCodecs(
		ECC_lossyYCbCr,
		EUC_never,					// UID generation (never create new UID's)
		OFFalse,							// verbose
		OFTrue);							// optimize huffman table
	
	DJDecoderRegistration::registerCodecs();

	DcmXfer opt_oxferSyn(pc.acceptedTransferSyntax);
	E_TransferSyntax ori_oxferSyn = dataset->getOriginalXfer();

    DcmXfer original_xfer(dataset->getOriginalXfer());

	if(opt_oxferSyn.getXfer() != ori_oxferSyn) {
		std::cout << "Converting object to accepted transfer-syntax " << opt_oxferSyn.getXferName() << std::endl;
	
		CONDITION cond;
		// create RepresentationParameter
		DJ_RPLossless rp_lossless(6, 0);
		DJ_RPLossy rp_lossy(m_CompressionQuality);

		// NEW
		
		const DcmRepresentationParameter *rp = NULL;

		if(opt_oxferSyn.getXfer() == EXS_JPEGProcess14SV1TransferSyntax || opt_oxferSyn.getXfer() == EXS_JPEGProcess14TransferSyntax) {
			rp = &rp_lossless;
		}
		else if(opt_oxferSyn.getXfer() == EXS_JPEGProcess1TransferSyntax || opt_oxferSyn.getXfer() == EXS_JPEGProcess2_4TransferSyntax) {
			rp = &rp_lossy;
		}

		// recompress ?
		if(rp != NULL) {
		    if(original_xfer.isEncapsulated()) {
				std::cout << "DICOM file is already compressed, convert to uncompressed xfer syntax first\n";
				if(EC_Normal != dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL)) {
					std::cout << "No conversion from compressed original to uncompressed xfer syntax possible!\n";
				}
			}
		}

		cond = dataset->chooseRepresentation(opt_oxferSyn.getXfer(), rp);
		if(cond.bad()) {
			DimseCondition::dump(cond);
		}

		// OLD

		/*if(opt_oxferSyn.getXfer() == EXS_JPEGProcess14SV1TransferSyntax) {
			cond = dataset->chooseRepresentation(EXS_JPEGProcess14SV1TransferSyntax, &rp_lossless);
			if(cond.bad()) {
				DimseCondition::dump(cond);
			}
		}
		else if(opt_oxferSyn.getXfer() == EXS_JPEGProcess1TransferSyntax) {
			dataset->chooseRepresentation(EXS_JPEGProcess1TransferSyntax, &rp_lossy);
		}
		else if(opt_oxferSyn.getXfer() == EXS_JPEGProcess2_4TransferSyntax) {
		    if(original_xfer.isEncapsulated()) {
				std::cout << "DICOM file is already compressed, convert to uncompressed xfer syntax first\n";
				if(EC_Normal != dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL)) {
					std::cout << "No conversion from compressed original to uncompressed xfer syntax possible!\n";
					return DIMSE_BADDATA;
				}
			}
			std::cout << "recompressing image ..." << std::endl;
			cond = dataset->chooseRepresentation(EXS_JPEGProcess2_4TransferSyntax, &rp_lossy);
			if(cond.bad()) {
				DimseCondition::dump(cond);
			}
		}
		else {
			std::cout << "decompressing image ..." << std::endl;
			dataset->chooseRepresentation(opt_oxferSyn.getXfer(), NULL);
		}*/
	
		if (dataset->canWriteXfer(opt_oxferSyn.getXfer())) {
			std::cout << "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written" << std::endl;
		}
		else {
			std::cout << "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!" << std::endl;
		}
	}

	// store it

	cond = DIMSE_storeUser(
				assoc, 
				presId, 
				&req, 
				NULL, 
				dataset, 
				NULL,
				NULL, 
				(m_timeout == 0) ? DIMSE_BLOCKING : DIMSE_NONBLOCKING,
				m_timeout, 
				&rsp, 
				&statusDetail);
	
	// increase message id
	msgId++;

	// what happened

	if(rsp.DataSetType == DIMSE_DATASET_PRESENT) {
		printf("Response with dataset:\n");
	}
	
	if (statusDetail != NULL) {
		printf("Status detail:\n");
		statusDetail->print(COUT, false);
		delete statusDetail;
	}

	if (cond != DIMSE_NORMAL)
	{
		return cond;
	}

	return (rsp.DimseStatus == STATUS_Success) ? DIMSE_NORMAL : DIMSE_BADDATA;
}

CONDITION DicomAssociation::SendObject(DcmFileFormat *dcmff)
{
	DcmDataset* dataset = dcmff->getDataset();
	return SendObject(dataset);
}

void DicomAssociation::Create(const char *title, const char *peer, int port, const char *ouraet, const char *abstractSyntax)
{
	// no connected association till now
	assoc = NULL;

	// fill in parameters
	if(abstractSyntax != NULL){
		m_abstractSyntax = (char*)abstractSyntax;
	}

	m_calledAET = title;
	m_calledPeer = peer;
	m_calledPort = port;
	
	m_ourAET = ouraet;

	msgId = 0;
	presId = 0;
}

bool DicomAssociation::SendEchoRequest()
{
	DIC_US status;
	DcmDataset *statusDetail = NULL;

	CONDITION cond = DIMSE_echoUser(assoc, msgId, DIMSE_NONBLOCKING, 10,
		&status, &statusDetail);

	if(SUCCESS(cond)) {
		delete statusDetail;
	}

	return SUCCESS(cond);	
}

bool DicomAssociation::AddKey(DcmDataset *query, const DcmTagKey& tag, int value) {
	static char temp[16];
	sprintf(temp, "%i", value);
	return AddKey(query, tag, temp);
}

bool DicomAssociation::AddKey(DcmDataset *query, const DcmTagKey& tag, double value, const char* format) {
	static char temp[16];
	sprintf(temp, format, value);
	return AddKey(query, tag, temp);
}

bool DicomAssociation::AddKey(DcmItem *query, const DcmTagKey& tag, int value) {
	static char temp[16];
	sprintf(temp, "%i", value);
	return AddKey(query, tag, temp);
}

bool DicomAssociation::AddKey(DcmItem *query, const DcmTagKey& tag, double value, const char* format) {
	static char temp[16];
	sprintf(temp, format, value);
	return AddKey(query, tag, temp);
}

bool DicomAssociation::AddKey(DcmDataset *query, const DcmTagKey& t, const char* value) {
	return AddCustomKey/*< DcmDataset >*/(query, t, value);
}

bool DicomAssociation::AddKey(DcmItem *query, const DcmTagKey& t, const char* value) {
	return AddCustomKey/*< DcmItem >*/(query, t, value);
}

bool DicomAssociation::AddQueryLevel(DcmDataset *query, const char *level)
{
	return AddKey(query, DCM_QueryRetrieveLevel, level);
}


void DicomAssociation::OnAddPresentationContext(T_ASC_Parameters *params, const char* transferSyntaxList[], int transferSyntaxListCount)
{

}

DicomNetwork* DicomAssociation::GetNetwork()
{
	return dcmNet;
}

CONDITION DicomAssociation::Connect(DicomNetwork *network, int lossy)
{
	dcmNet = network;
	return network->ConnectAssociation(this, lossy);
}

const char* DicomAssociation::GetOurAET() {
	return m_ourAET;
}

void DicomAssociation::SetPatientData(
			DcmDataset* dset,
			const char* PatientsName,
			const char* PatientID,
			const char* PatientsBirthDate,
			const char* PatientsSex)
{
	// PatientsName
	DicomAssociation::AddKey(dset, DCM_PatientsName, PatientsName);

	// PatientID
	DicomAssociation::AddKey(dset, DCM_PatientID, PatientID);

	// PatientBirthDate
	DicomAssociation::AddKey(dset, DCM_PatientsBirthDate, PatientsBirthDate);

	// PatientSex
	DicomAssociation::AddKey(dset, DCM_PatientsSex, PatientsSex);

}

const char* DicomAssociation::GetKey(DcmDataset* query, const DcmTagKey& tag) {
	OFString val;
	static char buffer[129];
	query->findAndGetOFString(tag, val, 0, OFTrue);
	strcpy(buffer, val.c_str());
	return buffer;
}

void DicomAssociation::SetSOPInstanceUID(DcmDataset* dset, const char* sop) {
	AddKey(dset, DCM_SOPInstanceUID, sop);
}

void DicomAssociation::SetTimeout(int t) {
	m_timeout = t;
}

int DicomAssociation::GetTimeout() {
	return m_timeout;
}

void DicomAssociation::SetCompressionQuality(int q) {
	m_CompressionQuality = q;
}
	
int DicomAssociation::GetCompressionQuality() {
	return m_CompressionQuality;
}

void DicomAssociation::SetProposeCompression(bool propose) {
	m_ProposeCompression = propose;
}

bool DicomAssociation::GetProposeCompression() {
	return m_ProposeCompression;
}
