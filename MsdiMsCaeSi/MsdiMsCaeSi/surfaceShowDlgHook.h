#pragma once

void surfaceShowCmd(WCharCP unparsed);

void Surface_adjustVSashDialogItems(MSDialogP   db, BSIRect     *pOldContent, bool        refreshItems);

void HookDialog_SurfaceShow(DialogMessage* dmP);

void HookTree_Surface_ClipList(DialogItemMessage* dimP);

void HookComboBox_Surface_Model(DialogItemMessage* dimP);

void HookComboBox_Surface_ResultType(DialogItemMessage* dimP);

void HookButton_Surface_ResultShow(DialogItemMessage* dimP);

void ShowHideMesh();//��ʾ���ص�CAEģ��

void ClearclipfaceShow();//�������չʾ���

void HookButton_Surface_ShowSet(DialogItemMessage *dmP);

void GetStrValAndEndVal(WString PlotName, double& tStrValue, double& tEndValue, int ProjectId, int CaseId, WString RstTypeCode, DPoint3d& minPoint, DPoint3d& maxPoint);

void HookPdm_Surface_Add(DialogItemMessage* dimP);

void HookPdm_Surface_Del(DialogItemMessage* dimP);

void HookButton_Surface_ShowMinMax(DialogItemMessage* dimP);

void HookSash_Surface_vSashHook(DialogItemMessage* dimP);

void SurfaceShow_setupPopupMenu(DialogMessage* dmP);

void RefreshSurfaceShowDlg();

void HookItem_LockAxis(DialogItemMessage* dmp);				//�������Ṧ��
void HookDialog_Surface_AddClipFace(DialogMessage* dmp);	//�������������dialog