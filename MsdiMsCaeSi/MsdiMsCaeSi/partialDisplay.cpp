#include "MsdiMsCaeSipch.h"

//pd:partial display
bvector<int> pd_id_Ma, pd_id_Et, pd_id_Rc;
bvector<WString> pd_val_Ma, pd_val_Et, pd_val_Rc;

extern bvector<Condition> pd_ConditionLst;

void showMeshsOfInstance(int iProjectId, WString NGName, bvector<ElementId> meshIDs, bool traversalAll)
{
	//����ָ���������µ�����Ԫ�أ�meshIDs�е�Ԫ����ʾ(ͬʱ��¼��XAttribution�б����ElemID����)����������
	bvector<WString> elemIDs;
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

	//��������������ȡ��Ӧ����ʾ�������ز���
	WString LvName = NGName + L"HIDE";
	LevelId lvID_show = 0, lvID_hide = 0;
	mdlLevel_getIdFromName(&lvID_hide, MASTERFILE, 0, LvName.data());
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, NGName.data());

	NamedGroupPtr namedGroup = NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data());
	if (namedGroup.IsValid())
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);

		for (UInt32 i = 0; i < graphicMembers; i++)
		{
			ElementId elemid = namedGroup->GetMember(i)->GetElementId();
			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
			{
				LevelId elemlv = eeh.GetElementDescrP()->el.ehdr.level;
				//����Ҫ����������������Ԫ���򲻽��й���,����������������д�����ʾ���Ԫ��
				if (!traversalAll && elemlv != lvID_show)
					continue;

				//����Ԫ�طŵ����ز���
				if (elemlv != lvID_hide)
				{
					eeh.GetElementDescrP()->el.ehdr.level = lvID_hide;
					eeh.ReplaceInModel(eeh.GetElementRef());
				}
			}
		}

		// Ԫ����ʾ
		for (auto iter = meshIDs.begin(); iter != meshIDs.end(); iter++)
		{
			/*cout << (*iter) << endl;*/
			EditElementHandle eeh;
			eeh.FindByID(*iter, MASTERFILE);

			if (eeh.IsValid())
			{
				//��ȡ��Ԫ�ص�XAttribution�����ݿ��Ӧ��ElemID
				XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
				MeshProperty *meshPro = NULL;

				if (xh.IsValid())
				{
					meshPro = (MeshProperty *)xh.PeekData();

					//��¼���ݿ��ж�Ӧ��ElemID
					if (meshPro != NULL)
					{
						WString ElemID_DB = num2Ws(meshPro->ElemID);
						elemIDs.push_back(ElemID_DB);
					}
				}

				eeh.GetElementDescrP()->el.ehdr.level = lvID_show;
				eeh.ReplaceInModel(eeh.GetElementRef());
			}
		}
	}

	showMeshsOfInstance_DB(iProjectId, NGName, elemIDs);
}

void showAllOfInstance(int iProjectId, WString NGName)
{
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

	//��������������ȡ��Ӧ����ʾ��
	LevelId lvID_show = 0;
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, NGName.data());

	NamedGroupPtr namedGroup = NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data());
	if (namedGroup.IsValid())
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);

		for (UInt32 i = 0; i < graphicMembers; i++)
		{
			ElementId elemid = namedGroup->GetMember(i)->GetElementId();

			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
			{
				//����Ԫ�طŵ���ʾ����
				if (eeh.GetElementDescrP()->el.ehdr.level != lvID_show)
				{
					eeh.GetElementDescrP()->el.ehdr.level = lvID_show;
					eeh.ReplaceInModel(eeh.GetElementRef());
				}
			}
		}
	}
	showAllOfInstance_DB(iProjectId, NGName);
}

void deleteCAEInstance(WString NGName)
{
	//ɾ����ʵ����Ӧ����ʾ��/���ز�
	WString levelNameDeleted = NGName;
	DgnPlatform::LevelId levelId;

	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, levelNameDeleted.data()))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}
	levelNameDeleted.append(L"HIDE");
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, levelNameDeleted.data()))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}

	//ɾ����ʵ����Ӧ��������
	NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data())->DeleteFromFile();

	//ɾ�����ݿ��е�FACES���Ӧ��¼
	deleteAllFaceOfInstance(NGName);

	//ɾ�����и���������model
	NGName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

	DgnIndexIteratorP   cellIterator = mdlModelIterator_create(mdlDgnFileObj_getMasterFile());
	DgnIndexItemP       IndexItem;
	mdlModelIterator_getFirst(cellIterator);
	mdlModelIterator_setAcceptCellsOnly(cellIterator, FALSE);
	while (NULL != mdlModelIterator_getNext(cellIterator))
	{
		IndexItem = mdlModelIterator_getCurrent(cellIterator);
		WChar wModelName[MAX_CELLNAME_LENGTH];
		//��ȡmodel��
		mdlModelItem_getName(IndexItem, wModelName, MAX_CELLNAME_LENGTH);
		WString wsModelName;
		wsModelName.append(wModelName);

		if (wsModelName.FindI(NGName.data()) != string::npos)
		{
			DgnModelRefP model = NULL;
			mdlModelRef_createWorkingByName(&model, mdlDgnFileObj_getMasterFile(), wModelName, TRUE, TRUE);
			mdlModelRef_deleteModel(model, false);
			mdlModelRef_freeWorking(model);
		}
	}
	mdlModelIterator_free(cellIterator);

	//ɾ��XAttribution�е���ؼ�¼
	bvector<InstanceInfo> infos = readInstanceInfo();
	for (bvector<InstanceInfo>::iterator iter = infos.begin(); iter != infos.end(); iter++)
	{
		if (NGName.CompareTo((*iter).NgName) == 0)
		{
			infos.erase(iter);
			savingInstanceInfo(infos);
			break;;
		}
	}
	
}

bool partialDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	//��ȡ�û������Ԫ��
	EditElementHandleP eeh = GetElementAgenda().GetFirstP();

	//��ȡԪ��������������/����
	WString NGName = getNGNameOfElem(*eeh);

	//��������������ȡ����ID
	int iProjectId;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//���ݹ���ID��ȡ��������Ԫ�����ͺ�ʵ�����Ŀ���ֵ
	//IDֵ
	getMaterialIDByProjectid(iProjectId, pd_id_Ma);
	getElemTypeIDByProjectid(iProjectId, pd_id_Et);
	getRealConsIDByProjectid(iProjectId, pd_id_Rc);
	//��ʾֵ
	pd_val_Ma.clear();
	for (int i = 0; i < (int)pd_id_Ma.size(); i++)
	{
		WString mName, mType, mEX, mNUXY, mDENS, mPRXY;
		getMaterialAllByPK(iProjectId, pd_id_Ma[i], mName, mType, mEX, mNUXY, mDENS, mPRXY);

		//Type-Nameƴ�ɲ�������
		if (mType.length() > 0)
			mType.append(L"-");
		mType.append(mName);

		pd_val_Ma.push_back(mType);
	}
	pd_val_Et.clear();
	for (int i = 0; i < (int)pd_id_Et.size(); i++)
	{
		WString elemTypeName;
		getElemNameByPK(iProjectId, pd_id_Et[i], elemTypeName);
		pd_val_Et.push_back(elemTypeName);
	}
	pd_val_Rc.clear();
	for (int i = 0; i < (int)pd_id_Rc.size(); i++)
	{
		WString realConsProp;
		getRealConsPropByPK(iProjectId, pd_id_Rc[i], realConsProp);
		pd_val_Rc.push_back(realConsProp);
	}

	//�򿪾ֲ���ʾɸѡ�Ի���
	int lastAction = -1;
	mdlDialog_openModal(&lastAction, NULL, DIALOGID_SolidPartShow);

	//���оֲ���ʾ,��û��ɸѡ�����򲻽����κβ���
	if (lastAction == ACTIONBUTTON_OK && !pd_ConditionLst.empty())
	{
		//���������ͬ��
		NamedGroupPtr namedGroup = NamedGroupCollection::Create(*ACTIVEMODEL)->FindByName(NGName.data());

		if (namedGroup.IsValid())
		{
			UInt32 graphicMembers;
			namedGroup->GetMemberCount(&graphicMembers, NULL);

			bvector<ElementId> shows;
			for (UInt32 i = 0; i < graphicMembers; i++)
			{
				ElementId elemid = namedGroup->GetMember(i)->GetElementId();

				EditElementHandle eeh;
				if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
				{
					XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
					XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
					MeshProperty *meshPro = NULL;

					if (xh.IsValid())
						meshPro = (MeshProperty *)xh.PeekData();

					//����ɸѡ�������з���
					bool needShow = true;
					for (int j = 0; j < (int)pd_ConditionLst.size(); j++)
					{
						Condition& condition = pd_ConditionLst[j];
						if (!matchCondition(*meshPro, condition))
						{
							needShow = false;
							break;
						}
					}

					if (needShow)
						shows.push_back(elemid);
				}
			}

			showMeshsOfInstance(iProjectId, NGName, shows, true);
		}	
	}
	mdlState_startDefaultCommand();
	return true;
}

bool fenceDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();
	int cnt = (int)GetElementAgenda().GetCount();
	EditElementHandleP end = curr + GetElementAgenda().GetCount();

	//����ѡ�����±�Ϊ0��Ԫ��ȷ��ʵ����(������/����)
	WString NGName = getNGNameOfElem(*curr);

	LevelId lvID = 0;
	mdlLevel_getIdFromName(&lvID, MASTERFILE, 0, NGName.data());

	//��������������ȡ����ID
	int iProjectID;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//����ѡ�񼯣���ȡѡ��������Ԫ��ID
	bvector<ElementId> idSS;
	for (; curr < end; curr++)
	{
		//������Ԫ����NGName���ڲ��в���Ҫ����
		if (curr->GetElementDescrP()->el.ehdr.level == lvID)
		{
			ElementId id = mdlElement_getID(curr->GetElementP());
			idSS.push_back(id);
		}
	}

	showMeshsOfInstance(iProjectID, NGName, idSS, false);

	mdlState_startDefaultCommand();

	return true;
}

bool recoverDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();

	//����ѡ�����±�Ϊ0��Ԫ��ȷ��ʵ����(������/����)
	WString NGName = getNGNameOfElem(*curr);

	//��������������ȡ����ID
	int iProjestID;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjestID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	showAllOfInstance(iProjestID, NGName);

	mdlState_startDefaultCommand();

	return true;
}

bool deleteDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();

	//����ѡ�����±�Ϊ0��Ԫ��ȷ��ʵ����(������/����)
	WString NGName = getNGNameOfElem(*curr);
	WString instanceName;
	splitNgName_Model(NGName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, instanceName, UNUSE_WSTRING);

	//��������������ȡ��ʵ������������ʾ��Ϣ
	WString msg;
	msg.append(L"ȷ��Ҫɾ��ģ��<");
	msg.append(instanceName.data());
	msg.append(L">�Լ�����ص�������?");
	if (ACTIONBUTTON_OK == mdlDialog_openMessageBox(DIALOGID_MsgBoxOKCancel, msg.data(), MessageBoxIconType::Warning))
	{
		//������������ɾ����ʵ��ģ��
		deleteCAEInstance(NGName);
	}

	//�˳�����
	mdlState_startDefaultCommand();

	return true;
}

void ShowCAESolidPartCmd(WCharCP unparsed)
{
	partialDisplayTool::InstallNewInstance(COMMAND_SHOWCAESOLIDPART);
}

void fenceDisplayCmd(WCharCP unparsed)
{
	//��ѡ��ʾ��Ϊ�������
	SelectionSetManagerR ssr = SelectionSetManager::GetManager();
	int numSelected = (int)ssr.NumSelected();
	//��ѡ�����Ѿ���Ԫ����ֱ�Ӳ���ѡ��
	if (numSelected != 0)
	{
		//����ѡ�����±�Ϊ0��Ԫ��ȷ��ʵ����(������/����)
		ElementRefP ref = NULL;
		DgnModelRefP model = NULL;
		ssr.GetElement(0, &ref, &model);
		EditElementHandle eeh(ref);
		WString NGName = getNGNameOfElem(eeh);

		LevelId lvID = 0;
		mdlLevel_getIdFromName(&lvID, MASTERFILE, 0, NGName.data());

		//��������������ȡ����ID
		int iProjectID;
		splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

		//����ѡ�񼯣���ȡѡ��������Ԫ��ID
		bvector<ElementId> idSS;
		for (int i = 0; i < numSelected; i++)
		{
			ssr.GetElement(i, &ref, &model);
			//������Ԫ����NGName���ڲ��в���Ҫ����
			if (ref->GetLevel() == lvID)
				idSS.push_back(ref->GetElementId());
		}

		showMeshsOfInstance(iProjectID, NGName, idSS, false);
	}
	else
	{
		fenceDisplayTool::InstallNewInstance(COMMAND_FENCEDISPLAY);
	}
}

void recoverDisplayCmd(WCharCP unparsed)
{
	SelectionSetManagerR ssr = SelectionSetManager::GetManager();
	int numSelected = (int)ssr.NumSelected();
	//ѡ�����Ѿ���Ԫ����ֱ�Ӳ���ѡ��
	if (numSelected != 0)
	{
		//����ѡ�����±�Ϊ0��Ԫ��ȷ��ʵ����(������/����)
		ElementRefP ref = NULL;
		DgnModelRefP model = NULL;
		ssr.GetElement(0, &ref, &model);
		EditElementHandle eeh(ref);
		WString NGName = getNGNameOfElem(eeh);

		//��������������ȡ����ID
		int iProjectID;
		splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

		showAllOfInstance(iProjectID, NGName);
	}
	else
	{
		recoverDisplayTool::InstallNewInstance(COMMAND_RECOVERDISPLAY);
	}
}

void deleteDisplayCmd(WCharCP unparsed)
{
	deleteDisplayTool::InstallNewInstance(COMMAND_DELETEDISPLAY);
}
