#pragma once

#include "BentlyCommonfile.h"
//���λ����ֵͼ
extern void OutValueLists(WString ngName, int ProjectID, int caseID, WString sUnit);

extern void CreateOutFrame(DataListParamas dataparams, OutLineData& outLine, DPoint3d orgminpt, DPoint3d orgmaxpt, EditElementHandleR cellHeader, WString sUnit);

extern void CreateTextExt
(
EditElementHandleR cellHeader, 
DPoint3d tPos, 
double fwidth, 
double fheight, 
WString tText, 
DgnModelRefP ModelRef, 
double iSlant, 
UInt32 iJustification
);

// ���ɵѿ�������Ļ��������
extern void CreateCartesianPoints(
	DPoint3d minPos, 
	DPoint3d maxPos, 
	DataListParamas& dataparams,
	bvector<bvector<DPoint3d>>& alloutPoints, 
	bvector<MSElementDescrP>& outLines
	);

extern void CaculateAndDisplayValue
(
bvector<bvector<DPoint3d>>&alloutPoints, 
DataListParamas& dataparams, 
DPoint3d orgminpt, 
DPoint3d orgmaxpt,	
bvector<bvector<ContourPoint>>& MeshPoints,
bvector<bvector<ContourPoint>>& MeshPoints_sx,
bvector<bvector<ContourPoint>>& MeshPoints_sy,
bvector<bvector<ContourPoint>>& MeshPoints_sxy,
EditElementHandleR cellHeader
);

extern void DisplayOutPointValue(
	bvector<bvector<PointOutData>>& outPointAndValues,
	const bvector<bvector<PointOutData>>& vecSectionMeshDats,
	DataListParamas& dataparams,
	DPoint3d orgminpt,
	DPoint3d orgmaxpt,
	EditElementHandleR cellHeader);

extern void CalcOutPointValues
(
bvector<bvector<PointOutData>>&  outPointAndValues,
const bvector<bvector<DPoint3d>>& alloutPoints,
const bvector<bvector<PointOutData>>& vecSectionMeshDats,
TransformCP tMatrix,
RotMatrixCR rotMatrix
);

extern void CreateOutLine(DataListParamas datalistpraramas, OutLineData&  outLine, 
	DPoint3d orgminpt, DPoint3d orgmaxpt, EditElementHandleR cellHeader, WString sUnit);

extern bool PtInRange(DPoint3d p, DPoint3d ptmin, DPoint3d ptmax);

extern bool IsInSameLineAndSetSingleValue//�ж��Ƿ��߲������м��valueֵ
(
PointOutData& mid_Point,
const bvector<PointOutData> vecMeshPoints
);

extern bool PtInPolygon(DPoint3d p, const bvector<PointOutData>& ptPolygon, size_t nCount);

/**
* @brief  ��ȡ�����MeshԪ��ֵ����
* @param[out] vecSectionMeshDat		�����MeshԪ��ֵ����
* @param[in] allNineDatas			ģ�͸�������͵�CAE������
*/
extern void GetSectionMeshDatas
(
bvector<bvector<PointOutData>>& vecSectionMeshDat,
const bmap<WString, bvector<double>>& allNineDatas
);

extern void GetMeshMinMaxPoints(DPoint3d& minpoint, DPoint3d& maxpoint, WString& ngName);

extern void InitOutValueType(bvector<WString>& outValueTypeWString);

// ���������������
void CreateFaceOutLine(bvector<DPoint3d>& linePoints, EditElementHandleR cellHeader);

extern WString O_CaseName;
extern WString O_tableTitle;//��ͷ����
extern double  O_titleFont;//��ͷ�����С
extern double  O_contentFont;//���������С
extern double  O_heightCoe;//�߶ȱ���
extern int O_XSpaceCnt;//X���껮�ָ���
extern int O_YSpaceCnt;//Y���껮�ָ���
extern double O_coorFont;//�������������С
extern int O_XValueCnt;//X������ֵ����
extern int O_YValueCnt;//Y��������
extern double O_valueFont;//��ֵ�����С
extern int O_decimal;//С����λ��

extern bvector<int>  outValueTypeint;
extern bvector<WString> outValueTypeWString;
extern DgnModelRefP OutDataModelRef;