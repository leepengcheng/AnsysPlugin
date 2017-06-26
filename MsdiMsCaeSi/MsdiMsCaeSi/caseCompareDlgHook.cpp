#include "MsdiMsCaeSipch.h"

extern CaseCompDlgInfo		g_caseCompDlgInfo;
// CAE���������
extern bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
extern bmap<WString, WString> g_RstType_Unit;
extern bvector<WString> vecRstTypeCodes;

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

#define CASECOMP_PROJECT_ID_SELECTED_L (g_caseCompDlgInfo.indexInstanL == -1 ? -1 : readInstanceInfo()[g_caseCompDlgInfo.indexInstanL].projectId)
#define CASECOMP_PROJECT_ID_SELECTED_R (g_caseCompDlgInfo.indexInstanR == -1 ? -1 : readInstanceInfo()[g_caseCompDlgInfo.indexInstanR].projectId)

//�����Ա�ʱ��һ��ģ�͵Ļ��ƻ���
DPoint3d s_SecCountourBasePt;

void caseCompareCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_CaseCompare, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_CaseCompare);
}

void HookComboBox_CaseComp_InstanceL(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexInstanL = -1;
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
			g_caseCompDlgInfo.indexInstanL = 0;
		}

		mdlDialog_comboBoxSetListModelP(pThis, pListModel);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		//���л�ʵ��ʱ��ˢ������ʾ��ͬʱ���������ʾ���ݣ����ر�ɫ��
		initCaseListTree(TREEID_CaseComp_CaseListL);

		ClearContour();
		ShowCAEModel();
		s_DrawColorShape.CloseDrawFunction();
		s_drawtempLine.CloseDrawFunction();
		checkCompareEnable(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_InstanceR(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexInstanR = -1;
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
			g_caseCompDlgInfo.indexInstanR = 0;
		}

		mdlDialog_comboBoxSetListModelP(pThis, pListModel);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		//���л�ʵ��ʱ��ˢ������ʾ��ͬʱ���������ʾ���ݣ����ر�ɫ��
		initCaseListTree(TREEID_CaseComp_CaseListR);

		ClearContour();
		ShowCAEModel();
		s_DrawColorShape_temp.CloseDrawFunction();
		s_drawtempLine.CloseDrawFunction();
		checkCompareEnable(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_SettingL(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//��������޸���ɫ������
		openColorShapeSetting(&s_DrawColorShape, SourceTypeDCS::CaseCompare);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_SettingR(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//�Ҳ������޸ĸ�ɫ������
		openColorShapeSetting(&s_DrawColorShape_temp, SourceTypeDCS::CaseCompare);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_Compare(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//��ɾ��֮ǰ���Ƶ�����ͼ����ʾCAEģ��
		ClearContour();
		ShowCAEModel();

		//����ͼ�۽�����๤�̵�CAEģ��λ��
		focusOnCAEModel(s_DrawColorShape.NgName);

		//������ʾ���ͽ��л���
		placeCompareContoursTool::InstallNewInstance(0, 0);

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_CaseComp_CaseList(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_INIT:
		{
			initCaseListTree(dimP->dialogItemP->id);
		}break;
		case DITEM_MESSAGE_STATECHANGED:
		{
			//���������ʱ����Ϊ��Ӧɫ����ֵ
			RawItemHdr*	riP = dimP->dialogItemP->rawItemP;
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(riP);

			//���ݱ�ѡ�нڵ���¶�Ӧ��caseID������Ӧ��ʼ��ɫ��
			int caseID;
			WString caseName;
			int	numSelect = -1;
			Point2d* selectP = NULL;
			if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, riP) && numSelect != 0)
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;
				MSValueDescr  nodeVal;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
				treeNode = mdlTreeCell_getNode(treeCell);

				mdlTreeNode_getValue(treeNode, nodeVal);
				caseID = nodeVal.GetInt32();

				mdlTreeNode_getDisplayTextWString(treeNode, caseName);

				//����ѡ��Ĺ����ڵ����ݸ�ֵ��ɫ���ĳ�Ա����
				DrawColorShape* optDCS = NULL;
				int instanceComboBoxIndex;
				if (dimP->dialogItemP->id == TREEID_CaseComp_CaseListL)
				{
					optDCS = &s_DrawColorShape;
					instanceComboBoxIndex = g_caseCompDlgInfo.indexInstanL;

					// ������ĿID�͹���ID�����ý������
					SetResultType(dimP, COMBOBOXID_CaseComp_ResultTypes, readInstanceInfo().at(instanceComboBoxIndex).projectId, caseID, vecRstTypeCodes);
				}
				else
				{
					optDCS = &s_DrawColorShape_temp;
					instanceComboBoxIndex = g_caseCompDlgInfo.indexInstanR;
				}

				//��������ͷ����仯ʱ����Ҫ���¼���ɫ��ֵ
				optDCS->ResultTypeCode = vecRstTypeCodes.at(g_caseCompDlgInfo.indexReType);

				//Header
				optDCS->Header = g_RstType_Name[optDCS->ResultTypeCode];
				//Unit
				optDCS->Unit = g_RstType_Unit[optDCS->ResultTypeCode];

				//showType
				optDCS->showType = g_caseCompDlgInfo.indexShowType;

				//ProjectName & ProjectId
				WString instanceName;
				splitNgName_Model(readInstanceInfo().at(instanceComboBoxIndex).NgName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, instanceName, UNUSE_WSTRING);
				int index;
				splitInstanceName(instanceName, optDCS->ProjectName, index);
				optDCS->ProjectId = readInstanceInfo().at(instanceComboBoxIndex).projectId;

				//CaseName
				mdlTreeNode_getDisplayTextWString(treeNode, optDCS->CaseName);

				//CaseID
				optDCS->CaseID = nodeVal.GetInt32();

				//NgName				
				optDCS->NgName = makeNgName_Model(optDCS->ProjectId, index);

				//����Ϊ�Զ�����ɫ��
				optDCS->m_bAutomatic = true;
				//������ʵ������ɫ�������ֵ��Сֵ
				optDCS->initColorRangeFromDB();
				//�Զ��ȷ�ɫ����ֵ
				optDCS->divideColorRange();

			}
			checkCompareEnable(dimP->db);
		}break;
		default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookDialog_CaseCompare(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case  DIALOG_MESSAGE_INIT:
	{
		checkCompareEnable(dmP->db);
	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		ClearContour();
		ShowCAEModel();
		s_drawtempLine.CloseDrawFunction();
		s_DrawColorShape.CloseDrawFunction();
		s_DrawColorShape_temp.CloseDrawFunction();
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ResultType(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexReType = 0;
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case  DITEM_MESSAGE_STATECHANGED:
	{
		//��������ͷ����仯ʱ����Ҫ���¼���ɫ��ֵ
		s_DrawColorShape.ResultTypeCode = vecRstTypeCodes.at(g_caseCompDlgInfo.indexReType);
		//Header
		s_DrawColorShape.Header = g_RstType_Name[s_DrawColorShape.ResultTypeCode];
		//Unit
		s_DrawColorShape.Unit = g_RstType_Unit[s_DrawColorShape.ResultTypeCode];

		s_DrawColorShape.m_bAutomatic = true;

		s_DrawColorShape.initColorRangeFromDB();
		s_DrawColorShape.divideColorRange();

		//m_iResultTypeID
		s_DrawColorShape_temp.ResultTypeCode = s_DrawColorShape.ResultTypeCode;
		//Header
		s_DrawColorShape_temp.Header = g_RstType_Name[s_DrawColorShape_temp.ResultTypeCode];
		//Unit
		s_DrawColorShape_temp.Unit = g_RstType_Unit[s_DrawColorShape_temp.ResultTypeCode];

		s_DrawColorShape_temp.m_bAutomatic = true;

		s_DrawColorShape_temp.initColorRangeFromDB();
		s_DrawColorShape_temp.divideColorRange();
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ShowType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
		s_DrawColorShape.showType = s_DrawColorShape_temp.showType = g_caseCompDlgInfo.indexShowType;
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ShowModel(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_caseCompDlgInfo.ShowModel = 0;
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// �⻬
	}
		break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_caseCompDlgInfo.ShowModel == 1)
			ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// ����
		else
			ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// �⻬	
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void initCaseListTree(RscId treeID)
{
	MSDialog	*dbP = mdlDialog_find(DIALOGID_CaseCompare, NULL);
	if (dbP == NULL)
		return;
	DialogItem	*diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_Tree, treeID, 0);
	if (diP == NULL)
		return;

	RawItemHdr*		riP = diP->rawItemP;
	GuiTreeModel*	pModel = NULL;

	pModel = mdlTreeModel_create(1);

	GuiTreeNode*	pRoot = mdlTreeModel_getRootNode(pModel);

	//������ǰѡ��ʵ���µ����й�������Ϊ�ӽڵ����
	int iProjectID = treeID == TREEID_CaseComp_CaseListL ? CASECOMP_PROJECT_ID_SELECTED_L : CASECOMP_PROJECT_ID_SELECTED_R;

	bvector<int> vecCaseIDs;
	bvector<WString> vecCaseNames;
	bvector<WString> vecCaseProps;
	getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

	for (int i = 0; i < (int)vecCaseNames.size(); i++)
	{
		GuiTreeNode*	pNode = mdlTreeNode_create(pModel, FALSE);

		//������ʾֵ����������
		mdlTreeNode_setDisplayText(pNode, vecCaseNames[i].data());

		//��������ֵ��������ID
		MSValueDescr    valueDescr;
		valueDescr.SetInt32(vecCaseIDs[i]);
		mdlTreeNode_setValue(pNode, valueDescr, FALSE);

		mdlTreeNode_addChild(pRoot, pNode);
	}

	mdlTreeNode_expand(pRoot);
	mdlDialog_treeSetTreeModelP(riP, pModel);

	mdlDialog_itemSynch(dbP, diP->itemIndex);
}

void checkCompareEnable(MSDialogP db)
{
	//�����Ƿ���������������ѡ�н����Աȡ���ť��Ϊ��/����
	DialogItemP button = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_CaseComp_Submit, 0);
	for (int i = 0; i < 2; i++)
	{
		int	numSelect = -1;
		Point2d* selectP = NULL;
		DialogItem*	diP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, i == 0 ? TREEID_CaseComp_CaseListL : TREEID_CaseComp_CaseListR, 0);
		RawItemHdr*	riP = diP->rawItemP;
		if (SUCCESS != mdlDialog_treeGetSelections(&numSelect, &selectP, riP) || numSelect == 0)
		{
			mdlDialog_itemSetEnabledState(db, button->itemIndex, false, false);
			return;
		}
	}

	mdlDialog_itemSetEnabledState(db, button->itemIndex, true, false);

}
placeCompareContoursTool::placeCompareContoursTool(int toolId, int toolPrompt)
	: DgnPrimitiveTool(toolId, toolPrompt)
{}

void placeCompareContoursTool::_OnPostInstall()
{
	__super::_OnPostInstall();
	_EndDynamics();
	mdlAccuSnap_enableSnap(true);		//������ȷ��׽
	mdlAccuDraw_setEnabledState(false);	//�رվ�ȷ����ϵ
}

bool placeCompareContoursTool::_OnDataButton(DgnButtonEventCR ev)
{
	s_SecCountourBasePt = *ev.GetPoint();
	startShowContour(&s_DrawColorShape);
	startShowContour(&s_DrawColorShape_temp, ev.GetPoint(), false);

	mdlState_startDefaultCommand();
	return true;
}

void placeCompareContoursTool::InstallNewInstance(int toolId, int toolPrompt)
{
	placeCompareContoursTool* tool = new placeCompareContoursTool(toolId, toolPrompt);
	tool->InstallTool();
}