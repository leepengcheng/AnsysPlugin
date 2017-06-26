/*--------------------------------------------------------------------------------------+
|   ExportCalculation.r
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
|���ɼ�����Ի���                 |
-----------------------------------------------------------------------*/
//�Ի�������ı���Ϣ����
#define	TXT_EXPORTCALC_DIALOG	"���ɼ�����"

#define	TXT_EXPORTCALC_TABPAGE1	"���̸ſ�"
#define	TXT_EXPORTCALC_TABPAGE2	"���㹤�����غ�"
#define	TXT_EXPORTCALC_TABPAGE3	"����ģ��"
#define	TXT_EXPORTCALC_TABPAGE4	"����ɹ�"
#define	TXT_EXPORTCALC_TABPAGE4_1	"λ�Ƽ���ɹ�"
#define	TXT_EXPORTCALC_TABPAGE4_2	"Ӧ������ɹ�"
#define	TXT_EXPORTCALC_TABPAGE4_3	"��������"

#define TXT_EXPORTCALC_PUSHBTN_Submit	"����"
#define	TXT_EXPORTCALC_PUSHBTN_Cancel	"ȡ��"

//�Ի�����سߴ綨��
#define DIALOG_W	60*XC
#define	DIALOG_H	34*YC

#define PUSHBTN_H	2*YC
#define PUSHBTN_W	10*XC

#define TABPAGE_X	1*XC		//ע��tabpage�пؼ����������ԶԻ���Ϊ���յ�
#define	TABPAGE_Y	2*YC
#define	TABPAGE_W	(DIALOG_W-2*TABPAGE_X)
#define	TABPAGE_H	(DIALOG_H-TABPAGE_Y-PUSHBTN_H)
#define TABPAGE_SUBX	(TABPAGE_X + XC)
#define	TABPAGE_SUBY	(TABPAGE_Y + 2*YC)
#define TABPAGE_SUBW	(TABPAGE_W - 2*XC)
#define TABPAGE_SUBH	(TABPAGE_H - 2.5*YC)

#define	LISTBOX_W	(DIALOG_W - 4*XC)
#define	LISTBOX_H	10*YC
#define LISTBOX_SUBW	(TABPAGE_SUBW - 2*XC)
#define LISTBOX_SUBH	7*YC

//�Ի�����
DialogBoxRsc DIALOGID_ExportCalculation =
{
	DIALOGATTR_DEFAULT |  DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	DIALOG_W, DIALOG_H,
	NOHELP, MHELP,
	HOOKDIALOG_ExpCalc, NOPARENTID,
	TXT_EXPORTCALC_DIALOG,
	{
		//ѡ������Ĺ���
		{{8*XC, 0.5*YC,  17*XC, 1.4*YC}, ComboBox,   COMBOBOXID_ExportCalc_InstanceLists,  ON,0, "", ""}, 

		//���������ɴ��ڵ�TabPage��ҳ
		{{TABPAGE_X, TABPAGE_Y, TABPAGE_W, TABPAGE_H}, TabPageList, TPLISTID_ExportCalc_Main, ON, 0, "", ""},

		//��ť������
		{{DIALOG_W/2 - BUTTON_STDWIDTH, DIALOG_H - PUSHBTN_H + 0.2*YC, BUTTON_STDWIDTH, 0}, PushButton,PUSHBTNID_ExportCalc_Submit, ON, ALIGN_CENTER, "", ""},

		//��ť��ȡ��
		{{DIALOG_W/2 + BUTTON_STDWIDTH/2, DIALOG_H - PUSHBTN_H + 0.2*YC, BUTTON_STDWIDTH, 0}, PushButton,PUSHBTNID_ExportCalc_Cancel, ON, ALIGN_CENTER, "", ""},
	}
};

//TabPage�б���
DItem_TabPageListRsc  TPLISTID_ExportCalc_Main =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_TABSFITPAGEWIDTH,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Project,		ON, 0,"",""},		//page1
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Case,			ON, 0,"",""},		//page2		
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Model,		ON, 0,"",""},		//page3
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Results,		ON, 0,"",""},		//page4
	}								
};							

//TabPage1����
DItem_TabPageRsc TABPAGEID_ExportCalc_Project =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE1,
	{	
		//���̸ſ�
		{{TABPAGE_X + XC, TABPAGE_Y + 2*YC, 56*XC, TABPAGE_H - 4.5*YC}, MLText, MLTEXTID_ExportCalc_ProjectDescr, ON, 0, "", ""},

		//����ſ��ĵ�
		{{TABPAGE_SUBX + 45*XC ,  30*YC, 0, 0}, PushButton, PUSHBTNID_ExportCalc_Import, ON, ALIGN_CENTER, "", ""},
	}
};

//TabPage2����
DItem_TabPageRsc TABPAGEID_ExportCalc_Case =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE2,
	{
		//�����б�
		{{TABPAGE_X + XC, TABPAGE_Y + 2*YC, LISTBOX_W, LISTBOX_H*2.75}, ListBox, LISTBOXID_ExportCalc_CaseList, ON, 0, "", ""},
	}
};

//TabPage3����
DItem_TabPageRsc TABPAGEID_ExportCalc_Model =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE3,
	{
		//��ʾ�ı�
		{{TABPAGE_X + XC ,  TABPAGE_Y + 3*YC,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "��������ģ��ͼ·����", ""},

		//ѡ��ģ��ͼ·����ť
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_Y + 2*YC + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelModelPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_Y + 2*YC + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddModelPath, ON, ALIGN_CENTER, "", ""},

		//��ѡģ��ͼ·���б�
		{{TABPAGE_X + XC, TABPAGE_Y + 4*YC, LISTBOX_W, LISTBOX_H}, ListBox, LISTBOXID_ExportCalc_ModelPaths, ON, 0, "", ""},

		//�ܵ�Ԫ��
		{{TABPAGE_X + 10*XC, TABPAGE_Y + 4.5*YC + LISTBOX_H, 8*XC, 0}, Text, TEXTID_ExportCalc_CellCount, ON, 0, "", ""},

		//�ڵ���
		{{TABPAGE_X + 30*XC, TABPAGE_Y + 4.5*YC + LISTBOX_H, 8*XC, 0}, Text, TEXTID_ExportCalc_NodeCount, ON, 0, "", ""},

		//����ģ�Ͳ�����
		{{TABPAGE_X + XC, TABPAGE_Y + 5.5*YC +LISTBOX_H, LISTBOX_W, LISTBOX_H + 4*YC}, ListBox, LISTBOXID_ExportCalc_ModelParamList, ON, 0, "", ""},
	}
};

//TabPage4����
DItem_TabPageRsc TABPAGEID_ExportCalc_Results =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4,
	{
		//����ɹ�ҳ��Tabpage��ҳ
		{{TABPAGE_SUBX, TABPAGE_SUBY, TABPAGE_SUBW, TABPAGE_SUBH}, TabPageList, TPLISTID_ExportCalc_Sub, ON, 0, "", ""},
	}
};

//TabPage�б���
DItem_TabPageListRsc  TPLISTID_ExportCalc_Sub =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_DEFAULT,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsWY,		ON, 0,"",""},		//page1
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsYL,		ON, 0,"",""},		//page2
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsPJ,		ON, 0,"",""},		//page3
	}								
};	

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsWY =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_1,
	{
		//λ�Ʋ�����
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH * 1.4}, ListBox, LISTBOXID_ExportCalc_WYParamList, ON, 0, "", ""},

		//��ʾ�ı�
		{{TABPAGE_SUBX + XC ,  TABPAGE_SUBY + 3*YC + LISTBOX_SUBH * 1.4,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "��ͼ���ֵ��ͼ·����", ""},

		//ѡ��·����ť
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH * 1.4 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelWYCloudPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH * 1.4 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddWYCloudPath, ON, ALIGN_CENTER, "", ""},

		//λ����ͼ���ֵ��ͼ·���б�
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 4*YC + LISTBOX_SUBH * 1.4, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_WYCloudPaths, ON, 0, "", ""},

	}
};

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsYL =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_2,
	{
		//Ӧ��������
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLParamList, ON, 0, "", ""},

		//��ʾ�ı�
		{{TABPAGE_SUBX + XC ,   TABPAGE_SUBY + 3*YC + LISTBOX_SUBH,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "��ͼ���ֵ��ͼ·����", ""},

		//ѡ��·����ť
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelYLCloudPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddYLCloudPath, ON, ALIGN_CENTER, "", ""},

		//Ӧ����ͼ���ֵ��ͼ·���б�
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 4*YC + LISTBOX_SUBH, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLCloudPaths, ON, 0, "", ""},

		//��ʾ�ı�
		{{TABPAGE_SUBX + XC ,    TABPAGE_SUBY + 5*YC + LISTBOX_SUBH*2,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "Ӧ����ֵͼ·����", ""},

		//ѡ��·����ť
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 4*YC + LISTBOX_SUBH*2 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelYLPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 4*YC + LISTBOX_SUBH*2 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddYLPath, ON, ALIGN_CENTER, "", ""},

		//Ӧ����ֵͼ·���б�
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 6*YC + LISTBOX_SUBH*2, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLPaths, ON, 0, "", ""},
	}
};

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsPJ =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_3,
	{
		//�ֽ������
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH * 2.0}, ListBox, LISTBOXID_ExportCalc_PJParamList, ON, 0, "", ""},
	}
};

//PushButton����
DItem_PushButtonRsc PUSHBTNID_ExportCalc_Submit = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_Submit, NOARG, NOCMD, LCMD, "", TXT_EXPORTCALC_PUSHBTN_Submit   
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_Cancel = 
{ 
	CANCEL_BUTTON, NOHELP, MHELP, 
	HOOKITEMID_Button_StandardAction, ACTIONBUTTON_CANCEL, NOCMD, MCMD, "", TXT_EXPORTCALC_PUSHBTN_Cancel  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_Import = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_Import, NOARG, NOCMD, LCMD, "", "�ſ�����"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddModelPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "ѡ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelModelPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "ɾ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddWYCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "ѡ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelWYCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "ɾ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddYLCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "ѡ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelYLCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "ɾ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddYLPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "ѡ��"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelYLPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "ɾ��"  
};

//ListBox����
DItem_ListBoxRsc LISTBOXID_ExportCalc_ModelPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{12*XC, 12, ALIGN_LEFT, "���"	 },
		{30*XC, 35, ALIGN_LEFT,  "ģ��ͼ·��"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_WYCloudPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "���"	 },
		{35*XC, 40, ALIGN_LEFT,  "��ͼ���ֵ��ͼ·��"	 },
    }
};	

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLCloudPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "���"	 },
		{35*XC, 40, ALIGN_LEFT,  "��ͼ���ֵ��ͼ·��"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "���"	 },
		{35*XC, 40, ALIGN_LEFT,  "Ӧ����ֵͼ·��"	 },
    }
};	

DItem_ListBoxRsc LISTBOXID_ExportCalc_CaseList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_CaseList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "������"	 },
		{35*XC, 40, ALIGN_LEFT,  "��Ҫ�غ�"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_ModelParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_ModelParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "���Ϻ�"	 },
		{10*XC, 20, ALIGN_LEFT,  "��������"	 },
		{8*XC, 15, ALIGN_LEFT,  "��Ԫ��"	 },
		{10*XC, 20, ALIGN_LEFT,  "����ģ��"	 },
		{8*XC, 15, ALIGN_LEFT,  "���ɱ�"	 },
		{10*XC, 20, ALIGN_LEFT,  "�ܶ�"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_WYParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_WYParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{7*XC, 10, ALIGN_LEFT, "������"	 },
		{11*XC, 20, ALIGN_LEFT,  "����λ��"	 },
		{11*XC, 15, ALIGN_LEFT,  "���X��λ��"	 },
		{11*XC, 20, ALIGN_LEFT,  "���Y��λ��"	 },
		{11*XC, 15, ALIGN_LEFT,  "���Z��λ��"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_YLParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{7*XC, 10, ALIGN_LEFT, "������"	 },
		{9*XC, 20, ALIGN_LEFT,  "X����Ӧ��"	 },
		{9*XC, 15, ALIGN_LEFT,  "Y����Ӧ��"	 },
		{9*XC, 20, ALIGN_LEFT,  "Z����Ӧ��"	 },
		{9*XC, 15, ALIGN_LEFT,  "��һ��Ӧ��" },
		{9*XC, 15, ALIGN_LEFT,  "������Ӧ��" },
    }
};
	
DItem_ListBoxRsc LISTBOXID_ExportCalc_PJParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_PJParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{10*XC, 20, ALIGN_LEFT,	"������"	 },
		{10*XC, 20, ALIGN_LEFT,	"������"	 },
		{10*XC, 20, ALIGN_LEFT,	"�������"},
		{8*XC,  10, ALIGN_LEFT, "�ṹϵ��"},
		{12*XC, 10, ALIGN_LEFT, "�ṹ��Ҫ��ϵ��"},
		{10*XC, 10, ALIGN_LEFT, "���״��ϵ��"},
		{10*XC, 10, ALIGN_LEFT, "�ֽ��ǿ��"},
		{12*XC, 10, ALIGN_LEFT, "����������ǿ��"},
		{16*XC, 20, ALIGN_LEFT,	"����Ӧ�����(mMPa)"},
		{16*XC, 20, ALIGN_LEFT,	"��������Ӧ�����(mMPa)"},
		{16*XC, 20, ALIGN_LEFT,	"����������(mMPa)"	 },
		{12*XC, 20, ALIGN_LEFT, "�ο����"},
		{8*XC,  20, ALIGN_LEFT,	"���(N)" },
		{8*XC,  20, ALIGN_LEFT,	"����(N)"},
		{12*XC, 20, ALIGN_LEFT,	"XY�������(N)"},
		{12*XC, 20, ALIGN_LEFT,	"YZ�������(N)"},
		{12*XC, 20, ALIGN_LEFT,	"XZ�������(N)"}
    }
};


//Text����
DItem_TextRsc TEXTID_ExportCalc_CellCount = 
{ 
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, NOARG, 
	20, "%d", "%d", "", "", NOMASK, TEXT_READONLY , "�ܵ�Ԫ����", 
	"g_expCalculationInfo.nCellCnt" 
};

DItem_TextRsc TEXTID_ExportCalc_NodeCount = 
{ 
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, NOARG, 
	20, "%d", "%d", "", "", NOMASK, TEXT_READONLY , "�ڵ�����", 
	"g_expCalculationInfo.nNodeCnt" 
};

//MLText����
DItem_MultilineTextRsc MLTEXTID_ExportCalc_ProjectDescr= 
{ 
	NOSYNONYM, NOHELP, MHELP, HOOKMLTEXT_ExpCalc_ProDescr, 1, 
	MLTEXTATTR_SBARINWIDTH , 10, "" 
}; 

//ComboBox����
DItem_ComboBoxRsc COMBOBOXID_ExportCalc_InstanceLists= 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_ExpCalc_ProjectLists, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, "��Ŀ����", "g_expCalculationInfo.proIndex", 
	{ {20*XC, 20, 0, ""}, } 
};

#undef DIALOG_W	
#undef	DIALOG_H	

#undef PUSHBTN_H	
#undef PUSHBTN_W	

#undef TABPAGE_X	
#undef	TABPAGE_Y	
#undef	TABPAGE_W	
#undef	TABPAGE_H	
#undef TABPAGE_SUBX
#undef	TABPAGE_SUBY
#undef TABPAGE_SUBW
#undef TABPAGE_SUBH

#undef	LISTBOX_W	
#undef	LISTBOX_H	
#undef LISTBOX_SUBW
#undef LISTBOX_SUBH