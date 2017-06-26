#include "MsdiMsCaeSipch.h"

nodeViewer::nodeViewer(int toolID, bool plotModel) : DgnElementSetTool(toolID)
{
	m_bPlotModel = plotModel;

	//��������ͼģʽ����Ҫ������ѡ���������ݿ��ж�ȡ����
	if (!m_bPlotModel)
	{
		StatusInt rc;
		AString strSql;
		Db m_db;
		Statement *m_statment = new Statement();

		rc = OpenDB(m_db);
		if (rc == BE_SQLITE_OK)
		{
			strSql.append("SELECT \n");
			strSql.append(ws2s(s_DrawColorShape.ResultTypeCode.GetWCharCP()).data());
			strSql.append(" FROM RESULTS WHERE ProjectId = @projectID AND CaseId = @caseID ORDER BY ID\n");

			rc = m_statment->Prepare(m_db, strSql.c_str());
			m_statment->BindInt(1, s_DrawColorShape.ProjectId);
			m_statment->BindInt(2, s_DrawColorShape.CaseID);

			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				// ʵ����
				m_vecResults.push_back(m_statment->GetValueDouble(0));
			}
		}

		delete m_statment;
		m_db.CloseDb();
	}

	s_DrawColorShape.m_bNodeView = true;
}

bool nodeViewer::_OnPostLocate(HitPathCP path, WStringR cantAcceptReason)
{
	if (!__super::_OnPostLocate(path, cantAcceptReason))
	{
		s_DrawColorShape.m_bNodeView_Temp = false;
		return false;
	}

	DPoint3d hitPoint;
	mdlHitPath_getHitPoint(path, hitPoint);
	ElementHandle eh(mdlDisplayPath_getElem((DisplayPathP)path, 0), mdlDisplayPath_getPathRoot((DisplayPathP)path));

	//��ȡԪ������ͼ����
	LevelId elemlv = eh.GetElementDescrCP()->el.ehdr.level;
	WChar	         wLevelName[MAX_LEVEL_NAME_LENGTH];
	mdlLevel_getName(wLevelName, MAX_LEVEL_NAME_LENGTH - 1, MASTERFILE, elemlv);

	WString csLevelName;
	csLevelName.append(wLevelName);
	//����ͼģʽ��ͼ����Ϊ"CAE��ͼ"ʱ
	if (m_bPlotModel && 
		(wcscmp(wLevelName, L"CAE��ͼ") == 0 || 
		wcscmp(wLevelName, L"CAE��ͼ����") == 0 || 
		wcscmp(wLevelName, L"CAE������ͼ") == 0))
	{
		//��ȡ��׽Ԫ�ص����ж������꣬��������������ͣλ��������������
		bvector<int> pIndices;
		bvector<DPoint3d> pXYZ;
		int iNumIndexPerFace;
		int iNumFace;
		if (SUCCESS == mdlMesh_getPolyfaceArrays(eh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
		{
			int iMax = 0;
			double d = mdlVec_distance(&hitPoint, &pXYZ[0]);

			for (int i = 1; i < (int)pXYZ.size(); i++)
			{
				if (mdlVec_distance(&hitPoint, &pXYZ[i]) < d)
				{
					d = mdlVec_distance(&hitPoint, &pXYZ[i]);
					iMax = i;
				}
			}

			s_DrawColorShape.m_NodePt_Temp = pXYZ[iMax];

			//��ȡ������������ϵĽ��ֵ
			s_DrawColorShape.m_dNodeInfo_Temp = 0;
			XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
			XAttributeHandle xh(eh.GetElementRef(), handlerId, XAttrID_PlotMeshProperty);
			MeshSurfaceValue *meshValues = NULL;
			if (xh.IsValid())
			{
				meshValues = (MeshSurfaceValue *)xh.PeekData();
				if (meshValues->nodeCount > iMax)
					s_DrawColorShape.m_dNodeInfo_Temp = meshValues->NodeValue[iMax];
			}
		}

		//����ʱ���ƵĽڵ���ƿ���
		s_DrawColorShape.m_bNodeView_Temp = true;
	}
	//������ͼģʽ��ͼ����ΪCAEMODEL$X$Xʱ
	else if ((!m_bPlotModel) && wcscmp(wLevelName, s_DrawColorShape.NgName.data()) == 0)
	{
		//��ȡ��׽Ԫ�ص����ж������꣬��������������ͣλ��������������
		bvector<int> pIndices;
		bvector<DPoint3d> pXYZ;
		int iNumIndexPerFace;
		int iNumFace;
		if (SUCCESS == mdlMesh_getPolyfaceArrays(eh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
		{
			//��ȡ������������ϵĽ��ֵ
			s_DrawColorShape.m_dNodeInfo_Temp = 0;
			XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
			XAttributeHandle xh(eh.GetElementRef(), handlerId, XAttrID_MeshProperty);
			MeshProperty *meshProp = NULL;
			if (xh.IsValid())
			{
			meshProp = (MeshProperty *)xh.PeekData();
			int iMin = -1;
			int iMin_Indices;
			double d;
			for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
			{
				int iIndices = pIndices.at(iIdx);
				if (iMin == -1 && iIndices != 0)//��һ�θ�ֵ
				{
					d = mdlVec_distance(&hitPoint, &pXYZ[iIndices - 1]);
					iMin = iIndices;
					iMin_Indices = iIdx;
				}
				else if (iIndices != 0)	// �ж�Mesh�ڵ��ŵ��±��Ƿ�Ϊ�ָ���('0')
				{
					if (mdlVec_distance(&hitPoint, &pXYZ[iIndices-1]) < d)
					{
						d = mdlVec_distance(&hitPoint, &pXYZ[iIndices - 1]);
						iMin = iIndices;
						iMin_Indices = iIdx;
					}
				}
			}
			s_DrawColorShape.m_NodePt_Temp = pXYZ[iMin - 1];
			int id = meshProp->ID[iMin_Indices];
				if (id > 0)
					s_DrawColorShape.m_dNodeInfo_Temp = m_vecResults.at(id - 1);
			}
		}
		//����ʱ���ƵĽڵ���ƿ���
		s_DrawColorShape.m_bNodeView_Temp = true;
	}
	else if (csLevelName.find(NGNAME_PREFIX_CAECLIP) != WString::npos)
	{
		//��ȡ��׽Ԫ�ص����ж������꣬��������������ͣλ��������������
		bvector<int> pIndices;
		bvector<DPoint3d> pXYZ;
		int iNumIndexPerFace;
		int iNumFace;

		bvector<double>   oCaeresults;
		GetCaeResult(oCaeresults, s_DrawColorShape.ProjectId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode);
		if (SUCCESS == mdlMesh_getPolyfaceArrays(eh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
		{
			XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
			XAttributeHandle xh(eh.GetElementRef(), handlerId, XAttrID_CutFace);
			CutPoints *cutPoints = NULL;

			if (xh.IsValid())
			{
				cutPoints = (CutPoints *)xh.PeekData();
			}
			double d =-1;
			CutPt MinPoint;
			for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
			{
				int iIndices = pIndices.at(iIdx);
				if (d == -1 && iIndices != 0)//��һ�θ�ֵ
				{
					d = mdlVec_distance(&hitPoint, &pXYZ[iIndices - 1]);
					MinPoint.dPt = pXYZ.at(iIndices - 1);
					MinPoint.StartID = cutPoints->StartID[iIndices - 1];
					MinPoint.EndID = cutPoints->EndID[iIndices - 1];
					MinPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];
				}
				else if (iIndices != 0)
				{
					if (mdlVec_distance(&hitPoint, &pXYZ[iIndices - 1]) < d)
					{
						d = mdlVec_distance(&hitPoint, &pXYZ[iIndices - 1]);
						MinPoint.dPt = pXYZ.at(iIndices - 1);
						MinPoint.StartID = cutPoints->StartID[iIndices - 1];
						MinPoint.EndID = cutPoints->EndID[iIndices - 1];
						MinPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];
					}					
				}
			}

			s_DrawColorShape.m_NodePt_Temp = MinPoint.dPt;
			double dValue;
			ClipFace::GetCutPointValueByeType(dValue, oCaeresults, MinPoint);
			if (d!=-1)
			{
				s_DrawColorShape.m_dNodeInfo_Temp = dValue;
			}
		}
		//����ʱ���ƵĽڵ���ƿ���
		s_DrawColorShape.m_bNodeView_Temp = true;
	}
	else
		s_DrawColorShape.m_bNodeView_Temp = false;

	m_CurNodePt = s_DrawColorShape.m_NodePt_Temp;
	m_dCurNodeInfo = s_DrawColorShape.m_dNodeInfo_Temp;

	return true;
}

void nodeViewer::_OnCleanup()
{
	//�ر���ʱ���ƵĽڵ���ƿ���
	s_DrawColorShape.m_vecNodeInfos.clear();
	s_DrawColorShape.m_vecNodePts.clear();

	s_DrawColorShape.m_bNodeView = false;
	s_DrawColorShape.m_bNodeView_Temp = false;
}

bool nodeViewer::_OnDataButton(DgnButtonEventCR ev)
{
	//���û�������ʱ����¼��ʱ�ڵ���Ϣ
	s_DrawColorShape.m_vecNodePts.push_back(m_CurNodePt);
	s_DrawColorShape.m_vecNodeInfos.push_back(m_dCurNodeInfo);

	return true;
}
void nodeViewerCmd(WCharP unparsed)
{
	//������ǰ�Ƿ���CAE��ͼͼ�㣬����������ǲ鿴��ͼ�ڵ㣬����鿴��ԭͼ��ڵ�
	DgnPlatform::LevelId levelId;
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ")
		|| SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ����")
		|| SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE������ͼ"))
		nodeViewer::InstallNewInstance(COMMAND_NODEVIEWER, true);
	else
		nodeViewer::InstallNewInstance(COMMAND_NODEVIEWER, false);
}

