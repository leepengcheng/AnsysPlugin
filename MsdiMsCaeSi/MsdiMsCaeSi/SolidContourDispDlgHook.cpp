#include "MsdiMsCaeSipch.h"

extern SolidDispDlgInfo  g_solidDispDlgInfo;
extern SurfaceShowDlgInfo g_surfaceShowDlgInfo;
// CAE���������
extern bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
extern bmap<WString, WString> g_RstType_Unit;

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

//�����ڱ��Ի�����������ݣ���¼�û�������й�������������
WString	ngName, caseName, sUnit;
int		iProjectId, caseID;

int m_iCamera;// ���״̬
int m_iRenderMode;// ��Ⱦģʽ

MSDialogP jdoytBar = NULL;
WCharP 	  jdwcMessageText = NULL;
int       jdPercent = 0;
bool	bConditionUpdFlg;
extern bvector<WString> vecRstTypeCodes;
extern bool alreadyUnload;

/*----------------------------------------------------------------------+
* The command entry point for user function.				            *
* @param        unparsed     unused unparsed argument to the command    *
+----------------------------------------------------------------------*/
void SetCAERstDispCmd(WCharCP unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
	if (Dlialog)
	{
		mdlDialog_show(Dlialog);
	}
	else
	{
		mdlDialog_open(NULL, DIALOGID_SolidContourDisp);
	}

}
void startShowCutFaceContour(DrawColorShape* pDCShape)
{

	ClearclipfaceShow();
	//��ɫ��
	pDCShape->StartDrawFunction();
	pDCShape->showType = g_surfaceShowDlgInfo.indexShowType;

	WString plotNmae = NGNAME_PREFIX_CAECLIP;
	DgnPlatform::LevelId levelId;
	if (g_surfaceShowDlgInfo.indexShowType == 0)//��ʾ��ͼ
	{
		// ������ȡ��CAE��ͼ��ͼ��
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE������ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE������ͼ");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFacePlot, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	else if (g_surfaceShowDlgInfo.indexShowType == 1)//��ʾ��ֵ��ͼ
	{
		// ������ȡ��CAE��ͼ��ͼ��
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE�����ֵ��ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE�����ֵ��ͼ");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceLine, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	else if (g_surfaceShowDlgInfo.indexShowType == 2)//��ʾʸ��ͼ
	{
		// ������ȡ��CAE��ͼ��ͼ��
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE����ʸ��ͼ", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE����ʸ��ͼ");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceVector, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	//�����Ҫ��ʾ���ֵ��Сֵ
	if (g_surfaceShowDlgInfo.bShowExtremum == -1)
	{
		bvector<DPoint3d> meshminpoints;
		bvector<DPoint3d> meshmaxpoints;
		DPoint3d minpoint;
		DPoint3d maxpoint;
		double tStrValue, tEndValue;
		GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, pDCShape->ProjectId, pDCShape->CaseID, pDCShape->ResultTypeCode, minpoint, maxpoint);

		s_pDCShape->m_vecMaxPts.clear();
		s_pDCShape->m_vecMinPts.clear();

		s_pDCShape->m_vecMaxPts.push_back(maxpoint);
		s_pDCShape->m_vecMinPts.push_back(minpoint);
		s_pDCShape->m_bExtremum = true;
	}
	else
	{
		s_pDCShape->m_bExtremum = false;
	}
	mdlLevelTable_rewrite(MASTERFILE);
}

void startShowContour(DrawColorShape* pDCShape, DPoint3dCP BasePt /*= NULL*/, bool bClear /*= true*/)
{
	//��ɾ��֮ǰ���Ƶ�����ͼ
	if (bClear)
		ClearContour();

	ShowCAEModel();

	//���ݴ�ɫ��
	pDCShape->StartDrawFunction();

	//��¼��������������ڲ�ID
	LevelId lvID_contour;
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15);
	if (pDCShape->showType == 0)//��ʾ��ͼ
	{
		lvID_contour = showContourPlot(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
	}
	else if (pDCShape->showType == 1)//��ʾ��ֵ��ͼ
	{
		lvID_contour = showContourLine(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
		showContourOutFaceLine(pDCShape->NgName, pDCShape, lvID_contour, BasePt);
	}
	else if (pDCShape->showType == 2)//��ʾʸ��ͼ
	{
		lvID_contour = showContourVector(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
		showContourOutFaceLine(pDCShape->NgName, pDCShape, lvID_contour, BasePt);
	}
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 90);
	//�����Ҫ��ʾ���ֵ��Сֵ
	if (s_pDCShape->m_bExtremum)
	{
		//�ۺ�ƫ��ֵ�õ���ʵ���겢�ڴ˴����Ʊ�ʶ
		DPoint3d basePt;
		WString sUnit;
		LevelId  lvID;
		MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, lvID, pDCShape->NgName);

		//����ɫ����DbStartValue��DbEndValue�������ݿ��ȡ��Ӧ��Node����
		DPoint3d ptMax = getNodeCoordinate(pDCShape->ProjectId, pDCShape->CaseID, ws2s(pDCShape->ResultTypeCode.data()).data(), pDCShape->DbEndValue - 0.000001);
		ptMax.Scale(UOR_PER_UNIT(sUnit));
		DPoint3d ptMin = getNodeCoordinate(pDCShape->ProjectId, pDCShape->CaseID, ws2s(pDCShape->ResultTypeCode.data()).data(), pDCShape->DbStartValue);
		ptMin.Scale(UOR_PER_UNIT(sUnit));

		ptMax.Add(basePt);
		ptMin.Add(basePt);

		pDCShape->m_vecMaxPts.clear();
		pDCShape->m_vecMinPts.clear();

		pDCShape->m_vecMaxPts.push_back(ptMax);
		pDCShape->m_vecMinPts.push_back(ptMin);

	}

	mdlLevelTable_rewrite(MASTERFILE);

	//����ͼ�۽�����Ӧ��CAEģ��λ��
	focusOnCAEModel(pDCShape->NgName);
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 100);
	mdlDialog_completionBarClose(jdoytBar);
}

void HookSash_SolidDisp_vSashHook
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
				SolidDisp_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_SolidDisp(DialogItemMessage *dimP)
{
	RawItemHdr      *riP = dimP->dialogItemP->rawItemP;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		GuiTreeNode     *pRoot;
		MSValueDescr     valueDescr;

		GuiTreeModel* pModel = mdlTreeModel_create(1);

		pRoot = mdlTreeModel_getRootNode(pModel);
		mdlTreeNode_setDisplayText(pRoot, L"ROOT");

		NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
		for each (NamedGroupPtr namedGroup in *ngcCAESolid)
		{
			WString ngName = namedGroup->GetName();

			WString instanceName;
			if (splitNgName_Model(ngName, UNUSE_WSTRING, iProjectId, UNUSE_INT, instanceName, sUnit))
			{
				//����ʵ���ڵ�
				GuiTreeNode*	pProjectNode = NULL;
				pProjectNode = mdlTreeNode_create(pModel, TRUE);

				//���ýڵ����ʾ����
				mdlTreeNode_setDisplayText(pProjectNode, instanceName.data());

				//����Ӧ��������������
				MSValueDescr    valueDescr;
				valueDescr.SetWChar(ngName.GetWCharCP());
				mdlTreeNode_setValue(pProjectNode, valueDescr, FALSE);

				mdlTreeNode_setCollapsedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedSelectedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//���ڵ���ӵ����ڵ���
				mdlTreeNode_addChild(pRoot, pProjectNode);

				bvector<int> vecCaseIDs;
				bvector<WString> vecCaseNames;
				bvector<WString> vecCaseProps;
				getCaseInfoByProjectid(iProjectId, vecCaseIDs, vecCaseNames, vecCaseProps);
				for (int i = 0; i < vecCaseNames.size(); i++)
				{
					// ���ù����ڵ���ʾ����
					WString caseName = vecCaseNames.at(i);
					GuiTreeNode*	pCaseNode = NULL;
					pCaseNode = mdlTreeNode_create(pModel, FALSE);
					mdlTreeNode_setDisplayText(pCaseNode, caseName.c_str());

					// ���ù����ڵ�ֵ
					MSValueDescr    valueDescr;
					valueDescr.SetInt32(vecCaseIDs.at(i));
					mdlTreeNode_setValue(pCaseNode, valueDescr, FALSE);

					mdlTreeNode_setLeafIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

					// ��ӽڵ㵽�ؼ���
					mdlTreeNode_addChild(pProjectNode, pCaseNode);
				}
			}
		}

		mdlTreeNode_expand(pRoot);

		mdlDialog_treeSetTreeModelP(riP, pModel);
		break;
	}
	case DITEM_MESSAGE_STATECHANGED://�����������Ӧ
	{
		//���tree�н�����Ҫ���µ�ǰ������
		wcscpy(g_solidDispDlgInfo.current, L"δѡ�񹤿�");

		//ȱʡ�����ֿؼ��費���ã���ѡ���˹���������Ϊ����
		enableWidget(dimP->db, false);

		GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(riP);
		int	numSelect = -1;
		Point2d* selectP = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, riP) && numSelect != 0)
		{
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
			treeNode = mdlTreeCell_getNode(treeCell);

			//�����û��û�ѡ��Ľ����¡���ǰѡ�񡱵�����
			wcscpy(g_solidDispDlgInfo.current, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());

			//���ýڵ��Ѿ�û���ӽڵ�����Ϊѡ���˹����ڵ�
			if (!mdlTreeNode_getAllowsChildren(treeNode))
			{
				MSValueDescr  nodeVal;

				WString ngNameNew;
				mdlTreeNode_getValue(mdlTreeNode_getParent(treeNode), nodeVal);
				nodeVal.GetWString(ngNameNew);

				//���˴�ѡ���ʵ��������һ�εĲ�һ��,��������²���
				if (ngName.CompareTo(ngNameNew) != 0)
				{
					//�ر���һ��ʵ������ͼ��ʾ���ر�ɫ���������ڵĻ���
					ShowCAEModel();
					ClearContour();
					s_pDCShape->CloseDrawFunction();
					s_drawtempLine.CloseDrawFunction();
					//�۽�����ʵ��λ��
					focusOnCAEModel(ngNameNew);

					//��ղ�����ʾɸѡ����
					DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
					ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
					mdlListModel_empty(pListModel, true);
					mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);

					//���¼��ض�Ӧ������ֵ
					initConditionValuesByTypes(dimP->db);
				}
				//CaseID
				mdlTreeNode_getValue(treeNode, nodeVal);
				s_pDCShape->CaseID = nodeVal.GetInt32();

				// ������ĿID�͹���ID�����ý������
				SetResultType(dimP, COMBOBOXID_ResultType, iProjectId, s_pDCShape->CaseID, vecRstTypeCodes);

				//��������ͷ����仯ʱ����Ҫ���¼���ɫ��ֵ
				s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_solidDispDlgInfo.ResultType);

				//�����ֿؼ���Ϊ����
				enableWidget(dimP->db, true);
				dimP->db->Refresh();

				//����ѡ��Ĺ����ڵ����ݸ�ֵ��ɫ���ĳ�Ա����
				//Header
				s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
				//Unit
				s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

				//showType
				s_pDCShape->showType = g_solidDispDlgInfo.ShowType;

				//ProjectName & ProjectId
				splitNgName_Model(ngNameNew, s_pDCShape->ProjectName, s_pDCShape->ProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

				//CaseName
				mdlTreeNode_getDisplayTextWString(treeNode, s_pDCShape->CaseName);

				//NgName
				s_pDCShape->NgName = ngNameNew;

				//����Ϊ�Զ�����ɫ��
				s_pDCShape->m_bAutomatic = true;
				//������ʵ������ɫ�������ֵ��Сֵ
				s_pDCShape->initColorRangeFromDB();
				//�Զ��ȷ�ɫ����ֵ
				s_pDCShape->divideColorRange();

				ngName = ngNameNew;
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

void SolidDisp_adjustVSashDialogItems
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
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_SolidDis_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_SolidDis_ProjList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_SolidDis_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookDialog_SolidDisp(DialogMessage *dmP)
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

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_SolidDis_V, 0);
		if (!diP)
			break;

		SolidDisp_adjustVSashDialogItems(dmP->db, NULL, TRUE);
		break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING:
	{
		int        minSize = (30 * XC);

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

		SolidDisp_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		// �ر���ͼ��������������״̬
		ViewFlags flags;
		mdlView_getFlags(&flags, tcb->lstvw);
		m_iCamera = flags.camera;// ������ͼ�����״̬
		m_iRenderMode = flags.renderMode; // ������ͼ����Ⱦģʽ
		flags.camera = 0;// �ر���ͼ�����
		bConditionUpdFlg = false;
		mdlView_setFlags(&flags, tcb->lstvw);

		//ȱʡ�����ֿؼ��費���ã���ѡ���˹���������Ϊ����
		enableWidget(dmP->db, false);

		//��ʼ���ؼ���ʾ
		wcscpy(g_solidDispDlgInfo.current, L"δѡ�񹤿�");
		g_solidDispDlgInfo.ResultType = g_solidDispDlgInfo.ShowType = 0;
		//Ĭ���Թ⻬ģʽ��ʾ
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);

		mdlDialog_itemsSynch(dmP->db);

		//�˹���ʹ����ɫ��
		s_pDCShape = &s_DrawColorShape;

	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		// MS������ر�ʱ���ô���ִ��
		if (alreadyUnload != true)
		{
			ClearContour();
			ShowCAEModel();
			ClearBoundary();

			s_pDCShape->CloseDrawFunction();
			s_drawtempLine.CloseDrawFunction();
			// ��ԭ��ͼ����
			ViewFlags flags;
			mdlView_getFlags(&flags, tcb->lstvw);
			flags.camera = m_iCamera;
			flags.renderMode = m_iRenderMode;
			mdlView_setFlags(&flags, tcb->lstvw);

			//������ģʽ��ʾ
			ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// ����
		}
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_ShowSet(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		openColorShapeSetting(s_pDCShape, SourceTypeDCS::SolidDisplay);
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_ResultType(DialogItemMessage *dimP)
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
	case DITEM_MESSAGE_STATECHANGED:
	{
		//��������ͷ����仯ʱ����Ҫ���¼���ɫ��ֵ
		s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_solidDispDlgInfo.ResultType);

		//Header
		s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
		//Unit
		s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];


		s_pDCShape->m_bAutomatic = true;

		s_pDCShape->initColorRangeFromDB();
		s_pDCShape->divideColorRange();
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
void HookComboBox_SolidDisp_ShowModel(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
		g_solidDispDlgInfo.ShowModel = 0;
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// �⻬
		break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_solidDispDlgInfo.ShowModel == 1)
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
void HookComboBox_SolidDisp_CondType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_solidDispDlgInfo.indexCType = g_solidDispDlgInfo.indexCOpt = g_solidDispDlgInfo.indexCValue = 0;
	}
	break;
	case DITEM_MESSAGE_INIT:
	case DITEM_MESSAGE_STATECHANGED:
	{
		//�����������͡���ʼ���͸���ʱ����ѯ���ݿ��п��ܵ�ֵ����Ϊ������ֵ����ֵ
		initConditionValuesByTypes(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_CondVal(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		break;
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	case DITEM_MESSAGE_INIT:
	{

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_AddC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//���ݶ�ӦcomboBox�е����ݣ������в����Ӧ���ݲ�������ʵ�ʵ�����
		DialogItem* dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		ListModel*	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
		if (pListModel == NULL)
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);

		ListRowP	pRow = mdlListRow_create(pListModel);
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		//ͨ����Դ�ļ���StringList�����ȡcomboxBox����ʾֵ
		RscFileHandle   rfHandle;
		mdlResource_openFile(&rfHandle, L"partialDisplayDlg.r", RSC_READONLY);
		StringListP sl = NULL;
		WString		wsCBoxValue;

		//ɸѡ�������������±�
		cell = mdlListRow_getCellAtIndex(pRow, 0);

		sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_ConditionType);
		mdlStringList_getMemberString(wsCBoxValue, NULL, sl, (long)g_solidDispDlgInfo.indexCType);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());

		cellValue.SetInt32(g_solidDispDlgInfo.indexCType);
		mdlListCell_setValue(cell, cellValue, FALSE);

		//ɸѡ���������������±�
		cell = mdlListRow_getCellAtIndex(pRow, 1);

		sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_Condition);
		mdlStringList_getMemberString(wsCBoxValue, NULL, sl, (long)g_solidDispDlgInfo.indexCOpt);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());

		cellValue.SetInt32(g_solidDispDlgInfo.indexCOpt);
		mdlListCell_setValue(cell, cellValue, FALSE);

		//ɸѡ����ֵ��������ʵ��ID
		ListModel*	pListModel_CValue = mdlDialog_comboBoxGetListModelP(mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0)->rawItemP);
		cell = mdlListModel_getCellAtIndexes(pListModel_CValue, g_solidDispDlgInfo.indexCValue, 0);
		mdlListCell_getDisplayTextWString(cell, wsCBoxValue);
		mdlListCell_getValue(cell, cellValue);

		cell = mdlListRow_getCellAtIndex(pRow, 2);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());
		mdlListCell_setValue(cell, cellValue, FALSE);

		mdlListModel_addRow(pListModel, pRow);

		mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
		mdlDialog_itemsSynch(dimP->db);

		bConditionUpdFlg = true;
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_DelC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//ɾ���б��е�ѡ��������
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		int row = -1, col = -1;
		mdlDialog_listBoxGetLocationCursor(&row, &col, dip_cList->rawItemP);

		if (row != -1)
		{
			ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
			mdlListModel_removeRowAtIndex(pListModel, row, true);
			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
			mdlDialog_itemsSynch(dimP->db);

			bConditionUpdFlg = true;
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_ClearC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//����б��е�����ɸѡ����
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);

		if (pListModel != NULL && mdlListModel_getRowCount(pListModel) > 0)
		{
			mdlListModel_empty(pListModel, true);
			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
			mdlDialog_itemsSynch(dimP->db);

			bConditionUpdFlg = true;
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// ��չʾ->����鿴
void HookButton_SolidDisp_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		MSDialog *db = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
		if (db == NULL)
			return;
		//ȱʡ�����ֿؼ��費���ã���ѡ���˹���������Ϊ����
		enableWidget(dimP->db, false);

		//������
		size_t len = strlen(TXT_CAEShowIng) + 1;
		jdwcMessageText = (WCharP)malloc(len*sizeof(WChar));
		BeStringUtilities::CurrentLocaleCharToWChar(jdwcMessageText, TXT_CAEShowIng, len);
		// �򿪽�����
		jdoytBar = mdlDialog_completionBarOpen(jdwcMessageText);

		if (bConditionUpdFlg == true)
		{
			//������ͼ����չʾ�����޸����ݿ��еı�־λ
			stringstream strSql;
			Db m_db;

			if (OpenDB(m_db) != BE_SQLITE_OK)
				return;

			Statement *m_statment = new Statement();
			Savepoint *m_savept = new Savepoint(m_db, "asd", true);
			m_savept->Begin();

			DialogItem* dip_cList = mdlDialog_itemGetByTypeAndId(db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
			ListModel*	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
			int rowCnt;
			if (pListModel != NULL && (rowCnt = mdlListModel_getRowCount(pListModel)) != 0)
			{
				MSValueDescr value;

				//�Ƚ����ݿ��е�����ContourShowFlg��Ϊ0
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 0 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName ");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				Utf8String sNgName;
				BeStringUtilities::WCharToUtf8(sNgName, ngName.c_str());
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();

				//�������е�ɸѡ����ƴ�ӳ�update sql��佫����ContourShowFlg��Ϊ1
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 1 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND EXISTS(SELECT a.ElemID FROM ELEMENTS a WHERE a.ProjectId = @ProID AND  a.ElemID = FACES.ElemID ");
				for (int i = 0; i < rowCnt; i++)
				{
					ListCellP cell;
					strSql << (" AND ");

					//ɸѡ������
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 0);
					mdlListCell_getValue(cell, value);
					strSql << (value.GetInt32() == 0 ? "a.ElemType" : (value.GetInt32() == 1 ? "a.MaterialNumber" : "a.RealConstantNumber"));

					//ɸѡ������
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 1);
					mdlListCell_getValue(cell, value);
					strSql << (value.GetInt32() == 0 ? "=" : "<>");

					//ɸѡֵID
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 2);
					mdlListCell_getValue(cell, value);
					strSql << value.GetInt32();
				}
				strSql << (")");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();
			}
			else
			{
				//������ݿ��е�����ɸѡ����
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 1 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName ");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				Utf8String sNgName;
				BeStringUtilities::WCharToUtf8(sNgName, ngName.c_str());
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();
			}

			m_savept->Commit();

			delete m_savept;
			delete m_statment;

			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 5);
			m_db.CloseDb();

			//�Զ�����ʱ�����¼���ɫ����ֵ
			if (s_pDCShape->m_bAutomatic == true)
			{
				//������ʵ������ɫ�������ֵ��Сֵ
				s_pDCShape->initColorRangeFromDB();
				//�Զ��ȷ�ɫ����ֵ
				s_pDCShape->divideColorRange();
			}
			bConditionUpdFlg = false;
		}

		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 10);
		//����ɫ��������ʾ����
		startShowContour(s_pDCShape);
		jdoytBar = NULL;
		jdwcMessageText = L"";
		jdPercent = 0;

		//ȱʡ�����ֿؼ��費���ã���ѡ���˹���������Ϊ����
		enableWidget(dimP->db, true);

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_Damage_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{

		MSDialog *db = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
		if (db == NULL)
			return;
		//ȱʡ�����ֿؼ��費���ã���ѡ���˹���������Ϊ����
		enableWidget(dimP->db, false);

		int ProjectId;
		bvector<ElementId> shows;

		NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
		for each (NamedGroupPtr namedGroup in *ngcCAESolid)
		{

			WString ngName = namedGroup->GetName();
			splitNgName_Model(ngName, UNUSE_WSTRING, ProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

			NamedGroupPtr namedGroup = NamedGroupCollection::Create(*ACTIVEMODEL)->FindByName(ngName.data());
			if (namedGroup.IsValid())
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);

				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();

					EditElementHandle eeh;
					eeh.FindByID(elemid, ACTIVEMODEL);

					XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
					XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
					MeshProperty *meshPro = NULL;

					if (xh.IsValid())
						meshPro = (MeshProperty *)xh.PeekData();

					if (meshPro != NULL)
					{
						shows.push_back(elemid);
					}

				}
			}

			DgnModelRefP target = NULL;
			if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), ProjectId, TRUE, TRUE))
			{
				if (!mdlModelRef_areSame(MASTERFILE, target))
				{
					mdlModelRef_activateAndDisplay(target);
				}
			}
			Db m_db;
			StatusInt rc = OpenDB(m_db);
			if (rc == BE_SQLITE_OK)
			{
				Savepoint *m_savept = new Savepoint(m_db, "asd", true);
				m_savept->Begin();
				Statement *m_statment = new Statement();
				m_statment->BindInt(1, ProjectId);
				AString lsSql;
				//��ProjectId��ʽ��
				char id[50];
				sprintf(id,"%d", ProjectId);
				lsSql.append("select ElemId from damage where damage.ProjectId=");
				lsSql.append(id);
				lsSql.append("  and damage.Status=0;");
				rc = m_statment->Prepare(m_db, lsSql.c_str());
			//	rc = m_statment->Prepare(m_db, "select ElemId from damage where damage.ProjectId=0  and damage.Status=0;");
				if (rc == BE_SQLITE_OK)
				{
					DgnModelRefP target = NULL;
					mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), ProjectId, TRUE, TRUE);
					SelectionSetManagerR selectSetM = SelectionSetManager::GetManager();
					while (m_statment->Step() == BE_SQLITE_ROW)
					{
						int eid = m_statment->GetValueInt(0);
						for (auto iter = shows.begin(); iter != shows.end(); iter++)
						{
							EditElementHandle eeh;
							eeh.FindByID(*iter, ACTIVEMODEL);

							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
							XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
							MeshProperty *meshPro = NULL;

							if (xh.IsValid())
								meshPro = (MeshProperty *)xh.PeekData();

							if (meshPro != NULL)
							{
								if (eid == meshPro->ElemID)
								{	
									selectSetM.AddElement(eeh.GetElementRef(), MASTERFILE);
									break;
								}
							}

						}
					}
					ElementAgenda eAgenda;
					selectSetM.BuildAgenda(eAgenda);
					eAgenda.Hilite();
					mdlModelRef_freeWorking(target);
				}
			}
			mdlModelRef_freeWorking(target);
		}

	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_ShowType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		s_pDCShape->showType = g_solidDispDlgInfo.ShowType;
		s_pDCShape->m_bExtremum = g_solidDispDlgInfo.bShowExtremum == -1 ? true : false;

		DialogItemP diP_Widget = NULL;
		bool bEnable = false;

		// ֻ����ͼ��ʾʱ������鿴�ͽڵ�鿴���ܲ���ʹ��
		if (g_solidDispDlgInfo.ShowType == 0)
			bEnable = true;

		//��ͼ����鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, bEnable, false);

		//�ڵ�鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, bEnable, false);

		//mdlDialog_itemGetState

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void enableWidget(MSDialogP db, bool enable)
{
	DialogItemP diP_Widget = NULL;

	//����չʾ��ؿؼ�
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionType, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionOpt, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_AddC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_DelC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_ClearC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

	//��ʾ��ͼ
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_Show, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	//�ƻ���ʾ
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_Damage, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	

	// ֻ����ͼ��ʾʱ������鿴�ͽڵ�鿴���ܲ���ʹ��
	if (g_solidDispDlgInfo.ShowType == 0)
	{
		//��ͼ����鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

		//�ڵ�鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	}
	else
	{
		//��ͼ����鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, false, false);

		//�ڵ�鿴
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, false, false);
	}

	//��ʾ����
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_ShowType, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

	//����
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ShowSet, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
}

void initConditionValuesByTypes(MSDialogP db)
{
	DialogItem* dip_cValue = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0);
	if (dip_cValue == NULL)
		return;
	RawItemHdr*	rItem_cValue = dip_cValue->rawItemP;
	ListModel*	pListModel = mdlDialog_comboBoxGetListModelP(rItem_cValue);

	if (pListModel == NULL)
		mdlListModel_create(MAX_COLUMN_COUNT);
	else
		mdlListModel_empty(pListModel, true);

	//���ݵ�ǰѡȡ��ʵ����Ӧ�Ĺ���ID��ȡ��ɸѡ����ֵID,������ID��ȡ���Ӧ������
	bvector<int> vecValueIDs;
	bvector<WString> vecValueNames;
	switch (g_solidDispDlgInfo.indexCType)
	{
		//��Ԫ����
	case 0:
	{
		getElemTypeIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString elemTypeName;
			getElemNameByPK(iProjectId, vecValueIDs[i], elemTypeName);
			vecValueNames.push_back(elemTypeName);
		}
	}break;
	//��������
	case 1:
	{
		getMaterialIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString mName, mType;
			getMaterialAllByPK(iProjectId, vecValueIDs[i], mName, mType, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING);

			//Type-Nameƴ�ɲ�������
			if (mType.length() > 0)
				mType.append(L"-");
			mType.append(mName);
			vecValueNames.push_back(mType);
		}
	}break;
	//ʵ����
	case 2:
	{
		getRealConsIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString realConsProp;
			getRealConsPropByPK(iProjectId, vecValueIDs[i], realConsProp);
			vecValueNames.push_back(realConsProp);
		}
	}break;
	default:
		break;
	}

	if (vecValueIDs.empty())
		return;

	//����ɸѡ����ֵ������
	ListRowP	pRow = NULL;
	ListCellP	cell = NULL;
	MSValueDescr cellValue;

	for (int i = 0; i < (int)vecValueIDs.size(); i++)
	{
		pRow = mdlListRow_create(pListModel);
		cell = mdlListRow_getCellAtIndex(pRow, 0);

		//����������Ϊ��ʾ����
		mdlListCell_setDisplayText(cell, vecValueNames[i].data());

		//��ӦID�洢����Value��			
		cellValue.SetInt32(vecValueIDs[i]);
		mdlListCell_setValue(cell, cellValue, FALSE);
		mdlListModel_addRow(pListModel, pRow);
	}

	mdlDialog_comboBoxSetListModelP(rItem_cValue, pListModel);

	g_solidDispDlgInfo.indexCValue = 0;

	mdlDialog_itemsSynch(db);
}


BENTLEY_NAMESPACE_NAME::StatusInt GetBoundarySolidSQL(Statement* pStatement, BeSQLiteDbCR db)
{
	AString strSql;
	strSql.append("SELECT \n");
	strSql.append(" a.ElemID,BCType,MaterialNumber,ElemType,RealConstantNumber, \n");
	strSql.append(" ID1,X1,Y1,Z1,ID2,X2,Y2,Z2,ID3,X3,Y3,Z3,ID4,X4,Y4,Z4 \n");
	strSql.append("FROM \n");
	strSql.append(" (SELECT ElemID, f.BCType as BCType,  \n");
	strSql.append(" node1.ID as ID1, node1.X as X1, node1.Y as Y1, node1.Z as Z1, \n");
	strSql.append(" node2.ID as ID2, node2.X as X2, node2.Y as Y2, node2.Z as Z2, \n");
	strSql.append(" node3.ID as ID3, node3.X as X3, node3.Y as Y3, node3.Z as Z3, \n");
	strSql.append(" node4.ID as ID4, node4.X as X4, node4.Y as Y4, node4.Z as Z4  \n");
	strSql.append(" FROM \n");
	strSql.append(" (SELECT FACES.ElemID,Node1,Node2,Node3,Node4,BCType FROM FACES INNER JOIN BOUNDARY ON FACES.NodeIdStr = BOUNDARY.NodeIdStr WHERE FACES.ProjectId = @ProjectId GROUP BY BOUNDARY.NodeIdStr HAVING COUNT(BOUNDARY.NodeIdStr) < 2) f \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node1 ON f.node1 = node1.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node2 ON f.node2 = node2.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node3 ON f.node3 = node3.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node4 ON f.node4 = node4.ID ) a \n");
	strSql.append("LEFT OUTER JOIN \n");
	strSql.append(" (SELECT ElemID,MaterialNumber,ElemType,RealConstantNumber FROM ELEMENTS WHERE ProjectId = @ProjectId) b \n");
	strSql.append("ON a.ElemID=b.ElemID \n");

	return pStatement->Prepare(db, strSql.c_str());
}

void BuildBoundaryFaces(int projectId, int caseId, WCharCP ngName)
{
	// ������ȡ�ñ߽���Ϣ��ͼ��
	DgnPlatform::LevelId levelId_boundary = 0;
	if (SUCCESS != mdlLevel_create(&levelId_boundary, MASTERFILE, L"BOUNDARY", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_boundary, MASTERFILE, LEVEL_NULL_ID, L"BOUNDARY");
	mdlLevel_setActive(levelId_boundary);
	
	//�����ݿ�
	Db db;
	if (BE_SQLITE_OK != OpenDB(db)) return;

	Statement* pStatement = new Statement();
	if (BE_SQLITE_OK != GetBoundarySolidSQL(pStatement,db)) return;
	// ���ò�ѯ����
	pStatement->BindInt(1, projectId);

	//��ȡbasePt
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);

	// ѭ��CAE��Ԫ�����ݣ���MS�����ɵ�Ԫ��
	MSElementDescrP resultEdp;
	while (pStatement->Step() == BE_SQLITE_ROW)
	{
		bvector<int> vecNodeIds;			// Mesh�����Ӧ��CAE����ڵ㼯��
		bvector<int> vecIndices;			// Mesh��Ľڵ㼯��
		bvector<DPoint3d> vecPoints;		// Mesh��Ķ������꼯��
		bvector<WString> vecFaceNodeStr;

		// ȡ��CAE��Mesh��Ľڵ�Ͷ������꼯��
		GetBoundaryFacesInfo(vecIndices, vecPoints, vecFaceNodeStr, vecNodeIds, &basePt, sUnit, pStatement);
		// ���ڵ���>=3�����ɵ�Ԫ���Ԫ��
		if (vecPoints.size() >= 3)
		{
			//����������
			MSElementDescrP edpFace = NULL;
			if (SUCCESS == mdlMesh_newVariablePolyface(&resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
			{
				// ����Mesh���ElementHandle
				EditElementHandle eeh(resultEdp, true, true, ACTIVEMODEL);

				ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
				//�߽�����(2,3,4,5,6,7,8,10,12,14,20,24,31,36,37)
				propsSetter->SetColor(5);
				propsSetter->SetLevel(levelId_boundary);
				propsSetter->Apply(eeh); 
				// ElementHandle��ӵ���ͼ��
				eeh.AddToModel();
			}
		}
	}

	pStatement->Finalize();

	db.CloseDb();
	mdlLevelTable_rewrite(MASTERFILE);
	MsdiMsCaeSiUtility::fitView(0);
	//mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);

	//��ȡ������
	//ShowOuterContour(projectId,caseId,levelId_boundary,basePt);
}

void GetBoundaryFacesInfo(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt, WStringCR sUnit, Statement* pStatement)
{
	int iIdx = 5;
	// ��Ľڵ�ID
	bvector<int> vecNodes;
	for (int i = 0; i < 4; i++)
	{
		int iNodeID = pStatement->GetValueInt(iIdx);
		if (iNodeID < 1) continue;

		auto l_it = find(vecNodes.begin(), vecNodes.end(), iNodeID);
		if (l_it == vecNodes.end())
		{
			vecNodes.push_back(iNodeID);
			vecPoints.push_back(GetPointFromBasePoint(basePt, sUnit, iIdx, pStatement));
		}
		iIdx += 4;
	}
	if (vecNodes.size() == 4)
	{
		//����Ϊ4ʱ���ı߾�����
		MakeBoundaryFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 2, 1);
	}
	else if (vecNodes.size() == 3)
	{
		//����Ϊ3ʱ��������
		MakeBoundaryFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 2, 1, -1);
	}
	bvector<int>().swap(vecNodes);
}

DPoint3d GetPointFromBasePoint(DPoint3dCP basePt, WStringCR sUnit, int iIdx, Statement* pStatement)
{
	DPoint3d ptTmp;

	ptTmp.x = basePt->x + pStatement->GetValueDouble(iIdx + 1) *UOR_PER_UNIT(sUnit);
	ptTmp.y = basePt->y + pStatement->GetValueDouble(iIdx + 2) *UOR_PER_UNIT(sUnit);
	ptTmp.z = basePt->z + pStatement->GetValueDouble(iIdx + 3) *UOR_PER_UNIT(sUnit);

	return ptTmp;
}

void MakeBoundaryFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4)
{
	WString sNodeIdKey;
	int iNodeId1 = -1, iNodeId2 = -1, iNodeId3 = -1, iNodeId4 = -1;
	iNodeId1 = vecNodes.at(node1 - 1);
	iNodeId2 = vecNodes.at(node2 - 1);
	iNodeId3 = vecNodes.at(node3 - 1);
	if (node4 > 0)
		iNodeId4 = vecNodes.at(node4 - 1);

	vecIndices.push_back(node1);
	vecIndices.push_back(node2);
	vecIndices.push_back(node3);
	if (node4 > 0)
		vecIndices.push_back(node4);
	vecIndices.push_back(0);

	vecNodeId.push_back(iNodeId1);
	vecNodeId.push_back(iNodeId2);
	vecNodeId.push_back(iNodeId3);
	if (node4 > 0)
		vecNodeId.push_back(iNodeId4);
	vecNodeId.push_back(0);

	sNodeIdKey = MakeNodeIdKey(iNodeId1, iNodeId2, iNodeId3, iNodeId4);
	vecFaceNodeStr.push_back(sNodeIdKey);
}

BENTLEY_NAMESPACE_NAME::WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4)
{
	WString sKey;
	int iTmp;

	if (NodeId4 != -1)
	{
		if (NodeId1 < NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1 < NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId1 < NodeId4) { iTmp = NodeId1; NodeId1 = NodeId4; NodeId4 = iTmp; }
		if (NodeId2 < NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2 < NodeId4) { iTmp = NodeId2; NodeId2 = NodeId4; NodeId4 = iTmp; }
		if (NodeId3 < NodeId4) { iTmp = NodeId3; NodeId3 = NodeId4; NodeId4 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d-%d", NodeId1, NodeId2, NodeId3, NodeId4);
	}
	else
	{
		if (NodeId1 < NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1 < NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2 < NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d", NodeId1, NodeId2, NodeId3);
	}

	return sKey;
}

//��ȡngName��ִ�б߽���ʾ
void HookButton_Boundary_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{    
	case DITEM_MESSAGE_BUTTON:
	{
		WString ngName;
		DialogItem*   pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_SolidDis_ProjList, 0);
		if (pTree)
		{
			GuiTreeModel* pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			int	numSelect = -1;
			Point2d* selectP = NULL;
			if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, pTree->rawItemP) && numSelect != 0)
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, selectP[0].y, selectP[0].x);
				treeNode = mdlTreeCell_getNode(treeCell);

				//���ýڵ��Ѿ�û���ӽڵ�����Ϊѡ���˹����ڵ�
				MSValueDescr  nodeVal;
				if (!mdlTreeNode_getAllowsChildren(treeNode))
				{
					mdlTreeNode_getValue(mdlTreeNode_getParent(treeNode), nodeVal);
				}
				else
				{
					mdlTreeNode_getValue(treeNode, nodeVal);
				}
				nodeVal.GetWString(ngName);
			}
			if (!ngName.empty())
			{
				bvector<WString> names;
				int projectId, caseId;
				splitNgName_Model(ngName, UNUSE_WSTRING, projectId, caseId, UNUSE_WSTRING, UNUSE_WSTRING);
				BuildBoundaryFaces(iProjectId,caseId, ngName.data());

			}
		}
		break;
	}

	default:
		break;
	}
}

void ClearBoundary()
{
	//����߽���ʾ����ʱ��Ϣ
	DgnPlatform::LevelId levelId;
	// ȡ��CAE��ͼ��ͼ��
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"BOUNDARY"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}
}

////��ȡ�����ߴ���
//#pragma region    
//bool GetFaceInfos(int projectId, int caseId, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt)
//{
//	Db db;
//	Statement *l_statment;
//	WString strSql;
//	StatusInt rc;
//
//	strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z ");
//	strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z ");
//	strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z ");
//	strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z ");
//	strSql.append(L"\nFROM (SELECT ProjectId,Node1,Node2,Node3,Node4 FROM FACES WHERE ProjectId = @ProjectId GROUP BY NodeIdStr HAVING COUNT(NodeIdStr) < 2) a \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n1 ON a.ProjectId = n1.ProjectId AND a.Node1 = n1.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n2 ON a.ProjectId = n2.ProjectId AND a.Node2 = n2.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n3 ON a.ProjectId = n3.ProjectId AND a.Node3 = n3.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n4 ON a.ProjectId = n4.ProjectId AND a.Node4 = n4.ID \n");
//
//	rc = OpenDB(db);
//	if (rc == BE_SQLITE_OK)
//	{
//		l_statment = new Statement();
//
//		size_t len = strSql.length() + 1;
//		CharP cSQL = (CharP)malloc(len*sizeof(char));
//		strSql.ConvertToLocaleChars(cSQL);
//
//		rc = l_statment->Prepare(db, cSQL);
//
//		l_statment->BindInt(1, projectId);
//		l_statment->BindInt(2, caseID);
//
//		if (rc == BE_SQLITE_OK)
//		{
//			bvector<ContourPoint>	FacePoints;
//
//			while (l_statment->Step() == BE_SQLITE_ROW)
//			{
//				FacePoints.clear();
//				MyPushback(FacePoints, l_statment, basePt);
//
//				vecFaces.push_back(FacePoints);
//			}
//
//			bvector<ContourPoint>().swap(FacePoints);
//		}
//
//		l_statment->Finalize();
//
//		return true;
//	}
//	else
//		return false;
//}
//
//void ShowOuterContour(int projectId, int caseId, int levelId, DPoint3d basePt)
//{
//	bvector<bvector<ContourPoint>> vecFaces;
//
//	// ȡ��CAE������Mesh�漯�Ϻ���ڵ㼯��
//	if (true == GetFaceInfos(projectId, caseId, vecFaces, basePt))
//	{
//		map<std::string, bvector<bvector<ContourPoint>>> faceDatasMap;
//		bvector<std::string> normalstring;
//		if (vecFaces.size() < 1)
//		{
//			return ;
//		}
//		DrawContourOutFaceLine(normalstring, faceDatasMap, vecFaces);
//
//		bvector<DPoint3d>().swap(s_drawtempLine.linePoints);
//		for (size_t i = 0; i < normalstring.size(); i++)
//		{
//			std::string normalKey = normalstring.at(i);
//			DrawoutFaceLine(normalKey, faceDatasMap, levelId, s_drawtempLine.linePoints);
//		}
//		if (s_drawtempLine.linePoints.size() > 1)
//		{
//			s_drawtempLine.StartDrawFunction();
//		}
//		mdlLevel_setDisplay(MASTERFILE, levelId, false);
//	}
//}
//#pragma  endregion
