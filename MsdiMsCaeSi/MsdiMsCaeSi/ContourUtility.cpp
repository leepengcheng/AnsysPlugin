#include "MsdiMsCaeSipch.h"
extern bmap<WString, bmap<string, int>> m_OutFaceNodeStrMap;// ��ǰDgn�ļ�������CAEģ�͵ı�������

bool GetOutDataCaeResult(PointOutData& oPointDat, TransformCP tMatrix, RotMatrixCR rotMatrix)
{
	//tmatrix ��Ӧ��ͼ�еĦ¾���������ת����
	//rotMatrix �Ƕ�Ӧ���������Բ������ת����ƽ��X�����ת���󣨵ѿ�������ʱ��ֵΪ��λ����
	RotMatrix tempMatrix;
	RotMatrix prdMatrix;

	tMatrix->GetMatrix(tempMatrix);
	mdlRMatrix_multiply(&prdMatrix, &rotMatrix, &tempMatrix);

	DPoint3d tvec;
	double tValuesX = oPointDat.ResultVals[L"UX"];
	double tValuesY = oPointDat.ResultVals[L"UY"];
	double tValuesZ = oPointDat.ResultVals[L"UZ"];

	//������UX,UY,UZ��ʸ��
	tvec = DPoint3d::From(tValuesX, tValuesY, tValuesZ);
	//ʸ���ҳ����ҼнǾ���
	mdlRMatrix_rotatePoint(&tvec, &prdMatrix);
	//ȡ�ü�����λ��
	oPointDat.ResultVals[RESULTTYPE_UX] = tvec.x;
	oPointDat.ResultVals[RESULTTYPE_UY] = tvec.y;
	oPointDat.ResultVals[RESULTTYPE_UZ] = tvec.z;

	RotMatrix ts_prdMatrix, tarMatrix, DET;
	mdlRMatrix_transpose(&ts_prdMatrix, &prdMatrix);
	double tsx = oPointDat.ResultVals[RESULTTYPE_SX];
	double tsy = oPointDat.ResultVals[RESULTTYPE_SY];
	double tsz = oPointDat.ResultVals[RESULTTYPE_SZ];
	double tsxy = oPointDat.ResultVals[RESULTTYPE_SXY];
	double tsxz = oPointDat.ResultVals[RESULTTYPE_SXZ];
	double tsyz = oPointDat.ResultVals[RESULTTYPE_SYZ];

	DET = RotMatrix::FromRowValues(
		tsx, tsxy, tsxz,
		tsxy, tsy, tsyz,
		tsxz, tsyz, tsz);
	mdlRMatrix_multiply(&tarMatrix, &prdMatrix, &DET);
	mdlRMatrix_multiply(&tarMatrix, &tarMatrix, &ts_prdMatrix);

	oPointDat.ResultVals[RESULTTYPE_SX] = tarMatrix.GetComponentByRowAndColumn(0, 0);
	oPointDat.ResultVals[RESULTTYPE_SY] = tarMatrix.GetComponentByRowAndColumn(1, 1);
	oPointDat.ResultVals[RESULTTYPE_SZ] = tarMatrix.GetComponentByRowAndColumn(2, 2);
	oPointDat.ResultVals[RESULTTYPE_SXY] = tarMatrix.GetComponentByRowAndColumn(1, 0);
	oPointDat.ResultVals[RESULTTYPE_SXZ] = tarMatrix.GetComponentByRowAndColumn(0, 2);
	oPointDat.ResultVals[RESULTTYPE_SYZ] = tarMatrix.GetComponentByRowAndColumn(1, 2);

	double dx = tarMatrix.GetComponentByRowAndColumn(0, 0);
	double dy = tarMatrix.GetComponentByRowAndColumn(1, 1);
	double dxy = tarMatrix.GetComponentByRowAndColumn(1, 0);
	// ����Ӧ�� = (��x+��y)/2 + sqrt((��x-��y)*(��x-��y)/4 + ��xy * ��xy�� 
	oPointDat.dMaxS = (dx + dy) / 2 + sqrt((dx - dy)*(dx - dy) / 4 + dxy*dxy);
	// �����Ӧ�� = (��x+��y)/2 - sqrt((��x-��y)*(��x-��y)/4 + ��xy * ��xy�� 
	oPointDat.dMinS = (dx + dy) / 2 - sqrt((dx - dy)*(dx - dy) / 4 + dxy*dxy);
	// ������Ӧ������ƽ��ķ�����ԭx��н� 2��= atan(-1*(2��xy/(��x-��y)))
	double d�� = atan(-1 * ((2 * dxy) / (dx - dy))) / 2;
	// ���Ƕ�ת���ɻ���
	oPointDat.dAngleS = fc_piover180 * d��;

	return true;
}

//ͨ��projectID��caseid�Լ�ѡ���TYPE���ͣ���ȡ���нڵ�ֵ
bool GetCaeResult(bvector<double>& oCaeResultVec, int iProjectId, int iCaseId, WString sColNmae)
{
	Db m_db;
	Statement *m_statment;
	WString strSql;
	StatusInt rc;

	strSql.append(L"SELECT ");
	strSql.append(sColNmae);
	strSql.append(L" \n");
	strSql.append(L"FROM RESULTS \n");
	strSql.append(L"WHERE ProjectId = @ProjectId AND CaseId = @CaseId \n");
	strSql.append(L"ORDER BY ID \n");

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		m_statment = new Statement();

		size_t len = strSql.length() + 1;
		CharP cSQL = (CharP)malloc(len*sizeof(char));
		strSql.ConvertToLocaleChars(cSQL);

		rc = m_statment->Prepare(m_db, cSQL);

		m_statment->BindInt(1, iProjectId);
		m_statment->BindInt(2, iCaseId);

		if (rc == BE_SQLITE_OK)
		{
			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				oCaeResultVec.push_back(m_statment->GetValueDouble(0));
			}
		}

		m_statment->Finalize();
	}

	return true;
}

bool GetFaceNodePtAndVal(bvector<bvector<ContourPoint>>& vecCaeMeshPtVal, bvector<ContourPoint>& vecFacePtAndVal)
{
	ContourPoint					oCpTemp;
	bool							bRet = false;
	bool bTriangleFace = false;

	bvector<ContourPoint> vecTriangleFace;// ������
	if (vecFacePtAndVal.size() == 4)
	{
		// ȡ�õ�ǰ����������С��CAE�������Ľڵ���Ϣ
		ContourPoint oCpStart = vecFacePtAndVal.front();
		for (size_t i = 1; i < vecFacePtAndVal.size(); i++)
		{
			if (oCpStart.dVal > vecFacePtAndVal.at(i).dVal)
				oCpStart = vecFacePtAndVal.at(i);
		}

		// ȡ�õ�ǰ������������CAE�������Ľڵ���Ϣ
		ContourPoint oCpEnd = vecFacePtAndVal.back();
		for (size_t i = vecFacePtAndVal.size() - 2; i >= 0; i--)
		{
			if (oCpEnd.dVal < vecFacePtAndVal.at(i).dVal)
				oCpEnd = vecFacePtAndVal.at(i);
			if (i == 0)
				break;
		}

		// �ж�������С�Ľڵ��ھ��������Ƿ�Ϊ�Խǽڵ�
		if (abs(oCpStart.iIdx - oCpEnd.iIdx) == 2)
		{
			bTriangleFace = false;
		}
		else
		{
			int iColor_Start = -1;	// ������ĶԽ�������ɫID
			int iColor_End = -1;	// ������ĶԽ��յ����ɫID
			double dValStt;			// ������ĶԽ�����CAE���
			double dValEnd;			// ������ĶԽ��յ��CAE���

			// ȡ�þ�����ĶԽǽڵ�
			ContourPoint oCpOpposite_Start = vecFacePtAndVal.at((oCpStart.iIdx + 2) % 4);
			ContourPoint oCpOpposite_End = vecFacePtAndVal.at((oCpEnd.iIdx + 2) % 4);

			for (int iVal = 0; iVal < s_pDCShape->Color_num; ++iVal)
			{
				dValStt = s_pDCShape->ClrShapeVal[iVal];
				dValEnd = s_pDCShape->ClrShapeVal[iVal + 1];

				// �жϾ��ζԽǶ������ɫ�Ƿ���ͬһ����
				if ((dValStt <= oCpStart.dVal && oCpStart.dVal < dValEnd)
					&& (dValStt <= oCpOpposite_Start.dVal && oCpOpposite_Start.dVal < dValEnd))
				{
					iColor_Start = iVal;
				}

				// �жϾ��ζԽǶ������ɫ�Ƿ���ͬһ����
				if ((dValStt <= oCpEnd.dVal && oCpEnd.dVal < dValEnd)
					&& (dValStt <= oCpOpposite_End.dVal && oCpOpposite_End.dVal < dValEnd))
				{
					iColor_End = iVal;
				}
			}

			// �����ζԽǶ������ɫ��ͬһ�������뽻��ĶԽǶ������ɫ����ͬһ����ʱ��
			// ��Ҫ��������ָ�Ϊ��������
			if ((iColor_Start != iColor_End)
				&& iColor_Start != -1
				&& iColor_End != -1)
			{
				bTriangleFace = true;
			}
		}
	}

	// ��bTriangleFace��������ָ��־ΪTrueʱ����������ָ�Ϊ��������
	if (bTriangleFace == true)
	{
		// ȡ�þ���������Ľڵ�
		oCpTemp = GetFaceCenNode(vecFacePtAndVal);

		// ��������ָ�Ϊ��������
		vecFacePtAndVal.push_back(vecFacePtAndVal.front());

		// ѭ�����������ÿ���ߵĶ˵������������ĵ㹹����������
		for (int i = 0; i < vecFacePtAndVal.size() - 1; i++)
		{
			vecTriangleFace.push_back(vecFacePtAndVal.at(i));
			vecTriangleFace.push_back(oCpTemp);
			vecTriangleFace.push_back(vecFacePtAndVal.at(i + 1));
			vecTriangleFace.push_back(vecFacePtAndVal.at(i));

			vecCaeMeshPtVal.push_back(vecTriangleFace);
			bvector<ContourPoint>().swap(vecTriangleFace);
		}
	}
	else
	{
		vecFacePtAndVal.push_back(vecFacePtAndVal.front());
		vecCaeMeshPtVal.push_back(vecFacePtAndVal);
	}

	bvector<ContourPoint>().swap(vecTriangleFace);// ������

	bRet = true;
	return bRet;
}

// �����ֵ��
bool GetPosition(ContourPoint& ptResult, DPoint3d ptStart, DPoint3d ptEnd, double dStartVal, double dEndVal, double dVal)
{
	double dVal1;
	double dVal2;
	double index;
	bool bRet;

	dVal1 = dStartVal - dVal;
	dVal2 = dEndVal - dVal;
	index = dVal1 * dVal2;

	if (index > 0)
	{
		bRet = false;
	}
	else if (dStartVal == dVal || dEndVal == dVal)
	{
		bRet = false;
	}
	else
	{
		// ����0.0000000001��ֹ��ĸΪ0
		ptResult.dPt.x = ptStart.x + (dVal - dStartVal) * (ptEnd.x - ptStart.x) / (dEndVal - dStartVal + 0.0000000001);
		ptResult.dPt.y = ptStart.y + (dVal - dStartVal) * (ptEnd.y - ptStart.y) / (dEndVal - dStartVal + 0.0000000001);
		ptResult.dPt.z = ptStart.z + (dVal - dStartVal) * (ptEnd.z - ptStart.z) / (dEndVal - dStartVal + 0.0000000001);

		ptResult.dVal = dVal;

		bRet = true;
	}

	return bRet;
}

ContourPoint GetFaceCenNode(bvector<ContourPoint>& pFace)
{
	ContourPoint oFaceCenterNode;
	DSegment3d l1, l2;
	double dLen1, dLen1_Cen, dVal1, dLen2, dLen2_Cen, dVal2;

	l1.SetStartPoint(pFace[0].dPt);
	l1.SetEndPoint(pFace[2].dPt);

	dLen1 = mdlVec_distance(&pFace[0].dPt, &pFace[2].dPt);

	l2.SetStartPoint(pFace[1].dPt);
	l2.SetEndPoint(pFace[3].dPt);

	dLen2 = mdlVec_distance(&pFace[1].dPt, &pFace[3].dPt);

	if (SUCCESS == mdlVec_intersect(&oFaceCenterNode.dPt, &l1, &l2))
	{
		dLen1_Cen = mdlVec_distance(&pFace[0].dPt, &oFaceCenterNode.dPt);
		dLen2_Cen = mdlVec_distance(&pFace[1].dPt, &oFaceCenterNode.dPt);

		dVal1 = pFace[0].dVal + (pFace[2].dVal - pFace[0].dVal) * dLen1_Cen / dLen1;
		dVal2 = pFace[1].dVal + (pFace[3].dVal - pFace[1].dVal) * dLen2_Cen / dLen2;

		oFaceCenterNode.dVal = (dVal1 + dVal2) / 2;
	}

	oFaceCenterNode.iIdx = 0;

	return oFaceCenterNode;
}

void MyPushback(bvector<ContourPoint> & vecFace, Statement *l_statment, DPoint3d basePt, WString sUnit)
{
	ContourPoint oTmp;

	for (int i = 0; i < 4; i++)
	{
		if (l_statment->IsColumnNull(5 * i) == true) continue;

		oTmp.iIdx = i;
		oTmp.NodeId = l_statment->GetValueInt(5 * i);
		oTmp.dPt.x = l_statment->GetValueDouble(5 * i + 1) * UOR_PER_UNIT(sUnit) + basePt.x;
		oTmp.dPt.y = l_statment->GetValueDouble(5 * i + 2) * UOR_PER_UNIT(sUnit) + basePt.y;
		oTmp.dPt.z = l_statment->GetValueDouble(5 * i + 3) * UOR_PER_UNIT(sUnit) + basePt.z;
		oTmp.dVal = l_statment->GetValueDouble(5 * i + 4);

		vecFace.push_back(oTmp);
	}
}

bool getCaeFaceInfos(WString ngName, WString type, int caseID, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt)
{
	//��������������ȡ����ID
	int iProID;
	WString sUnit;
	splitNgName_Model(ngName, UNUSE_WSTRING, iProID, UNUSE_INT, UNUSE_WSTRING, sUnit);

	Db m_db;
	Statement *l_statment;
	WString strSql;
	StatusInt rc;

	strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, r1.").append(type.c_str());
	strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, r2.").append(type.c_str());
	strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, r3.").append(type.c_str());
	strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, r4.").append(type.c_str());
	strSql.append(L"\nFROM (SELECT ProjectId,Node1,Node2,Node3,Node4 FROM FACES WHERE ProjectId = @ProjectId AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 GROUP BY NodeIdStr HAVING COUNT(NodeIdStr) < 2) a \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n1 ON a.ProjectId = n1.ProjectId AND a.Node1 = n1.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n2 ON a.ProjectId = n2.ProjectId AND a.Node2 = n2.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n3 ON a.ProjectId = n3.ProjectId AND a.Node3 = n3.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n4 ON a.ProjectId = n4.ProjectId AND a.Node4 = n4.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,").append(type.c_str()).append(L" FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,").append(type.c_str()).append(L" FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,").append(type.c_str()).append(L" FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,").append(type.c_str()).append(L" FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		l_statment = new Statement();

		size_t len = strSql.length() + 1;
		CharP cSQL = (CharP)malloc(len*sizeof(char));
		strSql.ConvertToLocaleChars(cSQL);

		rc = l_statment->Prepare(m_db, cSQL);

		l_statment->BindInt(1, iProID);

		Utf8String sTmp;
		BeStringUtilities::WCharToUtf8(sTmp, ngName.c_str());
		l_statment->BindUtf8String(2, sTmp, l_statment->BindMakeCopy::MAKE_COPY_No);

		l_statment->BindInt(3, caseID);

		if (rc == BE_SQLITE_OK)
		{
			bvector<ContourPoint>	FacePoints;

			while (l_statment->Step() == BE_SQLITE_ROW)
			{
				FacePoints.clear();
				MyPushback(FacePoints, l_statment, basePt, sUnit);

				vecFaces.push_back(FacePoints);
			}

			bvector<ContourPoint>().swap(FacePoints);
		}

		l_statment->Finalize();

		return true;
	}
	else
		return false;
}

void ClearContour()
{
	if (MASTERFILE == NULL)
		return;
	s_drawtempLine.CloseDrawFunction();

	DgnPlatform::LevelId levelId;
	// ȡ��CAE��ͼ��ͼ��
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}

	// ȡ��CAE��ֵ��ͼ��ͼ��
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ֵ��ͼ"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}

	// ȡ��CAEʸ��ͼ��ͼ��
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAEʸ��ͼ"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}

	// ȡ��CAE��ͼ�����ͼ��
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ����"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}
	mdlLevelTable_rewrite(MASTERFILE);

	NamedGroupCollectionPtr ngcCAESolid;
	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
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
}

void ShowCAEModel()
{
	if (MASTERFILE == NULL)
		return;

	LevelId levelId_CaeModel;
	NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*MASTERFILE);

	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		WString NGName = namedGroup->GetName();
		if (NGName.find(NGNAME_PREFIX_CAEMODEL_DOLLAR) != WString::npos)
		{
			if (SUCCESS == mdlLevel_getIdFromName(&levelId_CaeModel, MASTERFILE, LEVEL_NULL_ID, NGName.data()))
			{
				mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, true);
			}
		}
	}

	mdlLevelTable_rewrite(MASTERFILE);
}

void focusOnCAEModel(WString ngName)
{
	//��������������ȡ����ID
	int iProjectId;
	splitNgName_Model(ngName, UNUSE_WSTRING, iProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//���ݲ�����ȡ��basePt
	DPoint3d	basePt;
	WString		sUnit;
	LevelId		caeLvID;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, caeLvID, ngName);

	//ͨ��ProjectID��ѯ���ݿ��ȡ��ڵ��XYZ��ֵ����+basePt�����õ�ʵ������ļ�ֵ
	DRange3d dRangeVec;
	dRangeVec.low.x = getMinCoordinateValueOfProject(iProjectId, "X") * UOR_PER_UNIT(sUnit) + basePt.x;
	dRangeVec.low.y = getMinCoordinateValueOfProject(iProjectId, "Y") * UOR_PER_UNIT(sUnit) + basePt.y;
	dRangeVec.low.z = getMinCoordinateValueOfProject(iProjectId, "Z") * UOR_PER_UNIT(sUnit) + basePt.z;

	dRangeVec.high.x = getMaxCoordinateValueOfProject(iProjectId, "X") * UOR_PER_UNIT(sUnit) + basePt.x;
	dRangeVec.high.y = getMaxCoordinateValueOfProject(iProjectId, "Y") * UOR_PER_UNIT(sUnit) + basePt.y;
	dRangeVec.high.z = getMaxCoordinateValueOfProject(iProjectId, "Z") * UOR_PER_UNIT(sUnit) + basePt.z;

	int viewIndex = tcb->lstvw;
	RotMatrix   rmat;
	Dpoint3d    viewpt[2];
	FitViewOptions    options;
	options.expandClippingPlanes = 1;
	options.forceActiveZToCenter = 0;
	options.disableCenterCamera = 0;
	options.optionPadding = 0;
	options.optionPadding2 = 0;

	double focusRatio = 3.0 / 4.0;
	double rL = (dRangeVec.high.x - dRangeVec.low.x), rW = (dRangeVec.high.y - dRangeVec.low.y), rH = (dRangeVec.high.z - dRangeVec.low.z);
	viewpt[0].x = dRangeVec.low.x - ((rL*(1.0 - focusRatio) / (2.0*focusRatio)));
	viewpt[0].y = dRangeVec.low.y - ((rW*(1.0 - focusRatio) / (2.0*focusRatio)));
	viewpt[0].z = dRangeVec.low.z - ((rH*(1.0 - focusRatio) / (2.0*focusRatio)));
	viewpt[1].x = dRangeVec.high.x + ((rL*(1.0 - focusRatio) / (2.0*focusRatio)));
	viewpt[1].y = dRangeVec.high.y + ((rW*(1.0 - focusRatio) / (2.0*focusRatio)));
	viewpt[1].z = dRangeVec.high.z + ((rH*(1.0 - focusRatio) / (2.0*focusRatio)));

	mdlView_getParameters(NULL, NULL, NULL, &rmat, NULL, viewIndex);
	mdlRMatrix_multiplyRange(&viewpt[0], &viewpt[1], &rmat);
	mdlView_fitViewToRange(&viewpt[0], &viewpt[1], &options, viewIndex);
	mdlView_updateSingle(viewIndex);
}