/*--------------------------------------------------------------------------------------+
|   clipResultShowDlg.r
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"
/*----------------------------------------------------------------------+
|��Ŀ����Ի���                 |
-----------------------------------------------------------------------*/
#define XSIZE       (70*XC)
#define YSIZE       (20*YC)

#define TREE_W		(18*XC)
#define LABEL_W		(12*XC)
#define TEXT_W		(6*XC)
#define GAP_X		(XC)
#define GAP_Y		(0.5*YC)

#define TABPAGE_X	(GAP_X)

#define TXT_CLIPRESULT_DIALOG	"��ֵ���"
#define TXT_CLIPRESULT_TREE		"�����б�"
#define	TXT_CLIPRESULT_TABPAGE1	"�ѿ�������ϵ"
#define	TXT_CLIPRESULT_TABPAGE2	"������ϵ"
#define TXT_CLIPRESULT_PUSHBTN_Clear "���"
#define TXT_CLIPRESULT_PUSHBTN_Submit "Ԥ��"
#define TXT_CLIPRESULT_PUSHBTN_OutDGN "����"

/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_ClipResultShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOG_ClipResult, NOPARENTID,
	TXT_CLIPRESULT_DIALOG,
	{
		//�����б���
		{ { 0, 0, 0, 0 }, Tree, TREEID_ClipResult_ClipList, ON, 0, "", "" },

		{ { TREE_W, 0, 0, 0 }, Sash, SASHID_ClipResult_V, ON, 0, "", "" },

		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_ClipResult_Detail, ON, 0, "", "" },	
	}
}
extendedIntAttributes
{
	{
		{ EXTINTATTR_DLOGATTRS, DIALOGATTRX_NETDOCKABLE }
	}
};

/*----------------------------------------------------------------------+
|                                                                       |
|   Sash Item Resources                                                 |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_SashRsc SASHID_ClipResult_V =
{
	NOHELP, MHELP,
	HOOKSASH_ClipResult_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ContainerPanel Item Resources                                       |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ContainerPanelRsc CTPANELID_ClipResult_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG,	0,
	CONTAINERID_ClipResult_Setting, 0,
	""
};

DItem_ContainerRsc CONTAINERID_ClipResult_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_ClipResult_Setting
};

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemList Resources                                            |
|                                                                       |
+----------------------------------------------------------------------*/

DialogItemListRsc DILISTID_ClipResult_Setting =
{
	{
		//��ǰѡ�й�����ʾ
		{ { LABEL_W, D_ROW(1.0), 0 , 0 }, Text, TEXTID_ClipResult_Current, ON, 0, "", "" },

		//��ֵͼ����
		{ { LABEL_W, D_ROW(2.0), 2*LABEL_W + 3*TEXT_W, 0 }, Text, TEXTID_ClipResult_TableTitle, ON, 0, "", "" },

		//�������ֺ� 
		{ { LABEL_W, D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_TitleFont, ON, 0, "", "" },
		//�������ֺ�
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_ContentFont, ON, 0, "", "" },

		//X��������&Y��������&���������С
		{ { LABEL_W, D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_XSpaceCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_YSpaceCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_CoorFont, ON, 0, "", "" },

		//X��ֵ����&Y��ֵ����&��ֵ�����С
		{ { LABEL_W, D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_XValueCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_YValueCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_ValueFont, ON, 0, "", "" },

		//С����λ��
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_Decimal, ON, 0, "", "" },

		//�ѿ�����&������
		{ { TABPAGE_X, D_ROW(6.0), 3*(LABEL_W + TEXT_W) - GAP_X, 10*YC}, TabPageList, TPLISTID_ClipResult_Main, ON, 0, "", ""},

		//���
		{ { LABEL_W*1/4 +(LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_Clear, ON, ALIGN_CENTER, "", ""},	

		//Ԥ��
		{ { LABEL_W*101/90 + (LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_Submit, ON, ALIGN_CENTER, "", ""},	
		
		//����
		{ { LABEL_W*2 + (LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_OutDGN, ON, ALIGN_CENTER, "", ""},	
	}
}

/*----------------------------------------------------------------------+
|									|
|   Tree Resources							|
|									|
+----------------------------------------------------------------------*/
DItem_TreeRsc TREEID_ClipResult_ClipList =
{
	NOHELP, MHELP,
	HOOKTREE_ClipResult_ClipList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_EDITABLE | TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_CLIPRESULT_TREE },
	}
};

//��ǰѡ��
DItem_TextRsc TEXTID_ClipResult_Current =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"��ǰѡ��",
	"g_clipResultDlgInfo.current"
};

//��ֵͼ����
DItem_TextRsc TEXTID_ClipResult_TableTitle =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, NOCONCAT,
	"��ֵͼ����",
	"g_clipResultDlgInfo.tableTitle"
};

//�������ֺ�
DItem_TextRsc TEXTID_ClipResult_TitleFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"�������ֺ�",
	"g_clipResultDlgInfo.titleFont"
};

//�������ֺ�
DItem_TextRsc TEXTID_ClipResult_ContentFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"�������ֺ�",
	"g_clipResultDlgInfo.contentFont"
};

//�߶�ϵ��
DItem_TextRsc TEXTID_ClipResult_HeightCoe =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"�߶�ϵ��",
	"g_clipResultDlgInfo.heightCoe"
};

//X���߶���
DItem_TextRsc TEXTID_ClipResult_XSpaceCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"X���߶���",
	"g_clipResultDlgInfo.XSpaceCnt"
};

//Y���߶���
DItem_TextRsc TEXTID_ClipResult_YSpaceCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"Y���߶���",
	"g_clipResultDlgInfo.YSpaceCnt"
};

//��������С
DItem_TextRsc TEXTID_ClipResult_CoorFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"����ֺ�",
	"g_clipResultDlgInfo.coorFont"
};

//X����ֵ����
DItem_TextRsc TEXTID_ClipResult_XValueCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"X����(mm)",
	"g_clipResultDlgInfo.XValueCnt"
};

//Y����ֵ����
DItem_TextRsc TEXTID_ClipResult_YValueCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"Y����(mm)",
	"g_clipResultDlgInfo.YValueCnt"
};
//С����λ��
DItem_TextRsc TEXTID_ClipResult_Decimal =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"С����λ��",
	"g_clipResultDlgInfo.decimal"
};

//��ֵ�����С
DItem_TextRsc TEXTID_ClipResult_ValueFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"��ֵ�ֺ�",
	"g_clipResultDlgInfo.valueFont"
};

//TabPage�б���
DItem_TabPageListRsc  TPLISTID_ClipResult_Main =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_DEFAULT,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ClipResult_Cartesian,		ON, 0,"",""},		//�ѿ���
		{{0,0,0,0}, TabPage, TABPAGEID_ClipResult_Cylind,		ON, 0,"",""},			//������	
	}								
};	

DItem_TabPageRsc TABPAGEID_ClipResult_Cartesian =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKTABPAGE_ClipResult_ResultType, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_CLIPRESULT_TABPAGE1,
	{	
		{ { TABPAGE_X + GAP_X, D_ROW(7.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_SX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_SY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_SZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(8.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_EPTOXY, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_EPTOYZ, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_EPTOXZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(9.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_UX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_UY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_UZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(10.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_SC, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(10.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_ALL, ON, 0, "", ""},	
	}
};

DItem_TabPageRsc TABPAGEID_ClipResult_Cylind =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKTABPAGE_ClipResult_ResultType, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_CLIPRESULT_TABPAGE2,
	{	
		{ { TABPAGE_X + GAP_X, D_ROW(7.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_SR, ON, 0, "", ""},// ����	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SH, ON, 0, "", ""},// ����
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SA, ON, 0, "", ""},// ����

		{ { TABPAGE_X + GAP_X, D_ROW(8.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_SRA, ON, 0, "", ""},	// ������	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SAH, ON, 0, "", ""},// ������
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SRH, ON, 0, "", ""},// ������	

		{ { TABPAGE_X + GAP_X, D_ROW(9.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_UX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_UY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_UZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(10.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SC, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(10.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_ALL, ON, 0, "", ""},	
	}
};

//�ѿ�����- X����Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"X����Ӧ��",
	"g_clipResultDlgInfo.D_SX"
};

//�ѿ�����- Y����Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Y����Ӧ��",
	"g_clipResultDlgInfo.D_SY"
};

//�ѿ�����- Z����Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Z����Ӧ��",
	"g_clipResultDlgInfo.D_SZ"
};

//�ѿ�����- ����Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SC = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"����Ӧ��",
	"g_clipResultDlgInfo.D_SC"
};

//�ѿ�����- XY���Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOXY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"XY���Ӧ��",
	"g_clipResultDlgInfo.D_SXY"
};

//�ѿ�����- YZ���Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOYZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"YZ���Ӧ��",
	"g_clipResultDlgInfo.D_SYZ"
};

//�ѿ�����- XZ���Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOXZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"XZ���Ӧ��",
	"g_clipResultDlgInfo.D_SXZ"
};

//�ѿ�����- X��λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"X��λ��",
	"g_clipResultDlgInfo.D_UX"
};

//�ѿ�����- Y��λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Y��λ��",
	"g_clipResultDlgInfo.D_UY"
};

//�ѿ�����- Z��λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Z��λ��",
	"g_clipResultDlgInfo.D_UZ"
};

//�ѿ�����- ȫ��ѡ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_ALL = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKOPTBUTTON_ClipResult_D_ALL, 
	NOARG, NOMASK, NOINVERT,	
	"ȫ��ѡ��",
	"g_clipResultDlgInfo.D_All"
};

//������- ������Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SR = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"������Ӧ��",
	"g_clipResultDlgInfo.Z_SR"
};

//������- ������Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"������Ӧ��",
	"g_clipResultDlgInfo.Z_SH"
};

//������- ������Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SA = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"������Ӧ��",
	"g_clipResultDlgInfo.Z_SA"
};

//������- �������Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SRA = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"��������Ӧ��",
	"g_clipResultDlgInfo.Z_SRA"
};

//������- �ỷ���Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SAH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"��������Ӧ��",
	"g_clipResultDlgInfo.Z_SAH"
};
//������- �������Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SRH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"��������Ӧ��",
	"g_clipResultDlgInfo.Z_SRH"
};

//������- ����λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"����λ��",
	"g_clipResultDlgInfo.Z_UX"
};

//������- ����λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"����λ��",
	"g_clipResultDlgInfo.Z_UY"
};

//������- ����λ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"����λ��",
	"g_clipResultDlgInfo.Z_UZ"
};

//������- ����Ӧ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SC = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"����Ӧ��",
	"g_clipResultDlgInfo.Z_SC"
};

//������- ȫ��ѡ��
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_ALL = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKOPTBUTTON_ClipResult_Z_ALL, 
	NOARG, NOMASK, NOINVERT,	
	"ȫ��ѡ��",
	"g_clipResultDlgInfo.Z_All"
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_Clear = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_Clear, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_Clear 
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_Submit = 
{ 
	DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_Submit, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_Submit  
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_OutDGN = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_OutDGN, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_OutDGN  
};

#undef XSIZE
#undef YSIZE
#undef TREE_W
#undef LABEL_W
#undef TEXT_W
#undef GAP_X
#undef GAP_Y
#undef TABPAGE_X