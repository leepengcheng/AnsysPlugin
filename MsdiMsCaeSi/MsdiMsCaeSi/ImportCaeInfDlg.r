/*--------------------------------------------------------------------------------------+
|   ImportCaeInfDlg.r
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
| Required resource for a native-code-only application.                 |
-----------------------------------------------------------------------*/
#define  W1			(30*XC)						/* ����ģ�� text #: width */
#define  W2			(5*XC)						/* ѡ�� buttom #: width */
#define  W3			(43.5*XC)					/* ѡ�� buttom #: width */
#define  ListboxBW  (36*XC)
#define  ToggleBW   (20*XC)

#define  H1		 10.0*YC

#define  X1		(8*XC)
#define  X2		(X1 + 12*XC)
#define  X3		(X1 + W1 + 0.5*XC)
#define  X4		(X1 - 7*XC)
#define  X5		(X1 - 6*XC)
#define  X6		(X5 + ListboxBW + 5*XC)
#define  X7		(X2 + W2 + 5*XC)

#define  Y1     GENY(1.0)				/* ����ģ�� ToggleButton position */
#define  Y2     Y1 + GENY(2.0)          /* ����ģ�� Text/ComboBox position */
#define  Y3     Y2 - GENY(0.8)          /* ѡ�� PushButton position */
#define  Y4     Y2 + GENY(2.3)          /* CAE�������� GroupBox position */
#define  Y5     Y4 + GENY(2.3)          /* �������� ListBox position */
#define  Y6     Y5 + GENY(0.5)          /* ��� PushButton position */
#define  Y7     Y6 + GENY(2.0)          /* ɾ�� PushButton position */
#define  Y8		Y4 + H1 + GENY(0.9)		/* ���� PushButton position */

#define  OVERALLWIDTH   X4 + W3 + 1.4*XC /* CAE�ɹ����봰�� width */
#define  OVERALLHEIGHT  Y8 + 2*YC		 /* CAE�ɹ����봰�� height */

DialogBoxRsc DIALOGID_ImportCaeInf =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_ALWAYSSETSTATE | DIALOGATTR_SINKABLE,
	OVERALLWIDTH, OVERALLHEIGHT,
	NOHELP, MHELP,
	HOOKDIALOGID_ImportCaeInf, NOPARENTID,
	"CAE�ɹ�����",
	{
		{ { X1, Y1, 0, 0 }, ToggleButton, TOGGLEID_MeshModel, ON, 0, "", "" },//����ģ��
		{ { X2, Y1, 0, 0 }, ToggleButton, TOGGLEID_Result, ON, 0, "", "" },//������
		{ { X1, Y2, W1, 0 }, ComboBox, COMBOBOXID_ImportCaeModel, ON, 0, "", "" },//����ģ��
		{ { X1, Y2, W1, 0 }, Text, TEXTID_ImportCaeModel, ON, 0, "", "" },//����ģ��
		{ { X3, Y3, W2, 0 }, PushButton, BUTTONID_SelectCaeModelFile, ON, 0, "ѡ��", "" },
		{ { X4, Y4, W3, H1 }, GroupBox, 0, ON, 0, "CAE��������", "" },
		{ { X5, Y5, ListboxBW, 0 }, ListBox, LISTBOXID_CaeResultFile, ON, 0, "", "" },
		{ { X3, Y6, W2, 0 }, PushButton, BUTTONID_AddResultFile, ON, 0, "���", "" },
		{ { X3, Y7, W2, 0 }, PushButton, BUTTONID_DelResultFile, ON, 0, "ɾ��", "" },
		{ { X2, Y8, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_OK, ON, 0, "����", "" },
		{ { X7, Y8, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_Cancel, ON, 0, "ȡ��", "" },
	}
};

// ����ģ��
DItem_ComboBoxRsc COMBOBOXID_ImportCaeModel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKCOMBOBOX_SelectCaeModel, NOARG,
	32, "", "", "", "", NOMASK,
	0, 5, 1, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_SORT | COMBOATTR_USEMODELVALUE,
	"����ģ��",
	"g_ImpDlgInfo.ProjectId",
	{
		{ 0, 32, ALIGN_LEFT, TXT_NULL },	/* set width to 0 for dynamic width capability */
	}
};

DItem_ToggleButtonRsc TOGGLEID_MeshModel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_TBSelImpDat,
	NOARG, NOMASK, NOINVERT,
	"����ģ��",
	"g_ImpDlgInfo.MeshModelVal"
};

DItem_ToggleButtonRsc TOGGLEID_Result =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_TBSelImpDat,
	NOARG, NOMASK, NOINVERT,
	"������",
	"g_ImpDlgInfo.ResultVal"
};

DItem_TextRsc TEXTID_ImportCaeModel =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	180, "%s", "%s", "", "", NOMASK, NOCONCAT,
	"����ģ��",
	""
};

DItem_TextRsc TEXTID_ImportCaeResult =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	180, "%s", "%s", "", "", NOMASK, NOCONCAT,
	"������",
	""
};

DItem_PushButtonRsc BUTTONID_SelectCaeModelFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_SelectCaeModel, NOARG,
	NOCMD, LCMD, "",
	"SelectCaeModelFile"
};

DItem_PushButtonRsc BUTTONID_AddResultFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_AddResultFile, NOARG,
	NOCMD, LCMD, "",
	"AddResultFile"
};

DItem_PushButtonRsc BUTTONID_DelResultFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_DelResultFile, NOARG,
	NOCMD, LCMD, "",
	"DelResultFile"
};

// ��������
DItem_ListBoxRsc LISTBOXID_CaeResultFile =
{
	NOHELP, MHELP, NOHOOK, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_GRID | LISTATTR_INDEPENDENTCOLS,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "�������ļ�" },
	}
};

#undef  W1		 	/* ����ģ�� text #: width */
#undef  W2		 	/* ѡ�� buttom #: width */
#undef  W3		 	/* ѡ�� buttom #: width */
#undef  ListboxBW  
#undef  ToggleBW 
#undef  H1	 
#undef  X1	 
#undef  X2	 
#undef  X3	 
#undef  X4	 
#undef  X5	 
#undef  X6	 
#undef  X7	 
#undef  Y1    	/* ����ģ�� ToggleButton position */
#undef  Y2      /* ����ģ�� Text/ComboBox position */
#undef  Y3      /* ѡ�� PushButton position */
#undef  Y4      /* CAE�������� GroupBox position */
#undef  Y5      /* �������� ListBox position */
#undef  Y6      /* ��� PushButton position */
#undef  Y7      /* ɾ�� PushButton position */
#undef  Y8		/* ���� PushButton position */

#undef  OVERALLWIDTH	/* CAE�ɹ����봰�� width */
#undef  OVERALLHEIGHT	/* CAE�ɹ����봰�� height */	

