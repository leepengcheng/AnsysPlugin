#include "MsdiMsCaeSipch.h"

extern SurfaceShowDlgInfo	g_surfaceShowDlgInfo;
extern outRebarDlgInfo g_outRebarDlgInfo;

extern int m_iCamera;// ���״̬
extern void ClipFaceCmd(WCharCP unparsed);
extern SolidDispDlgInfo	g_solidDispDlgInfo;
extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);
// CAE���������
extern bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
extern bmap<WString, WString> g_RstType_Unit;
extern bvector<WString> vecRstTypeCodes;
extern bool alreadyUnload;

// KEY=CAEģ��ID$CAEģ�͵�λ$����ID$����ModelID$��������
const int KEY_IDX_PROJECTID = 0;//CAEģ��ID
const int KEY_IDX_PROJECTUNIT = 1;//CAEģ�͵�λ
const int KEY_IDX_CASEID = 2;//����ID
const int KEY_IDX_SECTIONID = 3;//����ModelID 
const int KEY_IDX_NGNAME = 4;//�������� 

WString strCurrentNodeKey = L"";

std::string GetMidValueString(DPoint3d str, DPoint3d end)
{
	DPoint3d midPoint;
	midPoint.x = (str.x + end.x) / 2;
	midPoint.y = (str.y + end.y) / 2;
	midPoint.z = (str.z + end.z) / 2;

	std::string midString;
	char tempCx[20], tempCy[20], tempCz[20];

	itoa((int)(midPoint.x), tempCx, 10);
	itoa((int)(midPoint.y), tempCy, 10);
	itoa((int)(midPoint.z), tempCz, 10);

	midString.append(tempCx);
	midString.append(tempCy);
	midString.append(tempCz);

	return midString;
}

void DrawOutLineWhenContourLine(DgnPlatform::LevelId levelId)
{
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	propsSetter->SetLevel(levelId);

	NamedGroupCollectionPtr ngcCutFace;
	NamedGroupFlags ngfCutFace;
	NamedGroupPtr ngCutFace;
	ngcCutFace = NamedGroupCollection::Create(*ACTIVEMODEL);

	bvector<MSElementDescrP> outTempLines;
	bvector<DPoint3d>().swap(s_drawtempLine.linePoints);

	for each (NamedGroupPtr namedGroup in *ngcCutFace)
	{
		if (namedGroup.IsValid())
		{
			WString ngName = namedGroup->GetName();
			if (ngName.find(NGNAME_PREFIX_CAECLIP) != WString::npos)
			{
				size_t Index;
				Index = 0;

				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				map<std::string, int> stringmap;
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//�õ���ÿһ��mesh��
					{
						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
						{
							for (size_t j = 0; j < pXYZ.size(); j++)
							{
								if (j < pXYZ.size() - 1)
								{
									stringmap[GetMidValueString(pXYZ.at(j), pXYZ.at(j + 1))] += 1;
								}
								else
								{
									stringmap[GetMidValueString(pXYZ.at(j), pXYZ.at(0))] += 1;
								}

							}
						}
						bvector<int>().swap(pIndices);
						bvector<DPoint3d>().swap(pXYZ);
					}
				}

				MSElement tempElem;
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//�õ���ÿһ��mesh��
					{
						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;
						int								iNumIndexPerFace;
						int								iNumFace;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
						{
							for (size_t j = 0; j < pXYZ.size(); j++)
							{
								if (j < pXYZ.size() - 1)
								{
									if (stringmap[GetMidValueString(pXYZ.at(j), pXYZ.at(j + 1))] < 2)
									{
										DPoint3d points[2];
										points[0] = pXYZ.at(j);
										points[1] = pXYZ.at(j + 1);
										mdlLine_create(&tempElem, NULL, points);
										MSElementDescrP tempDes = MSElementDescr::Allocate(tempElem);
										outTempLines.push_back(tempDes);
									}

								}
								else
								{
									if (stringmap[GetMidValueString(pXYZ.at(j), pXYZ.at(0))] < 2)
									{
										DPoint3d points[2];
										points[0] = pXYZ.at(j);
										points[1] = pXYZ.at(0);
										mdlLine_create(&tempElem, NULL, points);
										MSElementDescrP tempDes = MSElementDescr::Allocate(tempElem);
										outTempLines.push_back(tempDes);
									}
								}
							}
						}

						bvector<int>().swap(pIndices);
						bvector<DPoint3d>().swap(pXYZ);
					}
				}
			}
		}
	}

	map<MSElementDescrP, int> descrMap;
	for (UInt32 k = 0; k < outTempLines.size(); k++)
	{
		MSElementDescrP outlineDes = outTempLines.at(k);
		descrMap[outlineDes] = 1;
	}
	for (UInt32 i = 0; i < outTempLines.size() - 1; i++) // ���������������ߣ���������������غ�(����)���������߲����
	{
		MSElementDescrP ioutlineDes = outTempLines.at(i);
		if (descrMap[ioutlineDes] <= 1)
		{
			DPoint3d stri, endi;
			mdlElmdscr_extractEndPoints(&stri, NULL, &endi, NULL, ioutlineDes, ACTIVEMODEL);
			for (UInt32 j = i + 1; j < outTempLines.size(); j++)
			{
				MSElementDescrP joutlineDes = outTempLines.at(j);
				DPoint3d strj, endj;
				mdlElmdscr_extractEndPoints(&strj, NULL, &endj, NULL, joutlineDes, ACTIVEMODEL);
				if ((MsdiMsCaeSiUtility::PtIsOnline(stri, strj, endj) && MsdiMsCaeSiUtility::PtIsOnline(endi, strj, endj)) ||
					(MsdiMsCaeSiUtility::PtIsOnline(strj, stri, endi) && MsdiMsCaeSiUtility::PtIsOnline(endj, stri, endi)))
				{
					descrMap[ioutlineDes] += 1;
					descrMap[joutlineDes] += 1;
				}
			}
		}
	}

	for (UInt32 k = 0; k < outTempLines.size(); k++)
	{
		MSElementDescrP outlineDes = outTempLines.at(k);
		if (descrMap[outlineDes] <= 1)
		{
			DPoint3d str, end;
			mdlElmdscr_extractEndPoints(&str, NULL, &end, NULL, outlineDes, ACTIVEMODEL);
			s_drawtempLine.linePoints.push_back(str);
			s_drawtempLine.linePoints.push_back(end);
		}
	}

	if (s_drawtempLine.linePoints.size() > 1)
	{
		s_drawtempLine.StartDrawFunction();
	}
}
void enablefaceWidget(MSDialogP db, bool enable)
{
	DialogItemP diP_btnSubmit = NULL;
	DialogItemP diP_btnNodeVal = NULL;
	//��ʾ��ͼ
	diP_btnSubmit = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_Surface_Submit, 0);
	mdlDialog_itemSetEnabledState(db, diP_btnSubmit->itemIndex, enable, false);
	//�ڵ�鿴
	diP_btnNodeVal = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_Surface_NodeVal, 0);
	mdlDialog_itemSetEnabledState(db, diP_btnNodeVal->itemIndex, enable, false);
	//����
	diP_btnSubmit = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_Surface_Setting, 0);
	mdlDialog_itemSetEnabledState(db, diP_btnSubmit->itemIndex, enable, false);
}

void GetStrValAndEndVal(WString PlotName, double& tStrValue, double& tEndValue, int ProjectId, int CaseId, WString RstTypeCode, DPoint3d& minPoint, DPoint3d& maxPoint)
{
	minPoint = DPoint3d::FromXYZ(0, 0, 0);
	maxPoint = DPoint3d::FromXYZ(0, 0, 0);
	EditElementHandle eh;
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr namedGroup;

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		if (namedGroup.IsValid())
		{
			bvector<double>   oCaeresults;
			GetCaeResult(oCaeresults, ProjectId, CaseId, RstTypeCode);

			WString gName = namedGroup->GetName();
			if (gName.find(PlotName) != WString::npos)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);

				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
					{
						bvector<CutPt> CutFacePoints;

						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;
						bvector<bvector<ContourPoint>>	vecSolidPtAndValTmp;
						bvector<ContourPoint>			vecFacePtAndVal;
						int								iNumIndexPerFace;
						int								iNumFace;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
						{
							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
							XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_CutFace);
							CutPoints *cutPoints = NULL;

							if (xh.IsValid())
							{
								cutPoints = (CutPoints *)xh.PeekData();
							}

							for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
							{
								int iIndices = pIndices.at(iIdx);
								if (iIndices != 0)
								{
									CutPt tempPoint;
									tempPoint.StartID = cutPoints->StartID[iIndices - 1];
									tempPoint.EndID = cutPoints->EndID[iIndices - 1];
									tempPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];
									double tValue;
									ClipFace::GetCutPointValueByeType(tValue, oCaeresults, tempPoint);
									if (i==0&&iIdx==0)
									{
										tStrValue = tValue;
										tEndValue = tValue;
										minPoint =  pXYZ[iIndices-1];
										maxPoint =  pXYZ[iIndices - 1];
									}
									else
									{
										if (tStrValue > tValue)
										{
											tStrValue = tValue;
											minPoint = pXYZ[iIndices - 1];
										}
										if (tEndValue < tValue)
										{
											tEndValue = tValue;
											maxPoint = pXYZ[iIndices - 1];
										}
									}
								}

							}
						}
					}
				}
			}

		}
	}
}


void SurfaceShow_setupPopupMenu
(
DialogMessage   *dmP
)
{
	int			 iItem;
	int			 menuId;
	RawItemHdr		*popupP;
	DialogItem		*popupDiP;
	DialogItem		*downDiP;
	DItem_PulldownMenu	*popupMenuP;
	MSDialogP		dbP = dmP->db;

	/* find the index of the item the reset button went down in */
	iItem = mdlDialog_findItemIndex(dbP, &dmP->u.button.pt, FALSE);
	if (iItem < 0)
		return;

	popupDiP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_PopupMenu, POPUPMENUID_Surface_ClipMenu, 0);
	if (!popupDiP)
		return;
	popupP = popupDiP->rawItemP;

	downDiP = mdlDialog_itemGetByIndex(dbP, iItem);
	switch (downDiP->type)
	{
	case RTYPE_Tree:
		menuId = PDMID_Surface_ClipMenu;
		break;

	default:
		return;
	}

	/* Popup Menus are a restricted form of MenuBar (they only have 1 top
	level menu). mdlDialog_menuBarxxx functions can be called with ptrs
	to popup menus. */
	popupMenuP = mdlDialog_menuBarGetMenu(popupP, 0);
	if (!popupMenuP)
		return;
	if (popupMenuP->id != menuId)
	{
		mdlDialog_menuBarDeleteMenu(popupMenuP, popupP, NULL, 0);
		mdlDialog_menuBarInsMenu(popupP, RTYPE_PulldownMenu, menuId, NULL);
	}

	/* force the button event to go to the popupMenu item */
	dmP->u.button.buttonRiP = popupP;
}

void HookSash_Surface_vSashHook
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
				Surface_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void Surface_adjustVSashDialogItems
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
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_Surface_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_Surface_ClipList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_Surface_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookDialog_SurfaceShow(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_CREATE:
	{
		dmP->u.create.interests.windowMoving = TRUE;
		dmP->u.create.interests.resizes = TRUE;
		dmP->u.create.interests.updates = TRUE;
		// �������ǰ���¼���Ӧ
		dmP->u.create.interests.preButtons = TRUE;
		dmP->u.create.interests.nonDataPoints = TRUE;

		break;
	}
	case DIALOG_MESSAGE_UPDATE:
	{
		DialogItem    *diP;

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_Surface_V, 0);
		if (!diP)
			break;

		Surface_adjustVSashDialogItems(dmP->db, NULL, TRUE);
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

		Surface_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_PREBUTTON:
	{
		if ((dmP->u.button.buttonTrans == BUTTONTRANS_DOWN) &&
			(dmP->u.button.buttonNumber == RESET))//�Ҽ��˵�
		{
			SurfaceShow_setupPopupMenu(dmP);
		}
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		//ȱʡ������ʾ����ť�費���ã���ѡ���˹���������Ϊ����
		enablefaceWidget(dmP->db, false);

		//��ʼ���ؼ���ʾ
		wcscpy(g_surfaceShowDlgInfo.current, L"δѡ�񹤿�");
		g_surfaceShowDlgInfo.indexReType = g_surfaceShowDlgInfo.indexShowModel = g_surfaceShowDlgInfo.indexShowType = 0;

		//�˹���ʹ����ɫ��
		s_pDCShape = &s_DrawColorShape;
		mdlDialog_itemsSynch(dmP->db);
	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		// MS������ر�ʱ���ô���ִ��
		if (alreadyUnload != true)
			ClearclipfaceShow();
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_Surface_ClipList(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
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
			/*********************************************ʵ���ڵ������********************************************/
			GuiTreeNode* instanceNode = mdlTreeNode_create(pModel, TRUE);			
			mdlTreeNode_addChild(pRoot, instanceNode);

			//��ʾ���ݡ���ʵ����
			WString instanceName;
			WString sUnit;
			int unuse1 = 0;
			splitNgName_Model(ngName, UNUSE_WSTRING, unuse1, UNUSE_INT, instanceName, sUnit);
			mdlTreeNode_setDisplayText(instanceNode, instanceName.data());

			mdlTreeNode_setCollapsedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedSelectedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

			//�������ݡ���"ModelID&NgName"
			WString instanceValue;
			instanceValue.Sprintf(L"%d%s%s", allInstance[iPro].modelId, SeparatorAnd, ngName);
			nodeVal.SetWChar(instanceValue.data());
			mdlTreeNode_setValue(instanceNode, nodeVal, FALSE);
			
#pragma region �����ڵ������
			//���ݹ���ID��ѯ���ݿ�õ���Ӧ�Ĺ����б�
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(allInstance[iPro].projectId, vecCaseIDs, vecCaseNames, vecCaseProps);

			for (int iCase = 0; iCase < vecCaseNames.size(); iCase++)
			{
				GuiTreeNode*	pCaseNode = NULL;
				pCaseNode = mdlTreeNode_create(pModel, TRUE);

				mdlTreeNode_setCollapsedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedSelectedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//��ʾ���ݡ���������					
				mdlTreeNode_setDisplayText(pCaseNode, vecCaseNames[iCase].data());

				//�������ݡ�������ID
				nodeVal.SetInt32(vecCaseIDs.at(iCase));
				mdlTreeNode_setValue(pCaseNode, nodeVal, FALSE);

				mdlTreeNode_addChild(instanceNode, pCaseNode);

#pragma region ����ڵ������
				//��������model���ҵ����ڸ�ʵ���Ľ���model
				ngName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

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

					/*********************************************����ڵ������********************************************/
					if (wsModelName.FindI(ngName.data()) != string::npos)
					{
						GuiTreeNode* clipNode = mdlTreeNode_create(pModel, FALSE);

						//��ʾ���ݡ���������
						WString clipName;
						splitNgName_Clip(wsModelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, clipName, UNUSE_WSTRING, UNUSE_WSTRING);
						mdlTreeNode_setDisplayText(clipNode, clipName.data());

						mdlTreeNode_setLeafIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

						//�������ݡ���(CAEģ��ID&CAEģ�͵�λ&����ID&����ModelID&��������)
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
	}break;
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.clicked && dimP->u.button.upNumber == 2)
		{
			enablefaceWidget(dimP->db, true);

			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
			int rowIndex, colIndex;
			if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
				treeNode = mdlTreeCell_getNode(treeCell);

				//�����û��û�ѡ��Ľ����¡���ǰ������������
				wcscpy(g_surfaceShowDlgInfo.current, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());

				int nodeLevel = mdlTreeNode_getLevel(treeNode);
				switch (nodeLevel)
				{
				//ʵ���ڵ�
				case 0:
				{
					enablefaceWidget(dimP->db, false);
					break;
				}
				//�����ڵ�
				case 1:
				{
					enablefaceWidget(dimP->db, false);
					break;
				}
				//����ڵ�
				case 2:
				{
					// ȡ�ù����ڵ�
					GuiTreeNode*	treePreNode = mdlTreeNode_getParent(treeNode);

					//�ж�˫���ڵ������
					MSValueDescr uvalue;
					mdlTreeNode_getValue(treeNode, uvalue);
					uvalue.GetWString(strCurrentNodeKey);
					bvector<WString> vecData;
					BeStringUtilities::Split(strCurrentNodeKey.data(), SeparatorAnd, vecData);

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);

							if (g_surfaceShowDlgInfo.indexShowModel == 1)
								ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// ����
							else
								ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// �⻬	
						}
						mdlModelRef_freeWorking(target);
						MsdiMsCaeSiUtility::fitView(0);
					}

					// CAEģ��ID
					int iProjectId = wtoi(vecData.at(KEY_IDX_PROJECTID).data());
					// ����ID
					int iCaseID = wtoi(vecData.at(KEY_IDX_CASEID).data());
					// ����CAEģ��ID�빤��ID�����ý������
					SetResultType(dimP, COMBOBOXID_SurfaceResultType, iProjectId, iCaseID, vecRstTypeCodes);

					double tStrValue = 0;
					double tEndValue = 0;
					s_pDCShape->m_bAutomatic = true;
					//ͨ���û���ѡ���ȡ�������
					s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_surfaceShowDlgInfo.indexReType);
					//Header
					s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
					//Unit
					s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

					DPoint3d minPos, maxPos;
					GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, iProjectId, iCaseID, s_pDCShape->ResultTypeCode, minPos, maxPos);
					//����ѡ��ľ��幤���ͽ����������ɫ��
					s_pDCShape->ProjectId = iProjectId;
					s_pDCShape->ProjectUnit = vecData.at(KEY_IDX_PROJECTUNIT);
					s_pDCShape->CaseID = iCaseID;
					s_pDCShape->StartValue = tStrValue;
					s_pDCShape->EndValue = tEndValue;
					bvector<DPoint3d>().swap(s_pDCShape->m_vecMaxPts);
					bvector<DPoint3d>().swap(s_pDCShape->m_vecMinPts);
					s_pDCShape->m_vecMaxPts.push_back(maxPos);
					s_pDCShape->m_vecMinPts.push_back(minPos);
					//���޸�ɫ���е�proName��caseName
					getProjectInfoById(s_pDCShape->ProjectName, UNUSE_WSTRING, iProjectId);

					WString strCaseName;
					mdlTreeNode_getDisplayTextWString(treePreNode, strCaseName);
					s_pDCShape->CaseName = strCaseName;

					//����Ϊ�Զ�����ɫ��
					s_pDCShape->m_bAutomatic = true;
					//�Զ��ȷ�ɫ����ֵ
					s_pDCShape->divideColorRange();

					break;
				}
				default:
					break;
				}				
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

void HookComboBox_Surface_Model(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
		g_surfaceShowDlgInfo.indexShowModel = 0;
		break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_surfaceShowDlgInfo.indexShowModel == 1)
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
void HookComboBox_Surface_ResultType(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_surfaceShowDlgInfo.indexReType = 0;
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_surfaceShowDlgInfo.indexReType);
		//Header
		s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
		//Unit
		s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

		DPoint3d minPos, maxPos;
		double tStrValue, tEndValue;
		GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, s_pDCShape->ProjectId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode, minPos, maxPos);
		//����ѡ��ľ��幤���ͽ����������ɫ��
		s_pDCShape->StartValue = tStrValue;
		s_pDCShape->EndValue = tEndValue;

		s_pDCShape->m_vecMaxPts.clear();
		s_pDCShape->m_vecMinPts.clear();
		s_pDCShape->m_vecMaxPts.push_back(maxPos);
		s_pDCShape->m_vecMinPts.push_back(minPos);

		s_pDCShape->m_bAutomatic = true;
		s_pDCShape->divideColorRange();
	}break;
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// ����չʾ->����鿴
void HookButton_Surface_ResultShow(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		enablefaceWidget(dimP->db, false);
		//��ɾ��֮ǰ���Ƶ�����ͼ����ʾCAEģ��
		ClearclipfaceShow();
		//��ɫ��
		s_pDCShape->showType = g_surfaceShowDlgInfo.indexShowType;
		//ͨ���û���ѡ���ȡ�������
		s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_surfaceShowDlgInfo.indexReType);
		s_pDCShape->StartDrawFunction();
		WString plotNmae = NGNAME_PREFIX_CAECLIP;
		if (g_surfaceShowDlgInfo.indexShowType == 0)//��ʾ��ͼ
		{
			DgnPlatform::LevelId levelId;
			// ������ȡ��CAE��ͼ��ͼ��
			if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE������ͼ", LEVEL_NULL_CODE))
				mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE������ͼ");
			ClipFace::ShowByType(plotNmae, ClipFace::DrawFacePlot, levelId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode, s_pDCShape->ProjectUnit);
		}
		else if (g_surfaceShowDlgInfo.indexShowType == 1)//��ʾ��ֵ��ͼ
		{
			DgnPlatform::LevelId levelId;
			// ������ȡ��CAE��ͼ��ͼ��
			if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE�����ֵ��ͼ", LEVEL_NULL_CODE))
				mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE�����ֵ��ͼ");
			ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceLine, levelId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode, s_pDCShape->ProjectUnit);
			DrawOutLineWhenContourLine(levelId);
		}
		else if (g_surfaceShowDlgInfo.indexShowType == 2)//��ʾʸ��ͼ
		{
			DgnPlatform::LevelId levelId;
			// ������ȡ��CAE��ͼ��ͼ��
			if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE����ʸ��ͼ", LEVEL_NULL_CODE))
				mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE����ʸ��ͼ");
			ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceVector, levelId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode, s_pDCShape->ProjectUnit);
			DrawOutLineWhenContourLine(levelId);
		}

		//�����Ҫ��ʾ���ֵ��Сֵ
		if (g_surfaceShowDlgInfo.bShowExtremum == -1)
		{
			s_pDCShape->m_bExtremum = true;
		}
		else
		{
			s_pDCShape->m_bExtremum = false;
		}

		mdlLevelTable_rewrite(MASTERFILE);
		enablefaceWidget(dimP->db, true);

		break;
	}break;
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_Surface_ShowMinMax(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
		s_pDCShape->m_bExtremum = g_surfaceShowDlgInfo.bShowExtremum == -1 ? true : false;
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
void HookPdm_Surface_Add(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		int lastAction = -1;
		if (SUCCESS == mdlDialog_openModal(&lastAction, NULL, DIALOGID_AddClipFace))
		{
			if (ACTIONBUTTON_OK == lastAction)
			{
				DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_Surface_ClipList, 0);
				GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
				int	numSelect = -1;
				Point2d* selectP = NULL;
				if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, tTree->rawItemP) && numSelect != 0)
				{
					GuiTreeCell*	treeCell = NULL;
					GuiTreeNode*	treeNode = NULL;

					treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
					treeNode = mdlTreeCell_getNode(treeCell);

					int treeLevel = mdlTreeNode_getLevel(treeNode);
					switch (treeLevel)
					{
					case 1:
					{
						treeNode = mdlTreeNode_getParent(treeNode);
					}break;
					case 2:
					{
						treeNode = mdlTreeNode_getParent(mdlTreeNode_getParent(treeNode));
					}
					break;
					default:
						break;
					}
					//������Ӧ��άModel��
					MSValueDescr uvalue;
					mdlTreeNode_getValue(treeNode, uvalue);

					WString instanceValue;

					uvalue.GetWString(instanceValue);
					bvector<WString> splitResult;
					BeStringUtilities::Split(instanceValue.data(), SeparatorAnd, splitResult);
					ModelId id = wtoi(splitResult[0].data());

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), id, TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						mdlModelRef_freeWorking(target);

						focusOnCAEModel(splitResult[1]);
					}

					WString ngName = splitResult[1];
					s_pDCShape->NgName = ngName;
					mdlInput_sendKeyin(L"msdi mscaesi action clipeface", 0, INPUTQ_EOQ, NULL);
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
void RefreshSurfaceShowDlg()
{
	DialogItemMessage dimP;
	MSDialog* mDialog =  mdlDialog_find(DIALOGID_SurfaceShow,NULL);
	if (mDialog!=NULL)
	{
		DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(mDialog, RTYPE_Tree, TREEID_Surface_ClipList, 0);
		GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
		mdlTreeModel_destroy(pModel, TRUE);
		dimP.messageType = DITEM_MESSAGE_INIT;
		dimP.dialogItemP = tTree;
		HookTree_Surface_ClipList(&dimP);
		mDialog->Refresh();
	}

}
void HookPdm_Surface_Del(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	 case DITEM_MESSAGE_BUTTON:
	 {
		 if (ClipFace::IsClipFaceModel())
		 {
			 s_pDCShape->CloseDrawFunction();
			 s_drawtempLine.CloseDrawFunction();
			 mdlModelRef_deleteModel(ACTIVEMODEL, FALSE);
			 RefreshSurfaceShowDlg();
		 }
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ClearclipfaceShow()//�������չʾ���
{
	if (ClipFace::IsClipFaceModel())
	{
		s_drawtempLine.CloseDrawFunction();
		ShowHideMesh();
		DgnPlatform::LevelId levelId;
		// ������ȡ��CAE��ͼ��ͼ��
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE������ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE������ͼ");
		{
			mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
			mdlLevel_delete(MASTERFILE, levelId);
		}
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE�����ֵ��ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE�����ֵ��ͼ");
		{
			mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
			mdlLevel_delete(MASTERFILE, levelId);
		}
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE����ʸ��ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE����ʸ��ͼ");
		{
			mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
			mdlLevel_delete(MASTERFILE, levelId);
		}

	}

}

void ShowHideMesh()//��ʾ���ص�CAEģ��
{
	DgnPlatform::LevelId levelId_CutFaceModel;
	WChar             name[MAX_MODEL_NAME_LENGTH];
	mdlModelRef_getModelName(ACTIVEMODEL, name);
	WString modelName;
	modelName.append(name);
	mdlLevel_getIdFromName(&levelId_CutFaceModel, MASTERFILE, LEVEL_NULL_ID, modelName.c_str());
	mdlLevel_setDisplay(MASTERFILE, levelId_CutFaceModel, TRUE);
}

void HookButton_Surface_ShowSet(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		openColorShapeSetting(s_pDCShape, SourceTypeDCS::SurfaceDisplay);
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void surfaceShowCmd(WCharCP unparsed)
{
	MSDialog *oDlialog = mdlDialog_find(DIALOGID_SurfaceShow, NULL);
	if (oDlialog)
		mdlDialog_show(oDlialog);
	else
		mdlDialog_open(NULL, DIALOGID_SurfaceShow);
}

void HookItem_LockAxis(DialogItemMessage* dmp)
{
	dmp->msgUnderstood = TRUE;
	switch (dmp->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_outRebarDlgInfo.chooseBtnIndex = 3;
		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		UInt32 msgId;
		if (0 == g_outRebarDlgInfo.chooseBtnIndex)
		{
			msgId = PROMPT_Surface_LockX;
		}
		else if (1 == g_outRebarDlgInfo.chooseBtnIndex)
		{
			msgId = PROMPT_Surface_LockY;
		}
		else if (2 == g_outRebarDlgInfo.chooseBtnIndex)
		{
			msgId = PROMPT_Surface_LockZ;
		}
		else
		{
			msgId = PROMPT_Surface_LockNone;
		}
		mdlOutput_rscPrintf(MSG_PROMPT, NULL, STRINGLISTID_Prompts, msgId);
		break;
	}
	default:
		dmp->msgUnderstood = FALSE;
		break;
	}
}

void HookDialog_Surface_AddClipFace(DialogMessage *dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_ACTIONBUTTON:
	{
		if (ACTIONBUTTON_OK == (dmP->u).actionButton.actionType)
		{
			WString strChk;
			strChk.append(g_outRebarDlgInfo.clipName);
			strChk.Trim();

			if (strChk.length() == 0)
			{
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"������������ƣ�", MessageBoxIconType::Information);
				//ȡ���Ի���Ĺر�
				dmP->u.actionButton.abortAction = TRUE;
			}
			else
			{
				wcscpy(g_outRebarDlgInfo.clipName, strChk.data());
			}
		}
	}
	break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}
