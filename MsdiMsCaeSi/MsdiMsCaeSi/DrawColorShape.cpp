#include "MsdiMsCaeSipch.h"
double CreateTable::tablewidth = 0.0;
void CreateTable::DrawTextInShape(DPoint3dCR origin_pos, WString drawtext, RotMatrixCR invRotation, TextStringPropertiesPtr pProp,
								IViewOutputP output, IndexedViewportR viewport)
{
	DPoint3d active_pos;
	viewport.ViewToActive(&active_pos, &origin_pos, 1);
	TextStringPtr theUText = TextString::Create(drawtext.GetWCharCP(), &active_pos, &invRotation, *pProp);
	output->SetSymbology(viewport.GetContrastToBackgroundColor(), viewport.GetContrastToBackgroundColor(), 4, 0);
	output->DrawTextString(*theUText);
}

/*--------------------------------------------------------------------------------------+
|CreateAllText��������������ֵ
|   HeaderText   ����ͷֵ
|   ColorNum     ����ɫ��������
|   StartValue   ��������ʼֵ
|   EndValue     ��������ֵֹ
|   output       ����Ӧ�����ͼ
|   viewport     ����Ӧ��ͼָ��
+--------------------------------------------------------------------------------------*/
void CreateTable::CreateAllText(WCharCP projectname,WCharCP casename,WCharCP HeaderText, WCharCP UnitText, int ColorNum, double ClrShapeVal_Legeng[],
	                           IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos)
{
	//λ�ö���
	DPoint3d Origin_pos = tablepos;
	DPoint3d active_pos;

	DPoint3d  viewScale;
	viewScale = *viewport.GetScale();
	DPoint2d  fontSize;
	if (viewport.IsCameraOn())
	{
		CameraParams tempCamera;
		viewport.GetCameraView(&tempCamera);
		//double theScale = viewport.GetCameraRoot()->focalLength;
		//double limit = viewport.GetCameraRoot()->limit;
		//fontSize = { 0.045, 0.075 };//��ͷ�����С
		fontSize = { 0.2 / tempCamera.focalLength, 0.3/ tempCamera.focalLength };//��ͷ�����С
	}
	else
	{
		fontSize = { 9 / viewScale.x, 10 / viewScale.y };//��ͷ�����С
	}


	//����TEXT��Ӧ��������
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	TextStringPropertiesPtr pProp = TextStringProperties::Create(DgnFontManager::GetDecoratorFont(),
		&DgnFontManager::GetDecoratorFont(), fontSize, *pActiveModel);
	pProp->SetJustification(TextElementJustification::CenterMiddle);
	pProp->SetIs3d(true);

	RotMatrix matrix;
	matrix = *viewport.GetRotMatrix();
	RotMatrix invRotation = matrix;
	invRotation.Invert();

	//����projecname->casename
	WString prjandcasename;
	prjandcasename.append(projectname);
	prjandcasename.append(L"(");
	prjandcasename.append(casename);
	prjandcasename.append(L")");
	Origin_pos.x = Origin_pos.x + tablewidth/9;
	Origin_pos.y = Origin_pos.y + TABLE_HEADERY;
	Origin_pos.z = 0;
	DrawTextInShape(Origin_pos, prjandcasename, invRotation, pProp, output, viewport);

	//��ͷλ������(��ֵ����)
	Origin_pos.y = Origin_pos.y + TABLE_UNITY;
	//������ͷ
	DrawTextInShape(Origin_pos, HeaderText, invRotation, pProp, output, viewport);

	//��λλ������
	Origin_pos.y = Origin_pos.y + TABLE_UNITY;
	//������λ
	WString allunitText;
	allunitText.append(L"Unit:");
	allunitText.append(UnitText);
	DrawTextInShape(Origin_pos, allunitText, invRotation, pProp, output, viewport);


	//����ֵ�����С������߶Ȼ�ȡ
	int Color_height;
	Color_height = (TABLE_HEIGHT * 8 / 10) / ColorNum;
	if (ColorNum < 16)
	{
		fontSize.x = fontSize.x / 1.0;
		fontSize.y = fontSize.y / 1.5;
	}
	else
	{
		fontSize.x = fontSize.x / ColorNum * 16;
		fontSize.y = fontSize.y / ColorNum * 8;
	}
	pProp->SetFontSize(fontSize);

	WString lsClrShapeVal;

	//����ֵ����
	Origin_pos.x = tablepos.x + tablewidth / 8 + tablewidth / 5 + 2;
	Origin_pos.z = 0;
	for (int i = 0; i < ColorNum; i++)
	{
		Origin_pos.y = tablepos.y + TABLE_HEIGHT / 6 + i*Color_height + TABLE_HEIGHT / 30;

		lsClrShapeVal.clear();
		lsClrShapeVal.Sprintf(L"%g", ClrShapeVal_Legeng[i]);
		viewport.ViewToActive(&active_pos, &Origin_pos, 1);
		TextStringPtr theText_i = TextString::Create(lsClrShapeVal.GetWCharCP(), &active_pos, &invRotation, *pProp);
		output->DrawTextString(*theText_i);
	}

	//�������ֵֹ
	Origin_pos.y = tablepos.y + TABLE_HEIGHT / 6 + ColorNum*Color_height + TABLE_HEIGHT / 30;

	lsClrShapeVal.clear();
	lsClrShapeVal.Sprintf(L"%g", ClrShapeVal_Legeng[ColorNum]);
	viewport.ViewToActive(&active_pos, &Origin_pos, 1);
	TextStringPtr theText_i = TextString::Create(lsClrShapeVal.GetWCharCP(), &active_pos, &invRotation, *pProp);
	output->DrawTextString(*theText_i);
}

/*--------------------------------------------------------------------------------------+
|CreateAllColorTable������������ɫ��
|   ColorNum        ����ɫ��������
|   lineColorTBGR   ��������ɫ
|   fillColorTBGR   ��������ɫ���Ӧ�������ɫ
|   output          ����Ӧ�����ͼ
|   viewport        ����Ӧ��ͼָ��
+--------------------------------------------------------------------------------------*/
void CreateTable::CreateAllColorTable(int ColorNum, UInt32 lineColorTBGR, UInt32 fillColorTBGR[], IViewOutputP output, IndexedViewportR viewport,DPoint3d tablepos)
{
	//��ɫ����ʼλ�û�ȡ
	DPoint3d Origin_pos = tablepos;
	if (fillColorTBGR == NULL)
	{
		return;
	}

	DPoint3d v_test[5], v_active[5];
	v_test[0] = Origin_pos;
	v_test[0].z = 0;
	v_test[1].x = Origin_pos.x + tablewidth;
	v_test[1].y = Origin_pos.y;
	v_test[1].z = 0;
	v_test[2].x = Origin_pos.x + tablewidth;
	v_test[2].y = Origin_pos.y + TABLE_HEIGHT;
	v_test[2].z = 0;
	v_test[3].x = Origin_pos.x;
	v_test[3].y = Origin_pos.y + TABLE_HEIGHT;
	v_test[3].z = 0;
	v_test[4] = Origin_pos;
	v_test[4].z = 0;

	//����ķ���
	viewport.ViewToActive(v_active, v_test, 5);
	output->SetSymbology(viewport.GetBackgroundColor(), viewport.GetBackgroundColor(), 0, 0);
	output->DrawShape3d(5, v_active, true, NULL);
	output->SetSymbology(viewport.GetContrastToBackgroundColor(), viewport.GetContrastToBackgroundColor(), 0, 0);
	output->DrawShape3d(5, v_active, false, NULL);

	//����������ɫ��
	double Color_height;
	Color_height = (TABLE_HEIGHT*8.0 / 10.0) / (double)ColorNum;
	for (int i = 0; i < ColorNum; i++)
	{
		DPoint3d temp_pos[4], temp_active[4];
		temp_pos[0] = Origin_pos;
		temp_pos[0].x = temp_pos[0].x + tablewidth / 8;
		temp_pos[0].y = temp_pos[0].y + TABLE_HEIGHT / 6 + i*Color_height;
		temp_pos[0].z = 0;
		temp_pos[1].x = temp_pos[0].x + tablewidth / 5;
		temp_pos[1].y = temp_pos[0].y;
		temp_pos[1].z = 0;
		temp_pos[2].x = temp_pos[0].x + tablewidth / 5;
		temp_pos[2].y = temp_pos[0].y + Color_height;
		temp_pos[2].z = 0;
		temp_pos[3].x = temp_pos[0].x;
		temp_pos[3].y = temp_pos[0].y + Color_height;
		temp_pos[3].z = 0;
		viewport.ViewToActive(temp_active, temp_pos, 4);
		output->SetSymbology(lineColorTBGR, fillColorTBGR[i], 4, 0);
		output->DrawShape3d(4, temp_active, true, NULL);
	}
}

bool DrawColorShape::_DrawDecoration(IndexedViewportR viewport)//����ʱԪ�غ���
{
	IViewOutputP output = viewport.GetIViewOutput();

	//��ֵ��ʶ�Ļ��ƣ�����ɫ����Ҫ��ʾ����㣬����Ȼ��Ƽ�ֵ��ʶ
	if (m_bExtremum)
	{
		UInt32     clr = viewport.GetContrastToBackgroundColor();
		output->SetSymbology(clr, clr, 0, 0);

		RotMatrix matrix;
		matrix = *viewport.GetRotMatrix();
		matrix.Invert();
		DgnFontR  font = DgnFontManager::GetDecoratorFont();
		DPoint3d  viewScale;
		viewScale = *viewport.GetScale();
		DPoint2d  fontSize = { 30 / viewScale.x, 24 / viewScale.y };
		DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
		TextStringPropertiesPtr pProp = TextStringProperties::Create(font, &font, fontSize, *pActiveModel);
		pProp->SetIs3d(true);
		for (int i = 0; i < (int)m_vecMaxPts.size(); i++)
		{
			WString ws;
			ws.AppendA("�LMX");
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecMaxPts[i], &matrix, *pProp);
			output->DrawTextString(*pText);
		}
		for (int i = 0; i < (int)m_vecMinPts.size(); i++)
		{
			WString ws;
			ws.AppendA("�LMN");
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecMinPts[i], &matrix, *pProp);
			output->DrawTextString(*pText);
		}
	}

	//�ڵ�鿴����Ļ���
	if (m_bNodeView)
	{
		UInt32     clr = viewport.GetContrastToBackgroundColor();
		output->SetSymbology(clr, clr, 0, 0);

		RotMatrix matrix;
		matrix = *viewport.GetRotMatrix();
		matrix.Invert();
		DgnFontR  font = DgnFontManager::GetDecoratorFont();
		DPoint3d  viewScale;
		viewScale = *viewport.GetScale();
		DPoint2d  fontSize = { 15 / viewScale.x, 12 / viewScale.y };
		DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
		TextStringPropertiesPtr pProp = TextStringProperties::Create(font, &font, fontSize, *pActiveModel);
		pProp->SetIs3d(true);
	
		//��ʱ�ڵ�鿴
		if (m_bNodeView_Temp)
		{
			WString ws;
			char nodeMsg[64];
			sprintf_s(nodeMsg, "�L%.6g", m_dNodeInfo_Temp);
			ws.AppendA(nodeMsg);
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_NodePt_Temp, &matrix, *pProp);
			output->DrawTextString(*pText);
		}

		//�ѱ���ڵ�鿴
		for (int i = 0; i < (int)m_vecNodePts.size(); i++)
		{
			WString ws;
			char nodeMsg[64];
			sprintf_s(nodeMsg, "�L%.6g", m_vecNodeInfos.at(i));
			ws.AppendA(nodeMsg);
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecNodePts.at(i), &matrix, *pProp);
			output->DrawTextString(*pText);
		}
	}

	//ɫ���̵Ļ���
	if (m_bDisplayed)
	{
		//��ȡ��ͼ��ز���
		BSIRect theRect;
		viewport.GetViewRect(theRect);
		Point2d origin = theRect.origin;//��Ļ���Ͻǣ�x����y����
		Point2d corner = theRect.corner;//��Ļ���½�

		//���û���ʱ������Ļ�϶�Ӧλ��
		DPoint3d v_tabPos;
		v_tabPos.x = corner.x + DrawPos.x;
		v_tabPos.y = origin.y + DrawPos.y;
		v_tabPos.z = 0;

		char prj[20];
		char casn[20];
		ProjectName.ConvertToLocaleChars(prj);
		CaseName.ConvertToLocaleChars(casn);
		CreateTable::tablewidth = (strlen(prj) + strlen(casn)) * 143 / 12;
		//����ɫ��
		CreateTable::CreateAllColorTable(ColorNum_Legeng, TABLE_LINCOLOR, fillColorTBGR, output, viewport, v_tabPos);
		//��ɫ���ע
		if (showType == 1)//����ֵ��
		{
			double dTmp[100];// ȡ������ֵ��֮���ƽ��ֵ
			GetContourLineColorValue(ColorNum_Legeng, dTmp, ClrShapeVal_Legeng);
			CreateTable::CreateAllText(ProjectName.GetWCharCP(), CaseName.GetWCharCP(), Header.GetWCharCP(),
				Unit.GetWCharCP(), ColorNum_Legeng, dTmp, output, viewport, v_tabPos);
		}
		else
		{
			CreateTable::CreateAllText(ProjectName.GetWCharCP(), CaseName.GetWCharCP(), Header.GetWCharCP(),
				Unit.GetWCharCP(), ColorNum_Legeng, ClrShapeVal_Legeng, output, viewport, v_tabPos);
		}
	}

	return false;
}

void DrawColorShape::initialize()
{
	/** ��ɫ�����������ɫ��� fillColorTBGR*/
	memset(fillColorTBGR, 0, 100 * sizeof(UInt32));
	/** ��ɫ�����������ɫ fillColor*/
	memset(fillColor, 0, 100 * sizeof(RgbColorDef));
	/** rgb��ɫ��Ӧ����ɫ��� fillCororIndex*/
	memset(fillCororIndex, 0, 100 * sizeof(UInt32));
	/** ��ʼ����ֵ StartValue*/
	StartValue = 0.0;
	/** ��������ֵ EndValue*/
	EndValue = 0.0;
	/** ��ʼ����ֵ(DB) DbStartValue*/
	DbStartValue = 0.0;
	/** ��������ֵ(DB) DbEndValue*/
	DbEndValue = 0.0;
	/** ��������ֵ(ͼ����ʾ) ClrShapeVal_Legeng*/
	memset(ClrShapeVal_Legeng, 0, 100 * sizeof(double));
	/** ��������ֵ ClrShapeVal*/
	memset(ClrShapeVal, 0, 100 * sizeof(double));
	/** ��ʾ���� DrawType*/
	ResultTypeCode.clear();
	/** ��ͷ Header*/
	Header.clear();
	/** ��Ӧ��λ Unit*/
	Unit.clear();

	ProjectName.clear();

	CaseName.clear();

	showType = 0;
	/** ��ӦCAEģ�͵������� NgName*/
	NgName.clear();

	//ȱʡ��Ϊautomtic
	m_bAutomatic = true;
	//ȱʡ����ʾ��ֵ
	m_bExtremum = false;
	//ȱʡ����ʾɫ��
	m_bDisplayed = false;
	//ȱʡ����ʾ�ڵ���Ϣ
	m_bNodeView = false;
	m_bNodeView_Temp = false;

	ColorNum_Legeng = 10;	
}

/*--------------------------------------------------------------------------------------+
|   StartDrawFunction����ʼ����ʱԪ��������Ӧ����
+--------------------------------------------------------------------------------------*/
void DrawColorShape::StartDrawFunction()
{
	if (StartValue == DbStartValue && EndValue == DbEndValue)
	{
		for (int i = 0; i <= ColorNum_Legeng; ++i)
		{
			ClrShapeVal[i] = ClrShapeVal_Legeng[i];
			if (i < ColorNum_Legeng)
				mdlColor_rawColorFromRGBColor(&fillCororIndex[i], &fillColor[i], ACTIVEMODEL);
		}

		Color_num = ColorNum_Legeng;
	}
	else
	{
		int iStep = 0;
		if (StartValue > DbStartValue)
		{
			ClrShapeVal[0] = DbStartValue;
			fillCororIndex[0] = 112;
			iStep = 1;
		}

		for (int i = 0; i <= ColorNum_Legeng; ++i)
		{
			ClrShapeVal[iStep + i] = ClrShapeVal_Legeng[i];
			if (i < ColorNum_Legeng)
				mdlColor_rawColorFromRGBColor(&fillCororIndex[iStep + i], &fillColor[i], ACTIVEMODEL);
		}

		if (EndValue < DbEndValue)
		{
			iStep += 1;
			ClrShapeVal[iStep + ColorNum_Legeng] = DbEndValue;
			fillCororIndex[iStep + ColorNum_Legeng - 1] = 112;
		}

		Color_num = ColorNum_Legeng + iStep;
	}

	if (showType == 1)//��ʾ��ֵ��ͼ
	{
		double dTmp[100];// ȡ������ֵ��֮���ƽ��ֵ
		GetContourLineColorValue(Color_num, dTmp, ClrShapeVal);
		memcpy(ClrShapeVal, dTmp, sizeof(double)*100);
	}

	m_bDisplayed = true;
}
/*--------------------------------------------------------------------------------------+
|   CloseStartDrawFunction��������ʱԪ�غ�Ҫ�ȹر����˳�����
+--------------------------------------------------------------------------------------*/
void DrawColorShape::CloseDrawFunction()
{
	initialize();
}

void DrawColorShape::divideColorRange()
{
	for (int i = 0; i < ColorNum_Legeng + 1; i++)//����������ֵ
	{
		ClrShapeVal_Legeng[i] = StartValue + (EndValue - StartValue) / ColorNum_Legeng * i;
	}

	for (int i = 0; i < ColorNum_Legeng; i++)//����ÿ����ɫ���Ӧ����ɫ
	{
		if (ColorNum_Legeng <= 0 || ColorNum_Legeng > 100)
		{
			return;
		}
		RgbColorDef theColor;
		// �ж��Ƿ�Ϊ�Ҷ���ʾ
		if (m_Gray)
		{
			// �Ҷ���ʾ����
			if (i == 0)//��1��ɫ��
			{
				if (ColorNum_Legeng == 1)
				{
					fillColorTBGR[0] = 0x003C3C3C;
					theColor.red = 60;
					theColor.green = 60;
					theColor.blue = 60;
				}
				else
				{
					fillColorTBGR[0] = 0x00C8C8C8;
					theColor.red = 200;
					theColor.green = 200;
					theColor.blue = 200;
				}
			}
			else
			{
				theColor.red = 200 - i*(140.0 / ColorNum_Legeng);
				theColor.green = 200 - i*(140.0 / ColorNum_Legeng);
				theColor.blue = 200 - i*(140.0 / ColorNum_Legeng);
				fillColorTBGR[i] = (theColor.blue << 16) + (theColor.green << 8) + theColor.red;
			}
		}
		else
		{
			if (i == 0)//��1��ɫ��
			{
				if (ColorNum_Legeng == 1)
				{
					fillColorTBGR[0] = 0x000000ff;
					theColor.red = 255;
					theColor.green = 0;
					theColor.blue = 0;
				}
				else
				{
					fillColorTBGR[0] = 0x00ff0000;
					theColor.red = 0;
					theColor.green = 0;
					theColor.blue = 255;
				}

			}
			else if (i != ColorNum_Legeng - 1)//�������һ��
			{
				int TheRange = -1;//����
				double ThePercent = 0.0;//����ٷֱ�
				double the_Pos = 0.0;
				the_Pos = (4.0 / (double)(ColorNum_Legeng - 1))*i;

				TheRange = the_Pos;
				ThePercent = the_Pos - TheRange;
				int r = 0;
				int g = 0;
				int b = 0;
				switch (TheRange)
				{
				case 0://��һ����
				{
					r = 0;
					g = 0;
					b = 255;
					g = 255.0*ThePercent + 0.5;
					break;
				}
				case 1://�ڶ�����
				{
					r = 0;
					g = 255;
					b = 255;
					b = 255.0*(1 - ThePercent) + 0.5;
					break;
				}
				case 2://��������
				{
					r = 0;
					g = 255;
					b = 0;
					r = 255.0*ThePercent + 0.5;
					break;
				}
				case 3://��������
				{
					r = 255;
					g = 255;
					b = 0;
					g = 255.0*(1 - ThePercent) + 0.5;
					break;
				}
				default:
					break;
				}
				fillColorTBGR[i] = (b << 16) + (g << 8) + r;
				theColor.red = r;
				theColor.green = g;
				theColor.blue = b;
			}
			else//���һ��ɫ��
			{
				fillColorTBGR[i] = 0x000000ff;
				theColor.red = 255;
				theColor.green = 0;
				theColor.blue = 0;
			}
		}
		fillColor[i] = theColor;
	}
}

void DrawColorShape::GetContourLineColorValue(int colornum, double out_value[], double in_value[])
{
	for (int i = 0; i < colornum; i++)//����������ֵ
	{
		out_value[i] = (in_value[i] + in_value[i + 1]) / 2;
	}
	out_value[colornum] = in_value[colornum];
}
void DrawColorShape::initColorRangeFromDB()
{
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);

	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		
		//���ݽ������codeֵ�͹���ID������ID��ȡ���ֵ��Сֵ(��������ͼ��ʾ��ʶΪ1��Face�ڵ�)
		strSql.append("SELECT MIN(");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append("), MAX(");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append(") \n");
		strSql.append(" FROM (SELECT ID, ");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append(" FROM RESULTS WHERE ProjectId = @ProID AND CaseID = @CaseID) rst \n");
		strSql.append(" INNER JOIN \n");
		strSql.append(" ( \n");
		strSql.append(" select Node1 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node2 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node3 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node4 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" ) fn \n");
		strSql.append(" ON (rst.ID = fn.NodeID) ");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		// ��ĿID
		m_statment->BindInt(1, ProjectId);
		// ��Ӧ����ID
		m_statment->BindInt(2, CaseID);
		// ��ӦCAEģ�͵�������
		Utf8String sNgName;
		BeStringUtilities::WCharToUtf8(sNgName, NgName.c_str());
		m_statment->BindUtf8String(3, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			StartValue = m_statment->GetValueDouble(0);
			EndValue = m_statment->GetValueDouble(1);
			DbStartValue = StartValue;
			DbEndValue = EndValue;// +0.000001;
		}
	}

	delete m_statment;
	m_db.CloseDb();
	
	//�ֶ�ˢ����ͼ
	mdlView_updateSingle(tcb->lstvw);
}
