#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"
#include "MsdiMsCaeSicmd.h"

//�Ի�������ı���Ϣ����
#define TXT_AddRebarLine					"���������"
#define TXT_RebarName						"���������"
#define TXT_UserDefine						"�Զ���"
#define	TXT_DIALOG_ReabarTitle				"����������"
#define TXT_OUTPUTREBAR_PUSHBTN_Refresh		"ˢ��"
#define TXT_OUTPUTREBAR_PUSHBTN_Calculate	"����"
#define TXT_OUTPUTREBAR_PUSHBTN_OutPut		"���"
#define TXT_Tree_SectionList				"�����б�"
#define TXT_STRU_SignifyCoefficient			"�ṹ��Ҫ��ϵ��"
#define TXT_STRU_Coefficient				"�ṹϵ��"
#define TXT_RebarType						"�ֽ�����"
#define TXT_DesignCoefficient				"���״��ϵ��"
#define TXT_DefaultPointSpan				"Ĭ�ϵ��(mm)"
#define TXT_Concrete						"����������"
#define TXT_StressOfConcrete				"����������ǿ��(MPa)"
#define TXT_StressOfRebar					"�ֽ��ǿ��(MPa)"
#define TXT_SectionHeight					"����߶�(mm)"

#define XSIZE		(120*XC)				/* dialog width */
#define	YSIZE		(26*YC)					/* dialog height */
#define TreeW	    (20*XC)					/* width of tree items*/
#define TxtW	    (10*XC)					/* width of ComboBox items */ 
#define TXT_COL1_X	8*XC
#define TXT_COL2_X	TXT_COL1_X + 16 * XC + TxtW
#define TXT_COL3_X	TXT_COL2_X + 12 * XC + TxtW  
#define TXT_COL4_X	TXT_COL3_X + 18 * XC + TxtW
#define GroupY      D_ROW(4.4)				
#define GROUP_STRESS_W 56 * XC
#define GROUP_RESULT_W 33 * XC
#define GroupH		YSIZE-GroupY

DialogBoxRsc DIALOGID_RebarCalculate =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_ALWAYSSETSTATE | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOGID_CalRebar_RebarCalculate, NOPARENTID,
	TXT_DIALOG_ReabarTitle,
	{
		{{0,0,0,0},	PopupMenu, POPUPMENUID_CalRebar_PopuMenu,  HIDDEN, 0, "", ""},

		//�����б���
		{ { 0, 0, 0, 0}, Tree, TREEID_CalRebar_SectionList, ON, 0, "", "" },
		//�ָ���
		{ { TreeW, 0, 0, 0 }, Sash, SASHID_CalRebar_V, ON, 0, "", "" },
		//panel
		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_CalRebar_Detail, ON, 0, "", "" },
	}
}
extendedIntAttributes
{
	{
		{ EXTINTATTR_DLOGATTRS, DIALOGATTRX_NETDOCKABLE }
	}
};

DItem_TreeRsc TREEID_CalRebar_SectionList =
{
	NOHELP, MHELP,
	HOOKITEMID_CalRebar_SectionTree, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, CTPANELID_CalRebar_Detail, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_Tree_SectionList },
	}
};
DItem_SashRsc SASHID_CalRebar_V =
{
	NOHELP, MHELP,
	HOOKSASH_CalRebar_RebarCalculate_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};
DItem_ContainerPanelRsc CTPANELID_CalRebar_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG, CTPANELATTR_BOUNDINGBOX,
	CONTAINERID_CalRebar_Setting, 0,
	""
};
DItem_ContainerRsc CONTAINERID_CalRebar_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_Rebar_Setting
};

DialogItemListRsc DILISTID_Rebar_Setting =
{
	{
		// ��ǰѡ�й�����ʾ
		{ { TXT_COL1_X, D_ROW(1.0), 90*XC, 0 }, Text, TEXTID_CalRebar_Current, ON, 0, "", "" },

		//��Ͽ�
		{ { XC				,	GroupY, GROUP_STRESS_W, GroupH+D_ROW(2.6) }, GroupBox, 0, ON, 0, "Ӧ���ֲ�", "" },
		{ { XC+GROUP_STRESS_W,	GroupY, GROUP_RESULT_W, GroupH+D_ROW(2.6) }, GroupBox, 0, ON, 0, "������", "" },
		
		//�ֽ�����
		{ {TXT_COL1_X, D_ROW(2.0), TxtW, 0 }, ComboBox, COMBOBOXID_CalRebar_RebarType, ON, 0, "", "" },
		//�ֽ��ǿ��
		{ {TXT_COL2_X, D_ROW(2.0), TxtW*0.8, 0 }, Text, TEXTID_CalRebar_RebarTesion, ON, 0, "", "" },
		//����������
		{ {TXT_COL3_X, D_ROW(2.0), TxtW, 0 }, ComboBox, COMBOBOXID_CalRebar_ConcrType, ON, 0, "", "" },
		//����������ǿ��
		{ {TXT_COL4_X, D_ROW(2.0), TxtW*0.8, 0 }, Text, TEXTID_CalRebar_ConcrTesion, ON, 0, "", "" },

		//�ṹϵ��
		{ {TXT_COL1_X, D_ROW(3.2), TxtW, 0 }, Text, TEXTID_CalRebar_StruFt, ON, 0, "", "" },
		//�ṹ��Ҫ��ϵ��
		{ {TXT_COL2_X, D_ROW(3.2), TxtW*0.8, 0 }, Text, TEXTID_CalRebar_SignifyStruFt, ON, 0, "", "" },
		//���״��ϵ��
		{ {TXT_COL3_X, D_ROW(3.2), TxtW, 0 }, Text, TEXTID_CalRebar_DesignFt, ON, 0, "", "" },
		//����߶�
		{ {TXT_COL4_X, D_ROW(3.2), TxtW*0.8, 0 }, Text, TEXTID_CalRebar_SectionHeight, ON, 0, "", "" },

		//Ӧ���ֲ��б�
		//Ĭ�ϵ��
		{{12*XC, GroupY + 0.9*YC, 0.75*TxtW, 0 }, Text, TEXTID_CalRebar_DefPointSpan, ON, 0, "", "" },
		//ˢ��
		{{12*XC + 0.8*TxtW, GroupY + 0.4*YC, 0.7*BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_CalRebar_Refresh, ON, ALIGN_CENTER, "", ""},
		{{1.5*XC				 , GroupY+D_ROW(2.2), GROUP_STRESS_W * 0.44, GroupH}, ListBox, LISTBOXID_CalRebar_StressVal, ON, 0, "", ""},
		{{2.0*XC+GROUP_STRESS_W*0.44, GroupY+D_ROW(1.1), GROUP_STRESS_W * 0.56-1.5*XC, GroupH+D_ROW(1.8)}, Generic, GENERICID_CalRebar_CANVA, ON, 0, "", ""},

		//�������б�
		{{1.5*XC+GROUP_STRESS_W,GroupY+D_ROW(2.2), GROUP_RESULT_W - XC, GroupH-D_ROW(2.8)}, ListBox, LISTBOXID_CalRebar_CalResult, ON, 0, "", ""},

		//�ֽ����,ֱ������Ϣ
		{{1.5*XC+GROUP_STRESS_W,GroupY + GroupH - D_ROW(1.2), GROUP_RESULT_W - XC, D_ROW(2.8)}, ListBox, LISTBOXID_CalRebar_RebarInfo, ON, 0, "", ""},

	    //�Ƿ������ء�����������
		{ {1.5*XC+GROUP_STRESS_W,  GroupY + 0.8*YC, 0, 0 }, ToggleButton, TOGGLEID_CalRebar_calcStress, ON, 0, "������ء�����������", ""},	
		
		//����
		{{1.5*XC+GROUP_STRESS_W + GROUP_RESULT_W - BUTTON_STDWIDTH*1.2, GroupY + 0.4*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_CalRebar_Calculate, ON, ALIGN_CENTER, "", ""},

		//���
		//{{1.5*XC+GROUP_STRESS_W + GROUP_RESULT_W/2, GroupY + 0.4*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_CalRebar_OutPut, ON, ALIGN_CENTER, "", ""},
	}
};

//0 ��ѡ�У�-1 ѡ��
DItem_ToggleButtonRsc TOGGLEID_CalRebar_calcStress = 
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK,
	NOARG, NOMASK, NOINVERT,
	"",
	"g_rebarCalDlgInfo.bCalcStress"
};

//��ǰѡ��
DItem_TextRsc TEXTID_CalRebar_Current =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"��ǰѡ��",
	"g_rebarCalDlgInfo.strCurrentSel"
};

//����������Ԥ������ǽ�Ľ���ͼ
DItem_GenericRsc GENERICID_CalRebar_CANVA =
{
  NOHELP, MHELP, HOOKGENERIC_CalRebar_Canva, NOARG
};

DItem_TextRsc TEXTID_CalRebar_SignifyStruFt =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	16, "%-g", "%g", "0", "", NOMASK, NOCONCAT,
	TXT_STRU_SignifyCoefficient,
	"g_rebarCalDlgInfo.dStruSignifyFt"
};

DItem_TextRsc TEXTID_CalRebar_StruFt =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	16, "%-g", "%g", "0", "", NOMASK, NOCONCAT,
	TXT_STRU_Coefficient,
	"g_rebarCalDlgInfo.dStruFt"
};

DItem_TextRsc TEXTID_CalRebar_RebarTesion =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKTXTID_CalRebar_RebarTesion, NOARG,
	16, "%-g", "%g", "0", "", NOMASK, NOCONCAT,
	TXT_StressOfRebar,
	"g_rebarCalDlgInfo.dRebarTesion"
};

DItem_TextRsc TEXTID_CalRebar_ConcrTesion =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKTXTID_CalRebar_ConcrTesion, NOARG,
	16,"%-g", "%g", "0", "", NOMASK, NOCONCAT,
	TXT_StressOfConcrete,
	"g_rebarCalDlgInfo.dConcrTesion"
};

DItem_TextRsc TEXTID_CalRebar_DefPointSpan =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	16, "%-ld", "%ld", "1", "", NOMASK, NOCONCAT,
	TXT_DefaultPointSpan,
	"g_rebarCalDlgInfo.dDefPointSpan"
};

DItem_TextRsc TEXTID_CalRebar_DesignFt =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	16, "%-g", "%g", "0", "", NOMASK, NOCONCAT,
	TXT_DesignCoefficient,
	"g_rebarCalDlgInfo.dDesignFt"
};

DItem_TextRsc TEXTID_CalRebar_SectionHeight =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	16, "%-ld", "%ld", "1", "", NOMASK, NOCONCAT,
	TXT_SectionHeight,
	"g_rebarCalDlgInfo.dSectionHeight"
};


StringList	STRINGLISTID_CalRebar_RebarType = 
{
  4,
  {
    {{0}, "HPB235" },
	{{0}, "HPB300"},
	{{0}, "HRB335" },
	{{0}, "HRB400"},
	{{0}, "RRB400" },
	{{0}, "HRB500�ݽ�"},
	{{0}, "HRB500����"},
	{{0}, TXT_UserDefine},
  }
};

DItem_ComboBoxRsc COMBOBOXID_CalRebar_RebarType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CalRebar_RebarType,
	NOARG, 6, "", "", "", "", NOMASK, STRINGLISTID_CalRebar_RebarType, 10, 0,
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	TXT_RebarType,
	"g_rebarCalDlgInfo.iRebarType",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

StringList STRINGLISTID_CalRebar_ConcrType = 
{
  4,
  {
    {{0}, "������C10" },
	{{0}, "������C15" },
	{{0}, "������C20" },
	{{0}, "������C25" },
	{{0}, "������C30" },
	{{0}, "������C35" },
	{{0}, "������C40" },
	{{0}, "������C45" },
	{{0}, "������C50" },
	{{0}, "������C55" },
	{{0}, "������C60" },
	{{0}, TXT_UserDefine},
  }
};

DItem_ComboBoxRsc COMBOBOXID_CalRebar_ConcrType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CalRebar_ConcrType,
	NOARG, 10, "", "", "", "", NOMASK, STRINGLISTID_CalRebar_ConcrType, 10, 0,
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	TXT_Concrete,
	"g_rebarCalDlgInfo.iConcrType",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

StringList STRINGLISTID_CalRebar_Diameter = 
{
  4,
  {
    {{0}, "6" },
	{{0}, "6.5" },
	{{0}, "8" },
	{{0}, "10" },
	{{0}, "12" },
	{{0}, "14" },
	{{0}, "16" },
	{{0}, "18" },
	{{0}, "20" },
	{{0}, "22" },
	{{0}, "25" },
	{{0}, "28"},
	{{0}, "32" },
	{{0}, "36"},
	{{0}, "40" },
	{{0}, "50"},
  }
};
DItem_ComboBoxRsc COMBOBOXID_CalRebar_ListCell_Diameter =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_CalRebar_CellCombbox_Diameter,
	NOARG, 10, "", "", "", "", NOMASK, STRINGLISTID_CalRebar_Diameter, 10, 0,
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	"",
	"g_rebarCalDlgInfo.iDameterIndex",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ListBoxRsc LISTBOXID_CalRebar_StressVal =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION|
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON|LISTATTR_EDITABLE ,
    5, 0, "",
    {
		{4*XC, 2, ALIGN_LEFT,	"No"	 },
		{8*XC, 20, ALIGN_LEFT,	"����(mm)"	 },
		{8*XC, 20, ALIGN_LEFT,  "Ӧ��(MPa)"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_CalRebar_CalResult =
{
    NOHELP, MHELP, HOOKLISTBOX_CalRebar_CalcResult, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION|
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{3*XC, 2, ALIGN_LEFT,	"No"	 },
		{18*XC, 36, ALIGN_LEFT,	"������"	 },
		{10*XC, 40, ALIGN_LEFT,  "���"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_CalRebar_RebarInfo =
{
    NOHELP, MHELP, HOOKLISTBOX_CalRebar_RebarInfo, NOARG,
    LISTATTR_EDITABLE| LISTATTR_GRID | LISTATTR_NOSELECTION|
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON|LISTATTR_INDEPENDENTCOLS  ,
    5, 0, "",
    {
		{10*XC, 15, ALIGN_LEFT,	"����"	 },
		{10*XC, 15, ALIGN_LEFT,  "ֱ��"	 },
		{12*XC, 30, ALIGN_LEFT,  "������"	 },
    }
};

DItem_TextRsc TEXTID_CalRebar_ListCell_RebarNumber =
{
   NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_CalRebar_CellTxt_RebarNumber, NOARG, 
   16, "%-ld", "%ld", "1", "", NOMASK, NOCONCAT, "",  "g_rebarCalDlgInfo.iRebarCount"
};

DItem_PushButtonRsc PUSHBTNID_CalRebar_OutPut = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_CalRebar_OutPut, NOARG, NOCMD, LCMD, "", TXT_OUTPUTREBAR_PUSHBTN_OutPut  
};

DItem_PushButtonRsc PUSHBTNID_CalRebar_Calculate = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_CalRebar_Calculate, NOARG, CMD_MSDI_MSCAESI_ACTION_CALCREBAR, LCMD, "", TXT_OUTPUTREBAR_PUSHBTN_Calculate 
};

DItem_PushButtonRsc PUSHBTNID_CalRebar_Refresh = 
{ 
	DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_CalRebar_Refresh, NOARG, NOCMD, LCMD, "", TXT_OUTPUTREBAR_PUSHBTN_Refresh 
};

//�Ҽ��˵�
DItem_PopupMenuRsc POPUPMENUID_CalRebar_PopuMenu = 
    {
    NOHELP, LHELP, NOHOOK, NOARG, 0,
    PulldownMenu, PDMID_CalRebar_PopuMenu    
    };

DItem_PulldownMenuRsc PDMID_CalRebar_PopuMenu = 
{
    NOHELP, OHELPTASKIDCMD,
    HOOKPULLMENU_CalRebar_PopuMenu,
    ON | ALIGN_LEFT, "",
	{	
		{"���������", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_CalRebar_AddRebarLine, MSID_AddRebarLine,	NOCMD, OTASKID, ""},
		{"ɾ�������", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_CalRebar_DelRebarLine, MSID_DelRebarLine,	NOCMD, OTASKID, ""},
	}
};

//��������߶Ի���
DialogBoxRsc DIALOGID_AddRebarLine =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_MODAL,
	40*XC, 6*YC,
	NOHELP, MHELP,
	HOOKDIALOGID_CalRebar_AddRebarLine, NOPARENTID,
	TXT_AddRebarLine,
	{
		{ {13*XC, D_ROW(1.6), 23*XC, 0 }, Text, TEXTID_CalRebar_AddLine_Name, ON, 0, "", "" },
		{ {10*XC, D_ROW(3.0), BUTTON_STDWIDTH, 0},	PushButton,  PUSHBUTTONID_OK, ON,  0, "", ""},
		{ {22*XC, D_ROW(3.0), BUTTON_STDWIDTH, 0},	PushButton,  PUSHBUTTONID_Cancel, ON,  0, "", ""},
	}
}

DItem_TextRsc TEXTID_CalRebar_AddLine_Name =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	30, "", "", "", "", NOMASK, NOCONCAT,
	TXT_RebarName,
	"g_rebarCalDlgInfo.rebarName"
};

#undef DIALOG_W
#undef DIALOG_H
#undef TreeW
#undef GroupY
#undef GROUP_STRESS_W
#undef GROUP_RESULT_W
#undef GroupH
#undef TxtW
#undef TXT_COL1_X
#undef TXT_COL2_X
#undef TXT_COL3_X
#undef TXT_COL4_X