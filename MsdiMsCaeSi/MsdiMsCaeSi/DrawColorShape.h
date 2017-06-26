#pragma once
/**************************************************
* @file     DrawColorShape.h
* @brief    ����ʱ��ɫ�������ֵ������ࡢ�����ͱ���
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*************************************************/
/** ���ȳ���ֵ TABLE_WIDTH*/
#define TABLE_WIDTH   143
/** ��߶ȳ���ֵ TABLE_WIDTH*/
#define TABLE_HEIGHT  160*2
/** ��ɫ���ȳ���ֵ TABLE_WIDTH*/
#define Color_WIDTH   60*0.4

#define TABLE_OFFSETX  -200  

#define TABLE_OFFSETY   50

#define TABLE_LINCOLOR  0x00000000

#define TABLE_HEADERX   TABLE_WIDTH / 5 
#define TABLE_HEADERY   15 

#define TABLE_UNITY     14

/**
* @brief ��ͼ���ۺ��࣬�����˻�ͼ�������ز�����������CreateTable���л�ͼ��
*/
struct DrawColorShape : IViewDecoration
{
public:
	/** ��ɫ����ɫ������ ColorNum_Legeng*/
	int ColorNum_Legeng;
	/** ��ɫ����ɫ������ Color_num*/
	int Color_num;
	/** ��ɫ�����������ɫ��� fillColorTBGR*/
	UInt32 fillColorTBGR[100];
	/** ��ɫ�����������ɫ fillColor*/
	RgbColorDef  fillColor[100];
	/** rgb��ɫ��Ӧ����ɫ��� fillCororIndex*/
	UInt32    fillCororIndex[100];
	/** ��ʼ����ֵ StartValue*/
	double StartValue;
	/** ��������ֵ EndValue*/
	double EndValue;
	/** ��ʼ����ֵ��DB�� StartValue*/
	double DbStartValue;
	/** ��������ֵ(DB) EndValue*/
	double DbEndValue;
	/** ��������ֵ(ͼ����ʾ) ClrShapeVal_Legeng*/
	double ClrShapeVal_Legeng[100];
	/** ��������ֵ ClrShapeVal*/
	double ClrShapeVal[100];

	/** ���ݿ��н������Code*/
	WString ResultTypeCode;
	/** ��ͷ Header*/
	WString Header;
	/** ��Ӧ��λ Unit*/
	WString Unit;

	Point2d DrawPos;

	int showType;

	/**
	* @brief ����ʱԪ�غ���
	* @param[in] viewport     ��Ӧ��ͼָ��
	* @return ���ػ�ͼ�ɹ����
	*/
	virtual bool _DrawDecoration(IndexedViewportR viewport);

	/** ��ӦCAEģ��ID ProjectId*/
	int ProjectId;

	/** ��ӦCAEģ�͵�λ ProjectUnit*/
	WString ProjectUnit;

	/** ��ӦCAEģ���� ProjectName*/
	WString ProjectName;

	/** ��Ӧ���� CaseID*/
	int CaseID;

	/** ��Ӧ������ CaseName*/
	WString CaseName;

	/** ��ӦCAEģ�͵������� NgName*/
	WString NgName;
	
	/** ��ֵ��ʶ��ر��� **/
	//�Ƿ���Ƽ�ֵ������ʶ��
	bool m_bExtremum;
	//���ֵ���������
	bvector<DPoint3d> m_vecMaxPts;
	//��Сֵ���������
	bvector<DPoint3d> m_vecMinPts;

	/** �ڵ��ʶ��ر��� **/
	//�Ƿ���ƽڵ���Ϣ��ʶ��
	bool m_bNodeView;
	bool m_bNodeView_Temp;
	//���ƽڵ������-��ʱ��
	DPoint3d m_NodePt_Temp;
	//���ƽڵ���Ϣ-��ʱ��
	double m_dNodeInfo_Temp;
	//���ƽڵ������-�����
	vector<DPoint3d> m_vecNodePts;
	//���ƽڵ���Ϣ-�����
	vector<double> m_vecNodeInfos;

	//��־λ���Ƿ�����Զ����ã��Զ�������������ݿ����ֵ��Χ�Զ����м���
	bool m_bAutomatic;
	bool m_Gray;//��־λ�Ƿ����ûҶ���ʾ��ͼ
	//�Ƿ����ɫ����ʶ��
	bool m_bDisplayed;

	void initialize();

	//��ʼ����ɫ��
	void StartDrawFunction();

	//�رջ���ɫ��
	void CloseDrawFunction();

	void divideColorRange();

	void GetContourLineColorValue(int colornum, double out_value[], double in_value[]);

	void initColorRangeFromDB();
};

/** ���廭��ɫ���ྲ̬���� s_DrawColorShape*/
extern DrawColorShape   s_DrawColorShape;
extern DrawColorShape	s_DrawColorShape_temp;
extern DrawColorShape*	s_pDCShape;

/** ��������ɫ��ָ�� ppPalette*/
extern DgnPlatform::BSIColorPalette*  ppPalette;

/**
* @brief ��ʱ��ɫ������
*/
class CreateTable
{
public:
	static void DrawTextInShape(DPoint3dCR origin_pos, WString drawtext, RotMatrixCR invRotation, TextStringPropertiesPtr pProp,
		IViewOutputP output, IndexedViewportR viewport);
	
	/**
	* @brief ������ֵ�������еĺ���
	* @param[in] HeaderText   ��ͷ����
	* @param[in] ColorNum     ��ɫ������
	* @param[in] StartValue   ��ʼ��ֵ
	* @param[in] EndValue     ��ֹ��ֵ
	* @param[in] output       ��Ӧ��ͼ��ͼ��ָ��
	* @param[in] viewport     ��Ӧ��ͼָ��
	* @return �޷���ֵ
	*/
	static void CreateAllText(WCharCP projectname, WCharCP casename, WCharCP HeaderText, WCharCP UnitText, int ColorNum, double ClrShapeVal_Legeng[],
		IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos);
	/**
	* @brief ����ɫ�黭�����еĺ���
	* @param[in] ColorNum     ��ɫ������
	* @param[in] lineColorTBGR   ��ɫ���Ե�ߵ���ɫ
	* @param[in] fillColorTBGR     ������ɫ����ɫ����ָ��
	* @param[in] output       ��Ӧ��ͼ��ͼ��ָ��
	* @param[in] viewport     ��Ӧ��ͼָ��
	* @return �޷���ֵ
	*/
	static void CreateAllColorTable(int ColorNum, UInt32 lineColorTBGR, UInt32 fillColorTBGR[],
		IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos);

	static double tablewidth;
};

