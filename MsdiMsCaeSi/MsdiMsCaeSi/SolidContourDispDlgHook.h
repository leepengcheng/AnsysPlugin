#pragma once

void SetCAERstDispCmd(WCharCP unparsed);

void startShowCutFaceContour(DrawColorShape* pDCShape);

void startShowContour(DrawColorShape* pDCShape, DPoint3dCP BasePt = NULL, bool bClear = true);

void enableWidget(MSDialogP db, bool enable);

void initConditionValuesByTypes(MSDialogP db);

void SolidDisp_adjustVSashDialogItems(MSDialogP   db, BSIRect     *pOldContent, bool        refreshItems);

void HookTree_SolidDisp(DialogItemMessage   *dimP);

void HookDialog_SolidDisp(DialogMessage *dmP);

void HookButton_SolidDisp_ShowSet(DialogItemMessage *dmP);

void HookComboBox_SolidDisp_ResultType(DialogItemMessage *dimP);

void HookComboBox_SolidDisp_ShowModel(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_CondType(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_CondVal(DialogItemMessage* dimP);

void HookButton_SolidDisp_AddC(DialogItemMessage* dimP);

void HookButton_SolidDisp_DelC(DialogItemMessage* dimP);

void HookButton_SolidDisp_ClearC(DialogItemMessage* dimP);

void HookButton_SolidDisp_Show(DialogItemMessage* dimP);

void HookButton_Damage_Show(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_ShowType(DialogItemMessage* dimP);

void HookSash_SolidDisp_vSashHook(DialogItemMessage* dimP);

void HookButton_Boundary_Show(DialogItemMessage* dimP);

//��ȡCAE�߽���Ϣ��Ԫ���sql���
StatusInt GetBoundarySolidSQL(Statement* pStatement, BeSQLiteDbCR db);
//�����߽���Ϣ���е���
void BuildBoundaryFaces(int projectId,int caseId, WCharCP ngName);
/**
* @brief  ȡ��CAE��Mesh��Ľڵ�ͽڵ����꼯��
* @param[out] vecIndices		Mesh��Ľڵ㼯��
* @param[out] vecPoints			Mesh��Ľڵ����꼯��
* @param[out] oFaceNodeStrMap	Mesh�����Ľڵ��ַ�������(�磺1-2-3-4)
* @param[in]  vecNodeId			nodeID����
* @param[in]  basePt			CAEģ�͵ķ��������(�����ԭ���ƫ��ֵ��
* @return �޷���ֵ
*/
void GetBoundaryFacesInfo(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt, WStringCR sUnit, Statement* pStatement);
void MakeBoundaryFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4);
WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4);
//Ϊ�������basePtȷ����ʵ����
DPoint3d GetPointFromBasePoint(DPoint3dCP basePt, WStringCR sUnit, int iIdx, Statement* pStatement);

//����߽���Ϣ
void ClearBoundary();

////��ȡ�����ߴ���
//#pragma region
////��ȡCAE����mesh�����ڵ㼯��
//bool GetFaceInfos(int projectId, int caseId, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt);
////��ȡ������
//void ShowOuterContour(int projectId, int caseId, int levelId, DPoint3d basePt);
//#pragma endregion
