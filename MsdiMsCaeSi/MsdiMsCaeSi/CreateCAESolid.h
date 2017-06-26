/**
* @file  CreateCAESolid.h
* @brief  ����CAEʵ�崴�������ͺ���
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
/**
* @brief ���ڴ���CAEģ�͵���
*/
struct CreateCAESolidTool : public DgnPrimitiveTool
{
	const double SQL_ELEMID_IDX = 0;
	const double SQL_MATERIAL_IDX = 1;
	const double SQL_ELEMTYPE_IDX = 2;
	const double SQL_REAL_IDX = 3;
	const double SQL_NODECOUNT_IDX = 4;
	const double SQL_NODE_IDX = 5;
protected:
	/**
	* @brief ���캯��
	* @param[in] toolName ��������
	* @return �޷���ֵ
	*/
	CreateCAESolidTool(int toolName, int toolPrompt, int iProjectID, WString sProjectUnit) : DgnPrimitiveTool(toolName, toolPrompt)
	{
		// CAEģ��ID
		m_ProjectID = iProjectID;
		// CAEģ�͵ĵ�λ
		m_ProjectUnit = sProjectUnit;
	}

	/**
	* @brief ���߼̳к���
	* @return �޷���ֵ
	*/
	virtual void _OnPostInstall() override;

	/**
	* @brief ������������
	* @return �޷���ֵ
	*/
	virtual void _OnRestartTool() override { InstallNewInstance(GetToolId(), GetToolPrompt(), m_ProjectID, m_ProjectUnit); }

	/**
	* @brief ����������Ӧ����
	* @return �޷���ֵ
	*/
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;

	/**
	* @brief ������������
	*/
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _ExitTool(); return true; }

	/**
	* @brief �Զ���λ����
	*/
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	/**
	* @brief ����CAEģ�ͺ���
	*/
	bool CreateElement(DPoint3dCP basePt);

	void SetupAndPromptForNextAction();
public:
	/**
	* @brief ������������
	*/
	static void InstallNewInstance(int toolId, int toolPrompt, int iProjectID, WString sProjectUnit);

private:
	RedrawElems m_tempElems;
	/** �������ݿ���� m_db*/
	Db m_db;

	/** ���屣�����ݵ� m_savept*/
	Savepoint *m_savept;

	/** �������ݿ�״̬���� m_statment*/
	Statement *m_statment;

	/** ����CAE���񹤳�ID m_ProjectID*/
	int m_ProjectID;

	/** ����CAEģ�͵ĵ�λ m_ProjectUnit*/
	WString m_ProjectUnit;

	/**
	* @brief �����ݿ��л�ȡ��ELEMENTS����node����
	* @return ��ȡ�������ݿ���
	*/
	DbResult GetSolidFromDatabase(void);

	/**
	* @brief �����ݿ�״̬������ȡ��Ӧ��ŵ�λ������
	* @param[in] iIdx node���
	* @return ��Ŷ�Ӧ�ĵ�����
	*/
	DPoint3d GetPointFromStatment(DPoint3dCP basePt, int iIdx);

	WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4);

	bool SaveFaceInfToDb(bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID);
	bool MakeFaceDataSql(AString& FaceDataSql, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID);

	/**
	* @brief  ȡ��CAE��Mesh��Ľڵ�ͽڵ����꼯��
	* @param[out] vecIndices		Mesh��Ľڵ㼯��
	* @param[out] vecPoints			Mesh��Ľڵ����꼯��
	* @param[out] oFaceNodeStrMap	Mesh�����Ľڵ��ַ�������(�磺1-2-3-4)
	* @param[in]  vecNodeId			nodeID����
	* @param[in]  basePt			CAEģ�͵ķ��������(�����ԭ���ƫ��ֵ��
	* @return �޷���ֵ
	*/
	void MakeMeshIndices(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt);
	void MakeFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4);

	//��ȡCAE������Ԫ����Ϣ��sql���
	StatusInt GetHiliteSolidSQL();

	//����������mesh��
	void BuildHiliteMeshes(const DPoint3dCP& basePt, DgnPlatform::LevelId  levelId_CaeModel);
};