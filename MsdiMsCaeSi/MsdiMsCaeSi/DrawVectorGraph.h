/**
* @file     DrawVectorGraph.h
* @brief    ��ʸ��ͼ
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
#pragma once
#include "MsdiMsCaeSipch.h"
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_SQLITE
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
/**
* @brief ��ʱ��ȡNodeID�ṹ��
*/
struct MeshSurfaceValue
{
	double NodeValue[30];
	int nodeCount;
};

/**
* @brief ��ʱ��ȡNodeID�ṹ��
*/
struct MeshProperty
{
	int ID[30];
	int ElemID;
	int MaterialNumber; // ��������
	int ElemType;		// ��Ԫ����
	int RealConstantNumber;// ʵ����
};
/**
* @brief ��ʸ��ͼ��
*/
class DrawVectorGraph : public IViewTransients
{
public:
	/**
	* @brief ��ʸ��ͼ����
	* @param[in] isPreUpdate   �Ƿ���ǰˢ��
	* @param[in] viewport     ��Ӧ��ͼָ��
	* @return �޷���ֵ
	*/
	virtual void     _DrawTransients(ViewContextR context, bool isPreUpdate);

	/**
	* @brief ��ȡ����mesh��������Ͷ��������Ӧ����ӦӦ������
	* @return �޷���ֵ
	*/
	void GetAllPoints();

	/**
	* @brief �����ݿ��л�ȡ���нڵ�����
	* @return ��ȡ���Ľڵ��������ݿ���
	*/
	DbResult GetNodeFromDatabase(void);

	/**
	* @brief �����ݿ��л�ȡ���нڵ��ӦӦ����������
	* @return ��ȡ���ķ����������ݿ���
	*/
	DbResult GetVectorFromDatabase(void);

	/** �ڵ����꼯�� NodePoints*/
	bvector<Dpoint3d> NodePoints;

	/** Ӧ�����꼯�� VectorPoints*/
	bvector<Dpoint3d> VectorPoints;

	/**
	* @brief  ͨ��view����ʱ�߶κ�����ʸ����ͷ���ɸ����߶���ɣ�
	* @param[in] iview_draw   ��ͼ��ͼ���ָ��
	* @param[in] points       �߶������յ�����
	* @param[in] num          ��ĸ�����Ĭ��Ϊ2��
	* @return �޷���ֵ
	*/
	void DrawLine(IViewDrawR iview_draw, DPoint3d* points, int num);
private:
	/** �������ݿ���� m_db*/
	Db m_db;
	/** �������ݿ�״̬���� m_statment*/
	Statement *m_statment;

	/**
	* @brief  �ɻ�ȡ���Ľڵ����ݿ����õ���Ӧ�Ľڵ�����ֵ
	* @param[out] vecPoints   �ڵ����꼯��
	* @return �޷���ֵ
	*/
	void GetNodePoints(bvector<DPoint3d>& vecPoints);

	/**
	* @brief  �ɶ�Ӧ��Ŵ����ݿ�����ȡ����Ӧ����ֵ
	* @param[out] iIdx   ���ֵ
	* @return ����ֵ
	*/
	DPoint3d GetPointFromStatment(int iIdx);

	/**
	* @brief  ��Ӧ��ֵ����ɫ���л�ȡ��Ӧ����Ӧ����ɫֵ
	* @param[in] theValue Ӧ����ֵ
	* @return ��ɫ�������
	*/
	UInt32 GetTheColor(double theValue);
};


