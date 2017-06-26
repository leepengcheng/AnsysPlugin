#include "MsdiMsCaeSipch.h"

#define PROJECT_ID_SELECTED g_expCalculationInfo.proIndex == -1 ? -1 : readInstanceInfo().at(g_expCalculationInfo.proIndex).projectId

extern bool writeCalculationDocument(const char path[MAXFILELENGTH]);

//���������ɴ�����ؿؼ���Ӧ�ṹ��
extern ExpCalculationDlgInfo g_expCalculationInfo;

//���治ͬ����ͼƬ·��������
list<wstring> lstModelPaths;			//��������ģ��ͼ
list<wstring> lstWYCloudPaths;			//λ����ͼ&��ֵ��ͼ
list<wstring> lstYLCloudPaths;			//Ӧ����ͼ&��ֵ��ͼ
list<wstring> lstYLPaths;				//Ӧ����ֵͼ

//������ListBox��Ӧ����
ListBoxDatas	lbd_CaseList;
ListBoxDatas	lbd_ModelParamList;
ListBoxDatas	lbd_WYParamList;
ListBoxDatas	lbd_YLParamList;
ListBoxDatas	lbd_PJParamList;

//����������Ի���Ĺ��Ӻ���
void HookDialog_expCalc(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
		case DIALOG_MESSAGE_INIT:
		{
			//����ڴ��в������������
			lstModelPaths.clear();
			lstWYCloudPaths.clear();
			lstYLCloudPaths.clear();
			lstYLPaths.clear();

			strcpy(g_expCalculationInfo.proDescr, "");

			mdlDialog_itemsSynch(dmP->db);
		}
		break;
		default:
			dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExpCalc_Import(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		BeFileName	path;
		if (SUCCESS == mdlDialog_fileOpen(path, NULL, 0, L"", L"*.docx", L"d:\\", L"��ѡ�񹤳̸����ĵ�"))
		{			
			ifstream in(ws2s(path.GetName()).data());
			string buffer;
			int index = 0;
			bool outOfRange = false;
			while (!in.eof())
			{
				in.get(g_expCalculationInfo.proDescr[index++]);				
				if (index == 4090)
					break;			
			}

			g_expCalculationInfo.proDescr[index++] = '\0';
			WString wsProDescr;
			wsProDescr.AppendA(g_expCalculationInfo.proDescr);
			DialogItem	*dip = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_MultilineText, MLTEXTID_ExportCalc_ProjectDescr, 0);
			MSValueDescr value;
			value.SetWChar(wsProDescr.data());
			mdlDialog_rItemValueSet(NULL, value, dip->rawItemP);
			mdlDialog_itemsSynch(dimP->db);
		}		
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

//����ͼƬ·����ť�Ĺ��Ӻ���
void HookPushBtn_ExpCalc_AddPath(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON:
		{
			//���ļ�ѡ��Ի����û�ѡ��
			//�û�ѡȡExcel�ļ�·��
			BeFileName    path;

			if (SUCCESS == mdlDialog_fileOpen(path, NULL, 0, L"", L"*.bmp", L"d:\\", L"��ѡ��ͼƬ��Դ����"))
			{
				//���ݹ��Ӻ�������Դ�ж϶���һ��LISTBOX���в���
				list<wstring>* optLstR = NULL;
				RscId btnID = dimP->dialogItemP->GetId();
				RscId lBoxID = 0;
				switch (btnID)
				{
				case PUSHBTNID_ExportCalc_AddModelPath:
					optLstR = &lstModelPaths;
					lBoxID = LISTBOXID_ExportCalc_ModelPaths;
					break;
				case PUSHBTNID_ExportCalc_AddWYCloudPath:
					optLstR = &lstWYCloudPaths;
					lBoxID = LISTBOXID_ExportCalc_WYCloudPaths;
					break;
				case PUSHBTNID_ExportCalc_AddYLCloudPath:
					optLstR = &lstYLCloudPaths;
					lBoxID = LISTBOXID_ExportCalc_YLCloudPaths;
					break;
				case PUSHBTNID_ExportCalc_AddYLPath:
					optLstR = &lstYLPaths;
					lBoxID = LISTBOXID_ExportCalc_YLPaths;
					break;
				default:
					break;
				}

				//��ȡ��ӦlistBox��listModel
				Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lBoxID, 0);
				if (listBox != NULL)
				{
					ListModel*	pListModel = mdlDialog_listBoxGetListModelP(listBox->GetRawItem());
					if (pListModel == NULL)
						pListModel = mdlListModel_create(MAX_COLUMN_COUNT);

					//��Ҫ�õ�listBox��������ȷ����һ����ŵ�ֵ
					int rowCnt = mdlListModel_getRowCount(pListModel);

					//�������У���һ��Ϊ��ţ��ڶ���ΪͼƬ·��
					MSValueDescr cellValue;
					ListRowP row = mdlListRow_create(pListModel);
					ListCellP cell = mdlListRow_getCellAtIndex(row, 0);
					wstring rowNO = num2Ws(rowCnt + 1).data();
					cellValue.SetWChar(rowNO.data());
					mdlListCell_setValue(cell, cellValue, TRUE);

					cell = mdlListRow_getCellAtIndex(row, 1);
					cellValue.SetWChar(path.GetName());
					mdlListCell_setValue(cell, cellValue, TRUE);

					mdlListModel_addRow(pListModel, row);

					//����ListBox����ʾ
					mdlDialog_listBoxSetListModelP(listBox->GetRawItem(), pListModel, 0);

					//ͬʱ�����ڴ��ж�Ӧ������
					wstring str_path = path.GetName();
					optLstR->push_back(str_path);
				}
			}
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExcCalc_DelPath(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//���ݹ��Ӻ�������Դ�ж϶���һ��LISTBOX���в���
		list<wstring>* optLstR = NULL;
		RscId btnID = dimP->dialogItemP->GetId();
		RscId lBoxID = 0;
		switch (btnID)
		{
		case PUSHBTNID_ExportCalc_DelModelPath:
			optLstR = &lstModelPaths;
			lBoxID = LISTBOXID_ExportCalc_ModelPaths;
			break;
		case PUSHBTNID_ExportCalc_DelWYCloudPath:
			optLstR = &lstWYCloudPaths;
			lBoxID = LISTBOXID_ExportCalc_WYCloudPaths;
			break;
		case PUSHBTNID_ExportCalc_DelYLCloudPath:
			optLstR = &lstYLCloudPaths;
			lBoxID = LISTBOXID_ExportCalc_YLCloudPaths;
			break;
		case PUSHBTNID_ExportCalc_DelYLPath:
			optLstR = &lstYLPaths;
			lBoxID = LISTBOXID_ExportCalc_YLPaths;
			break;
		default:
			break;
		}

		//��ȡ��ӦlistBox��listModel
		Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lBoxID, 0);
		if (listBox != NULL)
		{
			ListModel*	pListModel = mdlDialog_listBoxGetListModelP(listBox->GetRawItem());
			if (pListModel != NULL)
			{
				//��ȡָ��listBox��ǰѡ�������
				int row = -1, col = -1;
				mdlDialog_listBoxGetLocationCursor(&row, &col, listBox->rawItemP);

				if (row != -1)
				{
					mdlListModel_removeRowAtIndex(pListModel, row, true);
					mdlDialog_listBoxSetListModelP(listBox->rawItemP, pListModel, 0);
					mdlDialog_itemsSynch(dimP->db);

					list<wstring>::iterator iter = (*optLstR).begin();
					advance(iter, row);
					(*optLstR).erase(iter);
				}
			}
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExpCalc_Submit(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON:
		{
			//���ɼ����ĵ�
			BeFileName    path;

			if (SUCCESS == mdlDialog_fileCreate(path, NULL, 0, L"cal.docx", L"*.docx", L"d:\\", L"��ѡ����������·��"))
			{
				wstring wsPath = path.GetName();
				
				if (writeCalculationDocument(ws2s(wsPath).data()))
				{
					mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"�����ĵ�������ϣ�", MessageBoxIconType::Information);
				}
				else
				{
					mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"�����ĵ�����ʧ�ܣ�", MessageBoxIconType::Critical);
				}
			}

		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookListBox_ExpCalc_CaseList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->GetRawItem();
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//�������غ����ݳ�ʼ��
		case DITEM_MESSAGE_INIT:
		{
			//��ǰ����ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_CaseList);
		}
		break;
		//��������ʾ��ListBox��
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_CaseList.needFresh)
				initDatasIntoListBox(pThis, lbd_CaseList);
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_ModelParamList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//����ģ�Ͳ��������ݳ�ʼ��
		case DITEM_MESSAGE_INIT:
		{
			//��ǰ����ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_ModelParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_ModelParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_ModelParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_WYParamList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//λ�����ݳ�ʼ��
		case DITEM_MESSAGE_INIT:
		{
			//��ǰ����ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_WYParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_WYParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_WYParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_YLParamList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//Ӧ�����ݳ�ʼ��
		case DITEM_MESSAGE_INIT:
		{		
			//��ǰ����ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_YLParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_YLParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_YLParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_PJParamList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT_REBAR);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//�����������ݳ�ʼ��
		case DITEM_MESSAGE_INIT:
		{			
			//��ǰ����ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_PJParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_PJParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_PJParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookMlText_ExpCalc_ProDescr(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_FOCUSOUT:
		{
			MSValueDescr value;
			mdlDialog_rItemValueGet(value, dimP->dialogItemP->rawItemP);
			wstring ws = value.GetWCharCP();
		
			strcpy(g_expCalculationInfo.proDescr, ws2s(ws).data());
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_ExpCalc_ProjectLists(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_comboBoxSetListModelP(pThis, pListModel);
		}
		break;
		case DITEM_MESSAGE_INIT:
		{

			pListModel = mdlDialog_comboBoxGetListModelP(pThis);

			ListRowP	pRow = NULL;
			ListCellP	cell = NULL;
			MSValueDescr cellValue;
		
			//ʵ���б�comboBox����ʾʵ���������������Ӧ�Ĺ���ID
			bvector<InstanceInfo> instanceInfos = readInstanceInfo();
			for (int i = 0; i < (int)instanceInfos.size(); i++)
			{
				pRow = mdlListRow_create(pListModel);
				cell = mdlListRow_getCellAtIndex(pRow, 0);

				//���ö�Ӧ��ʵ������Ϊ��ʾ
				WString ngName = instanceInfos[i].NgName;
				WString instanceName;
				int projectID;
				splitNgName_Model(ngName, UNUSE_WSTRING, projectID, UNUSE_INT, instanceName, UNUSE_WSTRING);
				mdlListCell_setDisplayText(cell, instanceName.data());

				//��Ӧ�Ĺ���ID������cell��
				cellValue.SetInt32(projectID);
				mdlListCell_setValue(cell, cellValue, FALSE);

				mdlListModel_addRow(pListModel, pRow);
				g_expCalculationInfo.proIndex = 0;
			}

			mdlDialog_comboBoxSetListModelP(pThis, pListModel);

			//**�˴��������������Ի�����Text�ؼ��ĳ�ʼ��
			if (instanceInfos.empty())
				g_expCalculationInfo.proIndex = -1;

			//�ڵ���
			g_expCalculationInfo.nNodeCnt = getNodeCntByProjectid(PROJECT_ID_SELECTED);

			//�ܵ�Ԫ��
			g_expCalculationInfo.nCellCnt = getElemCntByProjectid(PROJECT_ID_SELECTED);

			mdlDialog_itemsSynch(dimP->db);
		}
		break;
		case DITEM_MESSAGE_STATECHANGED:
		{
			//�ı��ؼ�ˢ��
			//�ڵ���
			g_expCalculationInfo.nNodeCnt = getNodeCntByProjectid(PROJECT_ID_SELECTED);

			//�ܵ�Ԫ��
			g_expCalculationInfo.nCellCnt = getElemCntByProjectid(PROJECT_ID_SELECTED);

			//���³�ʼ��ListBoxData���ݲ�ǿ��draw
			for (RscId lb = LISTBOXID_ExportCalc_CaseList; lb <= LISTBOXID_ExportCalc_PJParamList; lb++)
			{
				initDatasFromDB(PROJECT_ID_SELECTED, lb);
				Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lb, 0);
				if (listBox != NULL)
					mdlDialog_itemDraw(dimP->db, listBox->itemIndex);
			}

			mdlDialog_itemsSynch(dimP->db);
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void initDatasIntoListBox(RawItemHdrP listBox, ListBoxDatas& datas)
{
	if (listBox == NULL)
		return;

	//ִ�и���ListBox����������Ӧlbd����λ��Ϊfalse��ֹ���޵ݹ�
	datas.needFresh = false;

	//����RawItem��ȡ��Ӧ��ListModelͬʱ���������
	ListModelP pListModel = mdlDialog_listBoxGetListModelP(listBox);
	if (pListModel == NULL)
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
	else
		mdlListModel_empty(pListModel, true);

	if (datas.rows.empty())
		return;

	//����datas�е�������䵽ListModel��
	ListRowP	row = NULL;
	ListCellP	cell = NULL;
	MSValueDescr cellValue;

	for (int i = 0; i < (int)datas.rows.size(); i++)
	{
		bvector<WString>& rowData = datas.rows[i];

		row = mdlListRow_create(pListModel);

		for (int j = 0; j < (int)rowData.size(); j++)
		{
			cell = mdlListRow_getCellAtIndex(row, j);
			cellValue.SetWChar(rowData[j].data());
			mdlListCell_setValue(cell, cellValue, TRUE);
		}

		mdlListModel_addRow(pListModel, row);
	}

	//����ListBox����ʾ
	mdlDialog_listBoxSetListModelP(listBox, pListModel, 0);

}

void initDatasFromDB(int iProjectID, RscId listBoxID)
{
	switch (listBoxID)
	{
		case LISTBOXID_ExportCalc_CaseList:
		{
			lbd_CaseList.rows.clear();

			//��ѯ����ID�µĹ���������Ҫ�غɣ�Property��
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			//��������䵽�ṹ����
			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.������
				rowData.push_back(vecCaseNames[i].data());

				//2.��Ҫ�غ�
				rowData.push_back(vecCaseProps[i].data());

				lbd_CaseList.rows.push_back(rowData);
			}

			//��ʼ����һ��ˢ��
			lbd_CaseList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_ModelParamList:
		{
			lbd_ModelParamList.rows.clear();

			//��ѯ����ID�µ����в���ID
			bvector<int> vecMaterialIDs;
			getMaterialIDByProjectid(iProjectID, vecMaterialIDs);

			//�������в���ID��������Ӧ����
			for (int i = 0; i < (int)vecMaterialIDs.size(); i++)
			{
				int mID = vecMaterialIDs[i];

				//���ݲ���ID�빤��ID��ȡ��Ӧ���ϵ�����
				WString mName, mType, mEX, mNUXY, mDENS, mPRXY;
				getMaterialAllByPK(iProjectID, mID, mName, mType, mEX, mNUXY, mDENS, mPRXY);

				//����ù�����ʹ�øò���ID�ĵ�Ԫ��
				int ElemCnt = getElemCntUsingMaterialByProjectid(iProjectID, mID);

				bvector<WString> rowData;

				//1.���Ϻ�
				WString mNumber = num2Ws(mID);
				rowData.push_back(mNumber);

				//2.��������
				//Type-Nameƴ�ɲ�������
				if (mType.length() > 0)
					mType.append(L"-");
				mType.append(mName);
				rowData.push_back(mType);

				//3.��Ԫ��
				WString wsElemCnt = num2Ws(ElemCnt);
				rowData.push_back(wsElemCnt);

				//4.����ģ��EX
				rowData.push_back(mEX);

				//5.���ɱ�NUXY
				rowData.push_back(mNUXY);

				//6.�ܶ�DENS
				rowData.push_back(mDENS);

				lbd_ModelParamList.rows.push_back(rowData);
			}

			//��ʼ����һ��ˢ��
			lbd_ModelParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_WYParamList:
		{
			lbd_WYParamList.rows.clear();

			//��ѯ����ID�µĹ���������Ҫ�غɣ�Property��
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			bvector<int> nodeLst;
			double maxValue;
			wostringstream oss;

			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.������
				rowData.push_back(vecCaseNames[i].data());
				
				//2.����λ��(��ֵ&�ڵ��)
				WString maxWY_SUM;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "USUM");
				if (nodeLst.front() == -1)
					maxWY_SUM = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxWY_SUM.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_SUM);

				//3.���X��λ��
				WString maxWY_X;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UX");
				if (nodeLst.front() == -1)
					maxWY_X = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxWY_X.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_X);

				//4.���Y��λ��
				WString maxWY_Y;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UY");
				if (nodeLst.front() == -1)
					maxWY_Y = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxWY_Y.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_Y);

				//5.���Z��λ��
				WString maxWY_Z;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UZ");
				if (nodeLst.front() == -1)
					maxWY_Z = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxWY_Z.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_Z);

				lbd_WYParamList.rows.push_back(rowData);
			}

			//��ʼ����һ��ˢ��
			lbd_WYParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_YLParamList:
		{
			lbd_YLParamList.rows.clear();

			//��ѯ����ID�µĹ���������Ҫ�غɣ�Property��
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			bvector<int> nodeLst;
			double maxValue;
			wostringstream oss;

			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.������
				rowData.push_back(vecCaseNames[i].data());

				//2.X�����Ӧ��(��ֵ&�ڵ��)
				WString maxYL_X;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SX");
				if (nodeLst.front() == -1)
					maxYL_X = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxYL_X.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_X);

				//3.Y�����Ӧ��
				WString maxYL_Y;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SY");
				if (nodeLst.front() == -1)
					maxYL_Y = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxYL_Y.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_Y);

				//4.Z�����Ӧ��
				WString maxYL_Z;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SZ");
				if (nodeLst.front() == -1)
					maxYL_Z = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxYL_Z.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_Z);

				//5.��һ��Ӧ��
				WString maxYL_1;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "S1");
				if (nodeLst.front() == -1)
					maxYL_1 = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxYL_1.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_1);

				//6.������Ӧ��
				WString maxYL_3;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "S3");
				if (nodeLst.front() == -1)
					maxYL_3 = L" ";
				else
				{
					oss << scientific << maxValue << L"\n�ڵ�ţ�" << nodeLst.front();
					maxYL_3.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_3);

				lbd_YLParamList.rows.push_back(rowData);
			}
			//��ʼ����һ��ˢ��
			lbd_YLParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_PJParamList:
		{
			lbd_PJParamList.rows.clear();

			//ȡ�����е������
			bmap<WString, ReBarInfo> allRebarResults;
			readRebarResultsFromX(allRebarResults);

			//����iProjectID���Ҷ�Ӧ���
			bmap<WString, ReBarInfo> currentResults;
			for (auto it = allRebarResults.begin(); it != allRebarResults.end();++it)
			{
				if (iProjectID == it->second.projectID)
				{
					currentResults[it->first] = it->second;
				}
			}
			allRebarResults.empty();

			for (auto it = currentResults.begin(); it != currentResults.end(); ++it)
			{
				ReBarInfo info = it->second;
				WString value;
				bvector<WString> rowData;

				//1 ������
				rowData.push_back(info.caseName);
				//2 ������
				rowData.push_back(info.sectionName);
				//3 �������
				rowData.push_back(info.rebarName);
				//4 �ṹϵ��
				value.Sprintf(L"%.2g", info.dStruFt);
				rowData.push_back(value);
				//5 �ṹ��Ҫ��ϵ��
				value.Sprintf(L"%.2g", info.dStruSignifyFt);
				rowData.push_back(value);
				//6 ���״��ϵ��
				value.Sprintf(L"%.2g", info.dDesignFt);
				rowData.push_back(value);
				//7 �ֽ��ǿ�� 
				value.Sprintf(L"%.3g", info.dRebarTesion);
				rowData.push_back(value);
				//8 ����������ǿ��
				value.Sprintf(L"%.3g", info.dConcrTesion);
				rowData.push_back(value);
		
				bvector<WString> vResult;
				BeStringUtilities::Split(it->second.strResult, SeparatorAnd, vResult);
				size_t resultNumber = vResult.size();
				if (resultNumber > 13)
				{
					//9 ����Ӧ����� 
					rowData.push_back(vResult[CALCRESULT_IDX_A]);
					//10 ��������Ӧ�����
					rowData.push_back(vResult[CALCRESULT_IDX_ACT]);
					//11 ����������
					rowData.push_back(vResult[CALCRESULT_IDX_AS]);
					//12 �ο����
					value.Sprintf(L"%d��%g", info.iRebarCount, info.dRebarDiameter);
					rowData.push_back(value);
					//13 ���
					rowData.push_back(vResult[CALCRESULT_IDX_BENDING]);
					//14 ����
					rowData.push_back(vResult[CALCRESULT_IDX_AXIAL]);
					//15 XY�������
					rowData.push_back(vResult[CALCRESULT_IDX_XYSHEAR]);
					//16 YZ�������
					rowData.push_back(vResult[CALCRESULT_IDX_YZSHEAR]);
					//17 XZ�������
					rowData.push_back(vResult[CALCRESULT_IDX_XZSHEAR]);
				}
				else
				{
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
				}

				lbd_PJParamList.rows.push_back(rowData);
			}

			//��ʼ����һ��ˢ��
			lbd_PJParamList.needFresh = true;

			break;
		}
		default:
		break;
	}
}

void exportCalDocCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_ExportCalculation, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_ExportCalculation);
}