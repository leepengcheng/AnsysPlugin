/*--------------------------------------------------------------------------------------+
|   surfaceShowDlg.r
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
|����鿴�Ի���															|
-----------------------------------------------------------------------*/
#define XSIZE       (49*XC)
#define YSIZE       (14*YC)

#define TREE_X	    (0.5*XC)
#define TREE_Y	    (1.5*YC)
#define TREE_H	    (YSIZE - TREE_Y - BUTTON_LARGEHEIGHT)
#define TREE_W	    (20*XC)
#define CBOX_W	    (13*XC)	
#define	TEXT_W		(9*XC)

#define SHOWBTX		(TEXT_W/2)

#define TXT_SURFACE_DIALOG	"������������鿴"
#define TXT_SURFACE_TREE	"�����б�"
#define	TXT_SURFACE_PUSHBTN_Setting	"����"
#define	TXT_SURFACE_PUSHBTN_Submit	"����鿴"
#define	TXT_SURFACE_PUSHBTN_NodeVal	"�ڵ�鿴"

/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_SurfaceShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOGID_SurfaceShow, NOPARENTID,
	TXT_SURFACE_DIALOG,
	{
		{{0,0,0,0},	PopupMenu, POPUPMENUID_Surface_ClipMenu,  HIDDEN, 0, "", ""},

		//�����б�Tree
		{ { 0, 0, 0, 0 }, Tree, TREEID_Surface_ClipList, ON, 0, "", "" },

		{ { TREE_W, 0, 0, 0 }, Sash, SASHID_Surface_V, ON, 0, "", "" },

		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_Surface_Detail, ON, 0, "", "" },
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
DItem_SashRsc SASHID_Surface_V =
{
	NOHELP, MHELP,
	HOOKSASH_Surface_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};

/*----------------------------------------------------------------------+
|									|
|   Tree Resources							|
|									|
+----------------------------------------------------------------------*/
DItem_TreeRsc TREEID_Surface_ClipList =
{
	NOHELP, MHELP,
	HOOKTREE_Surface_ClipList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_EDITABLE | TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_SURFACE_TREE },
	}
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ContainerPanel Item Resources                                       |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ContainerPanelRsc CTPANELID_Surface_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG,	0,
	CONTAINERID_Surface_Setting, 0,
	""
};

DItem_ContainerRsc CONTAINERID_Surface_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_Surface_Setting
};

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemList Resources                                            |
|                                                                       |
+----------------------------------------------------------------------*/

DialogItemListRsc DILISTID_Surface_Setting =
{
	{
		// ��ǰѡ�й�����ʾ
		{ { 7.5*XC, D_ROW(0.8), 0, 0 }, Text, TEXTID_Surface_Current, ON, 0, "", "" },

		// �������
		{ { 7.5*XC, D_ROW(1.8), CBOX_W, 0 }, ComboBox, COMBOBOXID_SurfaceResultType, ON, 0, "", "" },

		//��ʾ����
		{ { 7.5*XC, D_ROW(3.0), CBOX_W, 0 }, ComboBox, COMBOBOXID_Surface_ShowType, ON, 0, "", "" },

		// ��ɫ�����ð�ť
		{ { 8.0*XC + CBOX_W, D_ROW(2.8), CBOX_W/3, 0 }, PushButton, PUSHBTNID_Surface_Setting, ON, 0, "", "" },
		
		// ��ʾģʽ
		{ { 7.5*XC, D_ROW(4.2), CBOX_W, 0 }, ComboBox, COMBOBOXID_Surface_Model, ON, 0, "", "" },

		// ���/��Сֵ��ʶ
		{ { 7.5*XC, D_ROW(5.4), 0, 0 }, ToggleButton, TOGGLEID_Surface_ShowExtremum, ON, 0, "���/Сֵ��ʶ", ""},	

		//����鿴��ť
		{ { SHOWBTX, D_ROW(7.0), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_Surface_Submit, ON, 0, "", "" },

		//�ڵ�鿴��ť
		{ { SHOWBTX + BUTTON_STDWIDTH + TEXT_W/3, D_ROW(7.0), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_Surface_NodeVal, ON, 0, "", "" },
	}
}

//��ǰѡ��
DItem_TextRsc TEXTID_Surface_Current =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"��ǰѡ��",
	"g_surfaceShowDlgInfo.current"
};

//�������
DItem_ComboBoxRsc COMBOBOXID_SurfaceResultType = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_Surface_ResultType, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	"�������", 
	"g_surfaceShowDlgInfo.indexReType", 
	{ 
		{20*XC, 20, 0, ""}, 
	} 
};

//��ʾ����
DItem_ComboBoxRsc COMBOBOXID_Surface_ShowType = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowType, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_ComboBox2,
	"g_surfaceShowDlgInfo.indexShowType",
	{
		{20*XC, 20, 0, ""}, 
	}
};

//ɫ������
DItem_PushButtonRsc PUSHBTNID_Surface_Setting =
{
	NOT_DEFAULT_BUTTON,	NOHELP, MHELP, HOOKBUTTON_Surface_ShowSet, NOARG,
	NOCMD, LCMD, "",
	TXT_SURFACE_PUSHBTN_Setting
};

//��ʾģʽ
DItem_ComboBoxRsc COMBOBOXID_Surface_Model = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_Surface_Model,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowModel, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_ComboBox3,
	"g_surfaceShowDlgInfo.indexShowModel",
	{
		{20*XC, 20, 0, ""}, 
	}
};

//���/Сֵ��ʶ
//�Ƿ���ʾ��ʾ�����Сֵ��ʶ,0�����ѡ��, -1����ѡ��
DItem_ToggleButtonRsc TOGGLEID_Surface_ShowExtremum = 
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SurfaceDisp_ShowType,
	NOARG, NOMASK, NOINVERT,
	"",
	"g_surfaceShowDlgInfo.bShowExtremum"
};
//��ʾ��ť
DItem_PushButtonRsc PUSHBTNID_Surface_Submit =
{
	DEFAULT_BUTTON,	NOHELP, MHELP, HOOKBUTTON_Surface_ResultShow, NOARG,
	NOCMD, LCMD, "",
	TXT_SURFACE_PUSHBTN_Submit
};

//�ڵ���ʾ
DItem_PushButtonRsc PUSHBTNID_Surface_NodeVal =
{
	NOT_DEFAULT_BUTTON,	NOHELP, MHELP, NOHOOK, NOARG,
	CMD_MSDI_MSCAESI_ACTION_NODEVIEWER, LCMD, "",
	TXT_SURFACE_PUSHBTN_NodeVal
};

// �Ҽ��˵�
DItem_PopupMenuRsc POPUPMENUID_Surface_ClipMenu = 
    {
    NOHELP, LHELP, NOHOOK, NOARG, 0,
    PulldownMenu, PDMID_Surface_ClipMenu    
    };

DItem_PulldownMenuRsc PDMID_Surface_ClipMenu = 
{
    NOHELP, OHELPTASKIDCMD,
    HOOKPULLMENU_Surface_ClipMenu,
    ON | ALIGN_LEFT, "",
	{	
		{"��������", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_Surface_Add, 0,	NOCMD, OTASKID, ""},
		{"ɾ������", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_Surface_Del, 0,	NOCMD, OTASKID, ""},
	}
};

#undef XSIZE
#undef YSIZE
#undef TREE_H	 
#undef TREE_W	 
#undef CBOX_W
#undef TEXT_W	 
#undef SHOWBTX	 
