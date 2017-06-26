#pragma  once

/**
* @brief ���ڴ�������߲������������Ĺ�����
*/
struct ReinforceTool : public DgnElementSetTool
{
protected:
	virtual bool            _IsModifyOriginal() override { return false; }
	virtual StatusInt       _ProcessAgenda(DgnButtonEventCR ev) override { return SUCCESS; }
	virtual UsesFence       _AllowFence() override { return USES_FENCE_None; }
	virtual UsesSelection   _AllowSelection() override { return USES_SS_None; }
	virtual bool            _DoGroups() override { return false; }    // No graphics groups
	virtual ComponentMode   _GetComponentMode() override { return ComponentMode::SharedChild; }
	virtual bool			_NeedAcceptPoint() override { return true; }
	virtual bool			_WantDynamics() override { return false; }
	virtual bool			_WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }
	virtual StatusInt		_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual bool _OnPostLocate(HitPathCP path, WStringR cantAcceptReason);
protected:
	/**
	* @brief ���캯��
	* @param[in] toolName ��������
	* @return �޷���ֵ
	*/
	ReinforceTool(int toolName, int toolPrompt, MSDialogP dbP, int proID, int iCaseID, WString sNgName);

	/**
	* @brief ���߼̳к���
	* @return bool
	*/
	virtual bool _OnInstall() override;

	/**
	* @brief ���߼̳к���
	* @return �޷���ֵ
	*/
	virtual void _OnPostInstall() override;

	/**
	* @brief ������������
	* @return �޷���ֵ
	*/
	virtual void _OnRestartTool() override;

	/**
	* @brief ����������Ӧ����
	* @return �޷���ֵ
	*/
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;

	/**
	* @brief ������������
	*/
	virtual bool _OnResetButton(DgnButtonEventCR ev) override;

	/**
	* @brief �Զ���λ����
	*/
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	virtual void _OnCleanup() override;
	
	void SetupAndPromptForNextAction();

	void InsertTreeNode();
public:
	/**
	* @brief ���߰�װ����
	*/
	static void InstallNewInstance(int toolId, int toolPrompt, MSDialogP dbP, int proID, int iCaseID, WString sNgName);

	//��ʼ��XAttribute�Ľṹ��rebarInfo
	void InitRebarInfo(rebarInfo& orebarInfo);
private:
	//�Ƿ��Ѿ�ѡ�������
	bool	m_bFirstClicked;
	bool	m_bErrorClicked;

	//����ߵ���ʼ��
	DSegment3d m_clickPts;

	//��������ڵ�Zƽ��ֵ
	double	m_dPlaneZ;

	//�������������ModelRef
	DgnModelRefP m_modelOpt;

	ReBarInfo orebarInfo;

	MSDialogP m_dbP;

	// ȡ�����нڵ��UX,UY,UZ,SX,SY,SZ,SXY,SXZ,SYZ
	bmap<WString, bvector<double>> allNineDatas;

	// ����ڵ���Ϣ
	bvector<bvector<PointOutData>> vecSectionMeshDats;
	bvector<ElementId> m_lineList;

	TransformP tMatrix = NULL;

	bool isPointInFace(DPoint3d pt);
	void SmartLook(DPoint3d pt);
};
