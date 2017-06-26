#include "MsdiMsCaeSipch.h"
extern MSDialogP jdoytBar;
extern WCharP 	  jdwcMessageText;
extern int       jdPercent;
void ThreadFunc(size_t TSttPost, size_t TEndPost, bvector<bvector<ContourPoint>>& vecFaces, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape)
{
	for (size_t i = TSttPost; i < TEndPost; i++)
	{
		MakeContourPlotDat(vecFaces.at(i), PlotDatas, pDCShape);
	}
}

void MakeContourPlotDat(bvector<ContourPoint>& vecFacePtAndVal, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape)
{
	bvector<bvector<ContourPoint>> vecCaeMeshPtVal;
	bvector<ContourPoint> contourlist;
	bvector<ContourPoint> pMeshFace;
	bvector<ContourPoint> vecFaceAndValueTmp;
	ContourPoint oCpTemp;
	bool bContourPlotFace = false;

	if (GetFaceNodePtAndVal(vecCaeMeshPtVal, vecFacePtAndVal) == true)
	{
		for (int iFace = 0; iFace < vecCaeMeshPtVal.size(); iFace++)
		{
			contourlist.clear();
			vecFaceAndValueTmp.clear();

			vecFaceAndValueTmp = vecCaeMeshPtVal.at(iFace);

			// ѭ����������������нڵ�֮��ĵ�ֵ��
			for (int i = 0; i < vecFaceAndValueTmp.size() - 1; i++)
			{
				ContourPoint oCpStart = vecFaceAndValueTmp.at(i);
				ContourPoint oCpEnd = vecFaceAndValueTmp.at(i + 1);

				contourlist.push_back(oCpStart);

				if (oCpStart.dVal <= oCpEnd.dVal)
				{
					for (int iVal = 0; iVal <= pDCShape->Color_num; ++iVal)
					{
						// ����ڵ�֮��ĵ�ֵ��
						if (true == GetPosition(oCpTemp, oCpStart.dPt, oCpEnd.dPt, oCpStart.dVal, oCpEnd.dVal, pDCShape->ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
				else
				{
					for (int iVal = pDCShape->Color_num; iVal >= 0; --iVal)
					{
						// ����ڵ�֮��ĵ�ֵ��
						if (true == GetPosition(oCpTemp, oCpStart.dPt, oCpEnd.dPt, oCpStart.dVal, oCpEnd.dVal, pDCShape->ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
			}

			bool bAllValSame = true;//�����������ж����ֵ�Ƿ�һ��
			int iContourPtNow = 0;
			int iContourPtEnd = (int)contourlist.size() - 1;
			
			//WString myString;

			// �жϾ����������ж����ֵ�Ƿ�һ��
			for (iContourPtNow = 1; iContourPtNow <= iContourPtEnd; iContourPtNow++)
			{
				if (contourlist.at(0).dVal != contourlist.at(iContourPtNow).dVal)
				{
					bAllValSame = false;
					break;
				}
			}

			//myString.Sprintf(L"bAllValSame: %d", bAllValSame);

			// ������ͼ����ɫ������Ϣ��������ͼ
			for (int iVal = 0; iVal <pDCShape->Color_num; ++iVal)
			{
				double dValStt = pDCShape->ClrShapeVal[iVal];
				double dValEnd = pDCShape->ClrShapeVal[iVal + 1];

				iContourPtNow = 0;
				iContourPtEnd = (int)contourlist.size() - 1;
				ContourPoint ContourPt;
				ContourPoint ContourPtNxt;
				ContourPoint ContourPtEnd;

				// ���ݾ������ϵĵ�ֵ�㣬���ɸ��������ͼ
				for (iContourPtNow = 0; iContourPtNow <= iContourPtEnd; iContourPtNow++)
				{
					ContourPt = contourlist.at(iContourPtNow);
					if (dValStt <= ContourPt.dVal && ContourPt.dVal <= dValEnd)
					{
						if (bAllValSame == true)
						{
							pMeshFace.push_back(ContourPt);
						}
						else
						{
							if (pMeshFace.size() == 0 && ContourPt.dVal < dValEnd)
							{
								// ����ڵ�ĸ�������0�ҽڵ��ֵС����������ֵʱ��
								// �ȷ�����ҳ��ڵ�ǰ��������нڵ㣬
								// Ȼ������ڵ���뵽pMeshFace�����С�
								for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
								{
									ContourPtEnd = contourlist.at(iContourPtEnd);
									if (dValStt <= ContourPtEnd.dVal && ContourPtEnd.dVal < dValEnd)
									{
										pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
									}
									else if (ContourPtEnd.dVal >= dValEnd)
									{
										// ���������ֵʱ��������ҽڵ㴦�����������Ȼ�ж�ʽΪ">="�����������">dValEnd"�������
										pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
										break;
									}
								}

								// ������ڵ���뵽pMeshFace������
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
										// ������ͼչʾ����
										PlotThread tempPlot;
										tempPlot.pMeshFaces = pMeshFace;
										tempPlot.theColor = pDCShape->fillCororIndex[iVal];
										PlotDatas.push_back(tempPlot);

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
				}

				// ������ͼչʾ����
				if (pMeshFace.size() > 0)
				{
					PlotThread tempPlot2;
					tempPlot2.pMeshFaces = pMeshFace;
					tempPlot2.theColor = pDCShape->fillCororIndex[iVal];
					PlotDatas.push_back(tempPlot2);
				}

				pMeshFace.clear();
			}
		}
	}

	//// �ͷű���
	//bvector<bvector<ContourPoint>>().swap(vecCaeMeshPtVal);
	//bvector<ContourPoint>().swap(contourlist);
	//bvector<ContourPoint>().swap(pMeshFace);
	//bvector<ContourPoint>().swap(vecFaceAndValueTmp);
}

// ������ͼ
void DrawContourPlot(bvector<PlotThread>& PlotDatas, DgnPlatform::LevelId levelId, NamedGroupPtr ngCAESolidYT)
{
	bvector<int> vecIndices;
	bvector<DPoint3d> vecPoints;
	MeshSurfaceValue meshValues;
	
	for (UInt32 i = 0; i < PlotDatas.size(); i++)
	{
		int nodesCount = 0;
		vecIndices.clear();
		vecPoints.clear();

		PlotThread Tempplot = PlotDatas.at(i);
		if (Tempplot.pMeshFaces.size() < 3) continue;

		MSElementDescrP     m_resultEdp;
		ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
		UInt32 newColor = (Tempplot.theColor == 256) ? 0 : Tempplot.theColor;
		propsSetter->SetColor(newColor);
		propsSetter->SetFillColor(newColor);
		propsSetter->SetLevel(levelId);

		bvector<ContourPoint>::iterator iterNow = Tempplot.pMeshFaces.begin();
		bvector<ContourPoint>::iterator iterend = Tempplot.pMeshFaces.end();
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
			// ͨ��Ԫ��ID����������Ԫ��
			ngCAESolidYT->AddMember(eeh.GetElementP()->GetID(), ACTIVEMODEL, NamedGroupMemberFlags());	
		}

		bvector<ContourPoint>().swap(Tempplot.pMeshFaces);
	}

	bvector<int>().swap(vecIndices);
	bvector<int>().swap(vecIndices);
}

LevelId showContourPlot(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt /*= NULL*/)
{
	int percent = 70;
	double idex = 0.0;
	//��ȡbasePt����ʾ��ID
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel, levelId_ContourPlot;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);
	if (pBasePt != NULL)
		basePt = *pBasePt;

	NamedGroupCollectionPtr ngcCAESolidYT;
	NamedGroupFlags ngfCAESolidYT;
	NamedGroupPtr ngCAESolidYT;
	WString uniqueCaeYTName;

	ngcCAESolidYT = NamedGroupCollection::Create(*ACTIVEMODEL);
	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
	WString GroupNameYT;
	GroupNameYT.append(L"CAE��ͼ");
	ngfCAESolidYT.m_selectMembers = 0;// ��ʾ�Զ���ѡ
	ngfCAESolidYT.m_anonymous = 1;// �������鲻�����������б�����ʾ(�����ص���ʱ������)

	// ����������
	if (NG_Success != NamedGroup::Create(ngCAESolidYT, GroupNameYT.c_str(), L"CAEMODELGROUP", ngfCAESolidYT, ACTIVEMODEL))
	{
		return 0;
	}
	// ������ȡ��CAE��ͼ��ͼ��
	if (SUCCESS != mdlLevel_create(&levelId_ContourPlot, MASTERFILE, L"CAE��ͼ", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_ContourPlot, MASTERFILE, LEVEL_NULL_ID, L"CAE��ͼ");

	bvector<bvector<ContourPoint>> vecFaces;

	// ȡ��CAE������Mesh�漯�Ϻ���ڵ㼯��
	if (true == getCaeFaceInfos(ngName, type, caseID, vecFaces, basePt))
	{
		// ���ö��̼߳��������CAE�������Ԫ��
		bvector<PlotThread> PlotDatas1; // �߳�1�ļ�����
		bvector<PlotThread> PlotDatas2; // �߳�2�ļ�����
		bvector<PlotThread> PlotDatas3; // �߳�3�ļ�����
		bvector<PlotThread> PlotDatas4; // �߳�4�ļ�����
		bvector<PlotThread> PlotDatas5; // �߳�5�ļ�����
		bvector<PlotThread> PlotDatas6; // �߳�6�ļ�����
		bvector<PlotThread> PlotDatas7; // �߳�7�ļ�����
		bvector<PlotThread> PlotDatas8; // �߳�8�ļ�����

		// �߳�1(������Ϊ��Mesh������1/8)
		std::thread t1(ThreadFunc, 0, vecFaces.size() / 8, vecFaces, ref(PlotDatas1), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�2(������Ϊ��Mesh������1/8)
		std::thread t2(ThreadFunc, vecFaces.size() / 8, vecFaces.size() * 2 / 8, vecFaces, ref(PlotDatas2), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�3(������Ϊ��Mesh������1/8)
		std::thread t3(ThreadFunc, vecFaces.size() * 2 / 8, vecFaces.size() * 3 / 8, vecFaces, ref(PlotDatas3), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�4(������Ϊ��Mesh������1/8)
		std::thread t4(ThreadFunc, vecFaces.size() * 3 / 8, vecFaces.size() * 4 / 8, vecFaces, ref(PlotDatas4), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�5(������Ϊ��Mesh������1/8)
		std::thread t5(ThreadFunc, vecFaces.size() * 4 / 8, vecFaces.size() * 5 / 8, vecFaces, ref(PlotDatas5), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�6(������Ϊ��Mesh������1/8)
		std::thread t6(ThreadFunc, vecFaces.size() * 5 / 8, vecFaces.size() * 6 / 8, vecFaces, ref(PlotDatas6), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�7(������Ϊ��Mesh������1/8)
		std::thread t7(ThreadFunc, vecFaces.size() * 6 / 8, vecFaces.size() * 7 / 8, vecFaces, ref(PlotDatas7), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�8(������Ϊ��Mesh������1/8)
		std::thread t8(ThreadFunc, vecFaces.size() * 7 / 8, vecFaces.size(), vecFaces, ref(PlotDatas8), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));

		// �����߳�
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		t7.join();
		t8.join();

		// �ͷű���
		bvector<bvector<ContourPoint>>().swap(vecFaces);

		// ������ͼ
		DrawContourPlot(PlotDatas1, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas2, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas3, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas4, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas5, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas6, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas7, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas8, levelId_ContourPlot, ngCAESolidYT);
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 90);
		// �ͷű���
		bvector<PlotThread>().swap(PlotDatas1);
		bvector<PlotThread>().swap(PlotDatas2);
		bvector<PlotThread>().swap(PlotDatas3);
		bvector<PlotThread>().swap(PlotDatas4);
		bvector<PlotThread>().swap(PlotDatas5);
		bvector<PlotThread>().swap(PlotDatas6);
		bvector<PlotThread>().swap(PlotDatas7);
		bvector<PlotThread>().swap(PlotDatas8);
		ngCAESolidYT->WriteToFile(true);
		mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);
	}

	return levelId_ContourPlot;
}
