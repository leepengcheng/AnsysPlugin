/*--------------------------------------------------------------------------------------+
|   SolidContourDispDlg.r
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
#include "MsdiMsCaeSicmd.h"

/*----------------------------------------------------------------------+
|��Ŀ����Ի���                 |
-----------------------------------------------------------------------*/
#define XSIZE       (62*XC)
#define YSIZE       (16*YC)

#define TW	    (14*XC)		/* width of ��Ŀ���� items */
#define CW	    (13*XC)		/* width of ComboBox items */
#define MAXCHKW (10*XC)		/* width of ComboBox items */

#define CASEX	8*XC
#define TYPEX	CASEX+CW+8*XC
#define MAXCHKX	CASEX+CW+1*XC
#define SETBTX	TYPEX+CW
#define SHOWBTX	CASEX+CW/8
#define CLIPBTX	SHOWBTX+BUTTON_STDWIDTH+3*XC
#define NODEBTX	CLIPBTX+BUTTON_STDWIDTH+3*XC
#define DAMAGE	CLIPBTX+BUTTON_STDWIDTH+4*XC
/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_SolidContourDisp =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOGID_SolidContourDisp, NOPARENTID,
	TXT_TreeDialogTitle,
	{
		//ʵ��&�����б���
		{ { 0, 0, 0, 0 }, Tree, TREEID_SolidDis_ProjList, ON, 0, "", "" },

		{ { TW, 0, 0, 0 }, Sash, SASHID_SolidDis_V, ON, 0, "", "" },

		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_SolidDis_Detail, ON, 0, "", "" },
	}
};

/*----------------------------------------------------------------------+
|                                                                       |
|   Sash Item Resources                                                 |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_SashRsc SASHID_SolidDis_V =
{
	NOHELP, MHELP,
	HOOKSASH_SolidDisp_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ContainerPanel Item Resources                                       |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ContainerPanelRsc CTPANELID_SolidDis_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG,	0,
	CONTAINERID_SolidDis_Setting, 0,
	""
};

DItem_ContainerRsc CONTAINERID_SolidDis_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_SolidDis_Setting
};

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemList Resources                                            |
|                                                                       |
+----------------------------------------------------------------------*/

DialogItemListRsc DILISTID_SolidDis_Setting =
{
	{
		//��ǰѡ�й�����ʾ
		{ { CASEX, D_ROW(1.0), 0, 0 }, Text, TEXTID_showNameID, ON, 0, "", "" },

		//�������
		{ { CASEX, D_ROW(2.2), CW, 0 }, ComboBox, COMBOBOXID_ResultType, ON, 0, "", "" },

		//��ʾ����
		{ { TYPEX, D_ROW(2.2), CW, 0 }, ComboBox, COMBOBOXID_ShowType, ON, 0, "", "" },

		//��ɫ�����ð�ť
		{ { SETBTX, D_ROW(2.0), CW / 3, 0 }, PushButton, BUTTONID_ShowSet, ON, 0, "����", "" },

		//��ʾģʽ
		{ { CASEX, D_ROW(3.4), CW, 0 }, ComboBox, COMBOBOXID_ShowModel, ON, 0, "", "" },

		//���/��Сֵ��ʶ
		//{ { MAXCHKX, D_ROW(3.4), MAXCHKW, 0}, Label, 0, ON, ALIGN_LEFT, "���/Сֵ��ʶ", ""},
		{ { MAXCHKX, D_ROW(3.4), 0, 0 }, ToggleButton, TOGGLEID_ShowExtremum, ON, 0, "���/Сֵ��ʶ", "" },

		//������ʾ����
		{ { XC, D_ROW(4.6), 45 * XC, 8.2*YC }, GroupBox, 0, ON, 0, "������ʾ����", "" },
		//������&������&����ֵ
		{ { 9.2*XC + 4 * CW / 3, D_ROW(5.3), CW, 0 }, ComboBox, COMBOBOXID_SolidDisp_ConditionValue, ON, 0, "", "" },
		{ { 9 * XC, D_ROW(5.3), CW, 0 }, ComboBox, COMBOBOXID_SolidDisp_ConditionType, ON, 0, "", "" },
		{ { 9.2*XC + CW, D_ROW(5.3), CW / 3, 0 }, ComboBox, COMBOBOXID_SolidDisp_ConditionOpt, ON, 0, "", "" },

		//�����б�
		{ { 2.4*XC, D_ROW(7.2), 36.7*XC, 4.0*YC }, ListBox, LISTBOXID_SolidDisp_ConditionList, ON, 0, "", "" },

		//����������������
		{ { 9.5*XC + 7 * CW / 3, D_ROW(5.1), 0, 0 }, PushButton, PUSHBTNID_SolidDisp_AddC, ON, 0, "����", "" },
		//������������ɾ��
		{ { 9.5*XC + 7 * CW / 3, D_ROW(7.0), 0, 0 }, PushButton, PUSHBTNID_SolidDisp_DelC, ON, 0, "ɾ��", "" },
		//���������������
		{ { 9.5*XC + 7 * CW / 3, D_ROW(8.2), 0, 0 }, PushButton, PUSHBTNID_SolidDisp_ClearC, ON, 0, "���", "" },

		//��ʾ��ͼ��ť
		{ { SHOWBTX, D_ROW(10.6), BUTTON_STDWIDTH, 0 }, PushButton, BUTTONID_Show, ON, 0, "����鿴", "" },
		{ { DAMAGE, D_ROW(11.8), BUTTON_STDWIDTH, 0 }, PushButton, BUTTONID_Damage, ON, 0, "�ƻ��鿴", "" },
		{ { CLIPBTX, D_ROW(10.6), BUTTON_STDWIDTH, 0 }, PushButton, BUTTONID_ClipPlot, ON, 0, "���в鿴", "" },
		{ { NODEBTX, D_ROW(10.6), BUTTON_STDWIDTH, 0 }, PushButton, BUTTONID_NodeVal, ON, 0, "�ڵ�鿴", "" },
		{ { SHOWBTX, D_ROW(12.2), BUTTON_STDWIDTH, 0 }, PushButton, BUTTONID_ShowBoundary, ON, ALIGN_CENTER, "�߽�鿴", "" },
	}
};

/*----------------------------------------------------------------------+
|									|
|   Tree Resources							|
|									|
+----------------------------------------------------------------------*/
DItem_TreeRsc TREEID_SolidDis_ProjList =
{
	NOHELP, MHELP,
	HOOKITEMID_Tree, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_EDITABLE | TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, CTPANELID_SolidDis_Detail, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_Tree },
	}
};

DItem_TextRsc TEXTID_showNameID =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"��ǰѡ��",
	"g_solidDispDlgInfo.current"
};

DItem_PushButtonRsc BUTTONID_ShowSet =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, HOOKBUTTON_ShowSet, NOARG,
	NOCMD, LCMD, "",
	"ShowSet"
};

DItem_PushButtonRsc BUTTONID_Show =
{
	DEFAULT_BUTTON, NOHELP, MHELP, HOOKPUSHBTN_SolidDisp_Show, NOARG,
	NOCMD, LCMD, "",
	"Showresult"
};

DItem_PushButtonRsc BUTTONID_Damage =
{
	DEFAULT_BUTTON, NOHELP, MHELP, HOOKPUSHBTN_Damage_Show, NOARG,
	NOCMD, LCMD, "",
	"Showresult"
};
DItem_PushButtonRsc BUTTONID_ClipPlot =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, NOHOOK, NOARG,
	CMD_MSDI_MSCAESI_ACTION_CLIPPLOTEFACE, LCMD, "",
	"Clipresult"
};

DItem_PushButtonRsc BUTTONID_NodeVal =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, NOHOOK, NOARG,
	CMD_MSDI_MSCAESI_ACTION_NODEVIEWER, LCMD, "",
	"NodeValShow"
};
DItem_PushButtonRsc BUTTONID_ShowBoundary =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, HOOKPUSHBTN_BoundaryShow, NOARG,
	NOCMD, LCMD, "",
	"ShowBoundary"
};

DItem_PushButtonRsc PUSHBTNID_SolidDisp_AddC =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, HOOKBUTTON_SolidDisp_AddC, NOARG,
	NOCMD, LCMD, "",
	""
};

DItem_PushButtonRsc PUSHBTNID_SolidDisp_DelC =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, HOOKBUTTON_SolidDisp_DelC, NOARG,
	NOCMD, LCMD, "",
	""
};

DItem_PushButtonRsc PUSHBTNID_SolidDisp_ClearC =
{
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, HOOKBUTTON_SolidDisp_ClearC, NOARG,
	NOCMD, LCMD, "",
	""
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ComboBox Item Resources                                             |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ComboBoxRsc COMBOBOXID_ResultType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKSolidContourDisp_ResultType,
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0,
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	TXT_ComboBox1,
	"g_solidDispDlgInfo.ResultType",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ComboBoxRsc COMBOBOXID_ShowType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SolidDisp_ShowType,
	NOARG, 10, "", "", "", "", NOMASK,
	STRINGLISTID_ShowType, 10, 0, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	TXT_ComboBox2,
	"g_solidDispDlgInfo.ShowType",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ComboBoxRsc COMBOBOXID_ShowModel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SolidDisp_ShowModel,
	NOARG, 10, "", "", "", "", NOMASK,
	STRINGLISTID_ShowModel, 10, 0, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	TXT_ComboBox3,
	"g_solidDispDlgInfo.ShowModel",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ComboBoxRsc COMBOBOXID_SolidDisp_ConditionType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SolidDisp_CondType,
	NOARG, 10, "", "", "", "", NOMASK,
	STRINGLISTID_ConditionType, 10, 0, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	"��������",
	"g_solidDispDlgInfo.indexCType",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ComboBoxRsc COMBOBOXID_SolidDisp_ConditionOpt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK,
	NOARG, 10, "", "", "", "", NOMASK,
	STRINGLISTID_Condition, 10, 0, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	"",
	"g_solidDispDlgInfo.indexCOpt",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

DItem_ComboBoxRsc COMBOBOXID_SolidDisp_ConditionValue =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SolidDisp_CondVal,
	NOARG, 10, "", "", "", "", NOMASK,
	0, 10, 0, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE | COMBOATTR_DRAWPREFIXICON,
	"",
	"g_solidDispDlgInfo.indexCValue",
	{
		{ 20 * XC, 20, 0, "" },
	}
};

StringList STRINGLISTID_ShowType =
{
	1,
	{
		{ { 0 }, "��ͼ" },
		{ { 1 }, "��ֵ��ͼ" },
		{ { 2 }, "ʸ��ͼ" },
	}
};

StringList STRINGLISTID_ShowModel =
{
	1,
	{
		{ { 0 }, "�⻬" },
		{ { 1 }, "����" },
	}
};
/*----------------------------------------------------------------------+
|                                                                       |
|    ToggleButton Items                                                 |
|                                                                       |
+----------------------------------------------------------------------*/
//�Ƿ���ʾ��ʾ�����Сֵ��ʶ,0�����ѡ��, -1����ѡ��
DItem_ToggleButtonRsc TOGGLEID_ShowExtremum =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SolidDisp_ShowType,
	NOARG, NOMASK, NOINVERT,
	"",
	"g_solidDispDlgInfo.bShowExtremum"
};

//ɸѡ�����б�
DItem_ListBoxRsc LISTBOXID_SolidDisp_ConditionList =
{
	NOHELP, MHELP, NOHOOK, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_GRID,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "ɸѡ��" },
		{ 8 * XC, 45, ALIGN_LEFT, "������" },
		{ 8 * XC, 45, ALIGN_LEFT, "ɸѡֵ" },
	}
};

#undef XSIZE
#undef YSIZE	 
#undef TW	 
#undef CW	 
#undef MAXCHKW	 
#undef CASEX	 
#undef TYPEX
#undef MAXCHKX	
#undef SETBTX 
#undef SHOWBTX	 
#undef CLIPBTX