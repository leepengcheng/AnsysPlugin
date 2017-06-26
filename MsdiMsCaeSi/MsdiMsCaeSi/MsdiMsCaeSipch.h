#pragma once
#include    "BentlyCommonfile.h"
#include    "CommonStructFile.h"
#include	"MsdiMsCaeSiids.h"
#include	"MsdiMsCaeSiCmd.h"
#include	"transkit\MsdiMsCaeSitxt.h"
#include	"MsdiMsCaeSi.h"

#include	"pugixml\pugixml.hpp"
#include	"pugixml\pugiconfig.hpp"

#include	"ImportCAEInf.h"
#include	"CreateCAESolid.h"
#include    "DrawVectorGraph.h"
#include    "DrawColorShape.h"
#include    "ContourUtility.h"
#include    "ContourVector.h"
#include    "ClipFace.h"

#include    "exportTool.h"
#include    "log.h"
#include	"ImportCaeInfDlgHook.h"
#include	"SolidContourDispDlgHook.h"
#include	"SolidMeshShowDlgHook.h"
#include	"ColorandValueSetDlgHook.h"

#include    "ContourLine.h"
#include    "ContourPlot.h"
#include    "ClipPlotFace.h"
#include    "MoveClipPlot.h"
#include    "OutColumnData.h"
#include    "OutDataList.h"

#include    "SelectGroup.h"

#include	"nodeViewer.h"
#include	"partialDisplay.h"
#include	"dbFunction.h"
#include    "calculateRebar.h"
#include    "ExportCalcDlgHook.h"
#include	"partialDisplayDlgHook.h"
#include	"caseCompareDlgHook.h"
#include	"clipResultShowDlgHook.h"
#include	"surfaceShowDlgHook.h"
#include	"outputRebarDlgHook.h"
#include    "ContourOutFaceLine.h"
#include	"MsdiMsCaeSiUtility.h"

#define UOR_PER_METER	(mdlModelRef_getUorPerMeter(ACTIVEMODEL))
#define UOR_PER_MM		(mdlModelRef_getUorPerMeter(ACTIVEMODEL)/1000)
#define UOR_PER_MASTER	(mdlModelRef_getUorPerMaster(ACTIVEMODEL))
#define UOR_PER_UNIT(x)	MsdiMsCaeSiUtility::GetUnit(x)

#define Tolerance (1e-4)
#define INFINIT  (1e10)

//����������
#define NGNAME_PREFIX_CAEMODEL L"CAEMODEL"
#define NGNAME_PREFIX_CAEMODEL_DOLLAR L"CAEMODEL$"
#define NGNAME_PREFIX_CAECLIP L"CAECLIP"
#define NGNAME_PREFIX_CAECLIP_DOLLAR L"CAECLIP$"

//�ָ��
#define SeparatorDollar L"$"
#define SeparatorAnd L"&"

#define XAttrHandleID_Major		800		//XAttributionHandle��MajorID
#define XAttrHandleID_Minor		900		//XAttributionHandle��MinorID
#define XAttrHandleID_Matrix    1000	//XAttributionHandle��Matirx

#define XAttrID_RebarResults	125		//�����XAttribution��ID
#define	XAttrID_InstanceInfo	225		//����CAEģ��ʵ��XAttribution��ID
#define XAttrID_MeshProperty	1		//��Ԫ���Ԫ������XAttribution��ID
#define XAttrID_PlotMeshProperty 20		//��Ԫ���Ԫ������XAttribution��ID
#define XAttrID_CutSurfaceMeshProperty 22	//��Ԫ���Ԫ������XAttribution��ID
#define XAttrID_CutFace			10		//��ͼ�ڵ�����XAttribution��ID
#define	XAttrID_DBFlag			333		//���ݿ��ʶ��XAttribution��ID

#define XAttrID_Matirx	 666		//XAttributionHandle��MajorID


#define MAX_COLUMN_COUNT 10
#define MAX_COLUMN_COUNT_REBAR 17

#define DB_NAME L"MsdiMsCaeSi.db"
#define DOCUMENT_NAME L"CAE�����ĵ�ģ��.docx"

// CAE����������
#define RESULTTYPE_USUM L"USUM"
#define RESULTTYPE_UX L"UX"
#define RESULTTYPE_UY L"UY"
#define RESULTTYPE_UZ L"UZ"
#define RESULTTYPE_S1 L"S1"
#define RESULTTYPE_S2 L"S2"
#define RESULTTYPE_S3 L"S3"
#define RESULTTYPE_SX L"SX"
#define RESULTTYPE_SY L"SY"
#define RESULTTYPE_SZ L"SZ"
#define RESULTTYPE_EPTOEQV L"EPTOEQV"
#define RESULTTYPE_EPTOX L"EPTOX"
#define RESULTTYPE_EPTOY L"EPTOY"
#define RESULTTYPE_EPTOZ L"EPTOZ"
#define RESULTTYPE_SEQV L"SEQV"
#define RESULTTYPE_EPTOXY L"EPTOXY"
#define RESULTTYPE_EPTOYZ L"EPTOYZ"
#define RESULTTYPE_EPTOXZ L"EPTOXZ"
#define RESULTTYPE_SXY L"SXY"
#define RESULTTYPE_SYZ L"SYZ"
#define RESULTTYPE_SXZ L"SXZ"

// ��������= �����Ч���&0Ӧ�����&0Ӧ����Ӧλ��&����Ӧ�����&������ǿ�ȶ�Ӧ���&������ǿ�ȶ�Ӧλ��&��������Ӧ�����&������&�ο�������
const int CALCRESULT_IDX_MAXTVALIDNO = 0;//�����Ч���
const int CALCRESULT_IDX_ZEROTNO = 1;//0Ӧ�����
const int CALCRESULT_IDX_ZEROP = 2;//0Ӧ����Ӧλ��
const int CALCRESULT_IDX_A = 3;//����Ӧ�����
const int CALCRESULT_IDX_CONCRNO = 4;//������ǿ�ȶ�Ӧ���
const int CALCRESULT_IDX_CONCRP = 5;//������ǿ�ȶ�Ӧλ��
const int CALCRESULT_IDX_ACT = 6;//��������Ӧ�����
const int CALCRESULT_IDX_AS = 7;//������
const int CALCRESULT_IDX_REFERAREA = 8;//�ο����
const int CALCRESULT_IDX_BENDING = 9;//���
const int CALCRESULT_IDX_AXIAL = 10; //����
const int CALCRESULT_IDX_XYSHEAR = 11;//XY�������
const int CALCRESULT_IDX_YZSHEAR = 12;//YZ�������
const int CALCRESULT_IDX_XZSHEAR = 13;//XZ�������

#pragma warning(disable:4189)

