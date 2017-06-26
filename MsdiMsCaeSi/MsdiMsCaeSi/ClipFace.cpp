#include "MsdiMsCaeSipch.h"

extern outRebarDlgInfo	g_outRebarDlgInfo;

DPoint3d ClipFace::minPoint;
DPoint3d ClipFace::maxPoint;
bvector<double>   ClipFace::oCaeresultsux;
bvector<double>   ClipFace::oCaeresultsuy;
bvector<double>   ClipFace::oCaeresultsuz;
bmap<UInt32, vector<vector<DPoint3d>>> ClipFace::allLinePoints;
extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

//ɸѡ�����ཻ������̺߳���
void FilterMeshThreadFunc
(
UInt32 Starti,
UInt32 Endi,
NamedGroupPtr namedGroup,
bvector<ElementId>& meshSolids,
bvector<ElementId>& nitsmeshSolids,
DPoint3d CutFacePos,
DPoint3d CutNormal
)
{
	//ɸѡ����Ϊ�����巶Χ�����8��ֵ֮������ߣ��ж���������û�н�����һ���н�˵�����������н�
	for (UInt32 i = Starti; i < Endi; i++)
	{
		ElementId elemid = namedGroup->GetMember(i)->GetElementId();
		EditElementHandle eeh;
		if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
		{
			//��ȡ����ķ�Χ����
			DPoint3d pt1;
			DPoint3d pt2;
			mdlElmdscr_computeRange(&pt1, &pt2, eeh.GetElementDescrP(), NULL);

			double Xlength = pt2.x - pt1.x;
			double Ylength = pt2.y - pt1.y;
			//�ж�pt1��pt2֮��������������Ƿ��н����н��Ļ��������ID����������������
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal))
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.x = pt1.x + Xlength;
			pt2.x = pt2.x - Xlength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Xlength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.y = pt1.y + Ylength;
			pt2.y = pt2.y - Ylength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Ylength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.x = pt1.x - Xlength;
			pt2.x = pt2.x + Xlength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Xlength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			nitsmeshSolids.push_back(elemid);
		}
	}
}

//�����߶�ID���߶�ID�Ļ�ȡ��ʽ�������������IDֵ����IDֵ�ɴ�С���������߶�ID����ID-СID��
std::string ClipFace::getLinekeys(CutPt& pt1, CutPt& pt2)
{
	string sKey;
	char tempC[150];
	if (pt1.StartID > pt2.StartID)
	{
		sprintf_s(tempC, "%d-%d", pt2.StartID, pt1.StartID);
	}
	else
	{
		sprintf_s(tempC, "%d-%d", pt1.StartID, pt2.StartID);
	}
	sKey.append(tempC);

	return sKey;
}

//����ֱ������Ľ���
bool CalPlaneLineIntersectPoint(DVec3d planeVector, DPoint3d planePoint, DVec3d lineVector, DPoint3d linePoint1, DPoint3d linePoint2, DPoint3d& returnResult)
{
	//planeVector���淨��
	//planePoint:����һ��
	//lineVector:ֱ��ʸ��
	//linePoint��ֱ����һ��
	//returnResult������õ��������ߵĽ���
	double vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t, vpt;
	vp1 = planeVector.x;
	vp2 = planeVector.y;
	vp3 = planeVector.z;
	n1 = planePoint.x;
	n2 = planePoint.y;
	n3 = planePoint.z;
	v1 = lineVector.x;
	v2 = lineVector.y;
	v3 = lineVector.z;
	m1 = linePoint1.x;
	m2 = linePoint1.y;
	m3 = linePoint1.z;
	vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;

	DVec3d lineVec1 = DVec3d::FromStartEndNormalize(planePoint, linePoint1);
	DVec3d lineVec2 = DVec3d::FromStartEndNormalize(planePoint, linePoint2);
	double theLine1 = lineVec1.DotProduct(planeVector);
	double theLine2 = lineVec2.DotProduct(planeVector);
	//�����ж�ֱ���Ƿ���ƽ��ƽ��  
	if (fabs(vpt) < Tolerance)
	{
		return false;
	}
	else if (fabs(theLine1) < 0.000001) // ��Pt1������
	{
		returnResult = linePoint1;
	}
	else if (fabs(theLine2) < 0.000001) // ��Pt2������
	{
		returnResult = linePoint2;
	}
	else // �߶��м�ĳһ��������
	{
		t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
		returnResult.x = m1 + v1 * t;
		returnResult.y = m2 + v2 * t;
		returnResult.z = m3 + v3 * t;
	}
	return true;
}

//�������ߵĵ�����ȡ��������Ľ��㣬ԭ���ǣ��������ϵ�ÿ�����ϵ������߶Σ�����ͨ���߶�IDֵ���жϴ��߶���û���жϹ���û���жϹ����ͼ�����߶���������û��
//���㣬����н�������������꣬�����棬��¼���߶�ID��ʾ���߶��Ѿ��жϹ��ˡ��ظ����Ϲ���������н��㡣
bool ClipFace::GetIntersectPointsbyfacePoints(bvector<bvector<CutPt>>& allfacePoints, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace)
{
	WString strVal;
	double theDot1 = 0;
	double theDot2 = 0;
	//��ȡ���淨���Լ�������һ�������
	DPoint3d CutFacePos;
	DPoint3d CutNormal;
	mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, CutFace.GetElementDescrP(), NULL);

	//��¼�߶�ID��MAP
	bmap<string, int> lines;
	//����ÿһ����
	for (size_t i = 0; i < allfacePoints.size(); i++)
	{
		bvector<CutPt> facepoints = allfacePoints.at(i);
		//�������ϵ��߶�
		for (int j = 0; j < facepoints.size(); j++)
		{
			CutPt pt1, pt2;
			if (j == facepoints.size() - 1)
			{
				pt1.dPt = facepoints.at(j).dPt;
				pt1.StartID = facepoints.at(j).StartID;
				pt1.EndID = facepoints.at(j).EndID;
				pt1.StartRatio = facepoints.at(j).StartRatio;
				pt2.dPt = facepoints.at(0).dPt;
				pt2.StartID = facepoints.at(0).StartID;
				pt2.EndID = facepoints.at(0).EndID;
				pt2.StartRatio = facepoints.at(0).StartRatio;
			}
			else
			{
				pt1.dPt = facepoints.at(j).dPt;
				pt1.StartID = facepoints.at(j).StartID;
				pt1.EndID = facepoints.at(j).EndID;
				pt1.StartRatio = facepoints.at(j).StartRatio;
				pt2.dPt = facepoints.at(j + 1).dPt;
				pt2.StartID = facepoints.at(j + 1).StartID;
				pt2.EndID = facepoints.at(j + 1).EndID;
				pt2.StartRatio = facepoints.at(j + 1).StartRatio;
			}
			//��ȡ�߶�ID
			std::string linekeys = getLinekeys(pt1, pt2);
			if (lines[linekeys] == 0)//���߶λ�û���жϹ�
			{
				DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt1.dPt);
				DVec3d vec2 = DVec3d::FromStartEndNormalize(CutFacePos, pt2.dPt);
				DVec3d normalVec = DVec3d::From(CutNormal);

				theDot1 = vec1.DotProduct(normalVec);
				theDot2 = vec2.DotProduct(normalVec);

				strVal.Sprintf(L"%lf", fabs(theDot1));
				if (strVal.StartsWith(L"0.00000"))//ֱ�ߵĶ˵�1��������
				{
					InterSectPoint_PushBack(InterSectPoints, pt1);
				}

				strVal.Sprintf(L"%lf", fabs(theDot2));
				if (strVal.StartsWith(L"0.00000"))//ֱ�ߵĶ˵�2��������
				{
					InterSectPoint_PushBack(InterSectPoints, pt2);
				}

				if (theDot1*theDot2 < 0)//ֱ�������н���,�ҽ���λ�ò���ֱ�ߵĶ˵��ϡ�
				{
					DVec3d linevec = DVec3d::FromStartEndNormalize(pt1.dPt, pt2.dPt);
					DPoint3d intesectPoint;
					//�����߶�����Ľ���
					if (CalPlaneLineIntersectPoint(normalVec, CutFacePos, linevec, pt1.dPt, pt2.dPt, intesectPoint))
					{
						CutPt pt3;
						pt3.dPt = intesectPoint;
						pt3.StartID = pt1.StartID;
						pt3.EndID = pt2.EndID;

						double value_dis = mdlVec_distance(&pt1.dPt, &pt2.dPt);
						double mid_dis = mdlVec_distance(&pt1.dPt, &intesectPoint);
						pt3.StartRatio = (mid_dis / value_dis);

						InterSectPoint_PushBack(InterSectPoints, pt3);
					}
				}

				lines[linekeys] += 1;
			}
		}
	}
	return true;
}

void ClipFace::InterSectPoint_PushBack(bvector<CutPt>& InterSectPoints, CutPt intesectPoint)
{
	//���vector���д˽��㣬�Ͳ������
	bool bExist = false;
	for (int k = 0; k < InterSectPoints.size(); k++)
	{
		CutPt tpt = InterSectPoints.at(k);
		double dis = mdlVec_distanceSquared(&intesectPoint.dPt, &tpt.dPt);
		if (dis <= (0.001*UOR_PER_MASTER))
			//if (dis<=0.1)
		{
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		InterSectPoints.push_back(intesectPoint);
	}
}

//��ȡһ��mesh������������е�����
bool ClipFace::GetMeshNodeIntersectwithcutfacePoints(const EditElementHandleP eh, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace)
{
	bvector<int>					pIndices;
	bvector<DPoint3d>				pXYZ;
	int								iNumIndexPerFace;
	int								iNumFace;
	bool							bRet = false;
	//��ȡ�������ϵĽڵ㼰�ڵ��ϵ�IDֵ
	bvector<bvector<CutPt>> allfacePoints;
	if (SUCCESS == mdlMesh_getPolyfaceArrays(eh->GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		XAttributeHandle xh(eh->GetElementRef(), handlerId, XAttrID_MeshProperty);
		MeshProperty *meshPro = NULL;

		if (xh.IsValid())
		{
			meshPro = (MeshProperty *)xh.PeekData();
		}

		bvector<CutPt> facepoints;
		for (size_t i = 0; i < pIndices.size(); i++)
		{
			CutPt tempPoint;
			int k = pIndices.at(i);
			if (k == 0)
			{
				allfacePoints.push_back(facepoints);
				bvector<CutPt>().swap(facepoints);
			}
			else
			{
				tempPoint.dPt = pXYZ.at(k - 1);// �ڵ�����
				// ������ͼ����û��MeshProperty��XAttribute��
				// ������ͼ����ʱxh����û�б�ʵ����
				if (xh.IsValid())
				{
					tempPoint.StartID = meshPro->ID[i];// ��ʼ�ڵ���
					tempPoint.EndID = meshPro->ID[i];// �յ����
				}
				else
				{
					// ��ͼ����ʱ,�ڵ���ֱ����Mesh����ı��
					tempPoint.StartID = k;// ��ʼ�ڵ���
					tempPoint.EndID = k;// �յ����
				}
				tempPoint.StartRatio = 0.5;//������ʼ�����(��ʼΪ0.5)
				facepoints.push_back(tempPoint);
			}
		}

		//�����ϵĵ�õ�����
		GetIntersectPointsbyfacePoints(allfacePoints, InterSectPoints, CutFace);
	}
	return false;
}

//��������������ʵ��
void ClipFace::CutSolidsByFace(EditElementHandle& CutFace)
{
	//�����֮ǰ�洢��ID
	bvector<ElementId>().swap(meshSolids);
	DgnPlatform::LevelId levelId;
	// ������ȡ��CAE��ͼ��ͼ��
	if (SUCCESS != mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ"))//������ͼģʽ
	{
		//��ȡ����������û�н���ʵ��ID
		bvector<ElementId> NitsmeshSolids;
		//ɸѡ��ʵ���ཻ����ʵ��
		FliterMeshSolids(CutFace.GetElementDescrP(), meshSolids, NitsmeshSolids, s_DrawColorShape.NgName.GetWCharCP());
		//��ȡ����������������ʵ���ཻ��MESH��
		GetAllCutResultFace(CutFace);
		bvector<ElementId>().swap(NitsmeshSolids);
	}
}

bool ClipFace::_OnDataButton(DgnButtonEventCR ev)
{
	if (g_outRebarDlgInfo.chooseBtnIndex < 3)
	{
		CreateClipFace(ev, false);
		m_points.clear();
	}
	else
	{
		m_points.push_back(*ev.GetPoint());
		if (m_points.size() < 2)
		{
			return false;
		}
		else
		{
			CreateClipFace(ev, false);
			m_points.clear();
		}
	}

	return true; // Tool should exit after creating a single line if started in single-shot mode.
}
bool ClipFace::IsClipFaceModel()//�жϵ�ǰ�Ƿ�������model��
{
	WChar             name[MAX_MODEL_NAME_LENGTH];
	mdlModelRef_getModelName(ACTIVEMODEL, name);
	WString modelName;
	modelName.append(name);
	if (splitNgName_Clip(modelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//����ֱ��Ԫ��
bool ClipFace::CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points)
{
	if (2 != points.size())
		return false;

	// NOTE: Easier to always work with CurveVector/CurvePrimitive and not worry about element specific create methods, ex. LineHandler::CreateLineElement.
	if (SUCCESS != DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLine(DSegment3d::From(points.front(), points.back())), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
		return false;

	ElementPropertyUtils::ApplyActiveSettings(eeh);

	return true;
}

//��̬��ʾ������������
void ClipFace::_OnDynamicFrame(DgnButtonEventCR ev)
{
	CreateClipFace(ev, true);
}

//��ȡ����������������ʵ���ཻ��MESH��
void ClipFace::GetAllCutResultFace(EditElementHandle& CutFace)
{
	//��������������ȡ��Ӧ����ʾ�������ز���
	LevelId lvID_show = 0;
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, s_pDCShape->NgName.data());

	DgnModelRefP modelRef3d;

	NamedGroupFlags ngfCAESolid;
	NamedGroupPtr ngCAESolid;
	ngfCAESolid.m_selectMembers = 0;// ��ʾ�Զ���ѡ
	ngfCAESolid.m_anonymous = 1;// �������鲻�����������б�����ʾ(�����ص���ʱ������)

	//����CAEģ�͵�ngName�ͽ��������ɶ�Ӧ�Ľ���model��
	int proID, indexID;
	splitNgName_Model(s_DrawColorShape.NgName, UNUSE_WSTRING, proID, indexID, UNUSE_WSTRING, UNUSE_WSTRING);
	WString clipNgName = makeNgName_Clip(proID, indexID, g_outRebarDlgInfo.clipName);
	DgnPlatform::LevelId  levelId_CutfaceModel;

	MakeDgnModel(modelRef3d, clipNgName);

	// ������ȡ��CAEģ�͵���ʾͼ��
	if (SUCCESS != mdlLevel_create(&levelId_CutfaceModel, modelRef3d, clipNgName.c_str(), LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_CutfaceModel, modelRef3d, LEVEL_NULL_ID, clipNgName.c_str());
	// ����������
	if (NG_Success == NamedGroup::Create(ngCAESolid, clipNgName.data(), L"CAEģ����", ngfCAESolid, modelRef3d))
	{
		Transform	tMatrix;
		size_t Index;
		Index = 0;
		DPoint3d PlacePosPoint;
		DPoint3d FaceNormal;
		mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, CutFace.GetElementDescrP(), NULL);
		{//�����ת����,���Խ��������ɵ�����ת��XYƽ����

			//��ȡ�����ϵ�һ�����Լ�����ķ���
			DPoint3d PlacePosPoint;
			DPoint3d FaceNormal;
			mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, CutFace.GetElementDescrP(), NULL);
			DVec3d vec1, vec2, vec3;
			vec1.x = vec1.y = 0.0;
			vec1.z = 1.0;

			//XZƽ��ķ���
			vec2.x = 0.0;
			vec2.y = 1.0;
			vec2.z = 0.0;

			//XZƽ��ķ��򷴷���
			vec3.x = 0.0;
			vec3.y = -1.0;
			vec3.z = 0.0;

			double dot2 = FaceNormal.DotProduct(vec1);
			double dot3 = FaceNormal.DotProduct(vec3);
			if (dot3 > dot2)
			{
				vec2.y = -1.0;
				vec1.z = 1.0;
			}
			else
			{
				vec2.y = 1.0;
				vec1.z = -1.0;
			}

			RotMatrix  rMatrix, rMatrix1, rMatrix2;
			//��ƽ����ת��XZƽ��
			mdlRMatrix_fromVectorToVector(&rMatrix1, &FaceNormal, &vec2);
			//��XZƽ����ת��xyƽ��
			mdlRMatrix_fromVectorToVector(&rMatrix2, &vec2, &vec1);
			mdlRMatrix_multiply(&rMatrix, &rMatrix2, &rMatrix1);

			mdlTMatrix_fromRMatrix(&tMatrix, &rMatrix);
			mdlTMatrix_setOrigin(&tMatrix, &PlacePosPoint);
		}
		FaceNormal.Normalize();

		DVec3d lineFromX = DVec3d::FromStartEnd(prjstrPoint, prjendPoint);
		lineFromX.Normalize();
		for (size_t i = 0; i < meshSolids.size(); i++)//ÿһ��mesh�����
		{
			ElementId elemid = meshSolids.at(i);
			EditElementHandle eeh;
			bvector<CutPt> CutFacePoints;
			if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
			{
				LevelId elemlv = eeh.GetElementDescrP()->el.ehdr.level;
				//����Ԫ�طŵ���ʾ����
				if (elemlv != lvID_show)
				{
					continue;
				}

				//��ȡ������ʵ������н��㣬���㱣����InterSectPoints��
				GetMeshNodeIntersectwithcutfacePoints(&eeh, CutFacePoints, CutFace);
				if (CutFacePoints.size() < 3)
				{
					continue;
				}

				// �õ�������Ľ���󣬶Խ�����������������������MESH�档
				// ���򷽷��������ĵ��뽻��֮��ķ���ʸ��֮��ļнǴ�С������
				Dpoint3d midPos;
				midPos.x = midPos.y = midPos.z = 0;
				CutPt cutpt;
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					cutpt = CutFacePoints.at(i);
					midPos.x += cutpt.dPt.x;
					midPos.y += cutpt.dPt.y;
					midPos.z += cutpt.dPt.z;
				}
				midPos.x = midPos.x / CutFacePoints.size();
				midPos.y = midPos.y / CutFacePoints.size();
				midPos.z = midPos.z / CutFacePoints.size();

				CutPt tempLineX;
				tempLineX.dPt.x = midPos.x + 100 * lineFromX.x*UOR_PER_UNIT(m_Unit);
				tempLineX.dPt.y = midPos.y + 100 * lineFromX.y*UOR_PER_UNIT(m_Unit);
				tempLineX.dPt.z = midPos.z + 100 * lineFromX.z*UOR_PER_UNIT(m_Unit);
				CutFacePoints.insert(CutFacePoints.begin(), tempLineX);
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					DVec3d vec1 = DVec3d::FromStartEndNormalize(midPos, CutFacePoints.at(i).dPt);
					double theDot = -1;
					for (int j = i + 1; j < CutFacePoints.size(); j++)
					{
						DVec3d vec2 = DVec3d::FromStartEndNormalize(midPos, CutFacePoints.at(j).dPt);
						DVec3d tempNormal = DVec3d::FromCrossProduct(vec1, vec2);
						tempNormal.Normalize();
						double tempDot = tempNormal.DotProduct(FaceNormal);
						if (tempDot > 0.99)
						{
							double theDot2 = vec1.DotProduct(vec2);
							if (theDot2 > theDot)
							{
								CutPt tempPoint = CutFacePoints.at(j);
								CutFacePoints.at(j) = CutFacePoints.at(i + 1);
								CutFacePoints.at(i + 1) = tempPoint;
								theDot = theDot2;
							}
						}

					}
				}
				CutFacePoints.erase(CutFacePoints.begin());

				//���������Ϣ�����ڽṹ����
				CutPoints cutpts;
				memset(&cutpts, 0, sizeof(CutPoints));
				cutpts.Num = (int)CutFacePoints.size();
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					cutpts.StartID[i] = CutFacePoints.at(i).StartID;
					cutpts.EndID[i] = CutFacePoints.at(i).EndID;
					cutpts.StartRatio[i] = CutFacePoints.at(i).StartRatio;
				}

#pragma region ����һ��model������Mesh,�����������д�뵽mesh��	
				MSElementDescrP m_resultEdp = NULL;//mesh��ָ��
				MSElementDescrP m_resultEdp2 = NULL;//mesh��ָ��
				bvector<int> vecIndices; // Mesh��Ľڵ㼯��
				bvector<DPoint3d> vecPoints; // Mesh��Ķ������꼯��
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					vecIndices.push_back((int)(CutFacePoints.size() - i));
				}
				vecIndices.push_back(0);
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					vecPoints.push_back(CutFacePoints.at(i).dPt);
				}

				// ����CAE��Mesh��
				if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
				{
					// ����Mesh���ElementHandle
					EditElementHandle tmpeeh(m_resultEdp, true, true, modelRef3d);

					//��תmesh��
					TransformInfo tInfo(tMatrix);
					tmpeeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(tmpeeh, tInfo);

					// Ϊ��������ת��mesh��ڵ��Z���겻��һ��ƽ�������
					// ����ȡ�����нڵ�����꣬��z��Ϊ0.0
					bvector<int>		pIndices;
					bvector<DPoint3d>	pXYZ;
					if (SUCCESS == mdlMesh_getPolyfaceArrays(tmpeeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
					{
						bvector<DPoint3d>::iterator iterNow = pXYZ.begin();
						bvector<DPoint3d>::iterator iterend = pXYZ.end();
						// �����нڵ������z��Ϊ0.0
						for (; iterNow != iterend; iterNow++)
						{
							iterNow->z = 0.0;
						}

						// �����������յĽ���Mesh
						if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp2, NULL, pIndices.data(), (int)pIndices.size(), pXYZ.data(), (int)pXYZ.size()))
						{
							// ����Mesh���ElementHandle
							EditElementHandle tempeeh(m_resultEdp2, true, true, modelRef3d);

							// ����Mesh�����Ӧ��CAE����ڵ㼯��
							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

							// ��Mesh�����Ӧ��CAE����ڵ㼯�ϱ��浽ElementHandle��
							tempeeh.ScheduleWriteXAttribute(handlerId, XAttrID_CutFace, sizeof(CutPoints), &cutpts);

							//ת�����󱣴浽xattribute��
							XAttributeHandlerId handlerMatrixId(XAttrHandleID_Matrix, XAttrHandleID_Matrix);
							// ��Mesh�����Ӧ��ת�����󱣴浽ElementHandle��
							tempeeh.ScheduleWriteXAttribute(handlerMatrixId, XAttrID_Matirx, sizeof(Bentley::Transform), &tMatrix);

							// ElementHandle��ӵ���ͼ��
							ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
							propsSetter->SetLevel(levelId_CutfaceModel);
							propsSetter->Apply(tempeeh);

							// ElementHandle��ӵ���ͼ��
							tempeeh.AddToModel();

							// ͨ��Ԫ��ID����������Ԫ��
							ngCAESolid->AddMember(tempeeh.GetElementP()->GetID(), modelRef3d, NamedGroupMemberFlags());
						}
					}
				}
#pragma endregion
			}
		}

		ngCAESolid->WriteToFile(true);
		mdlModelRef_activateAndDisplay(modelRef3d);//�����ʾ������Model
		//�������ʾ�⻬ģʽ
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);
		MsdiMsCaeSiUtility::fitView(0);
	}
	mdlLevelTable_rewrite(modelRef3d);
	mdlModelRef_freeWorking(modelRef3d);

	//DPoint3d GrpTextPos;//����������
	//GrpTextPos = maxPoint;
	//GrpTextPos.x = GrpTextPos.x - (maxPoint.x - minPoint.x)/2;
	//GrpTextPos.y = GrpTextPos.y - 1 * UOR_PER_UNIT(m_Unit);
	//CreateText(GrpTextPos, UOR_PER_UNIT(m_Unit) / 2, UOR_PER_UNIT(m_Unit) / 2, clipName);
}

//��MESH�е�XAttribute��ȡ���ݣ���Ҫ��������ֵ���ʹ�ã�
void ClipFace::GetMeshPointsAndValueForOutDatalist(bvector<PointOutData>& MeshPoints, const bmap<WString, bvector<double>>& allNineDatas, EditElementHandleCR eeh)
{
	bvector<int>					pIndices;
	bvector<DPoint3d>				pXYZ;

	if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_CutFace);
		CutPoints *cutPoints = NULL;

		if (xh.IsValid())
		{
			cutPoints = (CutPoints *)xh.PeekData();
		}
		else
			return;

		for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
		{
			int iIndices = pIndices.at(iIdx);
			if (iIndices != 0)
			{
				CutPt cutPoint;
				cutPoint.dPt = pXYZ.at(iIndices - 1);
				cutPoint.StartID = cutPoints->StartID[iIndices - 1];
				cutPoint.EndID = cutPoints->EndID[iIndices - 1];
				cutPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];

				PointOutData tempPoint;
				tempPoint.dPt = pXYZ.at(iIndices - 1);
				double dVal = 0.0;
				for (auto iter = allNineDatas.begin(); iter != allNineDatas.end(); iter++)
				{
					dVal = 0.0;
					GetCutPointValueByeType(dVal, iter->second, cutPoint);
					tempPoint.ResultVals[iter->first] = dVal;
				}

				MeshPoints.push_back(tempPoint);
			}
		}
	}
	if (!MeshPoints.empty())
		MeshPoints.push_back(MeshPoints.at(0));
}

void ClipFace::GetMeshPointsAndValue(bvector<ContourPoint>& MeshPoints, EditElementHandle& eeh, bvector<double>& oCaeresults)
{
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
		else
			return;

		for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
		{
			int iIndices = pIndices.at(iIdx);
			if (iIndices != 0)
			{
				CutPt cutPoint;
				cutPoint.dPt = pXYZ.at(iIndices - 1);
				cutPoint.StartID = cutPoints->StartID[iIndices - 1];
				cutPoint.EndID = cutPoints->EndID[iIndices - 1];
				cutPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];

				ContourPoint tempPoint;
				tempPoint.dPt = pXYZ.at(iIndices - 1);
				GetCutPointValueByeType(tempPoint.dVal, oCaeresults, cutPoint);
				//tempPoint.dVectorPt = cutPoints->vecTorPoints[iIndices - 1];
				MeshPoints.push_back(tempPoint);

			}

		}
	}
	if (!MeshPoints.empty())
		MeshPoints.push_back(MeshPoints.at(0));
}
void ClipFace::MakeColorLine(DSegment3dCR segment, UInt32 color, DgnPlatform::LevelId levelId)
{
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	UInt32 newColor = (color == 256) ? 0 : color;
	propsSetter->SetColor(newColor);
	propsSetter->SetLevel(levelId);

	EditElementHandle eeh;
	if (SUCCESS == LineHandler::CreateLineElement(eeh, NULL, segment, true, *MASTERFILE))
	{
		propsSetter->SetLocked(eeh, true);
		propsSetter->Apply(eeh);
		eeh.AddToModel();
	}
}
void ClipFace::DrawFaceVector(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
	DPoint3d thePtmin;
	DPoint3d thePtmax;
	thePtmin = CutFacePoints.at(0).dPt;
	thePtmax = CutFacePoints.at(2).dPt;
	double theDistance = mdlVec_distanceXY(&thePtmin, &thePtmax);

	//ѭ����������������нڵ�֮��ĵ�ֵ��
	for (int i = 0; i < CutFacePoints.size(); i++)
	{
		CutPt oCp = CutFacePoints.at(i);
		DPoint3d dPt = oCp.dPt;
		DPoint3d dVecPt;
		GetCutPointValueByeType(dVecPt.x, oCaeresultsux, oCp);
		GetCutPointValueByeType(dVecPt.y, oCaeresultsuy, oCp);
		GetCutPointValueByeType(dVecPt.z, oCaeresultsuz, oCp);

		double dVecterLen = -1;// ʸ����Գ���
		int iClrNum = 1;
		double Lenth = 1;// ʸ���߳���
		DPoint3d tempPont;
		tempPont.x = 0;
		tempPont.y = 0;
		tempPont.z = 0;
		if (dVecPt.x == 0 && dVecPt.y == 0 && dVecPt.z == 0)
		{
			return;
		}

		double tValue;
		GetCutPointValueByeType(tValue, oCaeresults, oCp);
		UInt32 theColor = GetTheColor(tValue, iClrNum);

		DPoint3d pts[2];
		pts[0] = dPt;
		pts[1] = pts[0];
		double tempLenth;
		tempLenth = iClrNum*theDistance / 8;
		Lenth = (abs(dVecPt.x) > abs(dVecPt.y)) ? abs(dVecPt.x) : abs(dVecPt.y);
		Lenth = (Lenth > abs(dVecPt.z)) ? Lenth : abs(dVecPt.z);
		Lenth = tempLenth / Lenth;

		DSegment3d segment;
		segment.SetStartPoint(pts[0]);
		segment.SetEndPoint(pts[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		DPoint3d tempPoints[2];
		tempPoints[1] = pts[1];
		tempPoints[0].x = pts[0].x + (pts[1].x - pts[0].x) * 9 / 10;
		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);

		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);
	}

}

void ClipFace::DrawFaceLine(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
	bvector<ContourPoint> contourlist;
	bvector<ContourPoint> pMeshFace;
	// ѭ����������������нڵ�֮��ĵ�ֵ��
	for (int i = 0; i < CutFacePoints.size() - 1; i++)
	{
		CutPt oCpStart = CutFacePoints.at(i);
		CutPt oCpEnd = CutFacePoints.at(i + 1);

		ContourPoint oStart;
		ContourPoint oEnd;
		ContourPoint oCpTemp;
		ContourPoint TcpStart;


		TcpStart.dPt = oCpStart.dPt;
		GetCutPointValueByeType(TcpStart.dVal, oCaeresults, oCpStart);

		oStart.dVal = TcpStart.dVal;
		oStart.dPt = TcpStart.dPt;
		oEnd.dPt = oCpEnd.dPt;
		GetCutPointValueByeType(oEnd.dVal, oCaeresults, oCpEnd);
		contourlist.push_back(TcpStart);

		if (oStart.dVal <= oEnd.dVal)
		{
			for (int iVal = 0; iVal <= s_DrawColorShape.Color_num; ++iVal)
			{
				// ����ڵ�֮��ĵ�ֵ��
				if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
				{
					contourlist.push_back(oCpTemp);
				}
			}
		}
		else
		{
			for (int iVal = s_DrawColorShape.Color_num; iVal >= 0; --iVal)
			{
				// ����ڵ�֮��ĵ�ֵ��
				if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
				{
					contourlist.push_back(oCpTemp);
				}
			}
		}
	}

	// ������ͼ����ɫ������Ϣ�����ɵ�ֵ��
	for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
	{
		double dContourVal = s_DrawColorShape.ClrShapeVal[iVal];

		int iContourPtNow = 0;
		int iContourPtEnd = (int)contourlist.size() - 1;
		ContourPoint ContourPt;
		ContourPoint ContourPtEnd;

		// ���ݾ������ϵĵ�ֵ�㣬���ɸ��������ͼ
		for (; iContourPtNow <= iContourPtEnd; iContourPtNow++)
		{
			DSegment3d segment;
			ContourPt = contourlist.at(iContourPtNow);

			segment.SetStartPoint(ContourPt.dPt);

			if (abs(dContourVal - ContourPt.dVal) < 0.0000001)
			{
				for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
				{
					ContourPtEnd = contourlist.at(iContourPtEnd);
					if (abs(dContourVal - ContourPtEnd.dVal) < 0.0000001)
					{
						segment.SetEndPoint(ContourPtEnd.dPt);

						//��¼��ֵ�ߵ��������������ֵ����ֵ
						vector<DPoint3d> tempLinePoints;
						tempLinePoints.push_back(ContourPt.dPt);
						tempLinePoints.push_back(ContourPtEnd.dPt);
						allLinePoints[s_DrawColorShape.fillCororIndex[iVal]].push_back(tempLinePoints);

						break;
					}
				}
			}
		}
	}

}
void ClipFace::_OnPostInstall()
{
	//��ȡbasePt����ʾ��ID
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(m_basePt, m_Unit, levelId_CaeModel, s_pDCShape->NgName);

	getMaxMinXYZPtValOfProject(s_pDCShape->ProjectId, m_minx, m_maxx, m_miny, m_maxy, m_minz, m_maxz);

	m_minx = m_minx*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.x;
	m_maxx = m_maxx*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.x;

	m_miny = m_miny*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.y;
	m_maxy = m_maxy*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.y;

	m_minz = m_minz*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.z;
	m_maxz = m_maxz*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.z;

	AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.
	_BeginDynamics();
	__super::_OnPostInstall();
}

//��ȡ�е��Valueֵ��ͨ��type����
void ClipFace::GetCutPointValueByeType(double& cutValue, const bvector<double>& oCaeresults, CutPt cutpt)
{
	int startID = cutpt.StartID;
	int endID = cutpt.EndID;
	double startRatio = cutpt.StartRatio;
	double startValue, endValue;
	startValue = 0;
	endValue = 0;
	if (oCaeresults.size() < 1)
	{
		return;
	}
	startValue = oCaeresults.at(startID - 1);
	endValue = oCaeresults.at(endID - 1);
	cutValue = startValue + startRatio*(endValue - startValue);
}

void ClipFace::DrawFacePlot(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
	bvector<ContourPoint> contourlist;
	bvector<ContourPoint> pMeshFace;
	// ѭ����������������нڵ�֮��ĵ�ֵ��
	for (int i = 0; i < CutFacePoints.size() - 1; i++)
	{
		CutPt oCpStart = CutFacePoints.at(i);
		CutPt oCpEnd = CutFacePoints.at(i + 1);

		ContourPoint oStart;
		ContourPoint oEnd;
		ContourPoint oCpTemp;
		ContourPoint TcpStart;

		TcpStart.dPt = oCpStart.dPt;
		GetCutPointValueByeType(TcpStart.dVal, oCaeresults, oCpStart);

		oStart.dVal = TcpStart.dVal;
		oStart.dPt = TcpStart.dPt;
		oEnd.dPt = oCpEnd.dPt;
		GetCutPointValueByeType(oEnd.dVal, oCaeresults, oCpEnd);
		contourlist.push_back(TcpStart);

		if (oStart.dVal <= oEnd.dVal)
		{
			for (int iVal = 0; iVal <= s_DrawColorShape.Color_num; ++iVal)
			{
				// ����ڵ�֮��ĵ�ֵ��
				if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
				{
					contourlist.push_back(oCpTemp);
				}
			}
		}
		else
		{
			for (int iVal = s_DrawColorShape.Color_num; iVal >= 0; --iVal)
			{
				// ����ڵ�֮��ĵ�ֵ��
				if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
				{
					contourlist.push_back(oCpTemp);
				}
			}
		}
	}

	// ������ͼ����ɫ������Ϣ��������ͼ
	for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
	{
		double dValStt = s_DrawColorShape.ClrShapeVal[iVal];
		double dValEnd = s_DrawColorShape.ClrShapeVal[iVal + 1];

		int iContourPtNow = 0;
		int iContourPtEnd = (int)contourlist.size() - 1;
		ContourPoint ContourPt;
		ContourPoint ContourPtNxt;
		ContourPoint ContourPtEnd;

		// ���ݾ������ϵĵ�ֵ�㣬���ɸ��������ͼ
		for (; iContourPtNow <= iContourPtEnd; iContourPtNow++)
		{
			ContourPt = contourlist.at(iContourPtNow);
			if (dValStt <= ContourPt.dVal && ContourPt.dVal <= dValEnd)
			{
				if (pMeshFace.size() == 0 && ContourPt.dVal < dValEnd)
				{
					for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
					{
						ContourPtEnd = contourlist.at(iContourPtEnd);
						if (dValStt <= ContourPtEnd.dVal && ContourPtEnd.dVal < dValEnd)
						{
							pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
						}
						else if (ContourPtEnd.dVal >= dValEnd)
						{
							pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
							break;
						}
					}
					pMeshFace.push_back(ContourPt);
				}
				else if (ContourPt.dVal == dValEnd)
				{
					if (pMeshFace.size() <= 1)
					{
						pMeshFace.clear();
						pMeshFace.push_back(ContourPt);
					}
					else
					{
						pMeshFace.push_back(ContourPt);

						ContourPtNxt = ContourPt;
						if (iContourPtNow + 1 <= iContourPtEnd)
						{
							ContourPtNxt = contourlist.at(iContourPtNow + 1);
						}

						if (ContourPtNxt.dVal != dValStt)
						{
							// ������ͼ������
							ClipFace::MakeColorMesh(pMeshFace, s_DrawColorShape.fillCororIndex[iVal], levelId_ContourPlot);
							pMeshFace.clear();

							iContourPtNow--;
						}
					}
				}
				else
				{
					pMeshFace.push_back(ContourPt);
				}
			}
		}

		// ������ͼ������
		ClipFace::MakeColorMesh(pMeshFace, s_DrawColorShape.fillCororIndex[iVal], levelId_ContourPlot);
		pMeshFace.clear();
	}
}

void ClipFace::ShowByType(WString PlotName, void(*Showtype)(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults), DgnPlatform::LevelId levelId, int typeID, WString typeName, WString sUnit)
{
	EditElementHandle eh;
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr namedGroup;
	bvector<double>   oCaeresults;
	GetCaeResult(oCaeresults, s_DrawColorShape.ProjectId, typeID, typeName);
	bvector<double>().swap(oCaeresultsux);
	bvector<double>().swap(oCaeresultsuy);
	bvector<double>().swap(oCaeresultsuz);
	GetCaeResult(oCaeresultsux, s_DrawColorShape.ProjectId, typeID, L"UX");
	GetCaeResult(oCaeresultsuy, s_DrawColorShape.ProjectId, typeID, L"UY");
	GetCaeResult(oCaeresultsuz, s_DrawColorShape.ProjectId, typeID, L"UZ");

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);

	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		if (namedGroup.IsValid())
		{
			bmap<UInt32, vector<vector<DPoint3d>>>().swap(allLinePoints);
			WString gName = namedGroup->GetName();
			if (gName.find(NGNAME_PREFIX_CAECLIP) != WString::npos)
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
									tempPoint.dPt = pXYZ.at(iIndices - 1);
									tempPoint.StartID = cutPoints->StartID[iIndices - 1];
									tempPoint.EndID = cutPoints->EndID[iIndices - 1];
									tempPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];
									CutFacePoints.push_back(tempPoint);
								}

							}
						}
						if (CutFacePoints.size()>0)
							CutFacePoints.push_back(CutFacePoints.at(0));
						Showtype(CutFacePoints, levelId, typeID, typeName, oCaeresults);
					}
				}
				DgnPlatform::LevelId levelId_CutFaceModel;
				WChar             name[MAX_MODEL_NAME_LENGTH];
				mdlModelRef_getModelName(ACTIVEMODEL, name);
				WString modelName;
				modelName.append(name);
				mdlLevel_getIdFromName(&levelId_CutFaceModel, MASTERFILE, LEVEL_NULL_ID, modelName.c_str());
				mdlLevel_setDisplay(MASTERFILE, levelId_CutFaceModel, false);
			}
			//����ֵ�ߺ������ֵ����ֵ
			if (Showtype == ClipFace::DrawFaceLine)
			{
				//С����λ��
				int idec = 4;
				WString dec;
				dec.Sprintf(L"%%0.%dg", idec);
				//��ֵ����ֵ���ִ�С
				double valueSize = 0.5 * UOR_PER_UNIT(sUnit);
				//��ֵ����ֵ��������
				double dis = 15 * UOR_PER_UNIT(sUnit);
				// ������ͼ����ɫ������Ϣ�����ɵ�ֵ��
				for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
				{
					UInt32 theColor = s_DrawColorShape.fillCororIndex[iVal];
					double dContourVal = s_DrawColorShape.ClrShapeVal[iVal];
					vector <vector<DPoint3d>> colorLinePoints = allLinePoints[theColor];
					vector<deque<DPoint3d>> result = connectLineString(colorLinePoints);

					ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
					UInt32 newColor = (theColor == 256) ? 0 : theColor;
					propsSetter->SetColor(newColor);
					propsSetter->SetLevel(levelId);

					WChar buffer[20];
					unsigned int  old_exponent_format = _set_output_format(_TWO_DIGIT_EXPONENT);
					_swprintf(buffer, dec.GetWCharCP(), dContourVal);
					_set_output_format(old_exponent_format);
					WString wValue;
					wValue.append(buffer);
					for (int i = 0; i < result.size(); i++)
					{
						deque<DPoint3d> linePoints = result.at(i);
						if (linePoints.size() < 1)
						{
							continue;
						}
						EditElementHandle eeh;
						bvector<DPoint3d> bLinePoints;
						bLinePoints.insert(bLinePoints.end(), linePoints.begin(), linePoints.end());
						DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLineString(bLinePoints)
							, NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
						propsSetter->Apply(eeh);
						eeh.AddToModel();
						bvector<DPoint3d> lPoints;
						bvector<DPoint3d> tangPoints;
						GetDisPointsandTange(lPoints, tangPoints, eeh.GetElementDescrP(), dis);
						for (int j = 0; j < lPoints.size(); j++)
						{
							DPoint3d lpoint = lPoints.at(j);
							DPoint3d tpoint = tangPoints.at(j);
							DPoint3d tpt;
							if (tpoint.x > 0)
							{
								tpt = DPoint3d::From(1.0, 0.0, 0.0);
							}
							else
							{
								tpt = DPoint3d::From(-1.0, 0.0, 0.0);
							}

							RotMatrix rotM;
							mdlRMatrix_fromVectorToVector(&rotM, &tpt, &tpoint);

							CreateLineText(lpoint, valueSize, valueSize, wValue, levelId, ACTIVEMODEL, rotM);
						}
					}
				}
			}

		}

	}
}

void ClipFace::MakeDgnModel(DgnModelRefP& modelRef3d, WString modelName)
{

	//����model
	if (mdlDgnFileObj_findModelIDByName(mdlDgnFileObj_getMasterFile(), NULL, modelName.data()) != SUCCESS)
	{
		mdlDgnFileObj_createModel(&modelRef3d, mdlDgnFileObj_getMasterFile(), ACTIVEMODEL, modelName.data(), modelName.data(), NULL, NULL);
	}
	else
	{
		mdlModelRef_createWorkingByName(&modelRef3d, mdlDgnFileObj_getMasterFile(), modelName.data(), TRUE, TRUE);
	}
}
void ClipFace::MakeColorMesh(bvector<ContourPoint> pMeshFace, UInt32 color, DgnPlatform::LevelId levelId)
{
	MeshSurfaceValue meshValues;
	if (pMeshFace.size() < 3) return;

	MSElementDescrP     m_resultEdp;
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	UInt32 newColor = (color == 256) ? 0 : color;
	propsSetter->SetColor(newColor);
	propsSetter->SetFillColor(newColor);
	propsSetter->SetLevel(levelId);

	bvector<int> vecIndices;
	bvector<DPoint3d> vecPoints;
	int nodesCount = 0;
	bvector<ContourPoint>::iterator iterNow = pMeshFace.begin();
	bvector<ContourPoint>::iterator iterend = pMeshFace.end();
	for (; iterNow != iterend; iterNow++)
	{
		vecPoints.push_back((*iterNow).dPt);
		vecIndices.push_back((int)vecPoints.size());
		meshValues.NodeValue[nodesCount++] = (*iterNow).dVal;
	}
	vecIndices.push_back(0);
	meshValues.nodeCount = nodesCount;
	if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		EditElementHandle eeh(m_resultEdp, true, true, ACTIVEMODEL);
		// ��Mesh�����Ӧ��CAE����ڵ㼯�ϱ��浽ElementHandle��
		eeh.ScheduleWriteXAttribute(handlerId, XAttrID_PlotMeshProperty, sizeof(MeshSurfaceValue), &meshValues);
		propsSetter->SetLocked(eeh, true);
		propsSetter->Apply(eeh);

		eeh.AddToModel();
	}
	return;
}

//�ж�ֱ���Ƿ��н����жϷ�����ֱ�����˵��Ƿ������ͬһ��
bool ClipFace::IsinFacesSameSide(DPoint3d CutFacePos, DPoint3d pt1, DPoint3d pt2, DPoint3d CutNormal)
{
	//�������һ�����PT1�ķ���ʸ��1
	DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt1);
	//�������һ�����PT2�ķ���ʸ��2
	DVec3d vec2 = DVec3d::FromStartEndNormalize(CutFacePos, pt2);
	//�õ���ķ���
	DVec3d normalVec = DVec3d::From(CutNormal);
	double theDot1 = 0;
	double theDot2 = 0;
	//����ʸ��1�뷨��ĵ��
	theDot1 = vec1.DotProduct(normalVec);
	//����ʸ��2�뷨��ĵ��
	theDot2 = vec2.DotProduct(normalVec);
	//�������ʸ��1�뷨��ĵ���뷽��ʸ��2�뷨��ĵ�˵ĳ˻�Ϊ��ֵ��˵��ֱ�������н�
	if (theDot1*theDot2 < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ClipFace::FliterMeshSolids//ɸѡʵ��
(
MSElementDescr* CutFace, bvector<ElementId>& meshsolids, bvector<ElementId>& nitsmeshsolids, WCharCP groupName
)
{
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr namedGroup;
	WString uniqueName;

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	namedGroup = ngcCAESolid->FindByName(groupName);


	if (namedGroup.IsValid())
	{
		DPoint3d CutFacePos;
		DPoint3d CutNormal;
		mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, CutFace, NULL);

		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		bvector<ElementId> meshSolids1;
		bvector<ElementId> meshSolids2;
		bvector<ElementId> meshSolids3;
		bvector<ElementId> meshSolids4;
		bvector<ElementId> meshSolids5;
		bvector<ElementId> meshSolids6;
		bvector<ElementId> meshSolids7;
		bvector<ElementId> meshSolids8;

		bvector<ElementId> NItsmeshSolids1;
		bvector<ElementId> NItsmeshSolids2;
		bvector<ElementId> NItsmeshSolids3;
		bvector<ElementId> NItsmeshSolids4;
		bvector<ElementId> NItsmeshSolids5;
		bvector<ElementId> NItsmeshSolids6;
		bvector<ElementId> NItsmeshSolids7;
		bvector<ElementId> NItsmeshSolids8;


		// �߳�1(������Ϊ��Mesh������1/8)
		std::thread t1(FilterMeshThreadFunc, 0, graphicMembers / 8, namedGroup, ref(meshSolids1), ref(NItsmeshSolids1), CutFacePos, CutNormal);
		// �߳�2(������Ϊ��Mesh������1/8)
		std::thread t2(FilterMeshThreadFunc, graphicMembers / 8, graphicMembers * 2 / 8, namedGroup, ref(meshSolids2), ref(NItsmeshSolids2), CutFacePos, CutNormal);
		// �߳�3(������Ϊ��Mesh������1/8)
		std::thread t3(FilterMeshThreadFunc, graphicMembers * 2 / 8, graphicMembers * 3 / 8, namedGroup, ref(meshSolids3), ref(NItsmeshSolids3), CutFacePos, CutNormal);
		// �߳�4(������Ϊ��Mesh������1/8)
		std::thread t4(FilterMeshThreadFunc, graphicMembers * 3 / 8, graphicMembers * 4 / 8, namedGroup, ref(meshSolids4), ref(NItsmeshSolids4), CutFacePos, CutNormal);
		// �߳�5(������Ϊ��Mesh������1/8)
		std::thread t5(FilterMeshThreadFunc, graphicMembers * 4 / 8, graphicMembers * 5 / 8, namedGroup, ref(meshSolids5), ref(NItsmeshSolids5), CutFacePos, CutNormal);
		// �߳�6(������Ϊ��Mesh������1/8)
		std::thread t6(FilterMeshThreadFunc, graphicMembers * 5 / 8, graphicMembers * 6 / 8, namedGroup, ref(meshSolids6), ref(NItsmeshSolids6), CutFacePos, CutNormal);
		// �߳�7(������Ϊ��Mesh������1/8)
		std::thread t7(FilterMeshThreadFunc, graphicMembers * 6 / 8, graphicMembers * 7 / 8, namedGroup, ref(meshSolids7), ref(NItsmeshSolids7), CutFacePos, CutNormal);
		// �߳�8(������Ϊ��Mesh������1/8)
		std::thread t8(FilterMeshThreadFunc, graphicMembers * 7 / 8, graphicMembers, namedGroup, ref(meshSolids8), ref(NItsmeshSolids8), CutFacePos, CutNormal);

		// �����߳�
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		t7.join();
		t8.join();

		meshsolids.insert(meshsolids.end(), meshSolids1.begin(), meshSolids1.end());
		meshsolids.insert(meshsolids.end(), meshSolids2.begin(), meshSolids2.end());
		meshsolids.insert(meshsolids.end(), meshSolids3.begin(), meshSolids3.end());
		meshsolids.insert(meshsolids.end(), meshSolids4.begin(), meshSolids4.end());
		meshsolids.insert(meshsolids.end(), meshSolids5.begin(), meshSolids5.end());
		meshsolids.insert(meshsolids.end(), meshSolids6.begin(), meshSolids6.end());
		meshsolids.insert(meshsolids.end(), meshSolids7.begin(), meshSolids7.end());
		meshsolids.insert(meshsolids.end(), meshSolids8.begin(), meshSolids8.end());

		//��ȡ���в��ཻmesh�弯��
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids1.begin(), NItsmeshSolids1.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids2.begin(), NItsmeshSolids2.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids3.begin(), NItsmeshSolids3.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids4.begin(), NItsmeshSolids4.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids5.begin(), NItsmeshSolids5.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids6.begin(), NItsmeshSolids6.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids7.begin(), NItsmeshSolids7.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids8.begin(), NItsmeshSolids8.end());
		// �ͷű���
		bvector<ElementId>().swap(meshSolids1);
		bvector<ElementId>().swap(meshSolids2);
		bvector<ElementId>().swap(meshSolids3);
		bvector<ElementId>().swap(meshSolids4);
		bvector<ElementId>().swap(meshSolids5);
		bvector<ElementId>().swap(meshSolids6);
		bvector<ElementId>().swap(meshSolids7);
		bvector<ElementId>().swap(meshSolids8);

		bvector<ElementId>().swap(NItsmeshSolids1);
		bvector<ElementId>().swap(NItsmeshSolids2);
		bvector<ElementId>().swap(NItsmeshSolids3);
		bvector<ElementId>().swap(NItsmeshSolids4);
		bvector<ElementId>().swap(NItsmeshSolids5);
		bvector<ElementId>().swap(NItsmeshSolids6);
		bvector<ElementId>().swap(NItsmeshSolids7);
		bvector<ElementId>().swap(NItsmeshSolids8);
	}

}

void ClipFace::InstallmyNewInstance(int toolId)//���߿�ʼ���а�װ
{
	ClipFace* tool = new ClipFace(toolId);
	tool->cutType = 3;
	tool->InstallTool();
}


bool ClipFace::CreateClipFace(DgnButtonEventCR ev, bool bTempClipFace)
{
	bool bCreate = false;
	int type = g_outRebarDlgInfo.chooseBtnIndex;
	//��������ǰ��Ҫ�ȵõ�һ���ߣ��ߵĶ˵���tempLinePts��ȡ
	bvector<DPoint3d> tempLinePts;
	EditElementHandle  tempLine;

	//��������㡢�յ�����
	DPoint3d lstrPt, lsend;
	if (type < 3)
	{
		//�������յ����꣬�������������ȡlstrPt
		DPoint3d lendPt;

		if (0 == type)
		{
			//yzƽ��X���궼һ��
			lstrPt.x = ev.GetPoint()->x;
			lendPt.x = lstrPt.x;

			lstrPt.y = m_miny;
			lendPt.y = m_maxy;

			lstrPt.z = m_minz;
			lendPt.z = m_minz;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//�������յ�����,���������ȡ���������
			lsend.x = lstrPt.x;
			lsend.y = lstrPt.y;
			lsend.z = m_maxz;
		}
		else if (1 == type)
		{
			//xzƽ��y���궼һ��
			lstrPt.y = ev.GetPoint()->y;
			lendPt.y = lstrPt.y;

			lstrPt.x = m_minx;
			lendPt.x = m_maxx;

			lstrPt.z = m_minz;
			lendPt.z = m_minz;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//�������յ�����,���������ȡ���������
			lsend.x = lstrPt.x;
			lsend.y = lstrPt.y;
			lsend.z = m_maxz;
		}
		else if (2 == type)
		{
			//xyƽ��z���궼һ��
			lstrPt.z = ev.GetPoint()->z;
			lendPt.z = lstrPt.z;

			lstrPt.x = m_minx;
			lendPt.x = m_maxx;

			lstrPt.y = m_miny;
			lendPt.y = m_miny;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//�������յ�����,���������ȡ���������
			lsend.x = lstrPt.x;
			lsend.z = lstrPt.z;
			lsend.y = m_maxy;
		}
		bCreate = CreateElement(tempLine, tempLinePts);
	}
	else
	{
		//size=0ʱ��ִ��create
		int size = (int)m_points.size();
		if (1 == size)
		{
			tempLinePts.push_back(m_points[0]);
			tempLinePts.push_back(*ev.GetPoint());

			double dDistance = mdlVec_distance(&tempLinePts[0], &tempLinePts[1]);
			DPoint3d vNormal, vLength;
			vNormal.x = 0;
			vNormal.y = 0;
			vNormal.z = 1;

			RotMatrix rotmatrix;
			AccuDraw::GetInstance().GetRotation(rotmatrix);
			mdlVec_multiplyRotMatrixTransposeDVec3dArray(&vLength, &rotmatrix, &vNormal, 1);
			vLength.Normalize();
			vLength.Scale(dDistance);
			mdlVec_add(&tempLinePts.at(0), &tempLinePts.at(0), &vLength);
			vLength.Normalize();
			vLength.Scale(-2 * dDistance);
			mdlVec_add(&tempLinePts.at(1), &tempLinePts.at(0), &vLength);

			tPlineOrg = tempLinePts[0];
			tPlineEnd = tempLinePts[1];

			prjendPoint = *ev.GetPoint();
			mdlVec_projectPointToLine(&prjstrPoint, NULL, &prjendPoint, &tempLinePts[0], &tempLinePts[1]);
			bCreate = CreateElement(tempLine, tempLinePts);
			lsend = prjendPoint;
			lstrPt = prjstrPoint;
		}
		else if (size > 1 && false == bTempClipFace)
		{
			if (!ClipFace::IsClipFaceModel())
			{
				//����������ʸ��
				DVec3d lined;
				lined = DVec3d::FromStartEnd(prjstrPoint, prjendPoint);
				lined.Normalize();
				//�������������ǰ��������ֹ���ֲ��������д��������
				tPlineOrg.x = tPlineOrg.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				tPlineOrg.y = tPlineOrg.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				tPlineOrg.z = tPlineOrg.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);
				tempLinePts.push_back(tPlineOrg);
				tPlineEnd.x = tPlineEnd.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				tPlineEnd.y = tPlineEnd.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				tPlineEnd.z = tPlineEnd.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);

				//���������յ�������������ֹ���ֲ��������д��������
				prjstrPoint.x = prjstrPoint.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				prjstrPoint.y = prjstrPoint.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				prjstrPoint.z = prjstrPoint.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);

				prjendPoint.x = prjendPoint.x + 10 * lined.x*UOR_PER_UNIT(m_Unit);
				prjendPoint.y = prjendPoint.y + 10 * lined.y*UOR_PER_UNIT(m_Unit);
				prjendPoint.z = prjendPoint.z + 10 * lined.z*UOR_PER_UNIT(m_Unit);
				tempLinePts.push_back(tPlineEnd);

				bCreate = CreateElement(tempLine, tempLinePts);
				lsend = prjendPoint;
				lstrPt = prjstrPoint;
			}
			else
				bCreate = false;
		}
	}
	if (bCreate)
	{
		MSElementDescrP faceDescrP = nullptr;
		mdlCurrTrans_begin();
		mdlCurrTrans_identity();
		mdlSurface_project(&faceDescrP, tempLine.GetElementDescrP(), &lstrPt, &lsend, NULL);
		mdlCurrTrans_end();
		EditElementHandle faceHandle(faceDescrP, TRUE, TRUE);
		if (bTempClipFace)
		{
			RedrawElems redrawElems;
			redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
			redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
			redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
			redrawElems.DoRedraw(faceHandle);
		}
		else
		{
			CutSolidsByFace(faceHandle);
			RefreshSurfaceShowDlg();
		}
	}
	return bCreate;
}

void ClipFaceCmd(WCharCP unparsed)
{
	ClipFace::InstallmyNewInstance(COMMAND_CLIPEFACE);
}