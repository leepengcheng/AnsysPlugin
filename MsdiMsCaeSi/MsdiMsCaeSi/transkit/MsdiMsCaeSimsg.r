
/*----------------------------------------------------------------------+
|           Message List for MsdiMsCaeSi Application                       |
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>

#include "..\MsdiMsCaeSiids.h"

MessageList STRINGLISTID_Commands =
{
	{
		{COMMAND_IMPORTCAEINFO			,"ģ�͵���" },
		{COMMAND_SHOWCAESOLIDALL		,"ģ��չʾ" },
		{COMMAND_SHOWCAESOLIDPART		,"�ֲ���ʾ" },
		{COMMAND_SETCAERSTDISP			,"ģ��չʾ" },
		{COMMAND_CLEARCONTOURLINE		,"ģ�͵���" },
		{COMMAND_CLEARCONTOURPLOT		,"ģ��չʾ" },
		{COMMAND_EXPORTTOPARASOLID		,"ģ�͵���" },
		{COMMAND_SHOWCLIPEFACEPLOT		,"ģ��չʾ" },
		{COMMAND_SHOWCLIPEFACELINE		,"ģ�͵���" },
		{COMMAND_SHOWCLIPEFACEPVECTOR	,"ģ�͵���" },
		{COMMAND_CLEARCLIPEFACEPLOT		,"ģ��չʾ" },
		{COMMAND_CLEARCLIPEFACELINE		,"ģ�͵���" },
		{COMMAND_CLEARCLIPEFACEPVECTOR	,"ģ��չʾ" },
		{COMMAND_CLIPPLOTEFACE			,"��ͼ����" },
		{COMMAND_CLEARPLOTCUTFACE		,"ģ��չʾ" },
		{COMMAND_MOVECLIPPLOTFACE		,"ģ�͵���" },
		{COMMAND_SAVEPICTURE			,"ģ�͵���" },
		{COMMAND_CLIPEFACE				,"��������" },
		{COMMAND_EXPORTCAL				,"����������" },
		{COMMAND_CALCREBAR				,"������" },
		{COMMAND_FENCEDISPLAY			,"��ѡ��ʾ" },
		{COMMAND_RECOVERDISPLAY			,"�ָ���ʾ" },
		{COMMAND_CASECOMPARE			,"������ѡ"	},			
		{COMMAND_CLIPRESULT				,"λ�����"	},
		{COMMAND_SURFACESHOW			,"����չʾ"	},
		{COMMAND_OUTPUTREBAR			,"������" },
		{COMMAND_DELETEDISPLAY			,"ɾ��ģ��" },		
		{COMMAND_NODEVIEWER				,"�ڵ�鿴" },
		
	}
};

MessageList STRINGLISTID_Prompts =
{
	{
		{ PROMPT_SoildExport, "���������ѡ����Ҫ������ģ��/�������Ҽ��˳���" },
		{ PROMPT_SolidAllShow, "���������ѡ��ģ�͵ķ���λ��/�������Ҽ��˳���" },
		{ PROMPT_ImportCAESolid, " " },
		{ PROMPT_SELECT_FIRST_ELEMENT, " " },
		{ PROMPT_SELECT_NEXT_ELEMENT_OR_ACCEPT, " " },
		{ PROMPT_ACCEPT, " " },

		{ MESSAGEID_ModelDataDialogTitle, "ѡ��CAE���������ļ�" },
		{ MESSAGEID_ModelDataFilterString, "*.cdb" },
		{ MESSAGEID_ResultDataDialogTitle, "ѡ��CAE�����������ļ�" },
		{ MESSAGEID_ResultDataFilterString, "*.xml" },
		
		{ PROMPT_CalcRebar_ClickFirst,			"���ѡ����������/�Ҽ��˳����ߡ�" },
		{ PROMPT_CalcRebar_ClickNext,			"���ѡ������յ�/�Ҽ�����ѡ����㡣" },
		{ PROMPT_CalcRebar_ClickError,			"ѡ�������������յ���Ҫ�ڽ��淶Χ�ڣ�������ѡ��" },

		{ PROMPT_Recover_SelectInstance,		"���ѡ����Ҫ��ԭ��ʾ��CAEģ��/�Ҽ��˳�����." },
		{ PROMPT_Fence_SelectInstance,			"��ѡCAEģ����Ҫչʾ�Ĳ���/�Ҽ��˳�����." },

		{PROMPT_Surface_LockX,					"����X��"},
		{PROMPT_Surface_LockY,					"����Y��"},
		{PROMPT_Surface_LockZ,					"����Z��"},
		{PROMPT_Surface_LockNone,				"������"},

		{PROMPT_ClipResults_CentrePointStt,		"���ѡ��Բ�Ķ�λ�����/�Ҽ��˳�����"},
		{PROMPT_ClipResults_CentrePointEnd,		"���ѡ��Բ�Ķ�λ���յ�/�Ҽ��˳�����"},
		{PROMPT_ClipResults_ArcStt,				"���ѡ��Բ�������/�Ҽ��˳�����"},
		{PROMPT_ClipResults_ArcEnd,				"���ѡ��Բ�����յ�/�Ҽ��˳�����"},
	}
};
