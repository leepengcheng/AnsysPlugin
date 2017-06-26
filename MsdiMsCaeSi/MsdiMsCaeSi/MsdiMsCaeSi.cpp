/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSi.cpp
|
+--------------------------------------------------------------------------------------*/
#include    "MsdiMsCaeSipch.h"
#include    "BentlyCommonfile.h"

// ע����Ϣ
//typedef bool(*RegisterFunc)();

/*******************ȫ�ֱ�������**********************/
ImpInfDlgInfo		g_ImpDlgInfo;
SolidDispDlgInfo	g_solidDispDlgInfo;
ReBarCalDlgInfo		g_rebarCalDlgInfo;
ExpCalculationDlgInfo	g_expCalculationInfo;
PartialDisDlgInfo	g_partialDisDlgInfo;
CaseCompDlgInfo		g_caseCompDlgInfo;
ClipResultDlgInfo	g_clipResultDlgInfo;
SurfaceShowDlgInfo	g_surfaceShowDlgInfo;
OutRebarDlgInfo		g_outRebarDlgInfo;
ColorSettingDlgInfo	g_colorSettingDlgInfo;

BSIColorDescr*	m_pBackgroundColor;
BSIColorDescr*	m_pForegroundColor;

//��ɫ������������ʾ&�����Ա������ʾ
DrawColorShape  s_DrawColorShape;
//��ɫ���������ڹ����Ա��Ҳ���ʾ
DrawColorShape	s_DrawColorShape_temp;
//ɫ��ָ�룬����֮ǰ���붼��ȱʡʹ�õ���ɫ����������ʱ�޷���ɫ����Ϊ�������룬��˶����ɫ��ָ��
//֮ǰ����ʹ����ɫ����λ�ø�Ϊʹ�ø�ָ����ָɫ�����ڵ��û���������غ���֮ǰ��Ҫ�ֶ��޸ĸ�ָ���ָ����main������ȱʡָ����ɫ��
DrawColorShape*	s_pDCShape;

// CAE���������
bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
bmap<WString, WString> g_RstType_Unit;

bvector<WString> vecRstTypeCodes;

DgnPlatform::BSIColorPalette*  ppPalette;
bool alreadyUnload;

void ApplyCaeDisplayStyle(WString styleName, int viewIndex)
{
	ViewGroupCollectionCR viewGroupCollection = mdlDgnFileObj_getMasterFile()->GetViewGroups();
	DisplayStyleCP	tempStyle = DisplayStyleManager::FindDisplayStyleByName(styleName.GetWCharCP(), mdlDgnFileObj_getMasterFile());;

	if (tempStyle != NULL)
	{
		// �ж���ʾʽ���Ƿ�����ڱ�Dgn�ļ���
		if (DisplayStyleManager::DoesDisplayStyleExistInFile(styleName.GetWCharCP(), *mdlDgnFileObj_getMasterFile()) != true)
		{
			// û�а���ʱ��д�뵽��Dgn�ļ���
			DisplayStyleManager::WriteDisplayStyleToFile(*tempStyle, *mdlDgnFileObj_getMasterFile());
		}

		// Ӧ��ָ������ʾʽ��
		DisplayStyleManager::ApplyDisplayStyleToView(*tempStyle, *viewGroupCollection.GetActiveViewInfoP(viewIndex));
	}

	mdlView_updateSingle(viewIndex);
}

int copyFile(const char *SourceFile, const char *NewFile)
{
	ifstream in;
	ofstream out;
	in.open(SourceFile, ios::binary);//��Դ�ļ�
	if (in.fail())//��Դ�ļ�ʧ��
	{
		cout << "Error 1: Fail to open the source file." << endl;
		in.close();
		out.close();
		return 0;
	}
	out.open(NewFile, ios::binary);//����Ŀ���ļ� 
	if (out.fail())//�����ļ�ʧ��
	{
		cout << "Error 2: Fail to create the new file." << endl;
		out.close();
		in.close();
		return 0;
	}
	else//�����ļ�
	{
		out << in.rdbuf();
		out.close();
		in.close();
		return 1;
	}
}

#pragma region ��ͼ����
/*-----------------------------------------------------------------
*  ��������: ��ָ����ͼ�����ݱ���ΪBMPͼƬ��ʽ
*	@param	viewIndex	IN	ָ����ͼ����
*  @param	imageFile	IN	BMPͼƬ����·��
*
*  @return	��
-----------------------------------------------------------------*/
void captureViewToImage(int viewIndex, WCharCP imageFile)
{
	mdlView_updateSingle(viewIndex);
	BSIRect viewRect;
	byte*	pImage = NULL;
	Point2d size;
	MSWindow* p = mdlWindow_viewWindowGet(viewIndex);
	mdlView_getViewRectangle(&viewRect, p, DgnPlatform::DgnCoordSystem::Frustum);
	viewRect.Offset(0, 70);
	
	size.x = viewRect.corner.x - viewRect.origin.x + 1;
	size.y = viewRect.corner.y - viewRect.origin.y + 1;
	mdlImage_captureScreen(&pImage, &viewRect, viewIndex, IMAGEFORMAT_RGB);
	mdlImage_RGBToRGBSeparate(pImage, pImage, &size, ImageColorMode::RGB);
	WString wfile;
	wfile.append(imageFile);
	
	if (wfile.find(L"jpg") != WString::npos)//JPG��ʽ
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_JPEG, ImageColorMode::RGB, &size, pImage);
	}
	else if (wfile.find(L"tif") != WString::npos)
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_TIFF, ImageColorMode::RGB, &size, pImage);
	}
	else
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_BMP, ImageColorMode::RGB, &size, pImage);
	}
	mdlImage_freeImage(pImage, &size, IMAGEFORMAT_RGB);
}

void SavePicture(WCharCP unparsed)
{
	int result;
	BeFileName File_path;
	result = mdlDialog_fileCreate(File_path, 0, 0, L"", L"*.bmp,*.jpg,*.tif", L"d:\\", L"������ͼƬ");
	if (result == SUCCESS)
	{
		captureViewToImage(0, File_path.GetWCharCP());
	}

}
#pragma endregion ��ͼ����

void Select_exportHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		exportTool::InstallNewInstance(0);
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void All_exportHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		/*if (SUCCESS == mdlSelect_allElements())
		{
		exportTool::ExportSelect_toParasolid();
		}*/
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ExportToParasolidCmd(WCharCP unparsed)
{
	exportTool::InstallNewInstance(COMMAND_EXPORTTOPARASOLID);
}

// ��ȡλ������������
void loadClipResultDlgParams(void)
{
	int           status = ERROR;
	RscFileHandle userPrefsH;

	memset(&g_clipResultDlgInfo, 0, sizeof(g_clipResultDlgInfo));

	/* --- load Section and Hatching Preferences --- */
	if (SUCCESS == mdlDialog_userPrefFileOpen(&userPrefsH, RSC_READWRITE))
	{
		ClipResultDlgInfo *clipResultDlgRscP = NULL;

		if (NULL != (clipResultDlgRscP = (ClipResultDlgInfo *)mdlResource_loadByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, mdlSystem_getCurrTaskID())))
		{
			ULong rscSize;

			mdlResource_query(&rscSize, clipResultDlgRscP, RSC_QRY_SIZE);

			if (rscSize == sizeof(ClipResultDlgInfo))
			{
				status = SUCCESS;
				g_clipResultDlgInfo = *clipResultDlgRscP;
			}

			/* --- Free the memory used by the rsc --- */
			mdlResource_free(clipResultDlgRscP);
		}

		mdlResource_closeFile(userPrefsH);
	}

	/* --- if no resource was loaded load defaults now --- */
	if (status != SUCCESS)
	{
		//��ʼ������ֵ
		g_clipResultDlgInfo.titleFont = O_titleFont;
		g_clipResultDlgInfo.contentFont = O_contentFont;
		g_clipResultDlgInfo.heightCoe = O_heightCoe;
		g_clipResultDlgInfo.XSpaceCnt = O_XSpaceCnt;
		g_clipResultDlgInfo.YSpaceCnt = O_YSpaceCnt;
		g_clipResultDlgInfo.coorFont = O_coorFont;
		g_clipResultDlgInfo.XValueCnt = O_XValueCnt;
		g_clipResultDlgInfo.YValueCnt = O_YValueCnt;
		g_clipResultDlgInfo.valueFont = O_valueFont;
		g_clipResultDlgInfo.decimal =   O_decimal;
	}
}

// ����λ������������
void saveClipResultDlgParams(void)
{
	RscFileHandle  userPrefsH;
	if (SUCCESS == mdlDialog_userPrefFileOpen(&userPrefsH, RSC_READWRITE))
	{
		/* --- delete original section parameters --- */
		mdlResource_deleteByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, mdlSystem_getCurrTaskID());

		/* --- add new section parameters --- */
		mdlResource_addByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, &g_clipResultDlgInfo, sizeof(ClipResultDlgInfo), mdlSystem_getCurrTaskID());

		/* --- Close user preference file ---- */
		mdlResource_closeFile(userPrefsH);
	}
}

bool UnloadFunction(UnloadProgramReason unloadType)
{
	// ����λ������������
	saveClipResultDlgParams();

	if (alreadyUnload)
		return false;

	ClearContour();
	ShowCAEModel();

	IViewManager::GetManager().DropViewDecoration(&s_DrawColorShape);
	IViewManager::GetManager().DropViewDecoration(&s_DrawColorShape_temp);
	s_drawtempLine.CloseDrawFunction();
	alreadyUnload = true;

	return false;
}

void ExitDesignFileFunction()
{
	if (alreadyUnload)
		return;

	alreadyUnload = true;
	ClearContour();
	ShowCAEModel();

	s_DrawColorShape.CloseDrawFunction();
	s_DrawColorShape_temp.CloseDrawFunction();
	s_drawtempLine.CloseDrawFunction();
}

/*----------------------------------------------------+
|    constructsolidElemString    		    |
+----------------------------------------------------*/
void constructsolidElemString(char* descrStr, char *elemName, ElementRefP elRef, DgnModelRefP modelRef)
{
	MSElementDescrP  edP = NULL;
	mdlElmdscr_getByElemRef(&edP, elRef, modelRef, FALSE, NULL);
	if (NULL != edP)
	{
		sprintf(descrStr, elemName);
	}
}

void initProgramData()
{
	// ��ȡλ������������
	loadClipResultDlgParams();

	//����ɫ�����ݵĲ��ֳ�ʼ��
	s_DrawColorShape.initialize();
	s_DrawColorShape_temp.initialize();

	s_DrawColorShape.DrawPos.x = -200;
	s_DrawColorShape.DrawPos.y = 50;
	s_DrawColorShape_temp.DrawPos.x = -200;
	s_DrawColorShape_temp.DrawPos.y = 400;

	IViewManager::GetManager().AddViewDecoration(&s_DrawColorShape);
	IViewManager::GetManager().AddViewDecoration(&s_DrawColorShape_temp);

	//ɫ��ָ��ȱʡָ����ɫ��
	s_pDCShape = &s_DrawColorShape;

	mdlColorPal_create(&ppPalette, 255);

	//������������飬��������û��Ԫ����ɾ��֮
	for each (NamedGroupPtr namedGroup in *NamedGroupCollection::Create(*MASTERFILE))
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		// �����������û��Ԫ��,�ʹ�Dgn��ɾ����
		if (graphicMembers == 0)
		{
			namedGroup->DeleteFromFile();
		}
	}
	
	// ȡ��CAE�������
	getRstTypeInfFromDB(g_RstType_Name, g_RstType_Unit);

	alreadyUnload = false;
}

static  MdlCommandName cmdNames[] =
{
	{ ImportCAEInfCmd, "ImportCAEInf" },
	{ ShowCAESolidAllCmd, "ShowCAESolidAll" },
	{ ShowCAESolidPartCmd, "ShowCAESolidPart" },
	{ SetCAERstDispCmd, "SetCAERstDisp" },
	{ ExportToParasolidCmd, "EXPORTTOPARASOLID" },
	0,
};

static MdlCommandNumber cmdNumbers[] =
{
	{ (CmdHandler)ImportCAEInfCmd, CMD_MSDI_MSCAESI_ACTION_IMPORTCAEINFO },
	{ (CmdHandler)ShowCAESolidAllCmd, CMD_MSDI_MSCAESI_ACTION_SHOWCAESOLIDALL },
	{ (CmdHandler)ShowCAESolidPartCmd, CMD_MSDI_MSCAESI_ACTION_SHOWCAESOLIDPART },
	{ (CmdHandler)SetCAERstDispCmd, CMD_MSDI_MSCAESI_ACTION_SETCAERSTDISP },
	{ (CmdHandler)ExportToParasolidCmd, CMD_MSDI_MSCAESI_ACTION_EXPORTTOPARASOLID },

	{ (CmdHandler)ClipFaceCmd, CMD_MSDI_MSCAESI_ACTION_CLIPEFACE },
	{ (CmdHandler)ClipPlotFaceCmd, CMD_MSDI_MSCAESI_ACTION_CLIPPLOTEFACE },
	{ (CmdHandler)SavePicture, CMD_MSDI_MSCAESI_ACTION_SAVEPICTURE },

	{ (CmdHandler)exportCalDocCmd, CMD_MSDI_MSCAESI_ACTION_EXPORTCAL },
	{ (CmdHandler)fenceDisplayCmd, CMD_MSDI_MSCAESI_ACTION_FENCEDISPLAY },
	{ (CmdHandler)recoverDisplayCmd, CMD_MSDI_MSCAESI_ACTION_RECOVERDISPLAY },
	{ (CmdHandler)caseCompareCmd, CMD_MSDI_MSCAESI_ACTION_CASECOMPARE },
	{ (CmdHandler)clipResultShowCmd, CMD_MSDI_MSCAESI_ACTION_CLIPRESULT },
	{ (CmdHandler)surfaceShowCmd, CMD_MSDI_MSCAESI_ACTION_SURFACESHOW },
	{ (CmdHandler)outputRebarCmd, CMD_MSDI_MSCAESI_ACTION_OUTPUTREBAR },
	{ (CmdHandler)deleteDisplayCmd, CMD_MSDI_MSCAESI_ACTION_DELETEDISPLAY },
	{ (CmdHandler)nodeViewerCmd, CMD_MSDI_MSCAESI_ACTION_NODEVIEWER },
	0,
};

Public DialogHookInfo uHooks[] =
{
	{ HOOKDIALOGID_ImportCaeInf, (PFDialogHook)ImportCaeInfDialog_mdlDialogHook },
	{ HOOKITEMID_TBSelImpDat, (PFDialogHook)ImportCaeInfDialog_TBSelImpDatHook },
	{ HOOKBUTTON_DelResultFile, (PFDialogHook)ImportCaeInfDialog_DelResultFileHook },
	{ HOOKCOMBOBOX_SelectCaeModel, (PFDialogHook)ImportCaeInfDialog_SelectCaeModelHook },

	{ HOOKBUTTON_SelectCaeModel, (PFDialogHook)SelectCaeModelPath },
	{ HOOKBUTTON_AddResultFile, (PFDialogHook)ImportCaeInfDialog_AddResultFileHook },

	{ HOOKDIALOG_ColorSetting, (PFDialogHook)HookDialog_ColorSetting },
	{ HOOKTOGGLE_ColorSet_Automatic, (PFDialogHook)HookToggle_ColorSet_Automatic },
	{ HOOKTEXT_ColorSet_ColorValue, (PFDialogHook)HookText_ColorSet_ColorValue },
	{ HOOKPUSHBTN_ColorSet_Apply, (PFDialogHook)HookPushBtn_ColorSet_Apply },
	{ HOOKITEMID_PBtnColorChooser, (PFDialogHook)HookPushBtn_ColorSet_ColorChooser },
	
	{ HOOKTOGGLE_ColorSet_Gray, (PFDialogHook)HookPushBtn_ColorSet_Gray },
	{ HOOKITEMID_ColorChooser, (PFDialogHook)TColorChooser },
	{ HOOKITEMID_ListBox_DialogDemo, (PFDialogHook)ListBoxCreate },
	{ HOOKITEMID_Updownbutton, (PFDialogHook)Updownbutton },
	{ HOOKTEXT_CellOfColor, (PFDialogHook)HookText_CellOfColor },

	{ HOOKDIALOGID_SolidContourDisp, (PFDialogHook)HookDialog_SolidDisp },
	{ HOOKITEMID_Tree, (PFDialogHook)HookTree_SolidDisp },
	{ HOOKBUTTON_ShowSet, (PFDialogHook)HookButton_SolidDisp_ShowSet },
	{ HOOKSolidContourDisp_ResultType, (PFDialogHook)HookComboBox_SolidDisp_ResultType },
	{ HOOKCOMBOBOX_SolidDisp_ShowModel, (PFDialogHook)HookComboBox_SolidDisp_ShowModel },
	{ HOOKCOMBOBOX_SolidDisp_CondType, (PFDialogHook)HookComboBox_SolidDisp_CondType },
	{ HOOKCOMBOBOX_SolidDisp_CondVal, (PFDialogHook)HookComboBox_SolidDisp_CondVal },
	{ HOOKBUTTON_SolidDisp_AddC, (PFDialogHook)HookButton_SolidDisp_AddC },
	{ HOOKBUTTON_SolidDisp_DelC, (PFDialogHook)HookButton_SolidDisp_DelC },
	{ HOOKBUTTON_SolidDisp_ClearC, (PFDialogHook)HookButton_SolidDisp_ClearC },
	{ HOOKPUSHBTN_SolidDisp_Show, (PFDialogHook)HookButton_SolidDisp_Show },
	
	{ HOOKPUSHBTN_Damage_Show, (PFDialogHook)HookButton_Damage_Show },
	{ HOOKCOMBOBOX_SolidDisp_ShowType, (PFDialogHook)HookComboBox_SolidDisp_ShowType },
	// ����ķָ���
	{ HOOKSASH_SolidDisp_V, (PFDialogHook)HookSash_SolidDisp_vSashHook },
	{ HOOKPUSHBTN_BoundaryShow, (PFDialogHook)HookButton_Boundary_Show },
	

	{ HOOKBUTTON_SelectExport, (PFDialogHook)Select_exportHook },
	{ HOOKBUTTON_AllExport, (PFDialogHook)All_exportHook },

	{ HOOKDIALOGID_SolidAllShow, (PFDialogHook)SolidMeshShowDlg_SolidAllShowHook },

	{ HOOKDIALOG_ExpCalc, (PFDialogHook)HookDialog_expCalc },
	{ HOOKPUSHBTN_ExpCalc_Import, (PFDialogHook)HookPushBtn_ExpCalc_Import },
	{ HOOKPUSHBTN_ExpCalc_AddPath, (PFDialogHook)HookPushBtn_ExpCalc_AddPath },
	{ HOOKPUSHBTN_ExpCalc_DelPath, (PFDialogHook)HookPushBtn_ExcCalc_DelPath },
	{ HOOKPUSHBTN_ExpCalc_Submit, (PFDialogHook)HookPushBtn_ExpCalc_Submit },
	{ HOOKLISTBOX_ExpCalc_CaseList, (PFDialogHook)HookListBox_ExpCalc_CaseList },
	{ HOOKLISTBOX_ExpCalc_ModelParamList, (PFDialogHook)HookListBox_ExpCalc_ModelParamList },
	{ HOOKLISTBOX_ExpCalc_WYParamList, (PFDialogHook)HookListBox_ExpCalc_WYParamList },
	{ HOOKLISTBOX_ExpCalc_YLParamList, (PFDialogHook)HookListBox_ExpCalc_YLParamList },
	{ HOOKLISTBOX_ExpCalc_PJParamList, (PFDialogHook)HookListBox_ExpCalc_PJParamList },
	{ HOOKMLTEXT_ExpCalc_ProDescr, (PFDialogHook)HookMlText_ExpCalc_ProDescr },
	{ HOOKCOMBOBOX_ExpCalc_ProjectLists, (PFDialogHook)HookComboBox_ExpCalc_ProjectLists },

	{ HOOKCOMBOBOX_CalRebar_RebarType, (PFDialogHook)HookComboBox_CalRebar_RebarType },
	{ HOOKCOMBOBOX_CalRebar_ConcrType, (PFDialogHook)HookComboBox_CalRebar_ConcrType },

	{ HOOKCOMBOBOX_PartDis_CondName, (PFDialogHook)HookComboBox_PartDis_CondName },
	{ HOOKCOMBOBOX_PartDis_CondVal, (PFDialogHook)HookComboBox_PartDis_CondVal },
	{ HOOKBUTTON_PartDis_Add, (PFDialogHook)HookButton_PartDis_Add },
	{ HOOKBUTTON_PartDis_Del, (PFDialogHook)HookButton_PartDis_Del },
	{ HOOKBUTTON_PartDis_Clear, (PFDialogHook)HookButton_PartDis_Clear },
	{ HOOKLISTBOX_PartDis_Conditions, (PFDialogHook)HookListBox_PartDis_Conditions },

	{ HOOKDIALOG_CaseCompare, (PFDialogHook)HookDialog_CaseCompare },
	{ HOOKCOMBOBOX_CaseComp_InstanceL, (PFDialogHook)HookComboBox_CaseComp_InstanceL },
	{ HOOKCOMBOBOX_CaseComp_InstanceR, (PFDialogHook)HookComboBox_CaseComp_InstanceR },
	{ HOOKBUTTON_CaseComp_SettingL, (PFDialogHook)HookButton_CaseComp_SettingL },
	{ HOOKBUTTON_CaseComp_SettingR, (PFDialogHook)HookButton_CaseComp_SettingR },
	{ HOOKBUTTON_CaseComp_Compare, (PFDialogHook)HookButton_CaseComp_Compare },
	{ HOOKTREE_CaseComp_CaseList, (PFDialogHook)HookTree_CaseComp_CaseList },
	{ HOOKCOMBOBOX_CaseComp_ResultType, (PFDialogHook)HookComboBox_CaseComp_ResultType },
	{ HOOKCOMBOBOX_CaseComp_ShowType, (PFDialogHook)HookComboBox_CaseComp_ShowType },
	{ HOOKCOMBOBOX_CaseComp_ShowModel, (PFDialogHook)HookComboBox_CaseComp_ShowModel },

	{ HOOKDIALOG_ClipResult, (PFDialogHook)HookDialog_ClipResult },
	{ HOOKTREE_ClipResult_ClipList, (PFDialogHook)HookTree_ClipResult_ClipList },
	{ HOOKTABPAGE_ClipResult_ResultType, (PFDialogHook)HookTabpage_ClipResult_ResultType },
	{ HOOKOPTBUTTON_ClipResult_D_ALL, (PFDialogHook)HookOptButton_ClipResult_D_ALL },
	{ HOOKOPTBUTTON_ClipResult_Z_ALL, (PFDialogHook)HookOptButton_ClipResult_Z_ALL },
	{ HOOKBUTTON_ClipResult_Clear, (PFDialogHook)HookButton_ClipResult_Clear },
	{ HOOKBUTTON_ClipResult_Submit, (PFDialogHook)HookButton_ClipResult_Submit },
	{ HOOKBUTTON_ClipResult_OutDGN, (PFDialogHook)HookButton_ClipResult_OutDGN },
	// ����ķָ���
	{ HOOKSASH_ClipResult_V, (PFDialogHook)HookSash_ClipResult_vSashHook },

	{ HOOKDIALOGID_SurfaceShow, (PFDialogHook)HookDialog_SurfaceShow },
	{ HOOKTREE_Surface_ClipList, (PFDialogHook)HookTree_Surface_ClipList },
	{ HOOKCOMBOBOX_Surface_Model, (PFDialogHook)HookComboBox_Surface_Model },
	{ HOOKCOMBOBOX_Surface_ResultType, (PFDialogHook)HookComboBox_Surface_ResultType },
	{ HOOKBUTTON_Surface_ResultShow, (PFDialogHook)HookButton_Surface_ResultShow },
	{ HOOKCOMBOBOX_SurfaceDisp_ShowType, (PFDialogHook)HookButton_Surface_ShowMinMax},
	{ HOOKPDM_Surface_Add, (PFDialogHook)HookPdm_Surface_Add },
	{ HOOKPDM_Surface_Del, (PFDialogHook)HookPdm_Surface_Del },
	{ HOOKBUTTON_Surface_ShowSet, (PFDialogHook)HookButton_Surface_ShowSet },
	// ����ķָ���
	{ HOOKSASH_Surface_V, (PFDialogHook)HookSash_Surface_vSashHook },

	{ HOOKDIALOGID_CalRebar_RebarCalculate, (PFDialogHook)HookDialog_CalRebar_InterfaceDynamic },	
	{ HOOKITEMID_CalRebar_SectionTree, (PFDialogHook)HookTree_CalRebar_SectionTree },
	{ HOOKSASH_CalRebar_RebarCalculate_V, (PFDialogHook)HookSash_CalRebar_vSashHook },
	{ HOOKPUSHBTN_CalRebar_Calculate, (PFDialogHook)HookPushBtn_CalRebar_Calculate},
	{ HOOKPUSHBTN_CalRebar_Refresh, (PFDialogHook)HookPushBtn_CalRebar_Refresh },
	{ HOOKPUSHBTN_CalRebar_OutPut, (PFDialogHook)HookPushBtn_CalRebar_OutPut },
	{ HOOKGENERIC_CalRebar_Canva, (PFDialogHook)HookGeneric_CalRebar_Canva },
	{ HOOKLISTBOX_CalRebar_CalcResult, (PFDialogHook)HookListBox_CalRebar_CalcResult },
	{ HOOKLISTBOX_CalRebar_RebarInfo, (PFDialogHook)HookListBox_CalRebar_RebarInfo},
	{ HOOKITEMID_CalRebar_CellCombbox_Diameter, (PFDialogHook) HookItem_CalRebar_CellCombbox_Diameter},
	{ HOOKITEMID_CalRebar_CellTxt_RebarNumber, (PFDialogHook)HookItem_CalRebar_CellTxt_RebarNumber },
	{ HOOKTXTID_CalRebar_RebarTesion, (PFDialogHook)HOOKText_CalRebar_RebarTesion },
	{ HOOKTXTID_CalRebar_ConcrTesion,(PFDialogHook)HOOKText_CalRebar_ConcrTesion },
	{ HOOKPDM_CalRebar_AddRebarLine, (PFDialogHook)HookPdm_CalRebar_AddRebarLine },
	{ HOOKPDM_CalRebar_DelRebarLine, (PFDialogHook)HookPdm_CalRebar_DelRebarLine },

	{ HOOKDIALOGID_CalRebar_AddRebarLine, (PFDialogHook)HookDialog_CalRebar_AddRebarLine },
	{ HOOKITEMID_Surface_LockAxis, (PFDialogHook)HookItem_LockAxis},
	{ HOOKDIALOGID_Surface_AddClipFace, (PFDialogHook)HookDialog_Surface_AddClipFace },
};

//bool CheckRegister()
//{
//	HMODULE hModule = LoadLibrary("Decode.dll");
//	bool rt = false;
//	if (hModule != NULL)
//	{
//		RegisterFunc func = (RegisterFunc)GetProcAddress(hModule, "CheckRegister");
//		if (func == NULL)
//		{
//			rt = false;
//		}
//		else
//		{
//			rt = func();
//		}
//		FreeLibrary(hModule);
//	}
//	return rt;
//}

/*----------------------------------------------------------------------------------+
* ��������: �������															    *
* @param        argc     The count of args passed to the application			    *
* @param        argv[]   The array of args passed to this application as char arrays*
* @return																		    *
+-----------------------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain(int argc, WCharCP argv[])
{
	RscFileHandle   rfHandle;
	SymbolSet       *setP;
	StatusInt		iStat;

	//if (!CheckRegister())
	//{
	//	mdlInput_sendKeyin(L"mdl unload MsdiMsCaeSi", 0, INPUTQ_EOQ, NULL);
	//	return;
	//}

	mdlResource_openFile(&rfHandle, NULL, RSC_READONLY);

	mdlState_registerStringIds(STRINGLISTID_Commands, STRINGLISTID_Prompts);

	mdlSystem_registerCommandNames(cmdNames);
	mdlSystem_registerCommandNumbers(cmdNumbers);
	mdlParse_loadCommandTable(NULL);

	/* set up the variables we are going to set from dialog boxes */
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX, 0, FALSE);

	// �����ṹ���ͱ���
	iStat = mdlDialog_publishComplexVariable(setP, "impInfDlgInfo", "g_ImpDlgInfo", &g_ImpDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "solidDispDlgInfo", "g_solidDispDlgInfo", &g_solidDispDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "expCalculationDlgInfo", "g_expCalculationInfo", &g_expCalculationInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "reBarCalDlgInfo", "g_rebarCalDlgInfo", &g_rebarCalDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "partialDisDlgInfo", "g_partialDisDlgInfo", &g_partialDisDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "caseCompDlgInfo", "g_caseCompDlgInfo", &g_caseCompDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "clipResultDlgInfo", "g_clipResultDlgInfo", &g_clipResultDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "surfaceShowDlgInfo", "g_surfaceShowDlgInfo", &g_surfaceShowDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "outRebarDlgInfo", "g_outRebarDlgInfo", &g_outRebarDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "colorSettingDlgInfo", "g_colorSettingDlgInfo", &g_colorSettingDlgInfo);
	
	// �ù�������ַ����������ID���
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	// �趨����ж��ʱҪ�����ĺ���
	SystemCallback::SetUnloadProgramFunction(UnloadFunction);

	SystemCallback::SetExitDesignFileStateFunction(ExitDesignFileFunction);

	initProgramData();
}
