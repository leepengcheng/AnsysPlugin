#include "MsdiMsCaeSipch.h"

extern BSIColorDescr*	m_pBackgroundColor;
extern BSIColorDescr*	m_pForegroundColor;
extern DgnPlatform::BSIColorPalette*  ppPalette;
extern SurfaceShowDlgInfo	g_surfaceShowDlgInfo;
extern ColorSettingDlgInfo	g_colorSettingDlgInfo;

DrawColorShape	s_DCShape_Setting;
DrawColorShape*	s_pDCSSetting;

SourceTypeDCS s_sourceDCS;

extern DPoint3d s_SecCountourBasePt;

void HookDialog_ColorSetting(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_INIT:
	{
		//�����Ƿ�automatic����/����ָ���ؼ�
		DialogItem* diP_Widget = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP_Widget->itemIndex, !s_DCShape_Setting.m_bAutomatic, false);
		diP_Widget = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP_Widget->itemIndex, !s_DCShape_Setting.m_bAutomatic, false);

		//��Ӧ�ð�ť��Ϊ����
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP->itemIndex, true, false);

		mdlDialog_itemsSynch(dmP->db);
	}break;
	default:
		dmP->msgUnderstood = FALSE; 
		break;
	}
}

ListModel* createListModel(int nCols)
{
	//list���������
	ListModel*		pListModel = NULL;
	ListRow*		pRow = NULL;
	int				rowIndex;//��
	int				colIndex;//��

	pListModel = mdlListModel_create(nCols);

	//�ɶ�Ӧ����ɫ����������Ӧ�ı�
	for (rowIndex = 0; rowIndex < s_DCShape_Setting.ColorNum_Legeng; rowIndex++)
	{
		pRow = mdlListRow_create(pListModel);
		for (colIndex = 0; colIndex < nCols; colIndex++)
		{
			ListCell*	pCell = NULL;
			WChar		w_value[32];
			char        c_value[32];
			sprintf(c_value, "%d", rowIndex);

			BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);

			pCell = mdlListRow_getCellAtIndex(pRow, colIndex);

			switch (colIndex)//���ݲ�ͬ�������ò�ͬ��ֵ
			{
			case 0://���
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			case 1://��ɫ
			{
				BSIColorDescr*	m_pBackgroundColor;
				BSIColorDescr*	m_pForegroundColor;
				m_pBackgroundColor = mdlColorPal_getColorDescr(ppPalette, rowIndex);
				m_pForegroundColor = mdlColorPal_getColorDescr(ppPalette, rowIndex);
				sprintf(c_value, "(%d,%d,%d)", s_DCShape_Setting.fillColor[rowIndex].red, s_DCShape_Setting.fillColor[rowIndex].green,
					s_DCShape_Setting.fillColor[rowIndex].blue);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setIcon(pCell, ICONID_ColorPatch, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//	RgbColorDef testColor;
				//	testColor.red = s_DCShape_Setting.fillColor[rowIndex].red;
				//	testColor.green = s_DCShape_Setting.fillColor[rowIndex].green;
				//	testColor.blue = s_DCShape_Setting.fillColor[rowIndex].blue;

				mdlColorDescr_setByRgb(m_pForegroundColor, &s_DCShape_Setting.fillColor[rowIndex], FALSE);
				mdlListCell_setIconColorOverrides(pCell, m_pBackgroundColor, m_pForegroundColor);
				mdlListCell_setEditor(pCell, RTYPE_Container, CONTAINERID_ColorChooser, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);
			}
			break;
			case 2://������Сֵ
			{
				double	flag = s_DCShape_Setting.ClrShapeVal_Legeng[rowIndex];
				sprintf(c_value, "%0.5f", flag);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setLongValue(pCell, flag);
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			}
			case 3://�������ֵ
			{
				double	flag = s_DCShape_Setting.ClrShapeVal_Legeng[rowIndex + 1];
				sprintf(c_value, "%0.5f", flag);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setLongValue(pCell, flag);
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			}

			}
		}
		mdlListModel_addRow(pListModel, pRow);
	}

	return pListModel;
}

/*--------------------------------------------------------------------------------------+
|   updateColorListBox�����û��ڽ���������Ӧ��ֵ��ˢ����ɫ�����Ӧ����ֵ
+--------------------------------------------------------------------------------------*/
void updateColorListBox(MSDialog* db)
{	
	s_DCShape_Setting.StartValue = g_colorSettingDlgInfo.dStartValue;
	s_DCShape_Setting.EndValue = g_colorSettingDlgInfo.dEndValue;
	s_DCShape_Setting.ColorNum_Legeng = g_colorSettingDlgInfo.iColorNum;

	s_DCShape_Setting.divideColorRange();
	ListModel* pListModel = createListModel(4);
	mdlDialog_listBoxSetListModelP(db->GetItemByTypeAndId(RTYPE_ListBox, LISTBOXID_ColorSet_ColorList)->rawItemP, pListModel, 0);
}

/*--------------------------------------------------------------------------------------+
|   HookPushBtn_ColorSet_ColorChooser��ѡ����ɫ���ˢ�º���
+--------------------------------------------------------------------------------------*/
void HookPushBtn_ColorSet_ColorChooser(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		RscId	     	iconId = ICONID_ColorBoundary;
		RscType	  		iconType = RTYPE_Icon;
		BSIColorDescr*	pFgColorDesc = NULL;
		ListCell*		pCell = NULL;
		ValueUnion		uValue;

		mdlDialog_pushButtonSetIcon(dimP->dialogItemP->rawItemP, NULL, &iconId, &iconType, NULL);

		mdlListCell_getLongValue(pCell, &uValue.sLongFormat);

		mdlColorPal_getElemColorDescrByModelRef(&pFgColorDesc, uValue.sLongFormat, MASTERFILE, 0);
		mdlDialog_pushButtonSetIconColors(dimP->dialogItemP->rawItemP, &pFgColorDesc, NULL);

		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		mdlDialog_lastActionTypeSet(dimP->db, ACTIONBUTTON_OK);
		mdlWindow_hide((MSWindow *)dimP->db, HideReasonCode::Exiting, TRUE);
		mdlDialog_closeCommandQueue(dimP->db);

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   TColorChooser�������ɫ��ʱ����Ӧ����
+--------------------------------------------------------------------------------------*/
void TColorChooser(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		//int				format;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		//	Get a reference to the ListBox current ListCell
		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		//	Get this ColorChooser's Color index
		mdlDialog_itemGetValue(uValue, dimP->db, dimP->itemIndex);
		if (uValue.IsSet())
		{
			int theIdex = uValue.GetLong();
			m_pForegroundColor = mdlColorPal_getColorDescr(NULL, theIdex);
			RgbColorDef tRGB;
			mdlColorDescr_getRgb(&tRGB, m_pForegroundColor);
			s_DCShape_Setting.fillColor[row] = tRGB;
			s_DCShape_Setting.fillColorTBGR[row] = (tRGB.blue << 16) + (tRGB.green << 8) + tRGB.red;
			// ������ɫRGBֵ��ȡ��MS�����Ӧ����ɫID
			mdlColor_rawColorFromRGBColor(&s_DCShape_Setting.fillCororIndex[row], &s_DCShape_Setting.fillColor[row], ACTIVEMODEL);

			mdlListCell_setIconColorOverrides(pCell, m_pBackgroundColor, m_pForegroundColor);
			mdlDialog_itemDraw(dimP->db, pLBox->itemIndex);
			dimP->db->Refresh();
		}
		//	Inform the ListCell that we've finished editing
		mdlDialog_lastActionTypeSet(dimP->db, ACTIONBUTTON_OK);
		mdlWindow_hide((MSWindow *)dimP->db, HideReasonCode::HideWindow, TRUE);
		mdlDialog_closeCommandQueue(dimP->db);
		//printf ("ColorChooser_STATECHANGED value=%d\n", uValue.sLongFormat);
		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   ListBoxCreate����ɫ���öԻ��򹳺���
+--------------------------------------------------------------------------------------*/
void ListBoxCreate(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		RawItemHdr*	 pThis = dimP->dialogItemP->rawItemP;
		ListModel*	pListModel = createListModel(MAX_COLUMN_COUNT);
		mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   Updownbutton��ˢ�¼���Ӧ������
+--------------------------------------------------------------------------------------*/
void Updownbutton(DialogItemMessage *dimP)
{
	RawItemHdr*		pThis;
	pThis = dimP->dialogItemP->rawItemP;
	MSValueDescr uvalue;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		uvalue.SetDouble(g_colorSettingDlgInfo.iColorNum);
		mdlDialog_itemSetValue(NULL, uvalue, dimP->db, dimP->itemIndex);
		double minValue, maxValue, incAmount;
		bool isVertical;
		minValue = 1;
		maxValue = 50;
		incAmount = 1;
		isVertical = true;
		mdlDialog_upDnBtnsSetInfo(&minValue, &maxValue, &incAmount, &isVertical, TRUE, pThis);
		break;
	}
	case DITEM_MESSAGE_BUTTON:
	{
		mdlDialog_itemGetValue(uvalue, dimP->db, dimP->itemIndex);
		g_colorSettingDlgInfo.iColorNum = (int)uvalue.GetDouble();
		updateColorListBox(dimP->db);

		//��Ӧ�ð�ť��Ϊ����
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookText_CellOfColor(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		WCharCP textValue = NULL;
		mdlListCell_getStringValue(pCell, &textValue);

		bool theValue = true;
		dimP->dialogItemP->SetValue(&theValue, textValue);

		break;
	};
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		//mdlDialog_itemSetStringValue(&theValue, textValue, dbP, oDialogItemPath->GetItemIndex());
		WString setValue;
		dimP->dialogItemP->GetValue(setValue);

		double t_value = BeStringUtilities::Wtof(setValue.GetWCharCP());

#pragma region �ж��б�CELL�޸�ֵ�Ƿ����Ҫ��

		if (row == 0)//��һ��
		{
			if (col == 2)//��Сֵ
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//��һ����Сֵ���ܱ����ֵ��
				{
					mdlDialog_dmsgsPrint(L"���ܴ��������ֵ��");
					break;
				}
			}
			else
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 2])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"���ܴ��������ֵ��");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"����С������Сֵ��");
					break;
				}

			}
		}
		else if (row == s_DCShape_Setting.ColorNum_Legeng - 1)//���һ��
		{
			if (col == 2)//��Сֵ
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//��һ����Сֵ���ܱ����ֵ��
				{
					mdlDialog_dmsgsPrint(L"���ܴ��������ֵ��");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row - 1])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"����С������Сֵ��");
					break;
				}
			}
			else
			{
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"����С������Сֵ��");
					break;
				}
			}
		}
		else
		{
			if (col == 2)//��Сֵ
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//��һ����Сֵ���ܱ����ֵ��
				{
					mdlDialog_dmsgsPrint(L"���ܴ��������ֵ��");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row - 1])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"����С������Сֵ��");
					break;
				}
			}
			else
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 2])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"���ܴ��������ֵ��");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//��һ�����ֵ���ܱȵڶ������ֵ��
				{
					mdlDialog_dmsgsPrint(L"����С������Сֵ��");
					break;
				}
			}
		}

#pragma endregion

#pragma region ���޸ĺ��CELLֵ��д��list��
		if (row == 0)
		{
			if (col == 3)//�����
			{
				ListCell*		npCell = NULL;
				ListRow*		npRow = NULL;
				npRow = mdlListModel_getRowAtIndex(pModel, row + 1);
				npCell = mdlListRow_getCellAtIndex(npRow, col - 1);
				mdlListCell_setStringValue(npCell, setValue.GetWCharCP(), TRUE);
			}
		}
		else if (row == s_DCShape_Setting.ColorNum_Legeng - 1)
		{
			if (col == 2)//��С��
			{
				ListCell*		tpCell = NULL;
				ListRow*		tpRow = NULL;
				tpRow = mdlListModel_getRowAtIndex(pModel, row - 1);
				tpCell = mdlListRow_getCellAtIndex(tpRow, col + 1);
				mdlListCell_setStringValue(tpCell, setValue.GetWCharCP(), TRUE);
			}
		}
		else
		{
			if (col == 2)//��С��
			{
				ListCell*		tpCell = NULL;
				ListRow*		tpRow = NULL;
				tpRow = mdlListModel_getRowAtIndex(pModel, row - 1);
				tpCell = mdlListRow_getCellAtIndex(tpRow, col + 1);
				mdlListCell_setStringValue(tpCell, setValue.GetWCharCP(), TRUE);
			}
			if (col == 3)//�����
			{
				ListCell*		npCell = NULL;
				ListRow*		npRow = NULL;
				npRow = mdlListModel_getRowAtIndex(pModel, row + 1);
				npCell = mdlListRow_getCellAtIndex(npRow, col - 1);
				mdlListCell_setStringValue(npCell, setValue.GetWCharCP(), TRUE);
			}
		}
		mdlListCell_setStringValue(pCell, setValue.GetWCharCP(), TRUE);
#pragma endregion

		if (col == 2)
		{
			s_DCShape_Setting.ClrShapeVal_Legeng[row] = t_value;
			if (row == 0)
			{
				s_DCShape_Setting.StartValue = t_value;
			}
		}
		else
		{
			if (row == s_DCShape_Setting.ColorNum_Legeng - 1)
			{
				s_DCShape_Setting.EndValue = t_value;
			}
			s_DCShape_Setting.ClrShapeVal_Legeng[row + 1] = t_value;
		}

		/*dbP->Refresh();
		mdlDialog_itemsSynch(dbP);*/
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookToggle_ColorSet_Automatic(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem* diP_Widget = NULL;
		
		//��ѡ��AutoMaticʱ����ѯ���ݿ����ɫ����Χ�ļ��㣬������Χ�ؼ���Ϊ������
		if (g_colorSettingDlgInfo.bAutomatic != 0)
		{
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, false, false);
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, false, false);

			s_DCShape_Setting.m_bAutomatic = true;
			
		
			switch (s_sourceDCS)
			{
				//����ԴΪ��չʾ�����Զ����á���ʾ�����ݿ��м���
				case SolidDisplay:
				{
					s_DCShape_Setting.initColorRangeFromDB();
					s_DCShape_Setting.divideColorRange();
				}break;
				//����ԴΪ��չʾ�����Զ����á���ʾ��������ʵ�ʵ������м���
				case SurfaceDisplay:
				{
					double tStrValue = 0;
					double tEndValue = 0;
					//ͨ���û���ѡ���ȡ�������
					s_DCShape_Setting.CaseID = s_pDCShape->CaseID;
					s_DCShape_Setting.ProjectId = s_pDCShape->ProjectId;
					DPoint3d minPos, maxPos;
					GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, s_DCShape_Setting.ProjectId, s_DCShape_Setting.CaseID, s_DCShape_Setting.ResultTypeCode, minPos, maxPos);
					//����ѡ��ľ��幤���ͽ����������ɫ��
					s_DCShape_Setting.StartValue = tStrValue;
					s_DCShape_Setting.EndValue = tEndValue;
					bvector<DPoint3d>().swap(s_DCShape_Setting.m_vecMinPts);
					bvector<DPoint3d>().swap(s_DCShape_Setting.m_vecMaxPts);
					s_DCShape_Setting.m_vecMinPts.push_back(minPos);
					s_DCShape_Setting.m_vecMaxPts.push_back(maxPos);
					s_DCShape_Setting.divideColorRange();
				}
				break;
				//����ԴΪ�����Աȣ����Զ����á���ʾ�����ݿ��м���
				case CaseCompare:
				{
					s_DCShape_Setting.initColorRangeFromDB();
					s_DCShape_Setting.divideColorRange();
				}break;
			default:
				break;
			}

			//��������ɫ�����ֵ��Сֵͬ�����Ի�����
			g_colorSettingDlgInfo.dStartValue = s_DCShape_Setting.StartValue;
			g_colorSettingDlgInfo.dEndValue = s_DCShape_Setting.EndValue;			
		}
		//����ѡ��AutoMaticʱ�����ŷ�Χ�ؼ�
		else
		{
			s_DCShape_Setting.m_bAutomatic = false;
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, true, false);
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, true, false);
		}

		//��Ӧ�ð�ť��Ϊ����
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
	}
}
void HookPushBtn_ColorSet_Gray(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_colorSettingDlgInfo.Gray != 0)
		{
			s_DCShape_Setting.m_Gray = true;
		}
		else
		{
			s_DCShape_Setting.m_Gray = false;
		}

		//����ɫ��Χ&��ɫ���ؼ�ֵ���޸�ʱ��ˢ��ListBox����ʾ
		updateColorListBox(dimP->db);
	}
	default:
		dimP->msgUnderstood = FALSE;
	}
}
void HookText_ColorSet_ColorValue(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		//����ɫ��Χ&��ɫ���ؼ�ֵ���޸�ʱ��ˢ��ListBox����ʾ
		updateColorListBox(dimP->db);

		//ͬʱ�޸�UpDownButton�б����ֵ
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_UpDownButtons, BUTTONID_UpDown, 0);
		if (diP == NULL)
			return;

		MSValueDescr uvalue;
		uvalue.SetDouble(g_colorSettingDlgInfo.iColorNum);
		mdlDialog_itemSetValue(NULL, uvalue, dimP->db, diP->itemIndex);
		
		//��Ӧ�ð�ť��Ϊ����
		diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ColorSet_Apply(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//������ɫ�����Ƹ�ʵ�ʵ�ɫ������
		*s_pDCSSetting = s_DCShape_Setting;

		//���ݵ�ǰ�Ƿ��Ѿ���ɫ����������ͼչʾ
		switch (s_sourceDCS)
		{
		case SolidDisplay:
			if (s_pDCSSetting->m_bDisplayed)
				startShowContour(s_pDCSSetting);
			break;
		case SurfaceDisplay:
		if (s_pDCSSetting->m_bDisplayed)
				startShowCutFaceContour(s_pDCSSetting);
			break;
		case CaseCompare:
		{
			if (s_DrawColorShape.m_bDisplayed && s_DrawColorShape_temp.m_bDisplayed)
			{
				startShowContour(&s_DrawColorShape);
				startShowContour(&s_DrawColorShape_temp, &s_SecCountourBasePt, false);
			}
		}break;
		default:
			break;
		}		

		//��Ӧ����Ϊ������
		mdlDialog_itemSetEnabledState(dimP->db, dimP->itemIndex, false, false);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void openColorShapeSetting(DrawColorShape* optDCS, SourceTypeDCS source)
{
	s_pDCSSetting = optDCS;
	s_DCShape_Setting = *optDCS;
	s_sourceDCS = source;

	//ʹ�ô����ɫ�������ʼ����colorSetting�Ի���
	g_colorSettingDlgInfo.dStartValue = s_DCShape_Setting.StartValue;
	g_colorSettingDlgInfo.dEndValue = s_DCShape_Setting.EndValue;
	g_colorSettingDlgInfo.iColorNum = s_DCShape_Setting.ColorNum_Legeng;
	g_colorSettingDlgInfo.bAutomatic = s_DCShape_Setting.m_bAutomatic ? -1 : 0;
	g_colorSettingDlgInfo.Gray= s_DCShape_Setting.m_Gray ? -1 : 0;
	int lastAction = -1;
	mdlDialog_openModal(&lastAction, NULL, DIALOGID_SetContourColor);

	//�������öԻ���ķ���ֵ������Ӧ�Ĳ���
	if (lastAction == ACTIONBUTTON_OK)
	{
		//������ɫ�����Ƹ�ʵ�ʵ�ɫ������
		*s_pDCSSetting = s_DCShape_Setting;

		//���ݵ�ǰ�Ƿ��Ѿ���ɫ����������ͼչʾ
		switch (s_sourceDCS)
		{
		case SolidDisplay:
			if (s_pDCSSetting->m_bDisplayed)
				startShowContour(s_pDCSSetting);
			break;
		case SurfaceDisplay:
		if (s_pDCSSetting->m_bDisplayed)
				startShowCutFaceContour(s_pDCSSetting);
			break;
		case CaseCompare:
		{
			if (s_DrawColorShape.m_bDisplayed && s_DrawColorShape_temp.m_bDisplayed)
			{
				startShowContour(&s_DrawColorShape);
				startShowContour(&s_DrawColorShape_temp, &s_SecCountourBasePt, false);
			}
		}break;
		default:
			break;
		}
	}
}
