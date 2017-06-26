#include "MsdiMsCaeSipch.h"
extern MSDialogP jdoytBar;
extern WCharP 	  jdwcMessageText;
extern int       jdPercent;
void CreateLineText(DPoint3d tPos, double fwidth, double fheight, WString tText, DgnPlatform::LevelId  levelId_OutData, DgnModelRefP ModelRef,RotMatrix rot)
{
	if (ModelRef == NULL)
	{
		ModelRef = MASTERFILE;
	}
	DgnTextStylePtr myTextStyle = DgnTextStyle::GetActive();
	TextBlockPropertiesPtr testptr = TextBlockProperties::Create(*ModelRef->GetDgnModelP());
	testptr->SetIsViewIndependent(true);
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_Width, fwidth);
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_Height, fheight);
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_FullJustification, true);
	UInt32 foundNum;
	DgnFileP modelref = ISessionMgr::GetActiveDgnFile();
	DgnFontP pDGNFont = DgnFontManager::GetDgnFontMapP(ModelRef)->FindFontByName(&foundNum, L"����");
	if (!pDGNFont)
	{
		pDGNFont = DgnFontManager::GetDgnFontMapP(ModelRef)->FindFontByName(&foundNum, L"Batang");
	}
	if (!pDGNFont)
	{
		pDGNFont = &DgnFontManager::GetDgnFontMapP(ModelRef)->ResolveFont(-1);
	}
	myTextStyle->SetProperty(TextStyle_Font, pDGNFont);
	TextBlockPtr textBlock = TextBlock::Create(*myTextStyle, *ModelRef->GetDgnModelP());
	
	textBlock->SetUserOrigin(tPos);
	textBlock->AppendText(tText.GetWCharCP());
	textBlock->SetProperties(*testptr);
	textBlock->SetOrientation(rot);
	EditElementHandle eehTEXT;
	if (TEXTBLOCK_TO_ELEMENT_RESULT_Success == TextHandlerBase::CreateElement(eehTEXT, NULL, *textBlock))
	{
		if (levelId_OutData != 10000)
		{
			// ElementHandle��ӵ���ͼ��
			ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
			propsSetter->SetLevel(levelId_OutData);
			propsSetter->Apply(eehTEXT);
		}
		eehTEXT.AddToModel();
	}
}
void GetDisPointsandTange(bvector<DPoint3d>& arcPoints, bvector<DPoint3d>& tangPoints,MSElementDescrP Des, double dis)
{
	DPoint3d str, end;
	mdlElmdscr_extractEndPoints(&str, NULL, &end, NULL, Des, ACTIVEMODEL);
	double dis_w;
	mdlElmdscr_distanceAtPoint(&dis_w, NULL, NULL, Des, &end, 0);
	int dfNum = dis_w / dis;
	for (int i = 0; i <= dfNum; i++)
	{
		DPoint3d aPoint;
		DPoint3d tangPoint;
		mdlElmdscr_pointAtDistance(&aPoint, &tangPoint, i*dis, Des, 0.0001);
		arcPoints.push_back(aPoint);
		tangPoints.push_back(tangPoint);
	}
}
vector<deque<DPoint3d>> connectLineString(vector<vector<DPoint3d>> allPartLineString)
{
	vector<deque<DPoint3d>> result;

	if (allPartLineString.empty())
		return result;

	while (!allPartLineString.empty())
	{
		//ȡ��һ��С�߶ζΣ���������뵱ǰ׼�������Ĵ��ߴ���
		deque<DPoint3d> segment_Entire;
		segment_Entire.insert(segment_Entire.end(), allPartLineString[0].begin(), allPartLineString[0].end());
		allPartLineString.erase(allPartLineString.begin());

		do
		{
			//��ʶһ��ѭ���������Ƿ��ҵ�����С�߶Σ���δ���������ʾ�˴��ߴ��������
			bool bFound = false;

			//ѭ������ʣ��С�߶�,�ҵ��뵱ǰ���ߴ���������С�߶�
			for (int i = 0; i < (int)allPartLineString.size(); i++)
			{
				vector<DPoint3d>& segment_Part = allPartLineString[i];
				int partCnt = (int)segment_Part.size();
				int entireCnt = (int)segment_Entire.size();

				//��ǰ���ߴ������&�յ�
				DPoint3d entireOrg = segment_Entire.front();
				DPoint3d entireEnd = segment_Entire.back();

				//��С�߶εĶ˵�����ߴ��������ͬ
				if (point3d_equals(entireOrg, segment_Part.front())
					|| point3d_equals(entireOrg, segment_Part.back()))
				{
					//�жϴ��ߴ����������С�߶ε������ͬ�������յ���ͬ
					//�����ߴ��������С�߶ε������ͬ����С�߶ΰ����׵�β��˳����뵽���ߴ����
					if (point3d_equals(entireOrg, segment_Part.front()))
					{
						//��(���ߴ����)��(���ߴ�����һ��)�ĵ�λ������(С�ߴ����)��(С�ߴ�����һ��)��ͬ���������
						DVec3d tmpVector[2];
						mdlVec_computeNormal(&tmpVector[0], &segment_Part[1], &segment_Part[0]);
						mdlVec_computeNormal(&tmpVector[1], &segment_Entire[1], &segment_Entire[0]);

						if (tmpVector[0].x == tmpVector[1].x &&
							tmpVector[0].y == tmpVector[1].y &&
							tmpVector[0].z == tmpVector[1].z)
						{
							int a = 0;
						}
						else
						{
							//С�߶ΰ����׵�β��˳����뵽���ߴ����
							for (int j = 1; j < partCnt; j++)
								segment_Entire.push_front(segment_Part[j]);
						}

						//��С�߶μ�����ɾ����С�߶�
						allPartLineString.erase(allPartLineString.begin() + i);
						bFound = true;
						break;
					}
					//�����ߴ��������С�߶ε��յ���ͬ����С�߶ΰ���β���׵�˳����뵽���ߴ����
					else
					{
						//��(���ߴ����)��(���ߴ�����һ��)�ĵ�λ������(С�ߴ��յ�)��(С�ߴ��յ�ǰһ��)��ͬ���������
						DVec3d tmpVector[2];
						mdlVec_computeNormal(&tmpVector[0], &segment_Part[partCnt - 2], &segment_Part[partCnt - 1]);
						mdlVec_computeNormal(&tmpVector[1], &segment_Entire[1], &segment_Entire[0]);

						if (tmpVector[0].x == tmpVector[1].x &&
							tmpVector[0].y == tmpVector[1].y &&
							tmpVector[0].z == tmpVector[1].z)
						{
							int a = 0;
						}
						else
						{
							//С�߶ΰ���β���׵�˳����뵽���ߴ����
							for (int j = partCnt - 2; j >= 0; j--)
								segment_Entire.push_front(segment_Part[j]);
						}

						//��С�߶μ�����ɾ����С�߶�
						allPartLineString.erase(allPartLineString.begin() + i);
						bFound = true;
						break;
					}
				}

				//��С�߶εĶ˵�����ߴ����յ���ͬ
				if (point3d_equals(entireEnd, segment_Part.front())
					|| point3d_equals(entireEnd, segment_Part.back()))
				{
					//�жϴ��ߴ����յ�����С�߶ε������ͬ�������յ���ͬ
					//�����ߴ�������С�߶ε������ͬ����С�߶ΰ����׵�β��˳����뵽���ߴ��յ�
					if (point3d_equals(entireEnd, segment_Part.front()))
					{
						//��(���ߴ��յ�)��(���ߴ��յ�ǰһ��)�ĵ�λ������(С�ߴ����)��(С�ߴ�����һ��)��ͬ���������
						DVec3d tmpVector[2];
						mdlVec_computeNormal(&tmpVector[0], &segment_Part[1], &segment_Part[0]);
						mdlVec_computeNormal(&tmpVector[1], &segment_Entire[entireCnt - 2], &segment_Entire[entireCnt - 1]);

						if (tmpVector[0].x == tmpVector[1].x &&
							tmpVector[0].y == tmpVector[1].y &&
							tmpVector[0].z == tmpVector[1].z)
						{
							int a = 0;
						}
						else
						{
							//С�߶ΰ����׵�β��˳����뵽���ߴ��յ�
							for (int j = 1; j < partCnt; j++)
								segment_Entire.push_back(segment_Part[j]);
						}

						//��С�߶μ�����ɾ����С�߶�
						allPartLineString.erase(allPartLineString.begin() + i);
						bFound = true;
						break;
					}
					//�����ߴ����յ���С�߶ε��յ���ͬ����С�߶ΰ���β���׵�˳����뵽���ߴ��յ�
					else
					{
						//��(���ߴ��յ�)��(���ߴ��յ�ǰһ��)�ĵ�λ������(С�ߴ��յ�)��(С�ߴ��յ�ǰһ��)��ͬ���������
						DVec3d tmpVector[2];
						mdlVec_computeNormal(&tmpVector[0], &segment_Part[partCnt - 2], &segment_Part[partCnt - 1]);
						mdlVec_computeNormal(&tmpVector[1], &segment_Entire[entireCnt - 2], &segment_Entire[entireCnt - 1]);

						if (tmpVector[0].x == tmpVector[1].x &&
							tmpVector[0].y == tmpVector[1].y &&
							tmpVector[0].z == tmpVector[1].z)
						{
							int a = 0;
						}
						else
						{
							//С�߶ΰ���β���׵�˳����뵽���ߴ��յ�
							for (int j = partCnt - 2; j >= 0; j--)
								segment_Entire.push_back(segment_Part[j]);
						}

						//��С�߶μ�����ɾ����С�߶�
						allPartLineString.erase(allPartLineString.begin() + i);
						bFound = true;
						break;
					}
				}
			}

			if (bFound == false)
				break;

		} while (true);

		result.push_back(segment_Entire);
	}

	return result;
}

bool point3d_equals(const DPoint3d pt1, const DPoint3d pt2)
{
	double d = mdlVec_distance(&pt1, &pt2);
	if (d<0.1)
	{
		return true;
	}
	return false;
	//return (pt1.x == pt2.x && pt1.y == pt2.y && pt1.z == pt2.z);

}
void ThreadFunc(size_t TSttPost, size_t TEndPost, bvector<bvector<ContourPoint>>& vecFaces, bvector<LineThread>& LineDatas, DrawColorShape* pDCShape)
{
	for (size_t i = TSttPost; i < TEndPost; i++)
	{
		MakeContourLineDat(vecFaces.at(i), LineDatas, pDCShape);
	}
}

void MakeContourLineDat(bvector<ContourPoint>& vecFacePtAndVal, bvector<LineThread>& LineDatas, DrawColorShape* pDCShape)
{
	bvector<bvector<ContourPoint>> vecCaeMeshPtVal;
	bvector<ContourPoint> contourlist;
	bvector<ContourPoint> pMeshFace;
	bvector<ContourPoint> vecFaceAndValueTmp;
	ContourPoint oCpTemp;
	
	if (GetFaceNodePtAndVal(vecCaeMeshPtVal, vecFacePtAndVal) == true)
	{
		for (int iFace = 0; iFace < vecCaeMeshPtVal.size(); iFace++)
		{
			bvector<ContourPoint>().swap(contourlist);
			bvector<ContourPoint>().swap(vecFaceAndValueTmp);
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

			// ������ͼ����ɫ������Ϣ��������ͼ
			for (int iVal = 0; iVal < pDCShape->Color_num; ++iVal)
			{
				double dContourVal = pDCShape->ClrShapeVal[iVal];

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

					if (dContourVal == ContourPt.dVal)
					{
						for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
						{
							ContourPtEnd = contourlist.at(iContourPtEnd);
							if (dContourVal == ContourPtEnd.dVal)
							{
								segment.SetEndPoint(ContourPtEnd.dPt);

								// ���ɵ�ֵ��չʾ����
								LineThread tempLine;
								tempLine.LinePt = segment;
								tempLine.theColor = pDCShape->fillCororIndex[iVal];
								LineDatas.push_back(tempLine); 

								break;
							}
						}
					}
				}
			}
		}
	}

	// �ͷű���
	bvector<bvector<ContourPoint>>().swap(vecCaeMeshPtVal);
	bvector<ContourPoint>().swap(contourlist);
	bvector<ContourPoint>().swap(pMeshFace);
	bvector<ContourPoint>().swap(vecFaceAndValueTmp);
}

// ���Ƶ�ֵ��
void DrawContourLine(bvector<LineThread>& LineDatas, DgnPlatform::LevelId levelId)
{
	for (UInt32 i = 0; i < LineDatas.size(); i++)
	{
		LineThread TempLine = LineDatas.at(i);
		MakeColorLine(TempLine.LinePt, TempLine.theColor, levelId);
	}
}

void MakeColorLine(DSegment3dCR segment, UInt32 color, DgnPlatform::LevelId levelId)
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

void MergeBorderLines(bstdmap<string, BorderLine>& BorderLinesMain, bstdmap<string, BorderLine>& BorderLines)
{
	bstdmap<string, BorderLine>::iterator it;
	for (it = BorderLines.begin(); it != BorderLines.end(); ++it)
	{
		BorderLinesMain[it->first].Count += 1;
		BorderLinesMain[it->first].LinePt = it->second.LinePt;
	}
}

LevelId showContourLine(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt /*= NULL*/)
{
	int percent = 70;
	double idex = 0.0;
	bvector<bvector<ContourPoint>> vecFaces;
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel, levelId_ContourLine;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);
	if (pBasePt != NULL)
		basePt = *pBasePt;

	// ������ȡ��CAE��ͼ��ͼ��
	if (SUCCESS != mdlLevel_create(&levelId_ContourLine, MASTERFILE, L"CAE��ֵ��ͼ", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_ContourLine, MASTERFILE, LEVEL_NULL_ID, L"CAE��ֵ��ͼ");

	// ȡ��CAE������Mesh�漯�Ϻ���ڵ㼯��
	if (getCaeFaceInfos(ngName, type, caseID, vecFaces, basePt))
	{
		// ���ö��̼߳��������CAE�������Ԫ��
		bvector<LineThread> LineDatas1; // �߳�1�ļ�����
		bvector<LineThread> LineDatas2; // �߳�2�ļ�����
		bvector<LineThread> LineDatas3; // �߳�3�ļ�����
		bvector<LineThread> LineDatas4; // �߳�4�ļ�����
		bvector<LineThread> LineDatas5; // �߳�5�ļ�����
		bvector<LineThread> LineDatas6; // �߳�6�ļ�����
		bvector<LineThread> LineDatas7; // �߳�7�ļ�����
		bvector<LineThread> LineDatas8; // �߳�8�ļ�����

		// �߳�1(������Ϊ��Mesh������1/8)
		std::thread t1(ThreadFunc, 0, vecFaces.size() / 8, vecFaces, ref(LineDatas1), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�2(������Ϊ��Mesh������1/8)
		std::thread t2(ThreadFunc, vecFaces.size() / 8, vecFaces.size() * 2 / 8, vecFaces, ref(LineDatas2), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�3(������Ϊ��Mesh������1/8)
		std::thread t3(ThreadFunc, vecFaces.size() * 2 / 8, vecFaces.size() * 3 / 8, vecFaces, ref(LineDatas3), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�4(������Ϊ��Mesh������1/8)
		std::thread t4(ThreadFunc, vecFaces.size() * 3 / 8, vecFaces.size() * 4 / 8, vecFaces, ref(LineDatas4), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�5(������Ϊ��Mesh������1/8)
		std::thread t5(ThreadFunc, vecFaces.size() * 4 / 8, vecFaces.size() * 5 / 8, vecFaces, ref(LineDatas5), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�6(������Ϊ��Mesh������1/8)
		std::thread t6(ThreadFunc, vecFaces.size() * 5 / 8, vecFaces.size() * 6 / 8, vecFaces, ref(LineDatas6), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�7(������Ϊ��Mesh������1/8)
		std::thread t7(ThreadFunc, vecFaces.size() * 6 / 8, vecFaces.size() * 7 / 8, vecFaces, ref(LineDatas7), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// �߳�8(������Ϊ��Mesh������1/8)
		std::thread t8(ThreadFunc, vecFaces.size() * 7 / 8, vecFaces.size(), vecFaces, ref(LineDatas8), pDCShape);
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

		bmap<UInt32, vector<vector<DPoint3d>>> allLinePoints;
		//bvector<UInt32> colors;
		LineDatas1.insert(LineDatas1.end(), LineDatas2.begin(), LineDatas2.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas3.begin(), LineDatas3.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas4.begin(), LineDatas4.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas5.begin(), LineDatas5.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas6.begin(), LineDatas6.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas7.begin(), LineDatas7.end());
		LineDatas1.insert(LineDatas1.end(), LineDatas8.begin(), LineDatas8.end());

		for (int i = 0; i < LineDatas1.size();i++)//����ֵ��
		{
			vector<DPoint3d> tempLinePoints;
			DPoint3d str, end;
			LineDatas1.at(i).LinePt.GetStartPoint(str);
			LineDatas1.at(i).LinePt.GetEndPoint(end);
			tempLinePoints.push_back(str);
			tempLinePoints.push_back(end);
			allLinePoints[LineDatas1.at(i).theColor].push_back(tempLinePoints);
		}

		//С����λ��
		int idec = 2;
		WString dec;
		dec.Sprintf(L"%%0.%dg", idec);
		//��ֵ����ֵ���ִ�С
		double valueSize = 0.5 * UOR_PER_UNIT(sUnit);
		//��ֵ����ֵ��������
		double dis = 200 * UOR_PER_UNIT(sUnit);

		RotMatrix tempMatirx = RotMatrix::FromRowValues(1.0, 0.0, 0.0,
														0.0, 1.0, 0.0,
														0.0, 0.0, 1.0);
		// ������ͼ����ɫ������Ϣ�����ɵ�ֵ��
		for (int iVal = 0; iVal < pDCShape->Color_num; ++iVal)
		{
			UInt32 theColor = pDCShape->fillCororIndex[iVal];
			double dContourVal = pDCShape->ClrShapeVal[iVal];
			vector <vector<DPoint3d>> colorLinePoints = allLinePoints[theColor];
			vector<deque<DPoint3d>> result = connectLineString(colorLinePoints);

			ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
			UInt32 newColor = (theColor == 256) ? 0 : theColor;
			propsSetter->SetColor(newColor);
			propsSetter->SetLevel(levelId_ContourLine);

			WChar buffer[20];
			_swprintf(buffer, dec.GetWCharCP(), dContourVal);
			WString wValue;
			wValue.append(buffer);
			for (int i = 0; i < result.size();i++)
			{
				deque<DPoint3d> linePoints = result.at(i);
				if (linePoints.size()<1)
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
				for (int j = 0; j < lPoints.size();j++)
				{
					DPoint3d lpoint = lPoints.at(j);
					DPoint3d tpoint = tangPoints.at(j);
					CreateLineText(lpoint, valueSize, valueSize, wValue, levelId_ContourLine, ACTIVEMODEL, tempMatirx);
				}
 			}
		}

		// �ͷű���
		bvector<LineThread>().swap(LineDatas1);
		bvector<LineThread>().swap(LineDatas2);
		bvector<LineThread>().swap(LineDatas3);
		bvector<LineThread>().swap(LineDatas4);
		bvector<LineThread>().swap(LineDatas5);
		bvector<LineThread>().swap(LineDatas6);
		bvector<LineThread>().swap(LineDatas7);
		bvector<LineThread>().swap(LineDatas8);

		mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);
	}

	return levelId_ContourLine;
}