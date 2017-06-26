#include "MsdiMsCaeSipch.h"

extern ClipResultDlgInfo	g_clipResultDlgInfo; 
extern void OutColumnValueLists(WString ngName, int ProjectID, int caseID, WString sUnit);

// KEY=CAEģ��ID$CAEģ�͵�λ$����ID$����ModelID
const int KEY_IDX_PROJECTID = 0;//CAEģ��ID
const int KEY_IDX_PROJECTUNIT = 1;//CAEģ�͵�λ
const int KEY_IDX_CASEID = 2;//����ID
const int KEY_IDX_SECTIONID = 3;//����ModelID 
const int KEY_IDX_NGNAME = 4;//�������� 

WString strNgName;
int iProjectID;
int iCaseID;
WString strUnit;

void ClearClipOutData()//���������ֵ���
{
	if (ClipFace::IsClipFaceModel())
	{
		s_drawtempLine.CloseDrawFunction();
		DgnPlatform::LevelId levelId;
		// ������ȡ��CAE��ͼ��ͼ��
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CLIPOUTDATA", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CLIPOUTDATA");
		{
			mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
			mdlLevel_delete(MASTERFILE, levelId);
		}
	}
}

void ClipResult_adjustVSashDialogItems
(
MSDialogP   db,
BSIRect     *pOldContent,
bool        refreshItems
)
{
	DialogItem    *sashDiP;
	DialogItem    *treeDiP;
	DialogItem    *ctPanelDiP;

	/* Get the 3 main items on the dialog */
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_ClipResult_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_ClipResult_ClipList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_ClipResult_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookSash_ClipResult_vSashHook
(
DialogItemMessage   *dimP    /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.buttonTrans == BUTTONTRANS_DOWN)
		{
			BSIRect     contentRect;
			mdlWindow_contentRectGetLocal(&contentRect, (MSWindowP)dimP->db);
		}
		else if (dimP->u.button.buttonTrans == BUTTONTRANS_UP)
		{
			Sash_ButtonUpAuxInfo *buaiP = (Sash_ButtonUpAuxInfo *)dimP->auxInfoP;

			/* use buaiP->newXPos to determine where upperLeft corner
			of sash beveled rect will go.  This message is sent after
			sash has been erased from old position & moved, but before
			it has been drawn */
			if (buaiP->newXPos != buaiP->oldXPos)
				ClipResult_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookDialog_ClipResult(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_CREATE:
	{
		dmP->u.create.interests.windowMoving = TRUE;
		dmP->u.create.interests.resizes = TRUE;
		dmP->u.create.interests.updates = TRUE;
		break;
	}
	case DIALOG_MESSAGE_UPDATE:
	{
		DialogItem    *diP;

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_ClipResult_V, 0);
		if (!diP)
			break;

		ClipResult_adjustVSashDialogItems(dmP->db, NULL, TRUE);
		break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING:
	{
		int        minSize = (60 * XC);

		/* Don't process if only moving dialog box */
		if (dmP->u.windowMoving.whichCorners == CORNER_ALL ||
			dmP->u.windowMoving.whichCorners == CORNER_ALL_RESIZED)
			break;

		/* Minimum size for dialog */
		if (dmP->u.windowMoving.newWidth < minSize)
			dmP->u.windowMoving.newWidth = minSize;
		if (dmP->u.windowMoving.newHeight < minSize)
			dmP->u.windowMoving.newHeight = minSize;

		dmP->u.windowMoving.handled = TRUE;

		break;
	}

	case DIALOG_MESSAGE_RESIZE:
	{
		BSIRect     oldContent;

		/* Don't process if only moving dialog box */
		if (dmP->u.resize.whichCorners == CORNER_ALL)
			break;

		mdlWindow_pointToLocal(&oldContent.origin, (MSWindowP)dmP->db, &dmP->u.resize.oldContent.origin);
		mdlWindow_pointToLocal(&oldContent.corner, (MSWindowP)dmP->db, &dmP->u.resize.oldContent.corner);

		ClipResult_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		//ȱʡ����Ԥ��������������DGN����ť�費���ã���ѡ���˹���������Ϊ����
		DialogItemP button = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_Clear, 0);
		mdlDialog_itemSetEnabledState(dmP->db, button->itemIndex, false, false);

		DialogItemP button2 = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_Submit, 0);
		mdlDialog_itemSetEnabledState(dmP->db, button2->itemIndex, false, false);

		DialogItemP button3 = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_OutDGN, 0);
		mdlDialog_itemSetEnabledState(dmP->db, button3->itemIndex, false, false);
	
		//��ʼ���ؼ���ʾ
		wcscpy(g_clipResultDlgInfo.current, L"δѡ�����");

		mdlDialog_itemsSynch(dmP->db);
	}break;

	case DIALOG_MESSAGE_DESTROY:
	{
		ClearClipOutData();
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookTabpage_ClipResult_ResultType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (!dimP->u.stateChanged.reallyChanged)
			break;
		
		DialogItem	*dipXValueCnt = NULL,		//X����ֵ����
			*dipYValueCnt = NULL;// Y����ֵ����

		WString strLabelXValueCnt,
			strLabelYValueCnt;

		dipXValueCnt = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ClipResult_XValueCnt, 0);
		dipYValueCnt = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ClipResult_YValueCnt, 0);

		if (dimP->dialogItemP->id == TABPAGEID_ClipResult_Cartesian && TABPAGEID_ClipResult_Cartesian)//�ѿ�����
		{
			strLabelXValueCnt = L"X����(mm)";
			strLabelYValueCnt = L"Y����(mm)";
		}
		else
		{
			strLabelXValueCnt = L"������(mm)";
			strLabelYValueCnt = L"������(��)";
		}

		if (NULL != dipXValueCnt)
		{
			mdlDialog_itemSetLabel(dimP->db, dipXValueCnt->itemIndex, strLabelXValueCnt.data());
		}
		if (NULL != dipYValueCnt)
		{
			mdlDialog_itemSetLabel(dimP->db, dipYValueCnt->itemIndex, strLabelYValueCnt.data());
		}
		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookTree_ClipResult_ClipList(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		WString			instanceName;
		WString			caseValue;
		WString			sectionValue;
		GuiTreeNode     *pRoot;
		MSValueDescr	nodeVal;

		GuiTreeModel* pModel = mdlTreeModel_create(1);

		pRoot = mdlTreeModel_getRootNode(pModel);
		mdlTreeNode_setDisplayText(pRoot, L"ROOT");

		//��ȡXAttribution�õ�ȫ��ʵ��������
		bvector<InstanceInfo> allInstance = readInstanceInfo();

#pragma region ��Ŀ�ڵ������
		//��������ʵ����Ϊ�����ӽڵ�
		for (int iPro = 0; iPro < (int)allInstance.size(); iPro++)
		{
			WString ngName = allInstance[iPro].NgName;
			WString sUnit;
			/*********************************************ʵ���ڵ������********************************************/
			GuiTreeNode* instanceNode = mdlTreeNode_create(pModel, TRUE);
			mdlTreeNode_addChild(pRoot, instanceNode);

			//��ʾ���ݡ���ʵ����
			splitNgName_Model(ngName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, instanceName, sUnit);
			mdlTreeNode_setDisplayText(instanceNode, instanceName.data());

			mdlTreeNode_setCollapsedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedSelectedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

			//�������ݡ���"ModelID&NgName"
			WString instanceValue;
			instanceValue.Sprintf(L"%d%s%s%s%d", allInstance[iPro].modelId, SeparatorAnd, ngName);
			nodeVal.SetWChar(instanceValue.data());
			mdlTreeNode_setValue(instanceNode, nodeVal, FALSE);

			//��������model���ҵ����ڸ�ʵ���Ľ���model
			ngName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

#pragma region �����ڵ������
			/*********************************************�����ڵ������********************************************/
			//���ݹ���ID��ѯ���ݿ�õ���Ӧ�Ĺ����б�
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(allInstance[iPro].projectId, vecCaseIDs, vecCaseNames, vecCaseProps);

			for (int iCase = 0; iCase < vecCaseNames.size(); iCase++)
			{
				GuiTreeNode*	pCaseNode = mdlTreeNode_create(pModel, TRUE);

				mdlTreeNode_setCollapsedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedSelectedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//��ʾ���ݡ���������					
				mdlTreeNode_setDisplayText(pCaseNode, vecCaseNames[iCase].data());

				//�������ݡ�������ID
				caseValue.Sprintf(L"%d%s%s%s%d", allInstance[iPro].projectId, SeparatorAnd, sUnit, SeparatorAnd, vecCaseIDs.at(iCase));
				nodeVal.SetInt32(vecCaseIDs.at(iCase));
				mdlTreeNode_setValue(pCaseNode, nodeVal, FALSE);

				mdlTreeNode_addChild(instanceNode, pCaseNode);

#pragma region ����ڵ������
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

					if (wsModelName.FindI(ngName.data()) != string::npos)
					{
						GuiTreeNode* clipNode = mdlTreeNode_create(pModel, FALSE);

						mdlTreeNode_setLeafIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

						//��ʾ���ݡ���������
						WString clipName;
						splitNgName_Clip(wsModelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, clipName, UNUSE_WSTRING, UNUSE_WSTRING);
						mdlTreeNode_setDisplayText(clipNode, clipName.data());

						//�������ݡ���(CAEģ��ID&����ID&����ModelID&��������)
						sectionValue.Sprintf(L"%d%s%s%s%d%s%d%s%s", allInstance[iPro].projectId, SeparatorAnd, sUnit, SeparatorAnd, vecCaseIDs.at(iCase), SeparatorAnd, IndexItem->GetModelId(), SeparatorAnd, wsModelName);
						nodeVal.SetWChar(sectionValue.data());
						mdlTreeNode_setValue(clipNode, nodeVal, FALSE);

						mdlTreeNode_addChild(pCaseNode, clipNode);
					}
				}

				mdlModelIterator_free(cellIterator);
#pragma endregion ����ڵ������
			}
#pragma endregion �����ڵ������

			if (0 == mdlTreeNode_getChildCount(instanceNode))
				mdlTreeNode_setAllowsChildren(instanceNode, FALSE);
		}
#pragma endregion ��Ŀ�ڵ������

		mdlTreeNode_expand(pRoot);
		mdlDialog_treeSetTreeModelP(dimP->dialogItemP->rawItemP, pModel);

		break;
	}
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.clicked && dimP->u.button.upNumber == 2)
		{
			//ȱʡ����Ԥ��������������DGN����ť�費���ã���ѡ���˹���������Ϊ����
			DialogItemP button = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_Clear, 0);
			DialogItemP button2 = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_Submit, 0);
			DialogItemP button3 = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ClipResult_OutDGN, 0);

			//���tree�н�����Ҫ���µ�ǰ����������
			wcscpy(g_clipResultDlgInfo.current, L"δѡ�����");

			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
			int rowIndex, colIndex;
			if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;
				GuiTreeNode*	pretreeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
				treeNode = mdlTreeCell_getNode(treeCell);

				//�ж�˫���ڵ������
				MSValueDescr uvalue;
				mdlTreeNode_getValue(treeNode, uvalue);
				int nodeLevel = mdlTreeNode_getLevel(treeNode);
				switch (nodeLevel)
				{
				//ʵ���ڵ�
				case 0:
				{
					mdlDialog_itemSetEnabledState(dimP->db, button->itemIndex, false, false);
					mdlDialog_itemSetEnabledState(dimP->db, button2->itemIndex, false, false);
					mdlDialog_itemSetEnabledState(dimP->db, button3->itemIndex, false, false);

					break;
				}
				//�����ڵ�
				case 1:
				{
					mdlDialog_itemSetEnabledState(dimP->db, button->itemIndex, false, false);
					mdlDialog_itemSetEnabledState(dimP->db, button2->itemIndex, false, false);
					mdlDialog_itemSetEnabledState(dimP->db, button3->itemIndex, false, false);

					break;
				}
				//����ڵ�
				case 2:
				{
					mdlDialog_itemSetEnabledState(dimP->db, button->itemIndex, true, false);
					mdlDialog_itemSetEnabledState(dimP->db, button2->itemIndex, true, false);
					mdlDialog_itemSetEnabledState(dimP->db, button3->itemIndex, true, false);

					WString lsData;
					uvalue.GetWString(lsData);

					bvector<WString> vecData;
					BeStringUtilities::Split(lsData.data(), SeparatorAnd, vecData);

					iProjectID = wtoi(vecData.at(KEY_IDX_PROJECTID).data());
					strUnit = vecData.at(KEY_IDX_PROJECTUNIT);
					iCaseID = wtoi(vecData.at(KEY_IDX_CASEID).data());
					strNgName = vecData.at(KEY_IDX_NGNAME);

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						mdlModelRef_freeWorking(target);
						MsdiMsCaeSiUtility::fitView(0);
					}

					//��ȡ��Ӧ��������
					pretreeNode = mdlTreeNode_getParent(treeNode);
					mdlTreeNode_getDisplayTextWString(pretreeNode, O_CaseName);

					DPoint3d minpoint;
					DPoint3d maxpoint;
					GetMeshMinMaxPoints(minpoint, maxpoint, strNgName);

					double dis = fabs(maxpoint.x - minpoint.x) / UOR_PER_UNIT(strUnit);
					g_clipResultDlgInfo.titleFont = dis*0.6 / 35.0*0.7;//��ͷ�����С
					g_clipResultDlgInfo.contentFont = 0.8*g_clipResultDlgInfo.titleFont;//���������С
					g_clipResultDlgInfo.valueFont = ((dis / g_clipResultDlgInfo.XValueCnt) / 6.0) / 2.0;//��ֵ�����С
					g_clipResultDlgInfo.coorFont = g_clipResultDlgInfo.valueFont*1.5;//�������������С
					double PerHeight = fabs(maxpoint.y - minpoint.y) / g_clipResultDlgInfo.YValueCnt / UOR_PER_UNIT(strUnit);
					if (g_clipResultDlgInfo.coorFont > PerHeight)
					{
						g_clipResultDlgInfo.coorFont = PerHeight;
					}

					break;
				}
				default:
					break;
				}

				// �����û��û�ѡ��Ľ����¡���ǰ������������
				wcscpy(g_clipResultDlgInfo.current, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());
			}
		}

		mdlDialog_itemsSynch(dimP->db);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookOptButton_ClipResult_D_ALL(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		int* pI = &g_clipResultDlgInfo.D_SX;
		for (int i = 0; i < 10; i++)
			*(pI + i) = g_clipResultDlgInfo.D_All;
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
	}
}

void HookOptButton_ClipResult_Z_ALL(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		int* pI = &g_clipResultDlgInfo.Z_SR;
		for (int i = 0; i < 10; i++)
			*(pI + i) = g_clipResultDlgInfo.Z_All;
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
	}
}
void HookButton_ClipResult_Clear(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		ClearClipOutData();
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}
void HookButton_ClipResult_Submit(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		ClearClipOutData();
		WString headerstring;
		headerstring.append(g_clipResultDlgInfo.tableTitle);
		O_tableTitle = headerstring;//��ͷ����
		O_titleFont = g_clipResultDlgInfo.titleFont;//��ͷ�����С
		O_contentFont = g_clipResultDlgInfo.contentFont;//���������С
		O_heightCoe = g_clipResultDlgInfo.heightCoe;//�߶ȱ���
		O_XSpaceCnt = g_clipResultDlgInfo.XSpaceCnt;//X���껮�ָ���
		O_YSpaceCnt = g_clipResultDlgInfo.YSpaceCnt;//Y���껮�ָ���
		O_coorFont = g_clipResultDlgInfo.coorFont;//�������������С
		O_XValueCnt = g_clipResultDlgInfo.XValueCnt;//X������ֵ����
		O_YValueCnt = g_clipResultDlgInfo.YValueCnt;//Y��������
		O_valueFont = g_clipResultDlgInfo.valueFont;//��ֵ�����С	
		O_decimal = g_clipResultDlgInfo.decimal;//��ֵ��С�����λ����������
		
		DialogItemP buttonD = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ToggleButton, TOGGLEID_ClipResult_D_SX, 0);

		if (buttonD!=NULL)//�ѿ����������
		{	
			OutDataModelRef = MASTERFILE;
			OutValueLists(strNgName, iProjectID, iCaseID, strUnit);
		}
		else//���������
		{
			//Ҫ�������ֵ����
			OutDataModelRef = MASTERFILE;
			OutColumnValueLists(strNgName, iProjectID, iCaseID, strUnit);
		}
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}
void CopyMeshToDgn(DgnModelRefP meshModel, DgnModelRefP dgnModel,WString PlotName)
{
	ElementCopyContextP ccP = mdlCopyContext_create();
	NamedGroupCollectionPtr ngcCutFace;
	NamedGroupFlags ngfCutFace;
	NamedGroupPtr ngCutFace;
	ngcCutFace = NamedGroupCollection::Create(*meshModel);
	for each (NamedGroupPtr namedGroup in *ngcCutFace)
	{
		if (namedGroup.IsValid())
		{
			WString ngName = namedGroup->GetName();
			if (ngName.find(PlotName) != WString::npos)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//�õ���ÿһ��mesh��
					{
						EditElementHandle tempDsc;
						tempDsc.Duplicate(eeh);
						tempDsc.AddToModel();					
					}
				}

			}
		}
	}
	mdlCopyContext_free(&ccP);
}
void HookButton_ClipResult_OutDGN(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		Bentley::BeFileName fileName;
		if (SUCCESS == mdlDialog_fileCreate(fileName, NULL, 0, L"��ֵͼ", L"*.dgn", L"D:\\", L"�½�DGN"))
		{
			DgnModelRefP bluePrintModel;
			if (SUCCESS == mdlWorkDgn_createFile(&bluePrintModel, fileName, DgnPlatform::DgnFileFormatType::V8, MASTERFILE, DgnPlatform::SeedCopyFlags::DefaultData, NULL, NULL, TRUE))
			{
				DgnModelRefP newModelRef;
				mdlModelRef_createWorking(&newModelRef, mdlModelRef_getDgnFile(bluePrintModel), DEFAULTMODEL, TRUE, TRUE);

				DialogItemP buttonD = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ToggleButton, TOGGLEID_ClipResult_D_SX, 0);
				OutDataModelRef = newModelRef;
				CopyMeshToDgn(MASTERFILE, OutDataModelRef, NGNAME_PREFIX_CAECLIP);
				WString headerstring;
				headerstring.append(g_clipResultDlgInfo.tableTitle);
				O_tableTitle = headerstring;//��ͷ����
				O_titleFont = g_clipResultDlgInfo.titleFont;//��ͷ�����С
				O_contentFont = g_clipResultDlgInfo.contentFont;//���������С
				O_heightCoe = g_clipResultDlgInfo.heightCoe;//�߶ȱ���
				O_XSpaceCnt = g_clipResultDlgInfo.XSpaceCnt;//X���껮�ָ���
				O_YSpaceCnt = g_clipResultDlgInfo.YSpaceCnt;//Y���껮�ָ���
				O_coorFont = g_clipResultDlgInfo.coorFont;//�������������С
				O_XValueCnt = g_clipResultDlgInfo.XValueCnt;//X������ֵ����
				O_YValueCnt = g_clipResultDlgInfo.YValueCnt;//Y��������
				O_valueFont = g_clipResultDlgInfo.valueFont;//��ֵ�����С
				O_decimal =   g_clipResultDlgInfo.decimal;//С����λ��
				if (buttonD != NULL)//�ѿ����������
				{	
					OutValueLists(strNgName, iProjectID, iCaseID, strUnit);
				}
				else
				{
					OutColumnValueLists(strNgName, iProjectID, iCaseID, strUnit);
				}
				mdlWorkDgn_saveChanges(bluePrintModel);
				mdlModelRef_freeWorking(newModelRef);
				mdlWorkDgn_closeFile(bluePrintModel);
				OutDataModelRef = MASTERFILE;

				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"��ֵͼ�����ɣ�", MessageBoxIconType::Information);
			}
			else
			{
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"��ֵͼ���ʧ�ܣ�", MessageBoxIconType::Critical);
			}
			//	mdlOutput_messageCenter(DgnPlatform::OutputMessagePriority::Info, L"��ֵͼ������!", L"��ֵͼ������!", DgnPlatform::OutputMessageAlert::Balloon);
			//}
			//else
			//{
			//	mdlOutput_messageCenter(DgnPlatform::OutputMessagePriority::Error, L"��ֵͼ���ʧ��!", L"��ֵͼ���ʧ��!", DgnPlatform::OutputMessageAlert::Balloon);
			//}
		}
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}
void clipResultShowCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_ClipResultShow, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_ClipResultShow);
}