#ifndef COMMONSTRUCT
#define COMMONSTRUCT
#include "BentlyCommonfile.h"

/*************************
* @brief    ��ͼ�ṹ��
*************************/
// ��ͼ�ڵ�
typedef struct ContourPoint
{
	int iIdx;			// ���ڽڵ���
	int NodeId;			// �ڵ�ID
	int NodeType;		// �ڵ�����
	DPoint3d dPt;		// �ڵ�����
	double dVal;		// ��ֵ���ֵ
	DPoint3d dVectorPt;	// ʸ������������ʸ��ͼ
} ContourPoint;

struct  PlotThread
{
	bvector<ContourPoint> pMeshFaces;
	UInt32 theColor;
};

struct  LineThread
{
	DSegment3d LinePt;
	UInt32 theColor;
};

struct  BorderLine
{
	DSegment3d LinePt;
	int Count;
};


/*************************
* @brief    ��ͼ�ýṹ��
*************************/
typedef struct CutPt
{
	int StartID;			// ��ʼ�ڵ���
	int EndID;			   // �յ����
	double StartRatio;    //������ʼ�������StartRatio = dis(dpt,startPt)/dis(startPt,EndPt)��
	DPoint3d dPt;		// �ڵ�����
} CutPt;

struct OutLineData
{
	DPoint3d EleMinPoint;// ��������½�����
	DPoint3d EleMaxPoint;// ��������Ͻ�����
	DPoint3d OutLineMinPoint;// ��ֵͼ������½�����
	DPoint3d OutLineMaxPoint;// ��ֵͼ������Ͻ�����
};

struct PointOutData
{
	DPoint3d dPt;// �ڵ�����
	bmap<WString, double> ResultVals;
	double dMaxS;// �����Ӧ��
	double dMinS;// ��С��Ӧ��
	double dAngleS;// �����Ӧ����X��ļн�
};

struct  DataListParamas//��ֵ������ṹ
{
	double cordFontSize;//���������С
	double DataFontSize;//��ֵ�����С
	double  CasFontSize;//���������С
	double  TitFontSize;//���������С
	int     Decimal; //С����λ��

	WString OutTypeName;//�������
	WString ListTitle;//��ֵ����
	WString TypeName;//ֵ������	
	WString CutFaceName;//��������
	WString CaseName;//������
	WString UnitName;//��λ��

	int PerXLen;//X�򻮷ֲ���
	int PerYLen;//Y�򻮷ֲ���

	bool isS1;
};

/*************************
* @brief    ������ؽṹ��
*************************/
/**
* @brief ��������
*/
struct MATERIAL
{
	/** ����ID ID*/
	int ID;

	/** �������� Name*/
	WString Name;

	/** �������� Type*/
	WString Type;

	/** EX EX*/
	double EX;

	/** NUXY NUXY*/
	double NUXY;

	/** DENS DENS*/
	double DENS;

	/** PRXY PRXY*/
	double PRXY;
};

/**
* @brief ��Ԫ����
*/
struct ET
{
	/** ��Ԫ���� Elemtype*/
	int ElemType;

	/** ��Ԫ�������� ElemTypeName*/
	WString ElemTypeName;
};

/**
* @brief ʵ����
*/
struct RLBLOCK
{
	/** ʵ������ RealConstantNumber*/
	int RealConstantNumber;

	/** ʵ�������� Property*/
	WString Property;
};

/**
* @brief ����ڵ���Ϣ�Ľṹ��
*/
struct NBLOCK
{
	/** �ڵ�� NodeNumber*/
	int NodeNumber;

	/** �ڵ����� FaceNumber*/
	int FaceNumber;

	/** �ڵ����� NodeType*/
	int NodeType;

	/** ����ߵĽڵ�λ�� LineLocation*/
	int LineLocation;

	/** �ڵ��X���� XPoint*/
	double XPoint;

	/** �ڵ��Y���� YPoint*/
	double YPoint;

	/** �ڵ��Z���� YPoint*/
	double ZPoint;
};

/**
* @brief ��Ԫ����Ϣ
*/
struct EBLOCK
{
	/** ���Ϻ� MaterialNumber*/
	int MaterialNumber;

	/** ��Ԫ���ͺ� ElemType*/
	int ElemType;

	/** ʵ������ RealConstantNumber*/
	int RealConstantNumber;

	/** ����� SectionIDAttrNumber*/
	int SectionIDAttrNumber;

	/** ����� ElemCoordSysNumber*/
	int ElemCoordSysNumber;

	/** ��Ԫ���� BirthDeathFlag*/
	int BirthDeathFlag;

	/** ʵ�����ú� SolidModelRefNumber*/
	int SolidModelRefNumber;

	/** ��Ԫ��״ ElemShapeFlg*/
	int ElemShapeFlg;

	/** �ڵ���Ŀ NodeCount*/
	int NodeCount;

	/** ����λ FieldTmp*/
	int FieldTmp;

	/** ��Ԫ�� ElemNumber*/
	int ElemNumber;

	/** �ڵ�1 Node1*/
	int Node1;

	/** �ڵ�2 Node2*/
	int Node2;

	/** �ڵ�3 Node3*/
	int Node3;

	/** �ڵ�4 Node4*/
	int Node4;

	/** �ڵ�5 Node5*/
	int Node5;

	/** �ڵ�6 Node6*/
	int Node6;

	/** �ڵ�7 Node7*/
	int Node7;

	/** �ڵ�8 Node8*/
	int Node8;
};

struct BBLOCK
{
	/** �ڵ�� NodeNumber*/
	int Id;
	WString NodeIdStr;
	int  BCType;
};

/**
* @brief ������Ƭ��Ϣ
*/
struct FACETS
{
	/** ��Ԫ�� ElemID*/
	int ElemID;

	/** ��Ԫ�ڵ���� FaceNumber*/
	int FaceNumber;

	/** ��� FaceID*/
	int FaceID;

	/** �ڵ�1 Node1*/
	int Node1;

	/** �ڵ�2 Node2*/
	int Node2;

	/** �ڵ�3 Node3*/
	int Node3;

	/** �ڵ�3 Node3*/
	int Node4;
};

struct ListBoxDatas
{
	bool needFresh;
	bvector<bvector<WString>> rows;
};

struct CutPoints
{
	int Num;
	int StartID[10];			// ��ʼ�ڵ���
	int EndID[10];			   // �յ����
	double StartRatio[10];    //������ʼ�������StartRatio = dis(dpt,startPt)/dis(startPt,EndPt)��
};


#endif // !1
