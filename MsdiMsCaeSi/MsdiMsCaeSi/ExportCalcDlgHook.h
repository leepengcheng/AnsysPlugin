#pragma once

void HookDialog_expCalc(DialogMessage* dmP);

void HookPushBtn_ExpCalc_Import(DialogItemMessage* dimP);

void HookPushBtn_ExpCalc_AddPath(DialogItemMessage* dimP);

void HookPushBtn_ExcCalc_DelPath(DialogItemMessage* dimP);

void HookPushBtn_ExpCalc_Submit(DialogItemMessage *dimP);

void HookListBox_ExpCalc_CaseList(DialogItemMessage* dimP);

void HookListBox_ExpCalc_ModelParamList(DialogItemMessage* dimP);

void HookListBox_ExpCalc_WYParamList(DialogItemMessage* dimP);

void HookListBox_ExpCalc_YLParamList(DialogItemMessage* dimP);

void HookListBox_ExpCalc_PJParamList(DialogItemMessage* dimP);

void HookMlText_ExpCalc_ProDescr(DialogItemMessage* dimP);

void HookComboBox_ExpCalc_ProjectLists(DialogItemMessage* dimP);

void initDatasIntoListBox(RawItemHdrP listBox, ListBoxDatas& datas);

//�����飬����ָ������ID�����ݿ��ȡ��Ӧ������䵽�ṹ����(�ɶ�Ӧ��ListBox�ؼ�ID����)
void initDatasFromDB(int iProjectID, RscId listBoxID);

void exportCalDocCmd(char* unparsed);