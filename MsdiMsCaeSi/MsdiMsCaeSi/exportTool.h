/**
* @file     exportTool.h
* @brief    ��msģ�͵�����parasolid��ʽ�ļ�
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/


/**
* @brief parasolid�ļ�������
*/
struct exportTool :public  DgnElementSetTool
{
private:
	/** ��ȡ�������������λ�� thePoint*/
	static DPoint3d thePoint;
protected:

	/**
	* @brief  ���캯��
	*/
	exportTool(int toolID) : DgnElementSetTool(toolID)
	{

	}
	virtual UsesDragSelect  _AllowDragSelect() { return USES_DRAGSELECT_Box; }

	virtual bool WantDynamics()  { return false; }

	virtual bool NeedAcceptPoint()  { return true; }

	virtual bool IsModifyOriginal()  { return false; }

	virtual bool WantAccuSnap() { return false; }




	EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override


	virtual void _OnPostInstall();

	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _ExitTool(); return true; }


	virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	bool          _WantAdditionalLocate(DgnButtonEventCP ev); //override


	//void SetupForLocate(int msgId);

	void   _OnRestartTool();

	StatusInt       _OnElementModify(EditElementHandleR  el) override          { return ERROR; }
	///**
	//* @brief  �������߰�ť
	//* @param[in] ev   ��ťָ��
	//*/
	//bool            _OnResetButton(DgnButtonEventCR ev) override                { _OnRestartTool(); return true; }
	//
	///**
	//* @brief  �Ƿ���Ҫ������
	//*/
	//bool            _DoGroups() override                                       { return false; }
	//
	///**
	//* @brief  �Ƿ���Ҫ����Ĳ�����
	//*/
	//bool            _NeedAcceptPoint() override                                { return true; }
	//
	///**
	//* @brief  �Ƿ���Ҫ��ѡ����
	//*/
	//bool            _NeedPointForSelection() override                          { return true; }
	//
	///**
	//* @brief  �ı�Ԫ�غ���
	//*/
	//StatusInt       _OnElementModify(EditElementHandleR  el) override          { return ERROR; }

	//virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	///**
	//* @brief  �Ƿ�����ѡ������
	//*/
	//virtual UsesSelection   _AllowSelection() { return USES_SS_Required; }
	///**
	//* @brief  �Ƿ������ѡ����
	//*/
	//virtual UsesDragSelect  _AllowDragSelect() { return USES_DRAGSELECT_Box; }
	///**
	//* @brief  ɸѡ������
	//*/
	//void     SetupForLocate();

	///**
	//* @brief  ��������Ӧ����
	//*/
	//bool            _OnDataButton(DgnButtonEventCR ev);
	///**
	//* @brief  ���ܼ����»��ߵ���ʱ
	//*/
	//bool            _OnModifierKeyTransition(bool wentDown, int key); //override
	//
	///**
	//* @brief  �Ƿ���Ҫѡ����������
	//*/
	//bool            _WantAdditionalLocate(DgnButtonEventCP ev); //override

	///**
	//* @brief  �ɵ�ѡ·�����ѡ������
	//*/
	//   EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override

	///**
	//* @brief  ��װ����
	//*/
	//   bool            _OnInstall();
	///**
	//* @brief  ��������
	//*/
	//void   _OnRestartTool();	

public:
	/**
	* @brief  ��װһ���µĹ���
	*/
	static void InstallNewInstance(int toolId);

	/**
	* @brief  ����ѡ�е����嵽Parasolid
	*/
	static void ExportSelect_toParasolid(ElementAgendaR theAgenda);

	/**
	* @brief  �����Ի�����
	* @param[in] xmtVersionP    �����汾��
	* @param[in] masterUnitsP   �����ĵ�λ
	* @param[in] fileNameP      ������·��
	*/
	static int  xmtWrite_fileCreateDialog(int *xmtVersionP, int *masterUnitsP, BeFileNameR  fileNameP);

	/**
	* @brief  ��ѡ�еĵ�����λ�õ�ʵ�ʲ���
	* @param[in] masterUnits        �����ĵ�λ
	* @param[out] Lastmasterunits   �����ĵ�λ��Ӧʹ�ò���
	*/
	static void GetTheUnit(int masterUnits, DgnPlatform::StandardUnit& Lastmasterunits);

	/**
	* @brief  ��������άģ����ӵ�entitylist��
	* @param[in] edP        Ҫ��ӵ���άģ��
	* @param[in] modelRef   ģ�����ڵ�model
	* @param[in out] bodyListP   ��άģ�ͼ���
	* @param[in out] templateListP   ��άģ��ת��ΪENTITY��ģ����󼯺�
	* @param[in out] transListP      ��άģ��ת��ΪENTITY��ת�����󼯺�
	* @param[in out] remainingEdP     ��άģ��ת��ΪENTITY���ɹ�������ģ�ͼ���
	*/
	static void AllSelectTo_ListFunction(MSElementDescrP edP, DgnModelRefP modelRef,
		TAG_ENTITY_LIST  *bodyListP, NON_ENTITY_LIST *templateListP
		, NON_ENTITY_LIST *transListP, MSElementDescr *remainingEdP);
};

/**
* @brief  ms�ṩ�ķ�װ�����������ǽ����������ENTITY��parasolid��ʽ��ŵ���Ӧ·����
* @param[in] fileName        ���·��
* @param[in] ascii
* @param[in] xmtVersion     ת���汾
* @param[in] listP   ENTITY����
*/
int mdlSolid_saveParts(WCharCP  fileName, int  ascii, int xmtVersion, TAG_ENTITY_LIST *listP);
