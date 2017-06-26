/**
* @file  OutColumnData.h
* @brief  ��Ҫ�������������������ֵͼ
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
#include   "ClipFace.h"

/**
* @brief ���������������ֵͼ����
*/
class    OutColumnData : DgnPrimitiveTool
{
public:
	/** �����CAEģ��ID m_ProjectID��*/
	int m_ProjectID;

	/** �����CAEģ�͵�λ m_Unit��*/
	WString m_Unit;

	/** ����Ĺ���ID m_CaseID��*/
	int m_CaseID;

	/** ����������� m_ngName��*/
	WString m_ngName;

	/** ������¼���������� m_points*/
	bvector<DPoint3d>   m_points;

	/** ԭ������Сֵ����� orgMinPoint*/
	DPoint3d orgMinPoint;
	/** ԭ�������ֵ����� orgMaxPoint*/
	DPoint3d orgMaxPoint;

	/** ������¼��Щ��ֵ�����Ƿ������������ΪTURE���������Ϊfalse��˳��洢�� outColumnTypeint*/
	bvector<int> outColumnTypeint;

	/** ������¼�������ֵ���ͣ��ַ�����ʽ�� outColumnType*/
	bvector<WString> outColumnType;

	/** ������¼�������ֵ���ͣ��ַ�����ʽ�� outColumnTypeName*/
	bvector<WString> outColumnTypeName;

protected:

	OutColumnData(int toolId) : DgnPrimitiveTool(0, toolId) {}

	virtual void _OnPostInstall() override;
	virtual void _OnRestartTool() override { _ExitTool(); }
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); return true; }
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	void SetupAndPromptForNextAction();
	bool CreateLineElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);
	bool CreateArcElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);

	/**
	* @brief ��ʼ��Ҫ�����������ֵͼ������
	* @param[out] outColumnTypeWString    ����Ҫ�������������ֵͼ�����ͼ���
	* @return �޷���ֵ
	*/
	void InitColumnValueType();
public:
	static void InstallmyNewInstance(int toolId, WString ngName, int ProjectID, int caseID, WString sUnit);

	/**
	* @brief ��ȡ�е���ַ��������ӷ�ʽXYZ
	* @param[in] str     �������
	* @param[in] end     �յ�����
	* @return �����е����꣨���ַ�������ʽ��
	*/
	static WString GetMidValueString(DPoint3d str, DPoint3d end);

	/**
	* @brief ��������MESH����������,����������������ʱԪ�صķ�ʽ������
	* @param[out] outLines   ��������DESCRָ���������
	* @param[out] minPoint     mesh�����С����λ��
	* @param[out] maxPoint     mesh����������λ��
	* @param[in] NgName		   mesh������������
	* @return �޷���ֵ
	*/
	static void GetOutLineDsc(bvector<MSElementDescrP>& outLines, DPoint3d& minPoint, DPoint3d& maxPoint, WString NgName);

	/**
	* @brief ���㻮��Բ�����������ֵ�Բ�������
	* @param[out] arcPoints   ������ֺõĵ�����
	* @param[in] arcDes     ���뻮��ֱ��
	* @param[int] dfNum     ����Ҫ���ֵĵ����
	* @return �޷���ֵ
	*/
	void GetArcPoints(bvector<DPoint3d>& arcPoints, MSElementDescrP arcDes, double dfNum);

	/**
	* @brief ���㻮��Բ�����Բ�ĵ�����ߣ��������������ߵĽ���
	* @param[out] IntersectPoints   ����õ��Ľ���
	* @param[in] CenterPoint     Բ�ĵ�
	* @param[in] desPoint        ���ֵ�Բ����
	* @param[in] outLines        ��������DESCRָ���������
	* @return �޷���ֵ
	*/
	void GetAllIntersecPoints(bvector<DPoint3d>& IntersectPoints, DPoint3d CenterPoint, DPoint3d desPoint, bvector<MSElementDescrP>& outLines);
	
	/**
	* @brief ����ֵͼ���ⷽ��
	* @param[in] outLines   ��������DESCRָ���������
	* @param[in] dis_x      �����ֵͼʱƫ�����������ԭ��������ͼλ��
	* @param[in] levelId_OutData     ��ֵͼ�����ͼ��
	* @return �޷���ֵ
	*/
	bool DrawFaceOutline(bvector<MSElementDescrP>&outLines, EditElementHandleR cellHeader);

	/**
	* @brief ���ɸ���Բ��
	* @param[out] tCircle    ���ɵĸ���Բ��
	* @param[in] m_points   ����������������
	* @param[in] minpt      ������Сֵ�����
	* @return ���ش��������success��ʾ�ɹ�
	*/
	//int DrawArc(MSElement& tCircle,bvector<DPoint3d>& m_points, DPoint3d minpt);

	/**
	* @brief ��������Բ�����ֵ������������н���
	* @param[out] allIntersectPoints    �õ������н���
	* @param[in] mid_point   ����Բ�������ĵ�
	* @param[in] outLines    �����������߶μ���
	* @param[in] arcPoints   ����Բ�����ֵ�
	* @return �޷���ֵ
	*/
	void GetInsectPointswithOutLines(bvector<bvector<DPoint3d>>& allIntersectPoints, DPoint3d mid_point, bvector<MSElementDescrP> outLines, bvector<DPoint3d> arcPoints);
	
	/**
	* @brief ��Բ�����ֵ���������Ľ��㣬��ȡ����Բ����ֵ�������
	* @param[out] allOutPoints    ����Ҫ�������ֵ������
	* @param[in] PerDis      ÿһ��Բ���ϵȷ������ֵ�����
	* @param[in] allIntersectPoints    ��õ��������������н���
	* @param[in] mid_point   ����Բ�������ĵ�
	* @param[in] outLines    �����������߶μ���
	* @return �޷���ֵ
	*/
	void GetColumnoutPoints(bvector<bvector<DPoint3d>>& allOutPoints, double PerDis,
		bvector<bvector<DPoint3d>> allIntersectPoints, DPoint3d mid_point, bvector<MSElementDescrP>& outLines);
};

void OutColumnValueLists(WString ngName, int ProjectID, int caseID, WString sUnit);