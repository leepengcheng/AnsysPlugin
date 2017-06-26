/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSi.h
|
+--------------------------------------------------------------------------------------*/
#pragma once

#include <Mstn\MicroStation.r.h>

//TODO: header file
#if !defined (resource)

typedef struct impInfDlgInfo
{
	int		MeshModelVal;
	int		ResultVal;
	int		ProjectId;
} ImpInfDlgInfo;

typedef struct solidDispDlgInfo
{
	WChar	current[256];	//��ǰ������
	int		ResultType;		//��Ҫ��ʾ�ļ���������
	int		ShowType;		//��ʾ���ͣ�����ͼ����ֵ��ͼ��ʸ��ͼ��
	int		ShowModel;		//��ʾģʽ�����⻬������
	int		bShowExtremum;	//�Ƿ���ʾ���/��Сֵ:0-����ʾ,-1-��ʾ
	int		indexCType;		//����չʾ�������±�
	int		indexCOpt;		//����չʾ���������±�
	int		indexCValue;	//����չʾ����ֵ�±�
} SolidDispDlgInfo;

typedef struct surfaceShowDlgInfo
{
	WChar	current[256];	//��ǰ������
	int		indexReType;	//��Ҫ��ʾ�ļ���������
	int		indexShowType;	//��ʾ���ͣ�����ͼ����ֵ��ͼ��ʸ��ͼ��
	int		indexShowModel;	//��ʾģʽ�����⻬������
	int		bShowExtremum;	//�Ƿ���ʾ���/��Сֵ:0-����ʾ,-1-��ʾ
}SurfaceShowDlgInfo;

typedef struct expCalculationDlgInfo
{
	int		proIndex;		//����ģ��comboBox�±�
	char	proDescr[4096];	//���̸ſ�
	int		nCellCnt;		//�ܵ�Ԫ��
	int		nNodeCnt;		//�ڵ���

}ExpCalculationDlgInfo;

typedef struct reBarCalDlgInfo
{
	WChar	strCurrentKey[256];			//��ǰ�ڵ�Key
	WChar	rebarName[256];			    //��������ƣ�����
	WChar	strCurrentSel[256];			//��ǰѡ��
	double  dStruFt;					//�ṹϵ��
	double  dStruSignifyFt;				//�ṹ��Ҫϵ��
	double  dDesignFt;					//���״��ϵ��
	int		dDefPointSpan;				//Ĭ�ϵ��(mm)
	int		iRebarType;					//�ֽ������±꣬���ڻ�ȡfy
	double  dRebarTesion;				//�ֽ��ǿ��
	int		iConcrType;					//�����������±꣬���ڻ�ȡft
	double  dConcrTesion;				//���������Ŀ���ǿ��
	int		dSectionHeight;				//����߶�
	double	dDiameter;					//�ֽ�ֱ��
	int     iDameterIndex;				//�ֽ�listbox�е�0��1�����õ�commbox�±�
	int     iRebarCount;				//�ֽ����
	bool    bCalcStress;				//�Ƿ������ء�����������
} ReBarCalDlgInfo;

typedef struct partialDisDlgInfo
{
	int		indexCName;		//������comboBox�±�
	int		indexCOpt;		//����������comboBox�±�
	int		indexCVal;		//����ֵcomboBox�±�
}PartialDisDlgInfo;

typedef struct caseCompDlgInfo
{
	int		indexInstanL;	//���ʵ��comboBox�±�
	int		indexInstanR;	//�Ҳ�ʵ��comboBox�±�
	int		indexReType;	//�������comboBox�±�
	int		indexShowType;	//��ʾ����comboBox�±�
	int		ShowModel;		// ��ʾģʽ
}CaseCompDlgInfo;

typedef struct clipResultDlgInfo
{
	WChar	current[256];
	WChar	tableTitle[256];
	double	titleFont;
	double	contentFont;
	double	heightCoe;
	int		XSpaceCnt;
	int		YSpaceCnt;
	double	coorFont;
	int		XValueCnt;
	int		YValueCnt;
	double	valueFont;
	int     decimal;

	//�ѿ�������������,0�����ѡ��, -1����ѡ��
	int		D_SX;
	int		D_SY;
	int		D_SZ;
	int		D_SC;
	int		D_SXY;
	int		D_SYZ;
	int		D_SXZ;
	int		D_UX;
	int		D_UY;
	int		D_UZ;
	int		D_All;

	//�������������,0�����ѡ��, -1����ѡ��
	int		Z_SR;
	int		Z_SA;
	int		Z_SH;
	int		Z_SRA;
	int		Z_SAH;
	int     Z_SRH;
	int		Z_SC;
	int		Z_UX;
	int		Z_UY;
	int		Z_UZ;
	int		Z_All;

	// ���������ò���
	WChar	fileOpenPath[256]; // �ļ��򿪻����Ĭ��·��
}ClipResultDlgInfo;

typedef struct outRebarDlgInfo
{
	//���ڸý���ؼ��󶨱������٣���˰Ѷ������İ󶨱����������ڴ�
	int		indexInstance;

	bool     clipPlotNormalSide;
	//clipFace������
	WChar	clipName[256];
	int		chooseBtnIndex;
}OutRebarDlgInfo;

typedef struct colorSettingDlgInfo
{
	double	dStartValue;
	double	dEndValue;
	int		iColorNum;
	int		bAutomatic;
	int		Gray;//�Ҷ���ʾ
}ColorSettingDlgInfo;

typedef	struct treexmplinfo
{
	int         containerId;
	void        *pSingleLM;
	void        *pMultiLM;
	void        *pSingleLBRiP;
	void        *pMultiLBRiP;
	void        *pTreeModel;
	int         minSashX;
	int         maxSashX;
	double      range1;
	int         range2;
	double      comboBoxValue;
	int         radioButtonValue;
	double      spinBoxValue;
	//  long        treeComboBoxValue;
	WChar       treeComboBoxValue[256];

	unsigned long   toggle1 : 1;
	unsigned long   toggle2 : 1;
	unsigned long   toggle3 : 1;
	unsigned long   resBits : 29;

} TreeXmplInfo;

typedef struct exportinfo
{
	int             version;
	double          numerator;                      /* Units per meter fraction numerator */
	double          denominator;                    /* Units per meter fraction denominator */
}ExportInfo;

typedef struct instanceInfo
{
	WChar	NgName[256];
	ModelId	modelId;
	int		projectId;
}InstanceInfo;

typedef struct rebarInfo
{
	int		projectID;					//��ĿID
	int		caseID;						//����ID
	int		SectionID;					//����ModelID
	WChar   NgName[1024];				//����Model��
	ElementId ElemID;					//Ԫ��ID
	WChar   caseName[1024];				//��������WChar
	WChar	sectionName[1024];			//��������WChar
	WChar	rebarName[1024];			//��������ƣ�����WChar
	double	dLineLength;				//����߳���
	double  dStruSignifyFt;				//�ṹ��Ҫϵ��
	double  dDesignFt;					//���״��ϵ��
	double  dStruFt;					//�ṹϵ��
	double  dDefPointSpan;				//Ĭ�ϵ��(mm)
	int		iRebarType;					//�ֽ������±꣬���ڻ�ȡfy
	double  dRebarTesion;				//�ֽ��ǿ��
	int		iConcrType;					//�����������±꣬���ڻ�ȡft
	double  dConcrTesion;				//���������Ŀ���ǿ��
	int		dSectionHeight;				//����߶�
	bool    bCalcStress;				//�Ƿ������ء�����������
	WChar	strStress[2048];			//SY,��&�ָ�,����������

	double  bendingStress;				//���
	double  axialStress;				//����
	WChar   shearSXY[2048];				//SXY,��&�ָ�,���ڼ�������
	WChar	shearSYZ[2048];				//SYZ,��&�ָ�
	WChar	shearSXZ[2048];				//SXZ,��&�ָ�

	WChar	strResult[2048];			//������,��&�ָ�
	int		iRebarCount;				//�ֽ����
	double	dRebarDiameter;				//�ֽ�ֱ��
	double	dRebarArea;					//�ֽ����
} ReBarInfo;

#endif



