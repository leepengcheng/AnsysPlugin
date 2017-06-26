/*--------------------------------------------------------------------------------------+
|   SetContourColorDlg.r
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
//����ɫ�����öԻ���
#define  TXT_Start_X		(8*XC)
#define  TXT_End_X			(26*XC)
#define  TXT_colorNum_x		(8*XC)
#define  updown_button_x	(15*XC)
#define  btnSelect_x		(18.5*XC)

#define  dlg_w				(48*XC)
#define  dlg_h				(14.5*YC)

#define  BUTTON_W			(7*XC)
#define  TXT_W				(10*XC)
#define  TXT_colorNum_w		(7*XC)
#define  updown_button_w	(2*XC)
#define  Listbox_w			(47*XC)

DialogBoxRsc DIALOGID_SetContourColor =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_MODAL | DIALOGATTR_ALWAYSSETSTATE,
	dlg_w, dlg_h,
	NOHELP, MHELPTOPIC, HOOKDIALOG_ColorSetting, NOPARENTID,
	"��ʾ����",
	{
		{ { TXT_Start_X,	D_ROW(1.0), TXT_W, 0 }, Text, TEXTID_ColorSet_StartValue, ON, 0, "", "" },					//��ʼֵ����
		{ { TXT_End_X,		D_ROW(1.0), TXT_W, 0 }, Text, TEXTID_ColorSet_EndValue, ON, 0, "", "" },					//��ֵֹ����
		{ { 37*XC,			D_ROW(1.0), 0, 0}, ToggleButton, TOGGLEID_ColorSet_Automatic, ON, 0, "�Զ�����", ""},		//�Զ�����
		{ { TXT_colorNum_x,	D_ROW(2.1), TXT_colorNum_w, 0 }, Text, TEXTID_colorNumLabel, ON, 0, "", "" },				//��ɫ��
		{ { updown_button_x,D_ROW(2.1), updown_button_w, 0 }, UpDownButtons, BUTTONID_UpDown, ON, 0, "", "" },			//UpDown��ť
		{ { 0.5*XC,			D_ROW(4.0), Listbox_w, 7*YC }, ListBox, LISTBOXID_ColorSet_ColorList, ON, 0, "", "" },			//��ɫ�б�
		{ { 37*XC,			D_ROW(2.1), 0, 0}, ToggleButton, TOGGLEID_ColorSet_Gray, ON, 0, "�Ҷ���ʾ", ""},		//�Ҷ���ʾ
		//ȷ��		
		{ { dlg_w-3*BUTTON_STDWIDTH-5.0*XC, D_ROW(9.4), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_ColorSet_OK, ON, 0, "", "" },

		//ȡ��
		{ { dlg_w-2*BUTTON_STDWIDTH-4.0*XC, D_ROW(9.4), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_ColorSet_Cancel, ON, 0, "", "" },

		//Ӧ��
		{ { dlg_w-BUTTON_STDWIDTH-3.0*XC, D_ROW(9.4), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_ColorSet_Apply, ON, 0, "", "" },
	}
};

DItem_TextRsc TEXTID_ColorSet_StartValue =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKTEXT_ColorSet_ColorValue, NOARG,
	10, "%0.8f", "%f", "", "", NOMASK, NOCONCAT,
	"��ʼֵ",
	"g_colorSettingDlgInfo.dStartValue"
};

DItem_TextRsc TEXTID_ColorSet_EndValue =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKTEXT_ColorSet_ColorValue, NOARG,
	10, "%0.8f", "%f", "", "", NOMASK, NOCONCAT,
	"��ֵֹ",
	"g_colorSettingDlgInfo.dEndValue"
};

DItem_TextRsc TEXTID_colorNumLabel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKTEXT_ColorSet_ColorValue, NOARG,
	10, "%d", "%d", "1", "50", NOMASK, NOCONCAT,
	"��ɫ��",
	"g_colorSettingDlgInfo.iColorNum"
};
DItem_UpDownButtonsRsc BUTTONID_UpDown =
{
	HOOKITEMID_Updownbutton,
	NOARG, 0,
	99, 1,
	0.1,
	""
};

DItem_ListBoxRsc LISTBOXID_ColorSet_ColorList =
{
	NOHELP, MHELP, HOOKITEMID_ListBox_DialogDemo, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_EDITABLE |
	LISTATTR_GRID | LISTATTR_INDEPENDENTCOLS,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "���" },
		{ 8 * XC, 45, ALIGN_LEFT, "��ɫ" },
		{ 15 * XC, 45, ALIGN_LEFT, "������Сֵ" },
		{ 14 * XC, 45, ALIGN_LEFT, "�������ֵ" },
	}
};

IconRsc ICONID_ColorPatch =
{
	24, 12, FORMAT_MONOBITMAP, BLACK_INDEX, "swatch",
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	}
};
DItem_PushButtonRsc BUTTONID_ColorChooser =
{
	NOT_DEFAULT_BUTTON | PBUTATTR_DYNAMICBORDER | PBUTATTR_THINBORDER,
	NOHELP, LHELPCMD,
	HOOKITEMID_PBtnColorChooser, NOARG, NOCMD, LCMD,
	"Apply", ""
};
DItem_ColorChooserRsc COLORCHOOSERID_Color =
{
	NOCMD, MCMD,
	NOSYNONYM,
	NOHELP, MHELPTOPIC,
	HOOKITEMID_ColorChooser, NOARG,
	"",
	"" /*"g_dataVars.Color"*/
};
DialogItemListRsc DIALOGID_ColorChooser =
{
	{
		{ { XC * 1, YC*0.5, XC * 32, YC * 16 }, ColorChooser, COLORCHOOSERID_Color, ON, 0, "", "" },
	}
};

DItem_ContainerRsc CONTAINERID_ColorChooser =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG,
	ICONPOPUPATTR_NOLABEL | ICONPOPUPATTR_ALWAYSBORDERED /* | ICONPOPUPATTR_PADBOTTOM */,
	DIALOGID_ColorChooser
};

DItem_TextRsc TEXTID_ListCell =
{
     NOCMD, MCMD, NOSYNONYM, 
	 NOHELP, MHELPTOPIC,
     HOOKTEXT_CellOfColor, NOARG,
     10, "%0.8f", "%f", "", "", NOMASK, NOCONCAT,
     "",
     ""
};

DItem_ToggleButtonRsc TOGGLEID_ColorSet_Automatic = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKTOGGLE_ColorSet_Automatic, 
	NOARG, NOMASK, NOINVERT,	
	"",
	"g_colorSettingDlgInfo.bAutomatic"
};

DItem_ToggleButtonRsc TOGGLEID_ColorSet_Gray = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKTOGGLE_ColorSet_Gray, 
	NOARG, NOMASK, NOINVERT,	
	"",
	"g_colorSettingDlgInfo.Gray"
};

DItem_PushButtonRsc PUSHBTNID_ColorSet_OK = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKITEMID_Button_StandardAction, ACTIONBUTTON_OK, NOCMD, MCMD, "", "ȷ��" 
};

DItem_PushButtonRsc PUSHBTNID_ColorSet_Cancel = 
{ 
	CANCEL_BUTTON, NOHELP, MHELP, 
	HOOKITEMID_Button_StandardAction, ACTIONBUTTON_CANCEL, NOCMD, MCMD, "", "ȡ��" 
};

DItem_PushButtonRsc PUSHBTNID_ColorSet_Apply = 
{ 
	DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ColorSet_Apply, NOARG, NOCMD, MCMD, "", "Ӧ��" 
};