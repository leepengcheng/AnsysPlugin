#pragma once

void MakeContourOutFaceLine(bvector<ContourPoint>& vecFacePtAndVal, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape , LevelId  levelId);

void DrawContourOutFaceLine(bvector<std::string>& normalstd, map<std::string, bvector<bvector<ContourPoint>>>& faceDatas, bvector<bvector<ContourPoint>>& PlotDatas);

void PointPushback(bvector<ContourPoint> & vecFace, Statement *l_statment, DPoint3d basePt, WString sProUnit);

bool getCaeFaceInfos(WString ngName, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt);

LevelId showContourOutFaceLine(WString ngName, DrawColorShape* pDCShape,LevelId  levelId, DPoint3dCP pBasePt = NULL);

void DrawoutFaceLine(std::string normalKey, map<std::string, bvector<bvector<ContourPoint>>>& faceDatas, LevelId levelId, bvector<DPoint3d>& linePoints);
struct DrawTempOutLine : IViewDecoration
{
public:
	bvector<DPoint3d> linePoints;
	/**
	* @brief ����ʱԪ�غ���
	* @param[in] viewport  ��Ӧ��ͼָ��
	* @return ���ػ�ͼ�ɹ����
	*/
	virtual bool _DrawDecoration(IndexedViewportR viewport)
	{
		IViewOutputP output = viewport.GetIViewOutput();
		UInt32     clr = viewport.GetContrastToBackgroundColor();
		for (size_t i = 0; i < linePoints.size();i=i+2)
		{
			DPoint3d pts[2];
			if (i<linePoints.size()-1)
			{
				output->SetSymbology(clr, clr, 0, 0);
				pts[0] = linePoints.at(i);
				pts[1] = linePoints.at(i + 1);
				output->DrawLineString3d(2, pts, NULL);
			}
		}
		return false;
	}

	void StartDrawFunction()
	{
		IViewManager::GetManager().AddViewDecoration(this);
	}

	/*--------------------------------------------------------------------------------------+
	|   CloseStartDrawFunction��������ʱԪ�غ�Ҫ�ȹر����˳�����
	+--------------------------------------------------------------------------------------*/
	void CloseDrawFunction()
	{
		bvector<DPoint3d>().swap(linePoints);
		IViewManager::GetManager().DropViewDecoration(this); // add the decorator
	}

};

extern DrawTempOutLine   s_drawtempLine;