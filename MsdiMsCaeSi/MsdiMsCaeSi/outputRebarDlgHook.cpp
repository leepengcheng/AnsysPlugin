#include "MsdiMsCaeSipch.h"

#define  espinion 1e-10
#define  iszero(x) ((x) > -espinion && (x) < espinion)

extern ReBarCalDlgInfo g_rebarCalDlgInfo;
extern void GetAllNineDatas(bmap<WString, bvector<double>>& allDatas, int caseID, int ProjecID);

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);
// CAE���������
extern bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
extern bmap<WString, WString> g_RstType_Unit;
extern bvector<WString> vecRstTypeCodes;

// KEY=��ĿID$����ID$����ModelID$Ԫ��ID
const int KEY_IDX_PROJECTID = 0;//��ĿID
const int KEY_IDX_CASEID = 1;//����ID
const int KEY_IDX_SECTIONID = 2;//����ModelID 
const int KEY_IDX_NGNAME = 3;//����Model���� 
const int KEY_IDX_ELEMID = 4;//Ԫ��ID

//  �ֽ��ǿ���б�
double m_rebarStrengthList[7] = { 210.0, 270.0, 300.0, 360.0, 360.0, 420.0, 360.0 };
//  ����������ǿ���б�
double m_concrStrengthList[11] = { 0.64, 0.91, 1.10, 1.27, 1.43, 1.57, 1.71, 1.80, 1.89, 1.96, 2.04 };
//  ��׼�ֽ�ֱ��
double m_diameterList[16] = { 6, 6.5, 8, 10, 12, 14, 16, 18, 20, 22, 25, 28, 32, 36, 40, 50 };

bvector<WString> oCalcResultItemVec;

//������������ڹ��߼���ʱ��XAttribution��ȡ������رպ���д��XAttribution
bmap<WString, ReBarInfo> m_RebarResults;
//������
double m_dAs = 0.0;  

bool TesionCmpDesc(const double& dFirst, const double& dSecond)
{
	return dFirst > dSecond;
}
void InitReBarCalDlgInfo()
{
	swprintf(g_rebarCalDlgInfo.strCurrentKey, L"%s", L"");
	swprintf(g_rebarCalDlgInfo.rebarName, L"%s", L"");
	swprintf(g_rebarCalDlgInfo.strCurrentSel, L"%s", L"");

	g_rebarCalDlgInfo.dStruFt = 1.2;					//�ṹϵ��
	g_rebarCalDlgInfo.dStruSignifyFt = 1.0;				//�ṹ��Ҫϵ��
	g_rebarCalDlgInfo.dDesignFt = 1.0;					//���״��ϵ��
	g_rebarCalDlgInfo.dDefPointSpan = 200;				//Ĭ�ϵ��(mm)
	g_rebarCalDlgInfo.iRebarType = 0;					//�ֽ������±꣬���ڻ�ȡfy
	g_rebarCalDlgInfo.dRebarTesion = 0.0;				//�ֽ��ǿ��
	g_rebarCalDlgInfo.iConcrType = 0;					//�����������±꣬���ڻ�ȡft
	g_rebarCalDlgInfo.dConcrTesion = 0.0;				//���������Ŀ���ǿ��
	g_rebarCalDlgInfo.dSectionHeight = 1;				//����߶�
	g_rebarCalDlgInfo.dDiameter = 6.0;					//�ֽ�ֱ��
	g_rebarCalDlgInfo.iDameterIndex = 0;			
	g_rebarCalDlgInfo.iRebarCount = 1;					//�ֽ����
	g_rebarCalDlgInfo.bCalcStress = false;				//�Ƿ������ء�����������
}

// ��ʼ����������
bool InitFormData()
{
	//���������ʼ��
	InitReBarCalDlgInfo();

	// ����������Ŀ��ʼ��
	bvector<WString>().swap(oCalcResultItemVec);
	//Ӧ�����������
	oCalcResultItemVec.push_back(L"Ӧ����������� N:");
	//0Ӧ����Ӧ���
	oCalcResultItemVec.push_back(L"0Ӧ����Ӧ��� No:");
	//0Ӧ����λ��
	oCalcResultItemVec.push_back(L"0Ӧ����λ��(mm):");
	//����Ӧ�����
	oCalcResultItemVec.push_back(L"����Ӧ�����(mMPa):");
	//������ǿ�ȶ�Ӧ���
	oCalcResultItemVec.push_back(L"������ǿ�ȶ�Ӧ��� Nc:");
	//������ǿ�ȶ�Ӧλ��
	oCalcResultItemVec.push_back(L"������ǿ�ȶ�Ӧλ��(mm):");
	//��������Ӧ�����
	oCalcResultItemVec.push_back(L"��������Ӧ�����(mMPa):");
	//����������
	oCalcResultItemVec.push_back(L"����������(mm2)");
	//�ο����
	oCalcResultItemVec.push_back(L"�ο����:");
	//���
	oCalcResultItemVec.push_back(L"���(N):");
	//����
	oCalcResultItemVec.push_back(L"����(N):");
	//����XY
	oCalcResultItemVec.push_back(L"XY�������(N):");
	//����YZ
	oCalcResultItemVec.push_back(L"YZ�������(N):");
	//����XZ
	oCalcResultItemVec.push_back(L"XZ�������(N):");
	// ��ȡ�������
	readRebarResultsFromX(m_RebarResults);

	return true;
}

// ��ʼ������ListBox������
void InitListBoxData(MSDialogP dbP)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	ListCellP	pCell = NULL;
	int iRowCount;

	// ȡ��Ӧ��ListBox����
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	// ȡ��Ӧ��ListModel����
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		// �����������
		mdlListModel_empty(pListModel, true);
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}

	// ˢ��Ӧ��ͼ��ʾ
	DialogItem *ctPDiP;
	/* Redraw the ContainerPanel */
	ctPDiP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);
	if (ctPDiP)
	{
		mdlDialog_itemDrawEx(dbP, ctPDiP->itemIndex, true);
	}

	// ȡ�ü�����ListBox����
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_CalResult, 0);
	// ȡ�ü�����ListModel����
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);

		// �����ʾ���
		for (int iRow = 0; iRow < iRowCount; iRow++)
		{
			// ��������
			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);
			mdlListCell_setStringValue(pCell, L"", TRUE);
		}
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}

	// ȡ�������ListBox����
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_RebarInfo, 0);
	// ȡ�������ListModel����
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);
		// �����ʾ���
		for (int iRow = 0; iRow < iRowCount; iRow++)
		{
			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 0);
			mdlListCell_setStringValue(pCell, L"", TRUE);

			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
			mdlListCell_setStringValue(pCell, L"", TRUE);

			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);
			mdlListCell_setStringValue(pCell, L"", TRUE);
		}
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}
}

// �����������
void writeRebarResultsToX(bmap<WString, ReBarInfo>& mapRebarResults)
{
	if (mapRebarResults.size() <= 0) return;

	//���е��������ŵ�Defaultģ�͵�XAttribution��
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);

	//�Ƚ�bvectorתΪ�����ٽ���д��
	int cnt = (int)mapRebarResults.size();
	ReBarInfo* arrayResults = new ReBarInfo[cnt];

	int iRowCnt = 0;
	bmap<WString, ReBarInfo>::iterator iter;
	for (iter = mapRebarResults.begin(); iter != mapRebarResults.end(); iter++, iRowCnt++)
	{
		arrayResults[iRowCnt] = iter->second;
	}

	appSet.SaveSetting(handlerId, XAttrID_RebarResults, arrayResults, sizeof(ReBarInfo) * cnt);

	delete[] arrayResults;
	mdlModelRef_freeWorking(model);
}

// ��ȡ�������
void readRebarResultsFromX(bmap<WString, ReBarInfo>& mapRebarResults)
{
	mapRebarResults.clear();

	WString lsKey;
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);
	XAttributeHandle xah(appSet.GetElementRef(), handlerId, XAttrID_RebarResults);

	if (xah.IsValid() && xah.IsValid())
	{
		//�Ƚ���ȡ�����ݴ浽��������תΪbvector
		int size = xah.GetSize();
		int cnt = size / (int)sizeof(ReBarInfo);
		if (cnt != 0)
		{
			ReBarInfo* arrayResults = (ReBarInfo*)xah.PeekData();
			for (int i = 0; i < cnt; i++)
			{
				// ��ĿID&����ID&����ModelID&����Model��&Ԫ��ID
				lsKey.Sprintf(L"%d%s%d%s%d%s%s%s%d", arrayResults[i].projectID, SeparatorAnd, arrayResults[i].caseID, SeparatorAnd, arrayResults[i].SectionID, SeparatorAnd, arrayResults[i].NgName, SeparatorAnd, arrayResults[i].ElemID);
				mapRebarResults[lsKey] = arrayResults[i];
			}
		}
	}

	mdlModelRef_freeWorking(model);
}

// ȡ�������ָ�����λ���ϵ�Ӧ��ֵ
bool getRebarLineStressbyDistances(ReBarInfo& orebarInfo, const bvector<bvector<PointOutData>>& m_FacetVec, CurveVectorPtr curve, double dLineLength, TransformCP tMatrix, RotMatrixCR rotMatrix)
{
	bvector<double> distances; // �����ָ������λ��
	bvector<CurveLocationDetail> locations;// �����ָ�����λ�õ�����

	// ���������ָ������λ������
	int i = 1;
	double dDistance = g_rebarCalDlgInfo.dDefPointSpan;
	while (dDistance < dLineLength)
	{
		// �����Ҫ������λ
		distances.push_back(dDistance * UOR_PER_MM);
		i = i + 1;
		dDistance = g_rebarCalDlgInfo.dDefPointSpan * i;
	}

	// ȡ������ߵ������յ�
	DPoint3d StartPoint, EndPoint;
	curve->GetStartEnd(StartPoint, EndPoint);

	// ������������λ����������Ӧ��Ӧ��ֵ
	bvector<DPoint3d> outPoints;
	outPoints.push_back(StartPoint);

	// ȡ�������ָ�����λ�õ���������
	if (curve->AddSpacedPoints(distances, locations) == true)
	{
		// ���������ָ�����λ����������Ӧ��Ӧ��ֵ
		for (int iCnt = 0; iCnt < locations.size(); iCnt++)
		{
			outPoints.push_back(locations.at(iCnt).point);
		}
	}
	// ����������յ�λ����������Ӧ��Ӧ��ֵ
	outPoints.push_back(EndPoint);

	bvector<bvector<PointOutData>> outPointAndValues;
	bvector<bvector<DPoint3d>> alloutPoints;
	alloutPoints.push_back(outPoints);

	// �������������ֵ
	CalcOutPointValues(outPointAndValues, alloutPoints, m_FacetVec, tMatrix, rotMatrix);

	// ����Ӧ��ֵ
	WString strStress, shearSXY, shearSYZ, shearSXZ;
	WString buffer_sy, buffer_sxy, buffer_syz, buffer_sxz;
	for (int i = 0; i < outPointAndValues.at(0).size(); i++)
	{
		PointOutData oPtDat = outPointAndValues.at(0).at(i);
		// ������ʾ��ʽ
		buffer_sy.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SY]);
		buffer_sxy.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SXY]);
		buffer_syz.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SYZ]);
		buffer_sxz.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SXZ]);
		if (i > 0)
		{
			strStress.append(SeparatorAnd);
			shearSXY.append(SeparatorAnd);
			shearSYZ.append(SeparatorAnd);
			shearSXZ.append(SeparatorAnd);
		}
		strStress.append(buffer_sy);
		shearSXY.append(buffer_sxy);
		shearSYZ.append(buffer_syz);
		shearSXZ.append(buffer_sxz);
	}

	swprintf(orebarInfo.strStress, L"%s", strStress.data());//Ӧ��ֵSY
	swprintf(orebarInfo.shearSXY, L"%s", shearSXY.data());//Ӧ��ֵSXY
	swprintf(orebarInfo.shearSYZ, L"%s", shearSYZ.data());//Ӧ��ֵSYZ
	swprintf(orebarInfo.shearSXZ, L"%s", shearSXZ.data());//Ӧ��ֵSXZ

	return true;
} 

/*-----------------------------------------------------------------
*  ��������: ��ָ����Dialogָ����Item�л���Ӧ��ͼ
*	@param	dbP	IN	ָ����Dialogָ��
*	@param	diP	IN	ָ����DialogItemָ��
*
*  @return	��
-----------------------------------------------------------------*/
void drawStressView(MSDialogP dbP, DialogItemP diP)
{
	ViewFlags       viewflags;
	MSElementDescr *cellDP = NULL;
	bool            nDices;

	DialogItem  *diP_tree = NULL;
	ListModel   *pListModel = NULL;
	int iRowCount = 0;

	diP_tree = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	pListModel = mdlDialog_listBoxGetListModelP(diP_tree->rawItemP);

	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);
	}
	if (iszero(iRowCount)) return;

	ListRowP pRow = mdlListRow_create(pListModel);

	double dStress = 0.0;
	double dMaxDistance = 0.0;
	double dMaxStress = 0.0, dMinStress = 0.0;

	for (int iRow = 0; iRow < iRowCount; iRow++)
	{
		ListCellP cell_Stress = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);

		//Ӧ��ֵ��MPa��
		MSValueDescr cellValue;
		mdlListCell_getValue(cell_Stress, cellValue);
		dStress = _wtof(cellValue.GetWCharCP());
		if (iszero(iRow))
		{
			dMaxStress = dStress;
			dMinStress = dStress;
		}
		else
		{
			if (dMaxStress < dStress)
				dMaxStress = dStress;

			if (dMinStress > dStress)
				dMinStress = dStress;
		}

		if (iRow == iRowCount - 1)
		{
			ListCellP cell_distance = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
			mdlListCell_getValue(cell_distance, cellValue);
			dMaxDistance = _wtof(cellValue.GetWCharCP()) * 0.65;
		}
	}

	// ���Ӧ������ܳ�
	double dStressLen = abs(dMaxStress) + abs(dMinStress);

	bvector<DPoint3d> vecAxis;// ������������
	bvector<DPoint3d> vecVertex;// Ӧ��������
	bvector<DSegment3d> vecScale;// Ӧ����������
	double dDistances = 0.0;
	// ����Ӧ��������е����ݣ����ɻ���Ӧ��ͼ������
	for (int iRow = 0; iRow < iRowCount; iRow++)
	{
		ListCellP cell_distance = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
		ListCellP cell_Stress = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);

		DPoint3d ptAxis;// ������������
		DPoint3d ptVertex;// Ӧ��������
		DSegment3d ptScale;// Ӧ������

		//����(mm)
		MSValueDescr cellValue;
		mdlListCell_getValue(cell_distance, cellValue);
		dDistances = -1 * _wtof(cellValue.GetWCharCP()) * UOR_PER_MM;

		//Ӧ��ֵ��MPa��
		mdlListCell_getValue(cell_Stress, cellValue);
		dStress = (_wtof(cellValue.GetWCharCP()) / dStressLen) * dMaxDistance * UOR_PER_MM;

		// ���������ϸ��������
		ptAxis = DPoint3d::From(0.0, dDistances, 0.0);
		// ��Ӧ���ߵ�����
		ptVertex = DPoint3d::From(dStress, dDistances, 0.0);

		// ���Ӧ�����ߵ���ʼ���յ�
		ptScale.Init(ptAxis, ptVertex);

		// ���浽��������������
		vecAxis.push_back(ptAxis);
		// ���浽Ӧ����������
		vecVertex.push_back(ptVertex);
		// ���浽Ӧ������������
		vecScale.push_back(ptScale);
	}

	//������Ԫ�ط���һ����Ԫ��������ʾ
	EditElementHandle cellHeader;
	DPoint3d origin = { 0, 0, 0 };
	RotMatrix rMatrix;
	mdlRMatrix_getIdentity(&rMatrix);
	NormalCellHeaderHandler::CreateCellElement(cellHeader, L"StressView", origin, rMatrix, true, *MASTERFILE);

	// ���ݾ���������ֵ����
	EditElementHandle ehLineStringAxis;
	if (createLineString(ehLineStringAxis, vecAxis, ACTIVEMODEL) == true)
	{
		NormalCellHeaderHandler::AddChildElement(cellHeader, ehLineStringAxis);
	}

	// ����Ӧ��ֵ����Ӧ����
	EditElementHandle ehLineStringVertex;
	if (createLineString(ehLineStringVertex, vecVertex, ACTIVEMODEL) == true)
	{
		NormalCellHeaderHandler::AddChildElement(cellHeader, ehLineStringVertex);
	}

	// ����Ӧ��ֵ����Ӧ������
	for (int j = 0; j < vecScale.size(); j++)
	{
		// ����Ӧ������
		EditElementHandle ehLine;
		if (createLine(ehLine, vecScale.at(j), ACTIVEMODEL) == true)
		{
			NormalCellHeaderHandler::AddChildElement(cellHeader, ehLine);
		}

		// ����Ӧ�������ϵ�����
		WString tText;
		tText.Sprintf(L"%d", j + 1);
		double fwidth = (g_rebarCalDlgInfo.dDefPointSpan) * 0.35 * UOR_PER_MM;

		// ȡ��Ӧ�����������ֵķ�������
		DPoint3d tPos;
		vecScale.at(j).GetStartPoint(tPos);

		// ��������
		EditElementHandle eehText;
		if (MsdiMsCaeSiUtility::CreateText(eehText, ACTIVEMODEL, tPos, tText, fwidth, fwidth, 0.0, (UInt32)TextElementJustification::RightMiddle) == true)
		{
			NormalCellHeaderHandler::AddChildElement(cellHeader, eehText);
		}
	} 
	
	nDices = mdlModelRef_is3D(MASTERFILE);

	memset(&viewflags, 0, sizeof(viewflags));
	viewflags.patterns = 1;
	viewflags.ed_fields = 1;
	viewflags.on_off = 1;
	viewflags.constructs = 1;
	viewflags.dimens = 1;

	cellHeader.AddToModel();
	Draw3DCell(cellHeader.GetElementDescrP(), dbP, &diP->rect, &viewflags, nDices);
	cellHeader.DeleteFromModel();
}

void Draw3DCell
(
MSElementDescr      *cellP,
MSDialogP           dbP,
BSIRect             *localRectP,
ViewFlags           *viewflagsP,
bool                nDices
)
{
	RotMatrix       rMatrixArray;
	Dpoint3d        newextent;
	DRange3d        dRangeVec;

	UInt32          colorMap[MAX_CMAPENTRIES];
	byte            colorTable[MAX_CTBLBYTES];

	mdlColor_getColorTableByModelRef(colorTable, MASTERFILE);
	mdlColor_matchLongColorMap(colorMap, colorTable, 0, false);
	// ������Ϊ��ɫ
	colorMap [255] = 0xffffff;	

	//set up the views
	mdlView_getStandard(&rMatrixArray, StandardView::Top);

	mdlCnv_scanRangeToDRange(&dRangeVec, &cellP->el.hdr.dhdr.range);

	mdlRMatrix_multiplyRange(&dRangeVec.low, &dRangeVec.high, &rMatrixArray);
	mdlVec_subtractPoint(&newextent, &dRangeVec.high, &dRangeVec.low);

	mdlRMatrix_unrotatePoint(&dRangeVec.low, &rMatrixArray);

	if (newextent.z < fc_1) newextent.z = fc_1000;
	if (newextent.x < fc_1) newextent.x = fc_1000;
	if (newextent.y < fc_1) newextent.y = fc_1000;

	mdlWindow_clipRectSet((MSWindow *)dbP, localRectP);

	mdlElmdscr_extendedDisplayToWindow((MSWindow *)dbP, localRectP, viewflagsP, cellP, &rMatrixArray,
		&dRangeVec.low, &newextent, 0, -1, colorMap, true, NULL);

	mdlDialog_rectDrawEdge(dbP, localRectP, true);
	mdlWindow_clipRectSet((MSWindow *)dbP, NULL);
}

bool createLine(EditElementHandleR eeh, DSegment3dCR segment, DgnModelRefP modelRef)
{
	if (SUCCESS == LineHandler::CreateLineElement(eeh, NULL, segment, modelRef->Is3d(), *modelRef))
		return true;

	return false;
}

bool createLineString(EditElementHandleR eeh, bvector<DPoint3d> &points, DgnModelRefP modelRef)
{
	if (SUCCESS == LineStringHandler::CreateLineStringElement(eeh, NULL, &points[0], points.size(), modelRef->Is3d(), *modelRef))
		return true;

	return false;
}

void HookGeneric_CalRebar_Canva(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_DRAW:
	{
		BSIRect   clipRect;
		BSIColorDescr   *bgColorP;

		clipRect = dimP->dialogItemP->rect;
		mdlRect_inset(&clipRect, -2, -2);
		mdlDialog_rectDrawBeveled(dimP->db, &clipRect, FALSE, TRUE);

		bgColorP = mdlColorPal_getColorDescr(NULL, 0);
		mdlDialog_rectFillCD(dimP->db, &dimP->dialogItemP->rect, &dimP->dialogItemP->rect, bgColorP, NULL);

		drawStressView(dimP->db, dimP->dialogItemP);

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void     mdlDialog_rectInset
(
BSIRect        *rP,
int            deltaX,        /* positive moves in */
int            deltaY
)
{
	int width, height;

	width = rP->Width();
	height = rP->Height();
	if (width - 2 * deltaX <= 0 ||
		height - 2 * deltaY <= 0)
	{
		rP->origin.x = 0;
		rP->origin.y = 0;
		rP->corner.x = 0;
		rP->corner.y = 0;

		return;
	}

	rP->origin.x += deltaX;
	rP->corner.x -= deltaX;
	rP->origin.y += deltaY;
	rP->corner.y -= deltaY;
}

void HookDialog_CalRebar_InterfaceDynamic(DialogMessage *dmP)
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

		// ��ʼ����������
		InitFormData();

		break;
	}
	case DIALOG_MESSAGE_UPDATE:
	{
		DialogItem    *diP;

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_CalRebar_V, 0);
		if (!diP)
			break;

		CalRebar_adjustVSashDialogItems(dmP->db, NULL, TRUE);
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

		CalRebar_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_PREBUTTON:
	{
		if ((dmP->u.button.buttonTrans == BUTTONTRANS_DOWN) &&
			(dmP->u.button.buttonNumber == RESET))//�Ҽ��˵�
		{
			CalRebar_setupPopupMenu(dmP);
		}
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		break;
	}
	case DIALOG_MESSAGE_DESTROY:
	{
		writeRebarResultsToX(m_RebarResults);
		break;
	}
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void CalRebar_setupPopupMenu(DialogMessageP dmP)
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

	popupDiP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_PopupMenu, POPUPMENUID_CalRebar_PopuMenu, 0);
	if (!popupDiP)
		return;
	popupP = popupDiP->rawItemP;

	/* Popup Menus are a restricted form of MenuBar (they only have 1 top
	level menu). mdlDialog_menuBarxxx functions can be called with ptrs
	to popup menus. */
	popupMenuP = mdlDialog_menuBarGetMenu(popupP, 0);
	if (!popupMenuP)
		return;

	downDiP = mdlDialog_itemGetByIndex(dbP, iItem);
	switch (downDiP->type)
	{
	case RTYPE_Tree:

		int rowIndex, colIndex;
		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, downDiP->rawItemP))
		{
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(downDiP->rawItemP);
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//�жϽڵ������
			int nodeLevel = mdlTreeNode_getLevel(treeNode);

			// ����ڵ��Լ�����߽ڵ�ʱ����ʾ�Ҽ��˵�
			// �����ڵ㣬����ʾ�Ҽ��˵�;
			if (nodeLevel < 2)
				return;

			DItem_PulldownMenuItem  targetItem;
			mdlDialog_menuBarFindItem(&targetItem, &popupMenuP, popupP, NULL, PDMID_CalRebar_PopuMenu, MSID_DelRebarLine);
			if (nodeLevel == 2)
			{
				// ��ѡ�����ڵ�ʱ����ʾ�Ҽ��˵��е�ɾ������߲���ʹ��
				mdlDialog_textPDMItemSetEnabled(&targetItem, FALSE);
			}
			else
			{
				// ��ѡ������߽ڵ�ʱ����ʾ�Ҽ��˵��е�ɾ������߿�ʹ��
				mdlDialog_textPDMItemSetEnabled(&targetItem, TRUE);
			}

			menuId = PDMID_CalRebar_PopuMenu;
		}
		else
		{
			return;
		}

		break;

	default:
		return;
	}

	if (popupMenuP->id != menuId)
	{
		mdlDialog_menuBarDeleteMenu(popupMenuP, popupP, NULL, 0);
		mdlDialog_menuBarInsMenu(popupP, RTYPE_PulldownMenu, menuId, NULL);
	}

	/* force the button event to go to the popupMenu item */
	dmP->u.button.buttonRiP = popupP;
}

void HookPushBtn_CalRebar_Calculate(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//���������ѡ����������bmap<WString, ReBarInfo> m_RebarResults�в�ѯkey,���������Ϣд��key��Ӧ��value
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// ѡ��ڵ�Ϊ������ʱ������������
				int nodeType = mdlTreeNode_getLevel(pTreeNode);
				if (3 == nodeType)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						//����������Ϊ���޸ĸ�����߶�Ӧ��ReBarInfo�ṹ��
						if (calculateRebarArea(it_find->second.dLineLength, it_find->second) == true)
						{
							//�ؼ�ֵ���浽���������
							GetDataFromControls(it_find->second);

							//���д�������listbox
							SetResultValToListBox(dimP->db, it_find->second);

							//���д��ֽ�listbox
							SetRebarInfoToListBox(dimP->db, it_find->second);
						}
					}
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

//ˢ��
void HookPushBtn_CalRebar_Refresh(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		bvector<WString> vecData;
		BeStringUtilities::Split(g_rebarCalDlgInfo.strCurrentKey, SeparatorAnd, vecData);

		// ��������⣬������
		if (vecData.size() < 3) return;
		rebarInfo& orebarInfo = m_RebarResults[g_rebarCalDlgInfo.strCurrentKey];

#pragma region
		bmap<WString, bvector<double>> allNineDatas;
		// ȡ�����нڵ��UX,UY,UZ,SX,SY,SZ,SXY,SXZ,SYZ
		GetAllNineDatas(allNineDatas, wtoi(vecData.at(KEY_IDX_CASEID).data()), wtoi(vecData.at(KEY_IDX_PROJECTID).data()));

		// ��ȡ�����MeshԪ�����нڵ��CAE����
		bvector<bvector<PointOutData>> vecSectionMeshDats;
		GetSectionMeshDatas(vecSectionMeshDats, allNineDatas);

		EditElementHandle lineHandle(orebarInfo.ElemID, ACTIVEMODEL);
		DPoint3d linePts[2];
		int num = 0;
		mdlLinear_extract(linePts, &num, lineHandle.GetElementCP(), ACTIVEMODEL);
		
		DVec3d DvecX_Cur = DVec3d::FromStartEndNormalize(linePts[0], linePts[1]);
		DVec3d dpt = DVec3d::From(1.0, 0.0, 0.0);
		RotMatrix inMatrix;
		mdlRMatrix_fromVectorToVector(&inMatrix, &dpt, &DvecX_Cur);
		mdlRMatrix_transpose(&inMatrix, &inMatrix);

		// ��ȡ�����ת������(��ά����->��άƽ���ת������)
		TransformP tMatrix = NULL;
		MsdiMsCaeSiUtility::GetSectionTransform(&tMatrix, vecData.at(KEY_IDX_NGNAME).data());

		//Ԫ��ID
		ElementHandle eh(_wtoi64(vecData.at(KEY_IDX_ELEMID).data()), ACTIVEMODEL);

		// ȡ�������
		CurveVectorPtr  curve = ICurvePathQuery::ElementToCurveVector(eh);

		// ȡ������߳���
		double dLineLength = curve->Length() / UOR_PER_MM;

		// ȡ�������ָ�����λ���ϵ�Ӧ��ֵ
		getRebarLineStressbyDistances(orebarInfo, vecSectionMeshDats, curve, dLineLength, tMatrix, inMatrix);
#pragma endregion

		GetDataFromControls(orebarInfo);

		// ��Ӧ��ֵ��ʾ��ListBox��
		SetStressValToListBox(dimP->db, orebarInfo);

		// ˢ��Ӧ��ͼ��ʾ
		DialogItem *ctPDiP;
		/* Redraw the ContainerPanel */
		ctPDiP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);
		if (ctPDiP)
			mdlDialog_itemDrawEx(dimP->db, ctPDiP->itemIndex, true);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_CalRebar_OutPut(DialogItemMessage* dimP)
{

}

void CalRebar_adjustVSashDialogItems(MSDialogP   db, BSIRect  *pOldContent, bool refreshItems)
{
	DialogItem    *sashDiP;
	DialogItem    *treeDiP;
	DialogItem    *ctPanelDiP;

	/* Get the 3 main items on the dialog */
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_CalRebar_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookSash_CalRebar_vSashHook(DialogItemMessage* dimP)
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
				CalRebar_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}
		break;
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_CalRebar_SectionTree(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	WString lsKey;

	//WString myString;
	//myString.Sprintf(L"messageType: %d", dimP->messageType);
	//mdlDialog_dmsgsPrint(myString.GetWCharCP());

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		// ��ʼ������ʾ(������Ϣ)
		InitSectionTreeDisp(dimP->dialogItemP);

		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		GuiTreeModel*  pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
		int rowIndex = 0,
			colIndex = 0;
		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
		{
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//txt��ʾѡ����Ϣ
			wcscpy(g_rebarCalDlgInfo.strCurrentSel, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());
		}

		mdlDialog_itemsSynch(dimP->db);
		break;
	}
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.clicked)
		{
			int rowIndex, colIndex;
			if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
			{
				GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
				treeNode = mdlTreeCell_getNode(treeCell);

				//�жϵ���Ľڵ������
				MSValueDescr uvalue;
				mdlTreeNode_getValue(treeNode, uvalue);
				
				//KEY=��ĿID$����ID$����ModelID$��������
				uvalue.GetWString(lsKey);
				bvector<WString> vecData;
				BeStringUtilities::Split(lsKey.data(), SeparatorAnd, vecData);

				//����߽ڵ�
				int iNodeLevel = mdlTreeNode_getLevel(treeNode);
				if (2 == iNodeLevel)
				{
					//��ʼ��������ͼ�Ĳ���
					InitDrawColorShape(dimP, treeNode, vecData);
				}
				if (3 == iNodeLevel)
				{
				 	const rebarInfo& orebarInfo = m_RebarResults[lsKey];
					wcscpy(g_rebarCalDlgInfo.strCurrentKey, lsKey.data()); //��ǰ��ʾ�ڵ�Key

					g_rebarCalDlgInfo.dStruSignifyFt = orebarInfo.dStruSignifyFt;	//�ṹ��Ҫϵ��
					g_rebarCalDlgInfo.dDesignFt		= orebarInfo.dDesignFt;			//���״��ϵ��
					g_rebarCalDlgInfo.dStruFt		= orebarInfo.dStruFt;			//�ṹϵ��
					g_rebarCalDlgInfo.dDefPointSpan = orebarInfo.dDefPointSpan;		//Ĭ�ϵ��(mm)
					g_rebarCalDlgInfo.iRebarType	= orebarInfo.iRebarType;		//�ֽ������±꣬���ڻ�ȡfy
					g_rebarCalDlgInfo.dRebarTesion	= orebarInfo.dRebarTesion;		//�ֽ��ǿ��
					g_rebarCalDlgInfo.iConcrType	= orebarInfo.iConcrType;		//�����������±꣬���ڻ�ȡft
					g_rebarCalDlgInfo.dConcrTesion	= orebarInfo.dConcrTesion;		//���������Ŀ���ǿ��
					g_rebarCalDlgInfo.dSectionHeight= orebarInfo.dSectionHeight;	//����߶�
					g_rebarCalDlgInfo.bCalcStress	= orebarInfo.bCalcStress;		//�Ƿ���������������

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						
						// ��Ӧ��ֵ��ʾ��ListBox��
						SetStressValToListBox(dimP->db, orebarInfo);

						//���д�������listbox
						SetResultValToListBox(dimP->db, orebarInfo);

						//���д��ֽ�listbox
						SetRebarInfoToListBox(dimP->db, orebarInfo);

						// ˢ��Ӧ��ͼ��ʾ
						DialogItem *ctPDiP;
						/* Redraw the ContainerPanel */
						ctPDiP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);

						if (ctPDiP)
							mdlDialog_itemDrawEx(dimP->db, ctPDiP->itemIndex, true);

						// ����߸�����ʾ
						EditElementHandle eh(orebarInfo.ElemID, target);
						ElementAgenda eAgenda;
						SelectionSetManagerR selectSetM = SelectionSetManager::GetManager();
						selectSetM.EmptyAll();
						selectSetM.AddElement(eh.GetElementRef(), target);
						selectSetM.BuildAgenda(eAgenda);
						eAgenda.Hilite();

						mdlModelRef_freeWorking(target);
						MsdiMsCaeSiUtility::fitView(0);						
					}
				}
				else
				{
					//˫����λ����
					if (2 == iNodeLevel && 2 == dimP->u.button.upNumber)
					{
						DgnModelRefP target = NULL;
						if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
						{
							if (!mdlModelRef_areSame(MASTERFILE, target))
							{
								s_pDCShape->CloseDrawFunction();
								s_drawtempLine.CloseDrawFunction();
								ClearclipfaceShow();
								mdlModelRef_activateAndDisplay(target);
							}
							mdlModelRef_freeWorking(target);
							MsdiMsCaeSiUtility::fitView(0);
						}
					}
					// ���������ڵ�ʱ����ʼ������ListBox������
					InitListBoxData(dimP->db);
				}

				mdlDialog_itemsSynch(dimP->db);
			}
		}
		break;
	}
	case DIALOG_MESSAGE_DESTROY:
	{
		GuiTreeModel   *pTreeModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
		mdlTreeModel_destroy(pTreeModel, TRUE);

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// ��ʼ������ʾ(������Ϣ)
void InitSectionTreeDisp(DialogItemP dimp)
{
	WString			instanceValue;
	WString			caseValue;
	WString			sectionValue;
	WString			RebarValue;
	GuiTreeNode     *pRoot;
	MSValueDescr	nodeVal;
	GuiTreeModel*	pModel = mdlTreeModel_create(1);

	pRoot = mdlTreeModel_getRootNode(pModel);
	mdlTreeNode_setDisplayText(pRoot, L"ROOT");

	//��ȡXAttribution�õ�ȫ��ʵ��������
	bvector<InstanceInfo> allInstance = readInstanceInfo();

#pragma region ��Ŀ�ڵ������
	//��������ʵ����Ϊ�����ӽڵ�
	for (int iPro = 0; iPro < (int)allInstance.size(); iPro++)
	{
		WString ngName = allInstance[iPro].NgName;

		GuiTreeNode* instanceNode = mdlTreeNode_create(pModel, TRUE);
		mdlTreeNode_addChild(pRoot, instanceNode);

		//��ʾ���ݡ���ʵ����
		WString instanceName;
		int unuse1 = 0;
		splitNgName_Model(ngName, UNUSE_WSTRING, unuse1, UNUSE_INT, instanceName, UNUSE_WSTRING);

		mdlTreeNode_setDisplayText(instanceNode, instanceName.data());

		mdlTreeNode_setCollapsedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
		mdlTreeNode_setExpandedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
		mdlTreeNode_setExpandedSelectedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

		//�������ݡ���"ModelID&��NgName"
		instanceValue.Sprintf(L"%d%s%s", allInstance[iPro].modelId, SeparatorAnd, ngName);
		nodeVal.SetWChar(instanceValue.data());

		mdlTreeNode_setValue(instanceNode, nodeVal, FALSE);

#pragma region �����ڵ������
		//��������model���ҵ����ڸ�ʵ���Ľ���model
		ngName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

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

			//�������ݡ���(��ĿID$����ID)
			caseValue.Sprintf(L"%d%s%d", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase));
			nodeVal.SetWChar(caseValue.data());
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
					GuiTreeNode* clipNode = mdlTreeNode_create(pModel, TRUE);

					//��ʾ���ݡ���������
					WString clipName;
					splitNgName_Clip(wsModelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, clipName, UNUSE_WSTRING, UNUSE_WSTRING);
					mdlTreeNode_setDisplayText(clipNode, clipName.data());

					mdlTreeNode_setCollapsedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
					mdlTreeNode_setExpandedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
					mdlTreeNode_setExpandedSelectedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

					//�������ݡ���(��ĿID&����ID&����ModelID)
					sectionValue.Sprintf(L"%d%s%d%s%d%s%s", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase), SeparatorAnd, IndexItem->GetModelId(), SeparatorAnd, IndexItem->GetName());
					nodeVal.SetWChar(sectionValue.data());
					mdlTreeNode_setValue(clipNode, nodeVal, FALSE);

					mdlTreeNode_addChild(pCaseNode, clipNode);

#pragma region ����߽ڵ������
					bmap<WString, ReBarInfo>::iterator iter;
					for (iter = m_RebarResults.begin(); iter != m_RebarResults.end(); iter++)
					{
						/*arrayResults[iCnt] = iter->second;*/
						if (iter->first.StartsWith(sectionValue.data()))
						{
							GuiTreeNode* rebarline = mdlTreeNode_create(pModel, FALSE);

							mdlTreeNode_setDisplayText(rebarline, iter->second.rebarName);
							mdlTreeNode_setLeafIcon(rebarline, ICONID_RebarLine, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

							//�������ݡ���(��ĿID&����ID&����ModelID&����Model��&Ԫ��ID)
							RebarValue.Sprintf(L"%d%s%d%s%d%s%s%s%d", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase), SeparatorAnd, IndexItem->GetModelId(), SeparatorAnd, IndexItem->GetName(), SeparatorAnd, iter->second.ElemID);
							nodeVal.SetWChar(RebarValue.data());
							mdlTreeNode_setValue(rebarline, nodeVal, FALSE);

							mdlTreeNode_addChild(clipNode, rebarline);
						}
					}
#pragma endregion ����߽ڵ������
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

	//չ���ڵ�
	GuiTreeNode* treeNode = pRoot;
	while (mdlTreeNode_isParentPopulated(treeNode) && mdlTreeNode_getLevel(treeNode) < 3)
	{
		mdlTreeNode_expand(treeNode);
		treeNode = mdlTreeNode_getFirstChild(treeNode);
	}
	mdlDialog_treeSetTreeModelP(dimp->rawItemP, pModel);
}

void SetStressValToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	WChar buffer[256];

	// ȡ��Ӧ��ֵ
	bvector<WString> pointStress;
	BeStringUtilities::Split(info.strStress, SeparatorAnd, pointStress);

	// ȡ��Ӧ��ListBox����
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	// ȡ��Ӧ��ListModel����
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);

	if (pListModel != NULL)
	{
		// ��������ʱ��������������
		mdlListModel_empty(pListModel, true);
	}
	// �����µ�ListModel
	pListModel = mdlListModel_create(3);

	// ��Ӧ�������е�����д�뵽ListBox��
	for (int i = 0; i < pointStress.size(); i++)
	{
		// �����µ�ListRow
		ListRowP	pRow = mdlListRow_create(pListModel);
		ListCellP	cell = NULL;// ��Ԫ�����
		WString cellWs;
		MSValueDescr cellValue;

		//No
		swprintf(buffer, L"%d", i + 1);
		cell = mdlListRow_getCellAtIndex(pRow, 0);
		cellValue.SetWChar(buffer);
		mdlListCell_setValue(cell, cellValue, TRUE);

		//����(mm)
		if (i == pointStress.size() - 1)
		{
			// ���һ������ʱ������ֵΪ����ߵĳ���
			swprintf(buffer, L"%f", info.dLineLength);
		}
		else
		{
			// ����ֵ = ���ֵ * i 
			swprintf(buffer, L"%d", g_rebarCalDlgInfo.dDefPointSpan*i);
		}
		cell = mdlListRow_getCellAtIndex(pRow, 1);
		cellValue.SetWChar(buffer);
		mdlListCell_setValue(cell, cellValue, TRUE);

		//Ӧ��ֵ(MPa)
		cell = mdlListRow_getCellAtIndex(pRow, 2);
		cellValue.SetWChar(pointStress.at(i).data());
		mdlListCell_setValue(cell, cellValue, TRUE);

		// �����������ӵ�ListBox��
		mdlListModel_addRow(pListModel, pRow);
	}

	mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
}

void HookComboBox_CalRebar_RebarType(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = true;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_rebarCalDlgInfo.dRebarTesion = m_rebarStrengthList[0];
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		int iIndex = g_rebarCalDlgInfo.iRebarType;
		//�Զ������combbox�����һ��
		int iSize = sizeof(m_rebarStrengthList) / sizeof(m_rebarStrengthList[0]);
		if (iSize == iIndex){}
		else
		{
			g_rebarCalDlgInfo.dRebarTesion = m_rebarStrengthList[iIndex];	//�����ֽ��ǿ��
		}
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = false;
		break;
	}
}

void HookComboBox_CalRebar_ConcrType(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = true;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_rebarCalDlgInfo.dConcrTesion = m_concrStrengthList[0];
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		int iIndex = g_rebarCalDlgInfo.iConcrType;
		int iSize = sizeof(m_concrStrengthList) / sizeof(m_concrStrengthList[0]);
		if (iSize == iIndex){}
		else
		{
			g_rebarCalDlgInfo.dConcrTesion = m_concrStrengthList[iIndex];		//��������������ǿ��
		}
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = false;
		break;
	}
}

void HookPdm_CalRebar_AddRebarLine(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		int lastAction = -1;
		if (SUCCESS == mdlDialog_openModal(&lastAction, NULL, DIALOGID_AddRebarLine))
		{
			if (lastAction == ACTIONBUTTON_OK)
			{
				DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
				GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
				int	numSelect = -1;
				Point2d* selectP = NULL;
				if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, tTree->rawItemP) && numSelect != 0)
				{
					GuiTreeCell*	treeCell = NULL;
					GuiTreeNode*	treeNode = NULL;

					treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
					treeNode = mdlTreeCell_getNode(treeCell);

					//�жϽڵ������
					int nodeLevel = mdlTreeNode_getLevel(treeNode);

					// ����ڵ��Լ�����߽ڵ����⣬�˳�����;
					if (nodeLevel < 2)
						return;

					//ȡ�ù���ID
					MSValueDescr uvalue;
					mdlTreeNode_getValue(treeNode, uvalue);
					WString lsData;
					uvalue.GetWString(lsData);

					bvector<WString> vecData;
					BeStringUtilities::Split(lsData.data(), SeparatorAnd, vecData);

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						mdlModelRef_freeWorking(target);
					}

					// ��������߻��ƹ���
					ReinforceTool::InstallNewInstance(COMMAND_CALCREBAR, PROMPT_CalcRebar_ClickFirst, dimP->db, wtoi(vecData.at(KEY_IDX_PROJECTID).data()), wtoi(vecData.at(KEY_IDX_CASEID).data()), vecData.at(KEY_IDX_NGNAME));
				}
			}
		}
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// ɾ�������
void HookPdm_CalRebar_DelRebarLine(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		int rowIndex, colIndex;
		DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);

		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, tTree->rawItemP))
		{
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//ȡ������ߵ�KEY
			MSValueDescr uvalue;
			mdlTreeNode_getValue(treeNode, uvalue);
			WString lsKey;
			uvalue.GetWString(lsKey);

			const ReBarInfo& oRebarInfo = m_RebarResults[lsKey];
			// ɾ�������Ԫ��  
			EditElementHandle eh(oRebarInfo.ElemID, ACTIVEMODEL);
			eh.DeleteFromModel();
			NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
			for each (NamedGroupPtr namedGroup in *ngcCAESolid)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				// �����������û��Ԫ��,�ʹ�Dgn��ɾ����
				if (graphicMembers == 0)
				{
					namedGroup->DeleteFromFile();
				}
			}
			// ������������У�ɾ����ǰ�����������   
			m_RebarResults.erase(lsKey);

			if (SUCCESS == mdlTreeNode_destroy(treeNode))
			{
				mdlDialog_treeModelUpdated(tTree->rawItemP, true);
			}

			mdlDialog_itemsSynch(dimP->db);
		}
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookListBox_CalRebar_CalcResult(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;
	WChar buffer[256];

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(3);

		for (int i = 0; i < (int)oCalcResultItemVec.size(); i++)
		{
			ListRowP	pRow = mdlListRow_create(pListModel);

			ListCellP	cell = NULL;
			WString cellWs;
			MSValueDescr cellValue;

			//No
			swprintf(buffer, L"%d", i + 1);
			cell = mdlListRow_getCellAtIndex(pRow, 0);
			cellValue.SetWChar(buffer);
			mdlListCell_setValue(cell, cellValue, TRUE);

			//������
			cell = mdlListRow_getCellAtIndex(pRow, 1);
			cellValue.SetWChar(oCalcResultItemVec.at(i).data());
			mdlListCell_setValue(cell, cellValue, TRUE);

			//���
			cell = mdlListRow_getCellAtIndex(pRow, 2);
			cellValue.SetWChar(L"");
			mdlListCell_setValue(cell, cellValue, TRUE);

			mdlListModel_addRow(pListModel, pRow);
		}

		mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

//@return: 0/1����� 0�� ������ -1,Ft�������Ӧ�� -2����� -3
int checkTesionValid(const bvector<double>& tesionList, double dFt)
{
	int valid = -1;
	bool bSuperior = false;
	int count = (int)tesionList.size();
	for (int i = 0; i < count; ++i)
	{ 
		//Ft����С��һ��Ӧ��
		if (tesionList[i] > dFt)  
		{
			bSuperior = true;
			valid = 0;
		}
		else if (iszero(tesionList[i]))
		{
			//�жϼ��
			if (i > 0 && i < count-1)
			{
				if (tesionList[i - 1] > 0 && tesionList[i + 1] > 0)
				{
					valid = -3;
					break;
				}
			}
		}
	}
	if (bSuperior)
	{
		//0�����
		int nZero = 0;
		CountOfZeroPoint(tesionList, nZero);
		if (nZero > 1)
		{
			valid = -1;
		}
	}
	else
	{
		valid = -2;
	}
	return valid;
}
//0�����
void CountOfZeroPoint(const bvector<double>& tesionList, int& nZero)
{
	if (tesionList.empty()) return;

	int count = (int)tesionList.size();
	int iStart = 0;
	double dBegin = 0.0;

	//ȡ��0��Ӧ�����λ��
	for (int i = 0; i < count; ++i)
	{
		if (iszero(tesionList[i]))
		{
			++nZero;
		}
		else
		{
			iStart = i;
			dBegin = tesionList[i];
			break;
		}
	}
	//0�����
	if (dBegin > 0)
	{
		bool bOdd = true;  //0������Ƿ�Ϊ����,���������б���Ӧ��������0/�������ߴ�0/��������
		for (int i = iStart; i < count; ++i)
		{
			//������ 0/��
			if (bOdd)
			{
				if (tesionList[i] < 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = false;
				}
			}
			//��0/�� ����
			else
			{
				if (tesionList[i] > 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = true;
				}
			}
		}
	}
	else
	{
		bool bOdd = true;
		for (int i = iStart; i < count; ++i)
		{
			//��0/�� ����
			if (bOdd)
			{
				if (tesionList[i] > 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = false;
				}
			}
			//������0/��
			else
			{
				if (tesionList[i] < 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = true;
				}
			}
		}
	}
}

/* ���Ӧ���ж��С�ڻ���������ǿ��ʱ��ֻȡ��ӽ�����Ǹ����м���*/
//��ȡ�����������Ӧ����βλ��. 
void GetValidTesionHeadAndTail(const bvector<double>& tesionList, double Ft, int& iStart, int& iEnd)
{
	if (tesionList.empty()) return;
	iStart = 0;
	iEnd = 0;

	/*����ȡ��С�Ǹ�Ӧ��λ��*/
	int count = (int)tesionList.size();
	double dMin = 0.0;
	int iMin = 0;
	//�Ӻ���ǰȡ��һ���Ǹ�Ӧ��
	for (int i = count - 1; i >= 0; --i)
	{
		if (iszero(tesionList[i]) || tesionList[i] > 0)
		{
			iMin = i;
			dMin = tesionList[i];
			break;
		}
	}

	iEnd = iMin;
	iStart = 0;
	//�Ӻ���ǰ��������Ft��λ��
	if (tesionList[iMin] < Ft)
	{
		for (int i = iMin; i > 0; --i)
		{
			if (tesionList[i] > Ft)
			{
				if (tesionList[i] > tesionList[i - 1])
				{
					iStart = i;
					break;
				}
			}
		}
	}
	//С��Ftλ��
	else
	{
		for (int i = iEnd; i >= 0; --i)
		{
			if (tesionList[i] < 0)
			{
				if (i == iEnd)
				{
					iStart = iEnd;
				}
				else
				{
					iStart = i + 1;
				}
				break;
			}
			else if (tesionList[i] < Ft)
			{
				if (tesionList[i] < tesionList[i - 1] || tesionList[i - 1] < 0)
				{
					iStart = i;
					break;
				}
			}
		}
	}
}

void GetConcretePositions(const bvector<double>& tesionList, double iTail, double span, double Ft, double dLineLength, double dEndSpan, int& iConcr_No, double& dConcrP)
{
	iConcr_No = 1;
	int count = (int)tesionList.size();
	if (tesionList[iTail] < Ft)
	{
		for (int i = iTail; i >= 0; --i)
		{
			if (tesionList[i] > Ft)
			{
				iConcr_No = i + 1;
				break;
			}
		}
	}
	else
	{
		for (int i = iTail; i >= 0; --i)
		{
			if (tesionList[i] < Ft)
			{
				iConcr_No = i + 1;
				break;
			}
		}
	}

	if ((count - 2) == iConcr_No || 3 == count)
	{
		dConcrP = dLineLength;
	}
	else if ((count - 3) == iConcr_No)
	{
		double scale = 0.0;
		double dM = tesionList[iConcr_No - 1];
		if (!iszero(dM))
		{
			scale = fabs(Ft - tesionList[iConcr_No - 1]) / dM;
		}
		dConcrP = span*iConcr_No - span + dEndSpan*scale;
	}
	else
	{
		double scale = 0.0;
		double dM = fabs(tesionList[iConcr_No] - tesionList[iConcr_No - 1]);
		if (!iszero(dM))
		{
			scale = fabs(Ft - tesionList[iConcr_No - 1]) / dM;
		}
		dConcrP = span*iConcr_No - span + span*scale;
	}
}

double GetTrapezoidArea(double dTop, double bBottom, double dHeigh)
{
	return (dTop + bBottom)*dHeigh*0.5;
}
//����һ�ε��Ϊspan������Ӧ�����A�ͻ�������Ӧ�����Act
void GetAAndAct(double dPrev,double dNext,double Ft,double span,double& dA,double& dAct)
{
	//��dPrevΪA��dNextΪB
	//A >= B
	if (dPrev >= dNext)
	{
		//A-B-Ft
		if (Ft <= dPrev && Ft <= dNext)
		{
			dA += GetTrapezoidArea(Ft, dPrev, span);
		}
		//A-Ft-B
		else if (Ft <= dPrev)
		{
			double scale = 0.0;
			double dM = fabs(dPrev - dNext);
			if (!iszero(dM))
			{
				scale = fabs(Ft - dNext) / dM;
			}
			dAct += GetTrapezoidArea(dNext, Ft, span*scale);
			dA += GetTrapezoidArea(dPrev, dNext, span);
		}
		//Ft-A-B
		else
		{
			double area = GetTrapezoidArea(dPrev, dNext, span);
			dAct += area;
			dA += area;
		}
	}
	//A < B
	else
	{
		//Ft-A-B
		if (Ft <= dPrev && Ft <= dNext)
		{
			dA += GetTrapezoidArea(Ft, dPrev, span);
		}
		//A-Ft-B
		else if (Ft <= dPrev)
		{
			double scale = 0.0;
			double dM = fabs(dNext - dPrev);
			if (!iszero(dM))
			{
				scale = fabs(Ft - dPrev) / dM;
			}
			dAct += GetTrapezoidArea(dPrev, Ft, span*scale);
			dA += GetTrapezoidArea(dPrev, dNext, span);
		}
		//A-B-Ft
		else
		{
			double area = GetTrapezoidArea(dPrev, dNext, span);
			dAct += area;
			dA += area;
		}
	}
}

//@brief ��ȡ(x1��x2��һ�ε����),���M(M=FL��F=Sz*A)�����F , xΪ��࣬yΪӦ��,dDΪ��һ��Ӧ�������(��������е�Ϊ0��)�ľ���
double GetAPeriodBendingStress(double x1, double y1, double x2, double y2,double dD)
{
	double dM = (x2 - x1);
	double k = 0.0;
	if (iszero(dM)) 
		return 0.0;
	else
		k = (y2 - y1) / dM;

	//  ���ֺ󷽳�f(x) = -1/3kx^3+0.5dDkx^2+0.5kx1x^2-0.5y1x^2+dDy1x-dDkx1x
	//  ���		= f(x2)- f(x1)
	double fx2 = -1.0 / 3.0*k*x2*x2*x2 + 0.5*dD*k*x2*x2 + 0.5*k*x1*x2*x2 - 0.5*y1*x2*x2 + dD*y1*x2 - dD*k*x1*x2;
	double fx1 = -1.0 / 3.0*k*x1*x1*x1 + 0.5*dD*k*x1*x1 + 0.5*k*x1*x1*x1 - 0.5*y1*x1*x1 + dD*y1*x1 - dD*k*x1*x1;
	return fx2 - fx1;
}

//���������,��������е�Ϊ�����м���
double GetBendingStress(const bvector<double>& tesionList,double span,double dLineLength)
{ 
	double	dStress = 0.0,
			dD = dLineLength * 0.5;
	int count = (int)tesionList.size();

	if (2 == count)
	{	
		double	y1 = tesionList[0],
				y2 = tesionList[1];

		double y0 = (y2 - y1) * 0.5;
		dStress += GetAPeriodBendingStress(-dD, y1, 0.0, y0, dD);
		dStress += GetAPeriodBendingStress(0.0, y0, dD, y2, dD);
	}
	else
	{
		//����������ܺ�
		int nLeft = count * 0.5 - 1;
		for (int i = 0; i < nLeft; ++i)
		{
			dStress += GetAPeriodBendingStress(-dD + i*span,tesionList[i],-dD + (i + 1)*span,tesionList[i + 1],dD);
		}
		
		//�е�(���)��������������������
		double	y1 = tesionList[nLeft],
				y2 = tesionList[nLeft + 1];
		double k  = (dD - nLeft * span) / span;
		double x1 = -k * span;
		double x2 = span + x1;    
		double y0 = k * (y2 - y1) + y1;

		dStress += GetAPeriodBendingStress(x1, y1, 0.0, y0, dD);
		dStress += GetAPeriodBendingStress(0.0, y0, x2, y2, dD);

		//����ұ߳����һ��������ܺ�
		int index = 0;
		for (int i = nLeft + 1; i < count - 2; ++i)
		{
			dStress += GetAPeriodBendingStress(x2 + index*span, tesionList[i], x2 + (index + 1)*span, tesionList[i + 1], dD);
			++index;
		}

		//���һ�����
		dStress += GetAPeriodBendingStress(x2 + index * span,tesionList[count-2],dD,tesionList[count-1],dD);
	}
	return dStress;
}

//@brief ��ȡ(x1��x2��һ�ε�����)
double GetAPeriodAxialStress(double x1, double y1, double x2, double y2)
{
	double dM = (x2 - x1);
	double k = 0.0;
	if (iszero(dM))
		return 0.0;
	else
		k = (y2 - y1) / dM;
	//���ֺ󷽳�f(x)= 0.5kx^2-kx1x+y1x
	//���� = f(x2)- f(x1)
	return 0.5*k*x2*x2 - k*x1*x2 + y1*x2 + 0.5*k*x1*x1 - y1*x1;
}
//����������
double GetAxialStress(const bvector<double>& tesionList, double span, double dLineLength)
{
	double	dStress = 0.0,
			dD = dLineLength * 0.5;
	int count = (int)tesionList.size();

	if (2 == count)
	{
		double	y1 = tesionList[0],
				y2 = tesionList[1];

		double y0 = (y2 - y1) * 0.5;
		dStress += GetAPeriodAxialStress(-dD, y1, 0.0, y0);
		dStress += GetAPeriodAxialStress(0.0, y0, dD, y2);
	}
	else
	{
		//�����������ܺ�
		int nLeft = count * 0.5 - 1;
		for (int i = 0; i < nLeft; ++i)
		{
			dStress += GetAPeriodAxialStress(-dD + i*span, tesionList[i], -dD + (i + 1)*span, tesionList[i + 1]);
		}

		//�е�(���)���������������������
		double	y1 = tesionList[nLeft],
				y2 = tesionList[nLeft + 1];
		double k = (dD - nLeft * span) / span;
		double x1 = -k * span;
		double x2 = span + x1;
		double y0 = k * (y2 - y1) + y1;

		dStress += GetAPeriodAxialStress(x1, y1, 0.0, y0);
		dStress += GetAPeriodAxialStress(0.0, y0, x2, y2);

		//����ұ������ܺ�
		int index = 0;
		for (int i = nLeft + 1; i < count - 2; ++i)
		{
			dStress += GetAPeriodAxialStress(x2 + index*span, tesionList[i], x2 + (index + 1)*span, tesionList[i + 1]);
			++index;
		}

		//���һ������
		dStress += GetAPeriodAxialStress(x2 + index * span, tesionList[count - 2], dD, tesionList[count - 1]);
	}
	return dStress;
}
void getStressList(const WChar* field,bvector<double>& tesionList)
{
	bvector<WString> vecStress;
	BeStringUtilities::Split(field, SeparatorAnd, vecStress);

	for (int iDat = 0; iDat < (int)vecStress.size(); iDat++)
	{
		double dStress = _wtof(vecStress.at(iDat).data());
		tesionList.push_back(dStress);
	}
}
bool calculateRebarArea(double dLineLength, ReBarInfo& info)
{
	/*-------------------------------------------׼������---------------------------------------------*/
	// ȡ��SY����Ӧ������
	bvector<double> tesionList;
	getStressList(info.strStress, tesionList);

	//��֤Ӧ����Ч��
	if (tesionList.empty()) return false;
	double Ft = g_rebarCalDlgInfo.dConcrTesion;			//���������Ŀ���ǿ��
	int validflag = checkTesionValid(tesionList, Ft);
	
	//��鲻�ϸ���ʾ
	if (validflag < 0)
	{
		WString strmsg;
		if (-1 == validflag)
		{
			strmsg.append(L"�ж�����!");
		}
		else if (-2 == validflag)
		{
			strmsg.append(L"������Ļ���������ǿ�ȴ��ڵ�ǰ�����������Ӧ��!");
		}
		else 
		{
			strmsg.append(L"���,����������!");  //���
		}
		mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, strmsg.data(), MessageBoxIconType::Information);
		return false;
	}

	//��ȡ����
	double	dAs			= 0.0;							//������
	double	dA			= 0.0;							//����Ӧ�����
	double  dAct		= 0.0;							//����Ӧ�����
	int		iMaxTValid_No = 0;							//�����Ч���
	int		iZeroT_No	= 0;							//0Ӧ�����
	double  dZeroP		= 0.0;							//0Ӧ����Ӧλ��
	int		iConcr_No	= 0;							//��ǿ�ȶ�Ӧ��� 	
	double  dConcrP		= 0.0;							//��ǿ�ȶ�Ӧλ��
	double  dReferArea	= 0.0;							//�ο�������
	double span = g_rebarCalDlgInfo.dDefPointSpan,		//���
		Rd = g_rebarCalDlgInfo.dStruFt,					//�ṹϵ��
		Fy = g_rebarCalDlgInfo.dRebarTesion,			//�ֽ�ǿ�����ֵ
		SH = g_rebarCalDlgInfo.dSectionHeight;			//����߶�

	//����Ӧ��
	double dCoeff = g_rebarCalDlgInfo.dStruSignifyFt * g_rebarCalDlgInfo.dDesignFt;
	int count = (int)tesionList.size();
	for (int m = 0; m < count; ++m)
	{
		tesionList[m] *= dCoeff;
	}

	//��ȡ�������õ�Ӧ������
	int iHead = 0, iTail = 0;
	GetValidTesionHeadAndTail(tesionList, Ft, iHead, iTail);

	/*-------------------------------------------���㲿��---------------------------------------------*/
	double dEndSpan = span;
	if (2 == count)
	{
		if (tesionList[0] < 0)
		{
			double tempSpan = tesionList[1] / (tesionList[1] - tesionList[0]) * dLineLength;
			GetAAndAct(0.0, tesionList[1], Ft, tempSpan, dA, dAct);
		}
		else if (tesionList[1] < 0)
		{
			double tempSpan = tesionList[0] / (tesionList[0] - tesionList[1]) * dLineLength;
			GetAAndAct(tesionList[0], 0.0, Ft, tempSpan, dA, dAct);
		}
		else
		{
			GetAAndAct(tesionList[0], tesionList[1], Ft, dLineLength, dA, dAct);
		}
	}
	else
	{
		//��A��Act���
		for (int i = iHead; i < iTail; ++i)
		{
			GetAAndAct(tesionList[i], tesionList[i + 1], Ft, span, dA, dAct);
		}
		//���������һ�����
		if (count - 1 == iTail)
		{
			if (int(dLineLength / span) < count)
				dEndSpan = dLineLength - span*count + 2.0*span;
		}
		GetAAndAct(tesionList[iTail - 1], tesionList[iTail], Ft, dEndSpan, dA, dAct);
	}

	/*ȡ�����λ��*/
	//�����Ч���,0Ӧ�����,0Ӧ��λ��
	iMaxTValid_No = iTail + 1;
	iZeroT_No = iTail + 1;
	if (count - 1 == iTail || 2 == count)
	{
		dZeroP = dLineLength;
	}
	else if ((count - 2) == iTail)
	{
		double scale = 0.0;
		double dM = fabs((tesionList[iTail - 1] - tesionList[iTail]));
		if (!iszero(dM))
		{
			scale = tesionList[iTail - 1] / dM;
		}
		dZeroP = span*iTail - span + dEndSpan*scale;
	}
	else
	{
		dZeroP = span*iTail;
	}
	//��ǿ�ȶ�Ӧ���,��ǿ�ȶ�Ӧλ��
	GetConcretePositions(tesionList, iTail, span, Ft, dLineLength, dEndSpan, iConcr_No, dConcrP);

	//�ṹ������
	double dB = 1.0;
	//�����Ӧ����ȳ��Խ���߶ȴ�������֮������Ӧ��Ӧȫ���ɸֽ�е�
	if ((dConcrP / SH) > (3.0 / 2.0))
	{
		dAs = dB * dA * Rd / Fy;
	}
	//�������е��������ܴ���������30%���ṹϵ��Rd����0,�����¹淶
	else if (Rd > 0)
	{
		if (dAct > dA*0.3)
		{
			dAct = dA*0.3;
		}
		dAs = dB*(dA*Rd - 0.6*dAct) / Fy;
	}
	else
	{
		if (dAct > dA*0.25)
		{
			dAct = dA*0.25;
		}
		dAs = (dA - dAct) / Fy;
	}
	dAs *= 1000;

	//�ο�������
	int number = 0;
	dReferArea = GetAreaByDiameter(number, dAs, info.dRebarDiameter);

	//���
	double dBendingStress = 0.0;
	//����
	double dAxialStress = 0.0;
	//����
	double dShearSXY = 0.0,
		dShearSYZ = 0.0,
		dShearSXZ = 0.0;
	if (g_rebarCalDlgInfo.bCalcStress)
	{
		dBendingStress = GetBendingStress(tesionList, span, dLineLength);
		dAxialStress = GetAxialStress(tesionList, span, dLineLength);

		//SXY
		bvector<double>().swap(tesionList);
		getStressList(info.shearSXY, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		//���Ƕ��0�����ʱ�ż���
		if (0 == validflag || -2 == validflag)
		{
			dShearSXY = GetAxialStress(tesionList, span, dLineLength);
		}

		//SYZ
		bvector<double>().swap(tesionList);
		getStressList(info.shearSYZ, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		if (0 == validflag || -2 == validflag)
		{
			dShearSYZ = GetAxialStress(tesionList, span, dLineLength);
		}

		//SXZ
		bvector<double>().swap(tesionList);
		getStressList(info.shearSXZ, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		if (0 == validflag || -2 == validflag)
		{
			dShearSXZ = GetAxialStress(tesionList, span, dLineLength);
		}
	}

	/*-------------------------------------------����������---------------------------------------------*/
	WString strBend, strAxial, strShearSXY, strShearSYZ, strShearSXZ;
	if (g_rebarCalDlgInfo.bCalcStress)
	{
		strBend.Sprintf(L"%g", dBendingStress);
		strAxial.Sprintf(L"%g", dAxialStress);
		strShearSXY.Sprintf(L"%g", dShearSXY);
		strShearSYZ.Sprintf(L"%g", dShearSYZ);
		strShearSXZ.Sprintf(L"%g", dShearSXZ);
	}
	else
	{
		strBend.Sprintf(L"%s", " ");
		strAxial.Sprintf(L"%s", " ");
		strShearSXY.Sprintf(L"%s", " ");
		strShearSYZ.Sprintf(L"%s", " ");
		strShearSXZ.Sprintf(L"%s", " ");
	}

	//���������
	WString strResult;
	// ������= �����Ч���&0Ӧ�����&0Ӧ����Ӧλ��&����Ӧ�����&������ǿ�ȶ�Ӧ���&������ǿ�ȶ�Ӧλ��&��������Ӧ�����&������&�ο�������
	strResult.Sprintf(
		L"%d%s%d%s%g%s%g%s%d%s%g%s%g%s%g%s%g%s%s%s%s%s%s%s%s%s%s",
		iMaxTValid_No, SeparatorAnd,	//�����Ч���
		iZeroT_No, SeparatorAnd,		//0Ӧ�����
		dZeroP, SeparatorAnd,			//0Ӧ����Ӧλ��
		dA, SeparatorAnd,				//����Ӧ�����
		iConcr_No, SeparatorAnd,		//������ǿ�ȶ�Ӧ��� 
		dConcrP, SeparatorAnd,			//������ǿ�ȶ�Ӧλ��
		dAct, SeparatorAnd,				//��������Ӧ�����
		dAs, SeparatorAnd,				//������
		dReferArea, SeparatorAnd,		//�ο�������
		strBend, SeparatorAnd,			//���	
		strAxial, SeparatorAnd,			//����
		strShearSXY, SeparatorAnd,		//XY�������
		strShearSYZ, SeparatorAnd,		//YZ�������
		strShearSXZ);					//XZ�������
	swprintf(info.strResult, L"%s", strResult.data());

	//����ֽ������ 
	info.iRebarCount = number;
	info.dRebarArea = dReferArea;

	////��ӡ
	//if (resultList.size() == oCalcResultItemVec.size())
	//{
	//	WString buff;
	//	int index = 0;
	//	for (auto it = oCalcResultItemVec.begin(); it != oCalcResultItemVec.end(); ++it)
	//	{
	//		buff.append(*it);
	//		buff.append(L" ");
	//		buff.append(resultList.at(index++));
	//		buff.append(L"\n");	
	//	}
	//	char msg[1024];
	//	WideCharToMultiByte(CP_ACP, 0, buff.data(), -1, msg, 1024, NULL, NULL);
	//	mdlDialog_dmsgsClear();
	//	mdlDialog_dmsgsPrintA(msg);
	//}
	return true;
}

void SetResultValToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	// ȡ��Ӧ���������
	bvector<WString> pointResult;
	BeStringUtilities::Split(info.strResult, SeparatorAnd, pointResult);

	if (pointResult.begin() == pointResult.end())
	{
		//���listbox
		for (int i = 0; i < (int)oCalcResultItemVec.size(); ++i)
		{
			pointResult.push_back(L"");
		}
	}

	//���listbox
	DialogItemP dlgItemP = dbP->GetItemByTypeAndId(RTYPE_ListBox, LISTBOXID_CalRebar_CalResult);
	if (dlgItemP != NULL)
	{
		//����������������һ��
		if (oCalcResultItemVec.size() != pointResult.size())
			return;

		ListModelP listModelP = mdlDialog_listBoxGetListModelP(dlgItemP->rawItemP);
		if (listModelP != NULL)
		{
			int index = 0;
			for (auto it = pointResult.begin(); it != pointResult.end(); ++it)
			{
				ListRowP	rowP = mdlListModel_getRowAtIndex(listModelP, index++);
				ListCellP	cellP = mdlListRow_getCellAtIndex(rowP, 2); 
				mdlListCell_setStringValue(cellP, pointResult.at(index - 1).data(), TRUE);
			}

			mdlDialog_listBoxSetListModelP(dlgItemP->rawItemP, listModelP, 0);
		}
	}
}

double GetAreaByDiameter(int& number, double dAs, double diameter)
{
	number = 0;
	double r = diameter * 0.5;
	double area = fc_pi * r * r;
	number = 1;
	if (dAs > area)
	{
		dAs /= area;
		int count = dAs;
		if (count < dAs)
		{
			++count;
		}
		number = count;
	}
	return number * area;
}
double GetAreaByRebarCount(double& diameter, double dAs, int number)
{
	int maxIndex = sizeof(m_diameterList) / sizeof(m_diameterList[0]) - 1;
	double radius = sqrt((dAs / number) / fc_pi);
	diameter = radius * 2.0;
	if (diameter <= m_diameterList[0])
	{
		diameter = m_diameterList[0];
	}
	else if (diameter == m_diameterList[maxIndex])
	{
		diameter = m_diameterList[maxIndex];
	}
	else
	{
		bool bFind = false;
		for (int i = 0; i < maxIndex; ++i)
		{
			if (m_diameterList[i] < diameter && diameter < m_diameterList[i + 1])
			{
				diameter = m_diameterList[i + 1];
				bFind = true;
				break;
			}
		}
	}

	return fc_pi * diameter * diameter * 0.25 * number;
}

void HookListBox_CalRebar_RebarInfo(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		ListModelP listModelP = mdlDialog_listBoxGetListModelP(pThis);
		if (nullptr == listModelP)
		{
			listModelP = mdlListModel_create(3);
		}
		else
		{
			mdlListModel_empty(listModelP, true);
			listModelP = mdlListModel_create(3);
		}
		ListRowP	pRow = mdlListRow_create(listModelP);
		ListCellP	pCell = NULL;

		//����
		pCell = mdlListRow_getCellAtIndex(pRow, 0);
		mdlListCell_setStringValue(pCell, L"", TRUE);
		//��txt�ؼ���listbox��cell��
		mdlListCell_setEditor(pCell, RTYPE_Text, TEXTID_CalRebar_ListCell_RebarNumber, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);

		//ֱ��
		pCell = mdlListRow_getCellAtIndex(pRow, 1);
		mdlListCell_setStringValue(pCell, L"", TRUE);
		mdlListCell_setEditor(pCell, RTYPE_ComboBox, COMBOBOXID_CalRebar_ListCell_Diameter, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);

		//������
		mdlListCell_setStringValue(pCell = mdlListRow_getCellAtIndex(pRow, 2), L"", TRUE);

		mdlListModel_addRow(listModelP, pRow);
		mdlDialog_listBoxSetListModelP(pThis, listModelP, 3);
	}break;
	case DITEM_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookItem_CalRebar_CellCombbox_Diameter(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		WCharCP textValue = L"";
		GetListBoxEditCellStringValue(dimP, LISTBOXID_CalRebar_RebarInfo, &textValue);
		g_rebarCalDlgInfo.dDiameter = _wtof(textValue);
		g_rebarCalDlgInfo.iDameterIndex = -1;
		int dcount = sizeof(m_diameterList) / sizeof(m_diameterList[0]);
		for (int i = 0; i < dcount;++i)
		{
			if (g_rebarCalDlgInfo.dDiameter == m_diameterList[i])
			{
				g_rebarCalDlgInfo.iDameterIndex = i;
				break;
			}
		}
		break;
	};
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// ѡ��ڵ�Ϊ������ʱ������������
				if (mdlTreeNode_getLevel(pTreeNode) == 3)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						// �ֽ�ֱ��
						int dnumber = sizeof(m_diameterList) / sizeof(m_diameterList[0]);
						if (g_rebarCalDlgInfo.iDameterIndex > -1 && g_rebarCalDlgInfo.iDameterIndex < dnumber)
						{
							it_find->second.dRebarDiameter = m_diameterList[g_rebarCalDlgInfo.iDameterIndex];
						}

						bvector<WString> vecResultDat;
						BeStringUtilities::Split(it_find->second.strResult, SeparatorAnd, vecResultDat);
						// ȡ��������
						double dAs = _wtof(vecResultDat.at(CALCRESULT_IDX_AS).data());

						// ���ݸֽ�ֱ��������������
						it_find->second.dRebarArea = GetAreaByDiameter(it_find->second.iRebarCount, dAs, it_find->second.dRebarDiameter);

						//���д��ֽ�listbox
						SetRebarInfoToListBox(dimP->db, it_find->second);
					}
				}
			}
		}
	}
	break;
	case DITEM_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookItem_CalRebar_CellTxt_RebarNumber(DialogItemMessage* dmp)
{
	dmp->msgUnderstood = true;
	switch (dmp->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		WCharCP textValue = L"";
		GetListBoxEditCellStringValue(dmp, LISTBOXID_CalRebar_RebarInfo, &textValue);
		g_rebarCalDlgInfo.iRebarCount = _wtoi(textValue);
		break;
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dmp->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// ѡ��ڵ�Ϊ������ʱ������������
				if (mdlTreeNode_getLevel(pTreeNode) == 3)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						// �ֽ����
						it_find->second.iRebarCount = g_rebarCalDlgInfo.iRebarCount;

						bvector<WString> vecResultDat;
						BeStringUtilities::Split(it_find->second.strResult, SeparatorAnd, vecResultDat);
						// ȡ��������
						double dAs = _wtof(vecResultDat.at(CALCRESULT_IDX_AS).data());

						// ���ݸֽ������������������ֱ��
						it_find->second.dRebarArea = GetAreaByRebarCount(it_find->second.dRebarDiameter, dAs, it_find->second.iRebarCount);

						//���д��ֽ�listbox
						SetRebarInfoToListBox(dmp->db, it_find->second);
					}
				}
			}
		}
		break;
	}
	default:
		dmp->msgUnderstood = false;
		break;
	}
}
//���д��ֽ�listbox
void SetRebarInfoToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_RebarInfo, 0);
	ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
	ListCell*		pCell = NULL;

	//���listbox
	if (0 == wcscmp(info.strResult, L""))
	{
		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 0);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 1);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 2);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);//ˢ��listbox
		return;
	}

	//���޸ĺ��CELLֵ��д��listbox��
	WChar buffer[256];
	// ����
	swprintf(buffer, L"%d", info.iRebarCount);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 0);
	mdlListCell_setStringValue(pCell, buffer, TRUE);
	// �ֽ�ֱ��
	swprintf(buffer, L"%g", info.dRebarDiameter);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 1);
	mdlListCell_setStringValue(pCell, buffer, TRUE);

	// �ֽ����
	swprintf(buffer, L"%g", info.dRebarArea);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 2);
	mdlListCell_setStringValue(pCell, buffer, TRUE);

	mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);//ˢ��listbox

	//���½��listbox�ο����
	pLBox = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_CalResult, 0);
	pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);

	if (pModel != NULL)
	{
		DialogItem* pRebarCmb = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ComboBox,COMBOBOXID_CalRebar_RebarType,0);
		StringList* sl = mdlDialog_comboBoxGetStrListP(pRebarCmb->rawItemP);
		
		//RscFileHandle rfHandle;
		//mdlResource_openFile(&rfHandle, L"outputRebarDlg.r", RSC_READONLY);
		//StringListP sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_CalRebar_RebarType);
		WString strRebarType;
		mdlStringList_getMemberString(strRebarType, NULL, sl, (long)g_rebarCalDlgInfo.iRebarType);

		// ���²ο������
		swprintf(buffer, L"�ο����: %s��%d��%g", strRebarType, info.iRebarCount, info.dRebarDiameter);
		pCell = mdlListModel_getCellAtIndexes(pModel, 8, 1);
		mdlListCell_setStringValue(pCell, buffer, TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 8, 2);
		swprintf(buffer, L"%g", info.dRebarArea);
		mdlListCell_setStringValue(pCell, buffer, TRUE);

		mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);
	}
}

void HOOKText_CalRebar_RebarTesion(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	case DITEM_MESSAGE_STATECHANGED:
	{
		int count = sizeof(m_rebarStrengthList) / sizeof(m_rebarStrengthList[0]);
		g_rebarCalDlgInfo.iRebarType = count;
		double stress = g_rebarCalDlgInfo.dRebarTesion;
		for (int loop = 0; loop < count; ++loop)
		{
			if (stress == m_rebarStrengthList[loop])
			{
				g_rebarCalDlgInfo.iRebarType = loop;
				break;
			}
		}
		mdlDialog_itemsSynch(dimP->db);
		break;
	};

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HOOKText_CalRebar_ConcrTesion(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	case DITEM_MESSAGE_STATECHANGED:
	{
		int count = sizeof(m_concrStrengthList) / sizeof(m_concrStrengthList[0]);
		g_rebarCalDlgInfo.iConcrType = count;
		double stress = g_rebarCalDlgInfo.dConcrTesion;
		for (int loop = 0; loop < count; ++loop)
		{
			if (stress == m_concrStrengthList[loop])
			{
				g_rebarCalDlgInfo.iConcrType = loop;
				break;
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

void HookDialog_CalRebar_AddRebarLine(DialogMessage *dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_ACTIONBUTTON:
	{
		if (dmP->u.actionButton.actionType == ACTIONBUTTON_OK)
		{
			WString strChk;
			strChk.append(g_rebarCalDlgInfo.rebarName);
			strChk.Trim();

			if (strChk.length() == 0)
			{
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"��������������ƣ�", MessageBoxIconType::Information);
				//ȡ���Ի���Ĺر�
				dmP->u.actionButton.abortAction = TRUE;
			}
			else
			{
				wcscpy(g_rebarCalDlgInfo.rebarName, strChk.data());
			}
		}

		break;
	}
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void outputRebarCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_RebarCalculate, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_RebarCalculate);
}

void GetDataFromControls(rebarInfo& orebarInfo)
{
	orebarInfo.dStruSignifyFt	= g_rebarCalDlgInfo.dStruSignifyFt;	//�ṹ��Ҫϵ��
	orebarInfo.dDesignFt		= g_rebarCalDlgInfo.dDesignFt;		//���״��ϵ��
	orebarInfo.dStruFt			= g_rebarCalDlgInfo.dStruFt;		//�ṹϵ��
	orebarInfo.dDefPointSpan	= g_rebarCalDlgInfo.dDefPointSpan;	//Ĭ�ϵ��(mm)
	orebarInfo.iRebarType		= g_rebarCalDlgInfo.iRebarType;		//�ֽ������±꣬���ڻ�ȡfy
	orebarInfo.dRebarTesion		= g_rebarCalDlgInfo.dRebarTesion;	//�ֽ��ǿ��
	orebarInfo.iConcrType		= g_rebarCalDlgInfo.iConcrType;		//�����������±꣬���ڻ�ȡft
	orebarInfo.dConcrTesion		= g_rebarCalDlgInfo.dConcrTesion;	//���������Ŀ���ǿ��
	orebarInfo.dSectionHeight	= g_rebarCalDlgInfo.dSectionHeight;	//����߶�
	orebarInfo.bCalcStress		= g_rebarCalDlgInfo.bCalcStress;	//�Ƿ������ء�����������
}

void GetListBoxEditCellStringValue(DialogItemMessage* dimP, RscId listboxId, WCharCP* textValue)
{
	DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, listboxId, 0);
	if (pLBox)
	{
		int		    	row = -1, col = -1;
		MSValueDescr	uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		mdlListCell_getStringValue(pCell, textValue);
	}
}

void InitDrawColorShape(DialogItemMessage* dimP, GuiTreeNode* pRebarLineNode, const bvector<WString>& vecData)
{
	// ȡ�ù����ڵ�
	GuiTreeNode* caseNode = mdlTreeNode_getParent(pRebarLineNode);
	DgnModelRefP target = NULL;
	if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
	{
		if (!mdlModelRef_areSame(MASTERFILE, target))
		{
			s_pDCShape->CloseDrawFunction();
			s_drawtempLine.CloseDrawFunction();
			ClearclipfaceShow();
			mdlModelRef_activateAndDisplay(target);
			//ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// ����
			//ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// �⻬	
		}
		mdlModelRef_freeWorking(target);
		MsdiMsCaeSiUtility::fitView(0);
	}

	// ��ĿID
	int iProjectId = wtoi(vecData.at(KEY_IDX_PROJECTID).data());
	// ����ID
	int iCaseID = wtoi(vecData.at(KEY_IDX_CASEID).data());

	//��ȡ�������
	bvector<WString> vecRstTypeNames;
	getRstTypeByPjIdAndCaseId(iProjectId, iCaseID, vecRstTypeCodes, vecRstTypeNames);
	vecRstTypeNames.clear();
	bvector<WString>().swap(vecRstTypeNames);

	double tStrValue = 0;
	double tEndValue = 0;
	s_pDCShape->m_bAutomatic = true;
	//ͨ���û���ѡ���ȡ�������(ȡֵ�±��Ӧ���ݿ��RSTYPE��������ż�1,��ȡ������͵ĺ����дӵڶ���������ʼȡ)
	s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(10);
	//Header
	s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
	//Unit
	s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

	DPoint3d minPos, maxPos;
	GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, iProjectId, iCaseID, s_pDCShape->ResultTypeCode, minPos, maxPos);
	//����ѡ��ľ��幤���ͽ����������ɫ��
	s_pDCShape->ProjectId = iProjectId;
	s_pDCShape->CaseID = iCaseID;
	s_pDCShape->StartValue = tStrValue;
	s_pDCShape->EndValue = tEndValue;
	bvector<DPoint3d>().swap(s_pDCShape->m_vecMaxPts);
	bvector<DPoint3d>().swap(s_pDCShape->m_vecMinPts);
	s_pDCShape->m_vecMaxPts.push_back(maxPos);
	s_pDCShape->m_vecMinPts.push_back(minPos);
	//���޸�ɫ���е�proName��caseName
	getProjectInfoById(s_pDCShape->ProjectName, s_pDCShape->ProjectUnit, iProjectId);

	WString strCaseName;
	mdlTreeNode_getDisplayTextWString(mdlTreeNode_getParent(caseNode), strCaseName);
	s_pDCShape->CaseName = strCaseName;

	//����Ϊ�Զ�����ɫ��
	s_pDCShape->m_bAutomatic = true;
	//�Զ��ȷ�ɫ����ֵ
	s_pDCShape->divideColorRange();

	mdlDialog_itemsSynch(dimP->db);
}