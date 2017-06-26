#include "MsdiMsCaeSipch.h"

//ͨ��ProjectID��ȡ���еĹ���ID
void GetAllCaseIDByPRId(WString iProjectId, bvector<int>& vecCaseID, bvector<WString>& vecCaseName)
{
	StatusInt rc;
	AString strSql;
	Db m_db;
	Statement *m_statment = new Statement();
	

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		strSql.append("SELECT CASES.CaseId, CASES.CaseName\n");
		strSql.append(" FROM CASES\n");
		strSql.append(" WHERE CASES.ProjectId =\n");

		CharP cSQL = "";
		size_t len = strSql.length() + 1;
		cSQL = (CharP)malloc(len*sizeof(char));
		iProjectId.ConvertToLocaleChars(cSQL);
		strSql.append(cSQL);
		rc = m_statment->Prepare(m_db, strSql.c_str());

		if (rc == BE_SQLITE_OK)
		{
			vecCaseID.clear();
			vecCaseName.clear();
			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				// ����ID
				vecCaseID.push_back(m_statment->GetValueInt(0));
				// ��������
				WString thecasenamewchar;
				thecasenamewchar.AppendUtf8(m_statment->GetValueUtf8(1));
				vecCaseName.push_back(thecasenamewchar);
			}
		}
	}

	m_statment->Finalize();
	m_db.CloseDb();
}
bool	SelectGroup::_OnModifyComplete(DgnButtonEventCR ev)
{	
	EditElementHandleP start = GetElementAgenda().GetFirstP();

	/*bvector<ElementRefP> theGroups;
	NamedGroup::GetGroupsContaining(theGroups, start->GetElementRef());*/

	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupFlags ngfCAESolid;
	NamedGroupPtr ngCAESolid;
	WString uniqueName;
	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	bvector<int> CaseIds;
	bvector<WString> CaseNames;
	// ѭ������Ƿ���ڿ�������(��������û��Ԫ��)
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		if (namedGroup->IsMemberOf(start->GetElementRef(), ACTIVEMODEL))
		{
			WString ngName = namedGroup->GetName();

			//��������������ȡ����ID
			int iProID;
			splitNgName_Model(ngName, UNUSE_WSTRING, iProID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

			WString projectID;
			WString::Sprintf(projectID, L"%d", iProID);
			//��ȡ����ID�͹�������
			GetAllCaseIDByPRId(projectID, CaseIds, CaseNames);
			//���ɽڵ����ʾ����
			s_DrawColorShape.ProjectId = iProID;
			//��Ĭ��ȡ��һ������
			if (CaseIds.size()<1)
			{
				return false;
			}
			s_DrawColorShape.CaseID = CaseIds.at(0);
			s_DrawColorShape.NgName = ngName;

			DPoint3d pt1;
			DPoint3d pt2;
			mdlElmdscr_computeRange(&pt1, &pt2, start->GetElementDescrP(), NULL);
			DPoint3d pt3;
			pt3.x = pt1.x;
			pt3.y = pt1.y;
			pt3.z = pt1.z;

			break;
		}

	}
	return true;
}

bool    SelectGroup::_WantAdditionalLocate(DgnButtonEventCP ev)
{

	if (NULL == ev)
		return true; // This is a multi-locate tool...

	return ev->IsControlKey();
}
EditElementHandleP  SelectGroup::_BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev)
{
	EditElementHandle eeh(path->GetHeadElem(), path->GetRoot());
	return GetElementAgenda().Insert(eeh);
}

void   SelectGroup::_OnRestartTool()//������������
{
	_ExitTool();
	
}
void SelectGroup::_OnPostInstall()
{
	__super::_OnPostInstall();
	mdlAccuDraw_setEnabledState(false); // Don't enable AccuDraw w/Dynamics...
}

void SelectGroup::InstallNewInstance(int toolId)//���߿�ʼ���а�װ
{
	SelectGroup* tool = new SelectGroup(toolId);
	tool->InstallTool();
}