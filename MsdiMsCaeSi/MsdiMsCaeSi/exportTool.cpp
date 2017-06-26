#include "MsdiMsCaeSipch.h"

DPoint3d exportTool::thePoint = DPoint3d::FromXYZ(0.0, 0.0, 0.0);


bool	exportTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	//����ѡ�е�����
	exportTool::ExportSelect_toParasolid(GetElementAgenda());
	GetElementAgenda().clear();
	return true;
}

bool    exportTool::_WantAdditionalLocate(DgnButtonEventCP ev)
{

	if (NULL == ev)
		return true; // This is a multi-locate tool...

	return ev->IsControlKey();
}
EditElementHandleP  exportTool::_BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev)
{
	EditElementHandle eeh(path->GetHeadElem(), path->GetRoot());
	return GetElementAgenda().Insert(eeh);
}

void   exportTool::_OnRestartTool()//������������
{
	SelectionSetManager::GetManager().EmptyAll();
}
void exportTool::_OnPostInstall()
{
	__super::_OnPostInstall();
	mdlAccuDraw_setEnabledState(false); // Don't enable AccuDraw w/Dynamics...
}

void exportTool::InstallNewInstance(int toolId)//���߿�ʼ���а�װ
{
	exportTool* tool = new exportTool(toolId);
	tool->InstallTool();
}

int  exportTool::xmtWrite_fileCreateDialog(int *xmtVersionP, int *masterUnitsP, BeFileNameR  fileNameP)
{
	
	int             status;
	WChar         suggestName[256], dialogTitle[256];
	SymbolSet     *setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX, 0, TRUE);

	mdlResource_loadFromStringList(dialogTitle, NULL, STRINGLISTID_XMTMessages, MESSAGE_WriteXMTDialogTitle);//��string���л�ȡ�Ի�������

	mdlFile_parseName(tcb->dgnfilenm, NULL, NULL, suggestName, NULL);

	char tempc[256];
	size_t len = wcslen(suggestName) * 2 + 1;
	BeStringUtilities::WCharToCurrentLocaleChar(tempc, suggestName, len);
	strcat(tempc, ".x_t");
	len = sizeof(tempc) + 1;
	BeStringUtilities::CurrentLocaleCharToWChar(suggestName, tempc, len);

	*xmtVersionP = 0;

	mdlDialog_publishBasicPtr(setP, mdlCExpression_getType(TYPECODE_LONG), "xmtVersionP", &xmtVersionP);
	mdlDialog_publishBasicPtr(setP, mdlCExpression_getType(TYPECODE_LONG), "masterUnitsP", &masterUnitsP);

	status = mdlDialog_defFileCreate(fileNameP, NULL, DIALOGID_ExportXMT,
		suggestName, L"*.x_t", tcb->dgnfilenm, dialogTitle,
		DEFXMTWRITE_ID, NULL);
	mdlCExpression_freeSet(setP);

	return status;
}
void exportTool::GetTheUnit(int masterUnits, DgnPlatform::StandardUnit& Lastmasterunits)
{
	switch (masterUnits)
	{
	case 0:
		Lastmasterunits = DgnPlatform::StandardUnit::MetricMeters;
		break;
	case 1:
		Lastmasterunits = DgnPlatform::StandardUnit::MetricKilometers;
		break;
	case 2:
		Lastmasterunits = DgnPlatform::StandardUnit::MetricCentimeters;
		break;
	case 3:
		Lastmasterunits = DgnPlatform::StandardUnit::MetricMillimeters;
		break;
	case 4:
		Lastmasterunits = DgnPlatform::StandardUnit::MetricMicrometers;
		break;
	case 5:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishMiles;
		break;
	case 6:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishYards;
		break;
	case 7:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishFeet;
		break;
	case 8:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishInches;
		break;
	case 9:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishMils;
		break;
	case 10:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishMicroInches;
		break;
	case 11:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishSurveyMiles;
		break;
	case 12:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishSurveyFeet;
		break;
	case 13:
		Lastmasterunits = DgnPlatform::StandardUnit::EnglishSurveyInches;
		break;
	}
}
void exportTool::AllSelectTo_ListFunction(MSElementDescrP edP, DgnModelRefP modelRef,
	TAG_ENTITY_LIST  *bodyListP, NON_ENTITY_LIST *templateListP
	, NON_ENTITY_LIST *transListP, MSElementDescr *remainingEdP)
{
	mdlSolid_beginCurrTrans(modelRef);
	if (SUCCESS == mdlSolid_elementToBodyList(&bodyListP, &templateListP, &transListP,
		mdlSolid_isModelerElement(edP) ? NULL : &remainingEdP,edP, modelRef, true, true))
	{
		mdlSolid_endCurrTrans();
	}
}
void exportTool::ExportSelect_toParasolid(ElementAgendaR theAgenda)//��ѡ�е��ļ�����
{
	//��������������������ж�Ԫ���Ƿ�����ĳ��������
	
	int xmtVersion, masterUnits;
	xmtVersion = 0;//�汾��Ϣ
	masterUnits = 0;//������λ
	int status;
	status = 0;
	BeFileName fileName;//������ļ�·��������
	double Lastmasterunits;//����ת��ֵ
	Lastmasterunits = 1.0;

	//�򿪵����Ի������õ�������
	status = exportTool::xmtWrite_fileCreateDialog(&xmtVersion, &masterUnits, fileName);//�򿪵���Parasolid�ļ��Ի���
	if (status != SUCCESS)
	{
		return;
	}
	//�������
	theAgenda.ClearHilite();

	//��ȡ��λת����ϵ
	DgnPlatform::StandardUnit LastmasterunitsU;
	exportTool::GetTheUnit(masterUnits,LastmasterunitsU);
	//mdlUnits_getFromName (&gXmtUserPrefs.exportImportUnitInfo, L"Meters", TRUE);
	
	//��ȡ�汾��Ϣ
	DgnPlatform::UnitInfo  pMasterUnitInfo;
	mdlModelRef_getMasterUnit(ACTIVEMODEL, &pMasterUnitInfo);
	DgnPlatform::UnitInfo  pSelectUnitInfo;
	mdlUnits_getStandard(&pSelectUnitInfo,LastmasterunitsU);
	mdlUnits_getConversionFactor(&Lastmasterunits, &pSelectUnitInfo, &pMasterUnitInfo);

	//����X_T��Ҫ�õ����м����
	TAG_ENTITY_LIST  *bodyListP = NULL;
	NON_ENTITY_LIST *templateListP = NULL, *transformListP = NULL;
	BODY_TAG         bodyP = 0L;
	Transform        *transformP = NULL;
	MSElementDescr   *templateEdP = NULL;
	MSElementDescr   *remainingEdP = NULL;

	mdlSolid_listCreate(&bodyListP);
	mdlSolid_listCreateNonEntity(&templateListP);
	mdlSolid_listCreateNonEntity(&transformListP);
	Transform       m_transform;
	mdlSolid_beginCurrTrans(MASTERFILE);

	//����ѡ�е�������е�������
	EditElementHandleP start = theAgenda.GetFirstP();
	EditElementHandleP last = start + theAgenda.GetCount();
	for (EditElementHandleP epEDT = start; epEDT < last; epEDT++)
	{
		DgnModelRefP modelRef = epEDT->GetModelRef();
		ElementRefP  eP = epEDT->GetElementRef();
		ElementHandle theHandle(eP, modelRef);
		MSElementDescrP edP = epEDT->GetElementDescrP();
		if (epEDT == start)//��¼��һ��Ԫ�ص��м��
		{
			exportTool::thePoint.x = (edP->el.hdr.dhdr.range.xlowlim + edP->el.hdr.dhdr.range.xhighlim) / 2.0;
			exportTool::thePoint.y = (edP->el.hdr.dhdr.range.ylowlim + edP->el.hdr.dhdr.range.yhighlim) / 2.0;
			exportTool::thePoint.z = (edP->el.hdr.dhdr.range.zlowlim + edP->el.hdr.dhdr.range.zhighlim) / 2.0;
		}
		//����ƫ�Ƶ�ԭ��ķ���ʸ����Ϣ�����ھ��������峬��PARASOLID���Ե����ķ�Χ��������Ҫ��������ƫ�Ƶ�ԭ���ٵ�����
		DPoint3d      vec = DPoint3d::FromXYZ(exportTool::thePoint.x, exportTool::thePoint.y, exportTool::thePoint.z);
		DPoint3d      origin_s = DPoint3d::FromXYZ(0, 0, 0);
		vec.DifferenceOf(origin_s, vec);

		//���Ԫ��Ϊ106Ԫ�ز����������������Ԫ�أ��ж�Ϊ���Ԫ��
		if (edP->el.ehdr.type == 106 && NamedGroup::AnyGroupContains(eP))//�Զ���Ԫ��ʱ��ɢ�����
		{
			//���Ԫ����Ҫ�ȴ�ɢ���ٶԴ�ɢ��������е���
			ElementAgenda dropGeom;
			DropGeometry* geometry = DropGeometry::Create().get();
			geometry->SetOptions(DropGeometry::Options::OPTION_AppData);
			StatusInt theState;
			theState = epEDT->GetDisplayHandler()->Drop(theHandle, dropGeom, *geometry);
			size_t theCount = 0;
			if (theState == SUCCESS)
			{
				//�Դ�ɢ��ĵ���������е���
				theCount = dropGeom.GetCount();
				EditElementHandleP start = dropGeom.GetFirstP();
				EditElementHandleP last = start + dropGeom.GetCount();
				for (EditElementHandleP elementToModify = start; elementToModify < last; elementToModify++)
				{
					MSElementDescrP element_edP;
					element_edP = elementToModify->GetElementDescrP();

					m_transform.InitFrom(vec);
					TransformInfo tInfo(m_transform);
					elementToModify->GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(*elementToModify, tInfo);

					//Ҫ�õ������������ANSYS����ʾһ������Ҫת��������
					m_transform = Transform::FromRowValues//y,zֵ����(y=-z,z=y)
						(
						1.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, -1.0, 0.0, 0.0
						);
					TransformInfo tInfo2(m_transform);
					elementToModify->GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(*elementToModify, tInfo2);
					//��ʵ��ȫ��ת��ΪBODY
					exportTool::AllSelectTo_ListFunction(elementToModify->GetElementDescrP(), modelRef, bodyListP, templateListP, transformListP, remainingEdP);
					if (remainingEdP)
						mdlElmdscr_freeAll(&remainingEdP);
					mdlElmdscr_freeAll(&edP);
				}
			}

		}
		else//��������������Ԫ��
		{
			//����ƫ�Ʋ���
			m_transform.InitFrom(vec);
			TransformInfo tInfo(m_transform);
			epEDT->GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(*epEDT, tInfo);

			m_transform = Transform::FromRowValues//y,zֵ����(y=-z,z=y)
				(
				1.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, -1.0, 0.0, 0.0
				);
			TransformInfo tInfo2(m_transform);
			epEDT->GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(*epEDT, tInfo2);
			//ת��Ϊbody��������
			exportTool::AllSelectTo_ListFunction(edP, modelRef, bodyListP, templateListP, transformListP, remainingEdP);
			if (remainingEdP)
				mdlElmdscr_freeAll(&remainingEdP);
			mdlElmdscr_freeAll(&edP);
		}
	}

	mdlSolid_endCurrTrans();
	//��ת�����bodyת����ʵ��ռ�
	int nSelected2 = 0;
	mdlSolid_listCount(&nSelected2, bodyListP);
	for (int i = 0; SUCCESS == mdlSolid_listNthEl(&bodyP, bodyListP, i) &&
		SUCCESS == mdlSolid_listNthNonEntity((void**)&transformP, transformListP, i) &&
		SUCCESS == mdlSolid_listNthNonEntity((void**)&templateEdP, templateListP, i); i++)
	{
		DgnModelRefP    modlRef;

		modlRef = mdlModelRef_getActive();
		mdlSolid_beginCurrTrans(modlRef);
		if (mdlModelRef_isReference(modlRef))
		{
			double brepScale;
			brepScale = 1.0;
			Transform tMatrix;
			mdlTMatrix_referenceToMaster(&tMatrix, modlRef);

			if (transformP)
			{
				mdlTMatrix_multiply(&tMatrix, &tMatrix, transformP);
			}
			mdlSolid_applyTransform(bodyP, &tMatrix);
		}
		else if (transformP)
		{
			mdlSolid_applyTransform(bodyP, transformP);
		}
		if (transformP)
			dlmSystem_mdlFree(transformP);

		mdlElmdscr_freeAll(&templateEdP);
		mdlSolid_endCurrTrans();
	}
	mdlSolid_beginCurrTrans(MASTERFILE);

	//��������б������ź͵�������
	int nSelected = 0;
	if (SUCCESS == mdlSolid_listCount(&nSelected, bodyListP) && nSelected)//����֮ǰ����
	{
		double          scale = 1, totScale = 1;
		bool         alerted = false;
		Transform       scaleMatrix;
		scale = Lastmasterunits;
		mdlCnv_masterToUOR(&totScale, 1.0, MASTERFILE);
		mdlCurrTrans_invScaleDoubleArray(&totScale, &totScale, 1);
		totScale /= scale;
		totScale = 1 / totScale;
		mdlTMatrix_getIdentity(&scaleMatrix);
		mdlTMatrix_scale(&scaleMatrix, &scaleMatrix, totScale, totScale, totScale);

		for (int i = 0; SUCCESS == mdlSolid_listNthEl(&bodyP, bodyListP, i); i++)
		{
			mdlSolid_applyTransform(bodyP, &scaleMatrix);
			mdlSolid_deleteNodeIdAttributes(bodyP);

			if (!alerted && SUCCESS != mdlSolid_checkBody(bodyP, FALSE, FALSE, TRUE))
			{
				WString message;
				mdlOutput_rscsPrintf(message, 0L, STRINGLISTID_XMTMessages, MESSAGE_InvalidBodySize);
				mdlDialog_openInfoBox(message.data());

				alerted = true;
			}
		}

		mdlSolid_saveParts(fileName.GetWCharCP(), TRUE, 0, bodyListP);//������Parasolid			
	}
	else//������û��ʵ��
	{
		WString TESTL = L"no entity!";
		mdlDialog_openInfoBox(TESTL.GetWCharCP());
	}

	if (NULL != bodyListP)
		mdlSolid_listDelete(&bodyListP);
	if (NULL != templateListP)
		mdlSolid_listDeleteNonEntity(&templateListP);
	if (NULL != transformListP)
		mdlSolid_listDeleteNonEntity(&transformListP);
	if (mdlSelect_isActive())
	{
		mdlSelect_freeAll();//�ͷ�����ѡ��

	}
	if (WORKMODE_DWG == mdlSystem_getWorkmode() && mdlCapability_isEnabled(CAPABILITY_PARASOLID))//�ر�PARASOLID
	{
		mdlCapability_set(CAPABILITY_PARASOLID, FALSE);
	}
	mdlSolid_endCurrTrans();
	mdlLocate_restart(TRUE);
}
