/*--------------------------------------------------------------------------------------+
|   clipFaceCmdDlg.r
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>
#include <Mstn\cmdlist.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"

IconRsc ICONID_LockX =
{
    30, 30, ICONFORMAT_WINDOWS, BLACK_INDEX, "", "SectionCutZ"
};
IconRsc ICONID_LockY =
{
    30, 30, ICONFORMAT_WINDOWS, BLACK_INDEX, "", "SectionCutY"
};
IconRsc ICONID_LockZ =
{
    30, 30, ICONFORMAT_WINDOWS, BLACK_INDEX, "", "SectionCutX"
};
IconRsc ICONID_LockNone =
{
    30, 30, ICONFORMAT_WINDOWS, BLACK_INDEX, "", "PlaceLine"
};
DItem_ButtonGroupRsc BGROUPID_Surface_LockAxis= 
{ 
	NOSYNONYM, NOHELP, MHELP, 
	HOOKITEMID_Surface_LockAxis, NOARG, 
	0, 4, 1, 0, 0, YELLOW_INDEX, "", "g_outRebarDlgInfo.chooseBtnIndex", 
  { 
	{Icon, ICONID_LockX, NOCMD, LCMD, 0, NOMASK, ON, 0, "��X��"},
	{Icon, ICONID_LockY, NOCMD, LCMD, 1, NOMASK, ON, 0, "��Y��"},
	{Icon, ICONID_LockZ, NOCMD, LCMD, 2, NOMASK, ON, 0, "��Z��"},
	{Icon, ICONID_LockNone, NOCMD, LCMD, 3, NOMASK, ON, 0, "������"},
  } 
};
CmdItemListRsc ITEMLISTID_ClipFace = 
{
	{
	{{0,0, 0, 0}, ButtonGroup, BGROUPID_Surface_LockAxis, ON, 0, "", ""},
	}
};


//��������Ի���
DialogBoxRsc DIALOGID_AddClipFace =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_MODAL,
	40*XC, 6*YC,
	NOHELP, MHELP,
	HOOKDIALOGID_Surface_AddClipFace, NOPARENTID,
	"��������",
	{
		{ {13*XC, D_ROW(1.6), 23*XC, 0 }, Text, TEXTID_ClipFace_clipName, ON, 0, "", "" },
		{ {10*XC, D_ROW(3.0), BUTTON_STDWIDTH, 0},	PushButton,  PUSHBUTTONID_OK, ON,  0, "", ""},
		{ {22*XC, D_ROW(3.0), BUTTON_STDWIDTH, 0},	PushButton,  PUSHBUTTONID_Cancel, ON,  0, "", ""},
	}
}
DItem_TextRsc TEXTID_ClipFace_clipName =
{
    NOCMD, MCMD, NOSYNONYM,	NOHELP, MHELP, NOHOOK, NOARG,
    256, "", "", "", "", NOMASK, NOCONCAT, "������",
    "g_outRebarDlgInfo.clipName"
};

