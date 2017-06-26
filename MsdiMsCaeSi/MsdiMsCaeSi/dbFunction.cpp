#include "MsdiMsCaeSipch.h"

// CAE���������
extern bmap<WString, WString> g_RstType_Name;
// CAE������͵�λ
extern bmap<WString, WString> g_RstType_Unit;
extern int copyFile(const char *SourceFile, const char *NewFile);

string ws2s(const wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

WString s2ws(const char* str)
{
	WString ws;
	ws.AppendA(str);
	return ws;
}

WString num2Ws(int i)
{
	WString result = L"";
	wostringstream oss;
	oss << i;
	result.append(oss.str().data());
	oss.str(L"");
	return result;
}

WString num2Ws(double d)
{
	WString result = L"";
	wostringstream oss;
	oss << scientific << d;
	result.append(oss.str().data());
	oss.str(L"");
	return result;
}

int wtoi(WCharCP ws)
{
	if (wcslen(ws) == 0)
		return -1;
	else
		return _wtoi(ws);
}

WString getNGNameOfElem(EditElementHandleR eeh)
{
	WString NGName = L"";
	NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*(MASTERFILE));
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		if (namedGroup->IsMemberOf(eeh.GetElementRef(), MASTERFILE))
		{
			NGName.append(namedGroup->GetName());
			return NGName;
		}
	}
	return NGName;
}

void SetResultType(DialogItemMessage *dimP, RscId Itemid, int iProjectId, int caseID, bvector<WString>& vecRstTypeCodes)
{
	RawItemHdr*	pThis = dimP->db->GetItemByTypeAndId(RTYPE_ComboBox, Itemid)->rawItemP;
	bvector<WString> vecRstTypeNames;

	//���ݵ�ǰ���й���ʵ���Լ���Ӧ�Ĺ���ID
	getRstTypeByPjIdAndCaseId(iProjectId, caseID, vecRstTypeCodes, vecRstTypeNames);

	ListModelP	pListModel = mdlDialog_listBoxGetListModelP(pThis);
	if (pListModel == NULL)
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
	else
		mdlListModel_empty(pListModel, true);

	ListRowP	pRow = NULL;
	ListCellP	cell = NULL;
	MSValueDescr cellValue;

	int count = (int)vecRstTypeNames.size();
	for (int i = 0; i < count; i++)
	{
		pRow = mdlListRow_create(pListModel);
		cell = mdlListRow_getCellAtIndex(pRow, 0);

		cellValue.SetWChar(vecRstTypeNames[i].data());
		mdlListCell_setValue(cell, cellValue, TRUE);
		mdlListModel_addRow(pListModel, pRow);
	}

	mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	mdlDialog_itemsSynch(dimP->db);

	vecRstTypeNames.clear();
	bvector<WString>().swap(vecRstTypeNames);
}

StatusInt OpenDB(Db& db)
{
	StatusInt rc;
	WString ws = L"";

	ws.AppendA(initDBFilePath().data());

	Utf8String sProperty;
	BeStringUtilities::WCharToUtf8(sProperty, ws.c_str());
	rc = db.OpenBeSQLiteDb(sProperty.c_str(), BeSQLite::Db::CreateParams());
	return rc;
}

void setDBFlag(const char* dbFlag, string dbPath)
{
	StatusInt rc;
	AString strSql;
	Db m_db;
	
	WString ws = L"";
	ws.AppendA(dbPath.data());

	Utf8String sProperty;
	BeStringUtilities::WCharToUtf8(sProperty, ws.c_str());
	rc = m_db.OpenBeSQLiteDb(sProperty.c_str(), BeSQLite::Db::CreateParams());

	Statement *m_statment = new Statement();
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("update be_Prop set strData = @dbFlag where Namespace='be_Db' and Name='DbGuid'");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		WString wsFlag;
		Utf8String utf8Flag;
		wsFlag.AppendA(dbFlag);
		BeStringUtilities::WCharToUtf8(utf8Flag, wsFlag.c_str());
		m_statment->BindUtf8String(1, utf8Flag, m_statment->BindMakeCopy::MAKE_COPY_No);
		m_statment->Step();
	}

	delete m_statment;
	m_db.CloseDb();
}

WString getDBFlag(string dbPath)
{
	WString dbFlag;

	StatusInt rc;
	AString strSql;
	Db m_db;

	WString ws = L"";
	ws.AppendA(dbPath.data());

	Utf8String sProperty;
	BeStringUtilities::WCharToUtf8(sProperty, ws.c_str());
	rc = m_db.OpenBeSQLiteDb(sProperty.c_str(), BeSQLite::Db::CreateParams());

	Statement *m_statment = new Statement();
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("select strData from be_Prop where Namespace='be_Db' and Name='DbGuid'");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			
			dbFlag.AppendUtf8(m_statment->GetValueUtf8(0));

			break;
		}
	}

	delete m_statment;
	m_db.CloseDb();

	return dbFlag;
}

string initDBFilePath()
{
	//��ȡ��ǰdgn�ļ�·��
	DgnModelRefP model = MASTERFILE;
	WString wsDgnName = model->GetDgnFileP()->GetFileName();
	string dgnName = ws2s(wsDgnName.data());

	//��ȡ��dgn��Ӧdb·��
	int i_index = (int)dgnName.find_last_of(".");
	dgnName = dgnName.substr(0, i_index);
	string dbPath = dgnName + ".db";

	//��ȡdbģ������·��
	WString sysDir;
	mdlSystem_getCfgVar(&sysDir, L"_USTN_SYSTEMROOT");
	sysDir.append(L"data\\");
	sysDir.append(DB_NAME);

	//���Դ�db
	ifstream in;
	in.open(dbPath.data(), ios::binary);

	//��ȡ��ǰdgn��XAttribution�е����ݿ��ʶ��
	DgnModelRefP defaltModel;
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	mdlModelRef_createWorking(&defaltModel, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*defaltModel);
	XAttributeHandle xah(appSet.GetElementRef(), handlerId, XAttrID_DBFlag);

	//��������XAttribution���򿽱�dbģ�岢���ɱ�ʶ��д��dgn�����ݿ���
	if (!xah.IsValid())
	{
		copyFile(ws2s(sysDir.data()).data(), dbPath.data());
		
		//.������д��dgn��db��
		char timeFlag[256];
		time_t t = time(0);
		strftime(timeFlag, sizeof(timeFlag), "%Y/%m/%d-%X", localtime(&t));
		int len = (int)strlen(timeFlag);
		timeFlag[len] = '\0';

		appSet.SaveSetting(handlerId, XAttrID_DBFlag, timeFlag, sizeof(char) * 256);
		setDBFlag(timeFlag, dbPath);
	}
	//������XAttribution�����ж�db�ļ��Ƿ����
	else
	{
		//��XAttribution�ж�ȡ��ʶ��
		char timeFlag[256];
		strcpy(timeFlag, (char*)xah.PeekData());

		//��db�ļ��Ѿ����ڣ���ȡ���Ӧ�ı�ʶ���ж��Ƿ���dgnƥ��
		if (!in.fail())
		{
			in.close();

			//��ȡ���ݿ��еı�ʶ��
			WString dbFlag = getDBFlag(dbPath);

			//��dgn��db��ʶ����ƥ�䣬�򿽱�dbģ�帲�ǲ���д��ʶ��
			if (ws2s(dbFlag.data()).compare(timeFlag) != 0)
			{
				copyFile(ws2s(sysDir.data()).data(), dbPath.data());

				setDBFlag(timeFlag, dbPath);
			}

			//��dgn��db��ʶ��ƥ�䣬�򲻽����κ���������
		}
		//��db�ļ������ڣ�����dbģ�岢�����ݿ�д���ʶ��
		else
		{
			copyFile(ws2s(sysDir.data()).data(), dbPath.data());

			setDBFlag(timeFlag, dbPath);
		}
	}


	mdlModelRef_freeWorking(defaltModel);

	return dbPath;
}

void getProjectInfoById(WStringR sProjectName, WStringR sProjectUnit, int iProjectId)
{
	Db m_db;
	Statement *m_statment;
	StatusInt rc;
	WString lsProjectName;

	m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;

		// ��ѯCAE���̱����Ƿ���ڲ������Ĺ���ID
		// ������ڣ���CAE����ID�ʹӲ������Ĺ���ID֮�����
		strSql.append("SELECT ProjectName,ProjectUnit FROM PROJECTS \n");
		strSql.append("WHERE ProjectId = @ProjectId \n");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		// CAE����ID
		m_statment->BindInt(1, iProjectId);
		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			sProjectName.AppendUtf8(m_statment->GetValueUtf8(0));
			sProjectUnit.AppendUtf8(m_statment->GetValueUtf8(1));
			break;
		}
	}

	m_statment->Finalize();
	m_db.CloseDb();
}

int getNodeCntByProjectid(int iProjectID)
{
	int nodeCnt = 0;
	Db m_db;
	Statement *m_statment;
	StatusInt rc;
	WString lsProjectName;

	m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;

		strSql.append("SELECT NODES.ID FROM NODES \n");
		strSql.append("WHERE NODES.ProjectId = @projectID ORDER BY NODES.ID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		// CAE����ID
		m_statment->BindInt(1, iProjectID);		
		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			nodeCnt = m_statment->GetValueInt(0);
		}
	}

	delete m_statment;
	m_db.CloseDb();

	return nodeCnt;
}

int getElemCntByProjectid(int iProjectID)
{
	int ElemCnt = 0;
	Db m_db;
	Statement *m_statment;
	StatusInt rc;
	WString lsProjectName;

	m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;

		strSql.append("SELECT ELEMENTS.ElemID FROM ELEMENTS \n");
		strSql.append("WHERE ELEMENTS.ProjectId = @projectID ORDER BY ELEMENTS.ElemID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		// CAE����ID
		m_statment->BindInt(1, iProjectID);
		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			ElemCnt = m_statment->GetValueInt(0);
		}
	}

	delete m_statment;
	m_db.CloseDb();

	return ElemCnt;
}

void getCaseInfoByProjectid(int iProjectID, bvector<int>& vecCaseID, bvector<WString>& vecCaseName, bvector<WString>& vecCaseProp)
{
	vecCaseID.clear();
	vecCaseName.clear();
	vecCaseProp.clear();

	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT CASES.CaseId, CASES.CaseName, CASES.Property\n");
		strSql.append(" FROM CASES\n");
		strSql.append(" WHERE CASES.ProjectId = @projectID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			// ����ID
			vecCaseID.push_back(m_statment->GetValueInt(0));

			// ��������
			WString wsCaseName;
			wsCaseName.AppendUtf8(m_statment->GetValueUtf8(1));
			vecCaseName.push_back(wsCaseName);

			// ������������Ҫ�غɣ�
			WString wsCaseProp;
			wsCaseProp.AppendUtf8(m_statment->GetValueUtf8(2));
			vecCaseProp.push_back(wsCaseProp);

		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getMaterialIDByProjectid(int iProjectID, bvector<int>& materialIDs)
{
	materialIDs.clear();

	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT MATERIALS.ID\n");
		strSql.append(" FROM MATERIALS\n");
		strSql.append(" WHERE MATERIALS.ProjectId = @projectID ORDER BY MATERIALS.ID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			// ����ID
			materialIDs.push_back(m_statment->GetValueInt(0));
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getMaterialAllByPK(int iProjectID, int materialID, WString& mName, WString& mType, WString& mEx, WString& mNUXY, WString& mDENS, WString& mPRXY)
{
	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		 
		strSql.append("SELECT MATERIALS.NAME, MATERIALS.Type, MATERIALS.EX, MATERIALS.NUXY, MATERIALS.DENS, MATERIALS.PRXY\n");
		strSql.append(" FROM MATERIALS\n");
		strSql.append("  WHERE MATERIALS.ProjectId = @projectID AND MATERIALS.ID = @materialID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, materialID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			// Name
			mName.AppendUtf8(m_statment->GetValueUtf8(0));

			// Type
			mType.AppendUtf8(m_statment->GetValueUtf8(1));

			// EX
			mEx.AppendUtf8(m_statment->GetValueUtf8(2));

			// NUXY
			mNUXY.AppendUtf8(m_statment->GetValueUtf8(3));

			// DENS
			mDENS.AppendUtf8(m_statment->GetValueUtf8(4));

			// PRXY
			mPRXY.AppendUtf8(m_statment->GetValueUtf8(5));

			break;
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getElemTypeIDByProjectid(int iProjectID, bvector<int>& ElemTypeIDs)
{
	ElemTypeIDs.clear();

	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT ELEMTYPES.ElemType\n");
		strSql.append(" FROM ELEMTYPES\n");
		strSql.append(" WHERE ELEMTYPES.ProjectId = @projectID ORDER BY ELEMTYPES.ElemType\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			// Ԫ������ID
			ElemTypeIDs.push_back(m_statment->GetValueInt(0));
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getElemNameByPK(int iProjectID, int elemType, WString& elemName)
{
	elemName = L"";
 
	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT ELEMTYPES.ElemName\n");
		strSql.append(" FROM ELEMTYPES\n");
		strSql.append("  WHERE ELEMTYPES.ProjectId = @projectID AND ELEMTYPES.ElemType = @elemType\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, elemType);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			elemName = num2Ws(m_statment->GetValueInt(0));

			break;
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getRealConsIDByProjectid(int iProjectID, bvector<int>& RealConIDs)
{
	RealConIDs.clear(); 

	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT REALCONSTANTS.RealConstantNumber\n");
		strSql.append(" FROM REALCONSTANTS\n");
		strSql.append(" WHERE REALCONSTANTS.ProjectId = @projectID ORDER BY REALCONSTANTS.RealConstantNumber\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			// ʵ����
			RealConIDs.push_back(m_statment->GetValueInt(0));
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

void getRealConsPropByPK(int iProjectID, int realConstant, WString& realConstantProp)
{
	realConstantProp = L""; 
	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{

		strSql.append("SELECT REALCONSTANTS.Property\n");
		strSql.append(" FROM REALCONSTANTS\n");
		strSql.append(" WHERE REALCONSTANTS.ProjectId = @projectID AND REALCONSTANTS.RealConstantNumber = @realConsNumber\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, realConstant);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			realConstantProp.AppendUtf8(m_statment->GetValueUtf8(0));

			break;
		}
	}

	delete m_statment;
	m_db.CloseDb();
}

int getElemCntUsingMaterialByProjectid(int iProjectID, int materialID)
{
	int ElmCnt = 0; 
	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT COUNT(ELEMENTS.MaterialNumber)\n");
		strSql.append(" FROM ELEMENTS\n");
		strSql.append("  WHERE ELEMENTS.ProjectId = @projectID AND ELEMENTS.MaterialNumber = @materialID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, materialID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			ElmCnt = m_statment->GetValueInt(0);
	
			break;
		}
	}

	delete m_statment;
	m_db.CloseDb();

	return ElmCnt;
}

void getExtremumResultAndNode(int iProjectID, int caseID, bvector<int>& nodeLst, double& extremum, const char* field, bool bMax /*= true*/)
{
	nodeLst.clear();
	extremum = 0;

	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		//��ȡ��ֵ 
		strSql.append("SELECT ");
		if (bMax)
			strSql.append("MAX");
		else
			strSql.append("MIN");
		strSql.append("(RESULTS.");
		strSql.append(field);
		strSql.append(" )\n");
		strSql.append(" FROM RESULTS\n");
		strSql.append(" WHERE RESULTS.ProjectId = @projectID AND RESULTS.CaseId = @caseID\n");
		
		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, caseID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			extremum = m_statment->GetValueDouble(0);

			break;
		}

		m_statment->Finalize();

		//��ȡ��ֵ��Ӧ�Ľڵ�ID
		strSql.clear();
		strSql.append("SELECT RESULTS.ID \n");
		strSql.append(" FROM RESULTS\n");
		strSql.append(" WHERE RESULTS.");
		strSql.append(field);
		strSql.append(" = @extremum AND RESULTS.ProjectId = @projectID AND RESULTS.CaseId = @caseID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindDouble(1, extremum);
		m_statment->BindInt(2, iProjectID);
		m_statment->BindInt(3, caseID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			int nodeID = m_statment->GetValueInt(0);
			nodeLst.push_back(nodeID);
		}
	}
	if (nodeLst.empty())
	{
		nodeLst.push_back(-1);
	}

	delete m_statment;
	m_db.CloseDb();
}

void showMeshsOfInstance_DB(int iProjectID, WString NGName, bvector<WString> meshIDs)
{
	if (meshIDs.empty())
		return;

	//���ھֲ���ʾʱһ����ʾ�Ĳ���ռ���������Բ������²���
	//�Ƚ���ʵ���µ�����ElemShowFlg��Ϊ0��Ȼ��meshIDs��Ӧ��ElemShowFlg��Ϊ1
	int elemIDCnt = (int)meshIDs.size();
	StatusInt rc;
	AString strSql; 
	Db m_db;
	rc = OpenDB(m_db);
	
	// CAEԪ�ص������� 
	Utf8String sNgName;
	BeStringUtilities::WCharToUtf8(sNgName, NGName.c_str());

	Statement *m_statment = new Statement();

	if (rc == BE_SQLITE_OK)
	{
		//�򿪽�����
		WString strMsg;
		int percent = 0;
		strMsg = L"�ֲ���ʾ����...";
		MSDialogP bar = mdlDialog_completionBarOpen(strMsg.data());

		//����һ������
		Savepoint* savept = new Savepoint(m_db, "DBTransaction", true);
		
		bool bRet = true;
		savept->Begin();

		//����ʵ���µ�����ElemShowFlg��Ϊ0
		strSql.clear();
		strSql.append("UPDATE FACES SET ElemShowFlg = 0 ");
		strSql.append(" WHERE FACES.ProjectId = @projectID AND FACES.MSLevelName = @MSLevelName ");
		m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
		m_statment->Step();
		m_statment->Finalize();

		percent += 5;
		mdlDialog_completionBarUpdate(bar, strMsg.data(), percent);

		strSql.clear();
		strSql.append("UPDATE FACES SET ElemShowFlg = 1 ");
		strSql.append(" WHERE FACES.ProjectId = @projectID AND FACES.MSLevelName = @MSLevelName AND FACES.ElemID = @ElemID ");
		m_statment->Prepare(m_db, strSql.c_str());

		int elemID;
		for (int i = 0; i < elemIDCnt; i++)
		{
			elemID = wtoi(meshIDs[i].c_str());

			m_statment->Reset();
			m_statment->BindInt(1, iProjectID);
			m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
			m_statment->BindInt(3, elemID);

			if (m_statment->Step() != BE_SQLITE_DONE)
			{
				bRet = false;
				break;
			}

			percent = (int)(5 + 95.0 /elemIDCnt * i);
			mdlDialog_completionBarUpdate(bar, strMsg.data(), percent);
		} 
		 
		m_statment->Finalize();
		
		//�ύ����
		if (bRet)
		{
			savept->Commit();
		}
		else
		{
			savept->Cancel();
		}

		delete savept;

		//�رս�����
		mdlDialog_completionBarClose(bar);
	}

	delete m_statment;
	m_db.CloseDb();
}

void showAllOfInstance_DB(int iProjectID, WString NGName)
{
	StatusInt rc;
	AString strSql;
	Db m_db;
	rc = OpenDB(m_db);

	// CAEԪ�ص������� 
	Utf8String sNgName;
	BeStringUtilities::WCharToUtf8(sNgName, NGName.c_str());

	if (rc == BE_SQLITE_OK)
	{
		//����һ������
		Savepoint* savept = new Savepoint(m_db, "DBTransaction", true);
		Statement *m_statment = new Statement();
		bool bRet = true;
		savept->Begin();

		//����ʵ���µ�����ElemShowFlg��Ϊ0
		strSql.clear();
		strSql.append("UPDATE FACES SET ElemShowFlg = 1 ");
		strSql.append(" WHERE FACES.ProjectId = @projectID AND FACES.MSLevelName =  @MSLevelName");

		m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
		bool ret = m_statment->Step() == BE_SQLITE_DONE ? true : false;
		m_statment->Finalize();

		delete m_statment;
		if (ret)
			savept->Commit();
		else
			savept->Cancel();

		delete savept;
	}
}

// ȡ��CAE�������
void getRstTypeInfFromDB(bmap<WString, WString>& RstType_Name, bmap<WString, WString>& RstType_Unit)
{
	WString sCode;
	WString sName;
	WString sUnit;
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		RstType_Name.clear();
		RstType_Unit.clear();

		AString strSql;
		strSql.append("SELECT Code, Name, Unit FROM RSTTYPE\n");
		rc = m_statment->Prepare(m_db, strSql.c_str());

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			sCode.clear();
			sName.clear();
			sUnit.clear();
			// �������
			sCode.AppendUtf8(m_statment->GetValueUtf8(0));
			// ��������� 
			sName.AppendUtf8(m_statment->GetValueUtf8(1));
			// ������͵�λ
			sUnit.AppendUtf8(m_statment->GetValueUtf8(2));

			RstType_Name[sCode] = sName;
			RstType_Unit[sCode] = sUnit;
		}
	}

	delete m_statment;;
	m_db.CloseDb();
}

void getRstTypeByPjIdAndCaseId(int ProjectId, int CaseId, bvector<WString>& RstTypeCodes, bvector<WString>& RstTypeNames)
{
	RstTypeCodes.clear();
	RstTypeNames.clear();

	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();
	WString sRstTypeCode;
	WString sTmp;

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		strSql.append("SELECT * FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId AND ID = 1 \n");
		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, ProjectId);
		m_statment->BindInt(2, CaseId);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			for (int i = 3; i < m_statment->GetColumnCount(); i++)
			{
				if (!m_statment->IsColumnNull(i))
				{
					sRstTypeCode.clear();
					sRstTypeCode.AppendUtf8(m_statment->GetColumnName(i));
					RstTypeCodes.push_back(sRstTypeCode);
					RstTypeNames.push_back(g_RstType_Name[sRstTypeCode]);
				}
			}
		}

		m_statment->Finalize();
	}

	delete m_statment;;
	m_db.CloseDb();
}

bool getMaxMinXYZPtValOfProject(int iProjectID, double& minx, double& maxx, double& miny, double& maxy, double& minz, double& maxz)
{
	bool bResult = false;
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		//��ȡ���ֵ 
		strSql.append("SELECT MIN(NODES.X),MIN(NODES.Y),MIN(NODES.Z),MAX(NODES.X),MAX(NODES.Y),MAX(NODES.Z) \n");
		strSql.append(" FROM NODES\n");
		strSql.append(" WHERE NODES.ProjectId = @projectID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			minx = m_statment->GetValueDouble(0);
			miny = m_statment->GetValueDouble(1);
			minz = m_statment->GetValueDouble(2);
			maxx = m_statment->GetValueDouble(3);
			maxy = m_statment->GetValueDouble(4);
			maxz = m_statment->GetValueDouble(5);

			bResult = true;
			break;
		}
	}

	delete m_statment;;
	m_db.CloseDb();

	return bResult;
}

double getMaxCoordinateValueOfProject(int iProjectID, const char* field)
{
	double value = 0;
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		//��ȡ���ֵ 
		strSql.append("SELECT MAX(NODES.");
		strSql.append(field);
		strSql.append(" )\n");
		strSql.append(" FROM NODES\n");
		strSql.append(" WHERE NODES.ProjectId = @projectID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			value = m_statment->GetValueDouble(0);

			break;
		}
	}

	delete m_statment;;
	m_db.CloseDb();

	return value;
}

double getMinCoordinateValueOfProject(int iProjectID, const char* field)
{
	double value = 0;
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		//��ȡ���ֵ 
		strSql.append("SELECT MIN(NODES.");
		strSql.append(field);
		strSql.append(" )\n");
		strSql.append(" FROM NODES\n");
		strSql.append(" WHERE NODES.ProjectId = @projectID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			value = m_statment->GetValueDouble(0);

			break;
		}
	}

	delete m_statment;;
	m_db.CloseDb();

	return value;
}

DPoint3d getNodeCoordinate(int iProjectID, int nodeID)
{
	DPoint3d pt = { 0 };
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		strSql.append("SELECT NODES.X, NODES.Y, NODES.Z FROM NODES ");
		strSql.append(" WHERE NODES.ProjectId = @projectID AND NODES.ID = @nodeID\n");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, nodeID);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			pt.x = m_statment->GetValueDouble(0);
			pt.y = m_statment->GetValueDouble(1);
			pt.z = m_statment->GetValueDouble(2);
			break;
		}
	}

	delete m_statment;;
	m_db.CloseDb();

	return pt;
}

DPoint3d getNodeCoordinate(int iProjectID, int caseID, const char* field, double fieldValue)
{
	DPoint3d pt = { 0 };
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;

		strSql.append("Select X, Y, Z FROM(SELECT ID, ");
		strSql.append(field);
		strSql.append(" FROM RESULTS WHERE ProjectId = @projectID AND CaseID = @caseID AND ");
		strSql.append(field);
		strSql.append(" = @value) a ");
		strSql.append(" INNER JOIN (select ID,X,Y,Z FROM NODES WHERE ProjectId = @projectID) p ON (a.ID = p.ID)");
		strSql.append(" INNER JOIN(select Node1, Node2, Node3, Node4 FROM FACES WHERE ProjectId = @projectID AND ElemShowFlg = 1 AND ContourShowFlg = 1) n1 ");
		strSql.append(" ON(a.ID = n1.Node1 OR a.ID = n1.Node2 OR a.ID = n1.Node3 OR a.ID = n1.Node4)  limit 0, 1");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		m_statment->BindInt(1, iProjectID);
		m_statment->BindInt(2, caseID);
		m_statment->BindDouble(3, fieldValue);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			pt.x = m_statment->GetValueDouble(0);
			pt.y = m_statment->GetValueDouble(1);
			pt.z = m_statment->GetValueDouble(2);
			break;
		}
	}

	delete m_statment;;
	m_db.CloseDb();

	return pt;
}

void deleteAllFaceOfInstance(WString ngName)
{
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		AString strSql;

		strSql.append("DELETE FROM FACES WHERE MSLevelName = '");
		strSql.append(ws2s(ngName.GetWCharCP()).data());;
		strSql.append("'");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		m_statment->Step();
	}

	delete m_statment;;
	m_db.CloseDb();
}

WString UNUSE_WSTRING;
int	UNUSE_INT;

WString makeNgName_Model(int proID, int index /*= -1*/)
{
	//��ʽ����:
	//CAEMODEL$����ID$ʵ������
	
	//ǰ׺
	WString ngName;
	ngName.append(NGNAME_PREFIX_CAEMODEL);
	ngName.append(SeparatorDollar);

	//����ID
	ngName.append(num2Ws(proID));
	ngName.append(SeparatorDollar);

	//������ָ������С��0���ʾ�Զ���������������
	if (index < 0)
	{
		//������0��ʼ������ֱ��ngName����(���99)
		for (int index = 0; index < 1000; index++)
		{
			WString result;
			result.append(ngName);
			result.append(num2Ws(index).data());

			//�������е�����ģ�ͣ����Ѿ����������index����
			bvector<InstanceInfo> allInstance = readInstanceInfo();
			bool exsit = false;
			for (int i = 0; i < (int)allInstance.size(); i++)
			{
				if (result.CompareTo(allInstance[i].NgName) == 0)
				{
					exsit = true;
					break;
				}
			}

			if (!exsit)
				return result;
		}

		//���������������ֵ
		ngName.AppendA("MAX");
		return ngName;
	}
	else
	{
		ngName.append(num2Ws(index));
		return ngName;
	}
}

bool splitNgName_Model(WString ngName, WStringR proName, int& proID, int& index, WString& instanceName, WStringR proUnit)
{
	//��ʽ����:
	//CAEMODEL$����ID$ʵ������
	if (ngName.find(NGNAME_PREFIX_CAEMODEL_DOLLAR) == WString::npos)
	{
		return false;
	}
	else
	{
		//���ݷָ���$��ngName���
		bvector<WString> splitResult;
		BeStringUtilities::Split(ngName.data(), SeparatorDollar, splitResult);
		if (splitResult.size() != 3)
			return false;

		//��ȡngName�а���������
		//����ID
		WString wsProID = splitResult[1];
		proID = wtoi(wsProID.data());

		//ʵ������
		WString wsIndex = splitResult[2];
		index = wtoi(wsIndex.data());

		//���ݹ���ID��ѯ���ݿ��ȡ��Ӧ�Ĺ�����
		getProjectInfoById(proName, proUnit, proID);

		//ʵ����
		instanceName.clear();
		instanceName.append(makeInstanceName(proName, wtoi(wsIndex.data())));

		return true;
	}
}

WString makeNgName_Clip(int proID, int indexID, WString clipName)
{
	//��ʽ����:
	//CAECLIP$����ID$ʵ������$������

	//ǰ׺
	WString ngName;
	ngName.append(NGNAME_PREFIX_CAECLIP);
	ngName.append(SeparatorDollar);

	//����ID
	ngName.append(num2Ws(proID));
	ngName.append(SeparatorDollar);

	//ʵ������
	ngName.append(num2Ws(indexID));
	ngName.append(SeparatorDollar);

	//��������
	ngName.append(clipName);
	return ngName;
}

bool splitNgName_Clip(WString ngName, WStringR proName, int& proID, int& index, WStringR clipName, WStringR instanceName, WStringR proUnit)
{
	//��ʽ����:
	//CAECLIP$����ID$ʵ������$������
	if (ngName.find(NGNAME_PREFIX_CAECLIP_DOLLAR) == WString::npos)
	{
		return false;
	}
	else
	{
		//���ݷָ���$��ngName���
		bvector<WString> splitResult;
		BeStringUtilities::Split(ngName.data(), SeparatorDollar, splitResult);
		if (splitResult.size() != 4)
			return false;

		//��ȡngName�а���������
		//����ID
		WString wsProID = splitResult[1];
		proID = wtoi(wsProID.data());

		//ʵ������
		WString wsIndex = splitResult[2];
		index = wtoi(wsIndex.data());

		//���ݹ���ID��ѯ���ݿ��ȡ��Ӧ�Ĺ�����
		getProjectInfoById(proName, proUnit, proID);

		//��������
		clipName.clear();
		clipName.append(splitResult[3]);

		//ʵ����
		instanceName.clear();
		instanceName.append(makeInstanceName(proName, wtoi(wsIndex.data())));
		return true;
	}
}

WString makeInstanceName(WString proName, int index)
{
	WString instanceName;
	if (index == 0)
		instanceName.append(proName);
	else
	{
		instanceName.append(proName);
		instanceName.append(L"-");
		instanceName.append(num2Ws(index).data());
	}

	return instanceName;
}

void splitInstanceName(WString instanceName, WString& proName, int& index)
{
	bvector<WString> splitResult;
	BeStringUtilities::Split(instanceName.data(), L"-", splitResult);

	proName.clear();
	proName.append(splitResult[0]);

	//��ʵ���в����ڡ�-����ʾ��ʵ��Ϊ��һ��ʵ��������������������Ϊ0
	if (splitResult.size() != 2)
		index = 0;
	else
		index = wtoi(splitResult[1].data());
}

void savingInstanceInfo(bvector<InstanceInfo> instanceInfos)
{
	//���е��������ŵ�Defaultģ�͵�XAttribution��
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);

	//�Ƚ�bvectorתΪ�����ٽ���д��
	int cnt = (int)instanceInfos.size();
	if (cnt == 0)
	{
		XAttributeHandle xah(appSet.GetElementRef(), handlerId, XAttrID_InstanceInfo);
		if (xah.IsValid())
		{
			ITxnManager::GetManager().GetCurrentTxn().DeleteXAttribute(xah);
		}
	}
	else
	{
		InstanceInfo* arrayInstances = new InstanceInfo[cnt];
		for (int i = 0; i < cnt; i++)
		{
			arrayInstances[i] = instanceInfos[i];
		}

		appSet.SaveSetting(handlerId, XAttrID_InstanceInfo, arrayInstances, sizeof(InstanceInfo) * cnt);

		delete[] arrayInstances;
	}

	mdlModelRef_freeWorking(model);
}

bvector<InstanceInfo> readInstanceInfo()
{
	bvector<InstanceInfo> results;

	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);
	XAttributeHandle xah(appSet.GetElementRef(), handlerId, XAttrID_InstanceInfo);

	if (xah.IsValid())
	{
		//�Ƚ���ȡ�����ݴ浽��������תΪbvector
		int size = xah.GetSize();
		int cnt = size / (int)sizeof(InstanceInfo);
		if (cnt != 0)
		{
			InstanceInfo* arrayResults = (InstanceInfo*)xah.PeekData();
			for (int i = 0; i < cnt; i++)
			{
				results.push_back(arrayResults[i]);
			}
		}
	}
	mdlModelRef_freeWorking(model);

	return results;
}
