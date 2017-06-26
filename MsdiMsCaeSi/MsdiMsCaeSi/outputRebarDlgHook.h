#pragma once

void outputRebarCmd(char* unparsed);

// �����������
void writeRebarResultsToX(bmap<WString, ReBarInfo>& mapRebarResults);
// ��ȡ�������
void readRebarResultsFromX(bmap<WString, ReBarInfo>& mapRebarResults);
void CalRebar_adjustVSashDialogItems(MSDialogP  db, BSIRect *pOldContent, bool refreshItems);
void HookDialog_CalRebar_InterfaceDynamic(DialogMessage *dmP);	
void HookPushBtn_CalRebar_Refresh(DialogItemMessage* dimP);				//ˢ��
void HookPushBtn_CalRebar_Calculate(DialogItemMessage* dimP);			//������
void HookPushBtn_CalRebar_OutPut(DialogItemMessage* dimP);				//���

void HookSash_CalRebar_vSashHook(DialogItemMessage* dimP);				//�ָ��������¼�	
void HookGeneric_CalRebar_Canva(DialogItemMessage* dimP);
void HookTree_CalRebar_SectionTree(DialogItemMessage* dimP);			//������
void HookListBox_CalRebar_CalcResult(DialogItemMessage* dimP);			//���listbox
void HookListBox_CalRebar_RebarInfo(DialogItemMessage* dimP);			//�ֽ�listbox,���һ��,��txt�ؼ���0�е�1��
void HookItem_CalRebar_CellCombbox_Diameter(DialogItemMessage* dimP);	//�ֽ�listbox��ֱ��cell����txt���¼�
void HookItem_CalRebar_CellTxt_RebarNumber(DialogItemMessage* dmp);		//�ֽ�listbox�ĸ���cell����txt���¼�
void HookComboBox_CalRebar_RebarType(DialogItemMessageP dimP);			//�ֽ�����������			
void HookComboBox_CalRebar_ConcrType(DialogItemMessageP dimP);			//����������������
void HOOKText_CalRebar_RebarTesion(DialogItemMessageP dimP);			//�ֽ��ǿ��ֵ�䶯ʱ����combbox
void HOOKText_CalRebar_ConcrTesion(DialogItemMessageP dimP);			//����������ǿ��ֵ�䶯ʱ����combbox
void HookPdm_CalRebar_AddRebarLine(DialogItemMessage* dimP);			//����������Ҽ��˵�
void HookPdm_CalRebar_DelRebarLine(DialogItemMessage* dimP);			//ɾ�������	
void HookDialog_CalRebar_AddRebarLine(DialogMessage *dmP);				//����������Ի���


// ��ʼ������ʾ(������Ϣ)
void InitSectionTreeDisp(DialogItemP dimp);
// ��ʼ����������
bool InitFormData();									
// ��ʼ������ListBox������
void InitListBoxData(MSDialogP dbP);
//��ʼ���ؼ�����������Ľṹ��ReBarCalDlgInfo
void InitReBarCalDlgInfo();
//��ȡ�ؼ�ֵ
void GetDataFromControls(rebarInfo& orebarInfo);
//��ȡlistbox��EditCellֵ
void GetListBoxEditCellStringValue(DialogItemMessage* dimP, RscId listboxId, WCharCP* textValue);	
//��ʼ��������ͼ����ɫ����
void InitDrawColorShape(DialogItemMessage* dimP, GuiTreeNode* pRebarLineNode, const bvector<WString>& vecData);

void getStressList(const WChar* field, bvector<double>& tesionList);
double GetTrapezoidArea(double dTop, double bBottom, double dHeigh);	//ͨ��Ӧ���͵������������
double GetAreaByDiameter(int& number, double dAs, double diameter=6.0);	//����ֱ��������������:�ο�������������
double GetAreaByRebarCount(double& diameter, double dAs, int number);	//���ݸ���������������:�ο���������ֱ��
int checkTesionValid(const bvector<double>& tesionList, double dFt);
void CountOfZeroPoint(const bvector<double>& tesionList, int& nZero);
void GetValidTesionHeadAndTail(const bvector<double>& tesionList, double Ft, int& iStart, int& iEnd);
void GetAAndAct(double dPrev, double dNext, double Ft, double span, double& dA, double& dAct);
void GetConcretePositions(const bvector<double>& tesionList, double iTail, double span, double Ft, double dLineLength, double dEndSpan, int& iConcr_No, double& dConcrP);

double GetAPeriodBendingStress(double x1, double y1, double x2, double y2, double dD);
double GetBendingStress(const bvector<double>& tesionList, double span, double dLineLength);

double GetAPeriodAxialStress(double x1, double y1, double x2, double y2);
double GetAxialStress(const bvector<double>& tesionList, double span, double dLineLength);
/************************************************************************/
/*	dLineLength ����߳���
/*  ReBarInfo	��������Ͳ��ּ������Ľṹ��
/*	@brief		������
/************************************************************************/
bool calculateRebarArea(double dLineLength, ReBarInfo& info);

/*-----------------------------------------------------------------
*  ��������: ��ָ����Dialogָ����Item�л���Ӧ��ͼ
*	@param	dbP	IN	ָ����Dialogָ��
*	@param	diP	IN	ָ����DialogItemָ��
*
*  @return	��
-----------------------------------------------------------------*/
void drawStressView(MSDialogP dbP, DialogItemP diP);
bool createLineString(EditElementHandleR eeh, bvector<DPoint3d> &points, DgnModelRefP modelRef);
bool createLine(EditElementHandleR eeh, DSegment3dCR segment, DgnModelRefP modelRef);

void CalRebar_setupPopupMenu(DialogMessageP dmP);

// ȡ�������ָ�����λ���ϵ�Ӧ��ֵ
bool getRebarLineStressbyDistances(ReBarInfo& orebarInfo, const bvector<bvector<PointOutData>>& m_FacetVec, CurveVectorPtr curve, double dLineLength, TransformCP tMatrix, RotMatrixCR rotMatrix);

void SetStressValToListBox(MSDialogP dbP, const ReBarInfo& info);	//д��Ӧ��Listbox
void SetResultValToListBox(MSDialogP dbP, const ReBarInfo& info);	//������д�����б�͸ֽ��б�
void SetRebarInfoToListBox(MSDialogP dbP, const ReBarInfo& info);	//��ȡlistbox�ڶ���cell�ĸֽ����,������д��ֽ���Ϣlistbox

void     mdlDialog_rectInset
(
BSIRect        *rP,
int            deltaX,        /* positive moves in */
int            deltaY
);

void Draw3DCell
(
MSElementDescr      *cellP,
MSDialogP           dbP,
BSIRect             *localRectP,
ViewFlags           *viewflagsP,
bool                nDices
);
