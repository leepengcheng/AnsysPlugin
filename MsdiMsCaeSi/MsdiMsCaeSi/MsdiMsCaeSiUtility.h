
class MsdiMsCaeSiUtility
{
public:
	MsdiMsCaeSiUtility(void);
	~MsdiMsCaeSiUtility();
public:
	// ȡ��CAEģ�͵���ʾ��λ
	static double GetUnit(WString strProjectUnit);

	static void getBasePtAndLevelID(DPoint3dR basePt, WStringR sUnit, LevelId& levelId_CaeModel, WStringCR ngName);

	// ��ȡ�����ת������(��ά����->��άƽ���ת������)
	static void GetSectionTransform(TransformP *tMatrix, WString strNgName);

	//ˢ�½����б�ѡ�к���ı���ʾ
	static WString MakeSelectedTreeNodeForTxt(GuiTreeNodeP treeNode);

	// ���ָ�ʽ��ʾ
	static void FormatValDisplay(WStringR strVal, double& dVal, int iDecimal);

	// ���������������򣩣�SortType��0��x,1:y,2:z��
	static void SortPoints(bvector<DPoint3d>& vecPoints, int SortType);

	static bool CreateText(EditElementHandleR eehText, DgnModelRefP ModelRef, DPoint3d tPos, WString tText, double fwidth, double fheight, double iSlant = 0.0, UInt32 iJustification = -1);

	static bool GetPosition(ContourPoint& ptResult, DPoint3d ptStart, DPoint3d ptEnd, double dStartVal, double dEndVal, double dVal);

	static int PointInPolygon(DPoint3d p, const bvector<PointOutData>& ptPolygon);

	// �ж��߶��Ƿ������p0(p0����һ��)
	static bool PtIsOnline(DPoint3d p0, DPoint3d p1, DPoint3d p2);

	static void fitView(int viewIndex);

	//������ֱ�ߵĽ���
	static DPoint3d GetTwoLineIntersect(DPoint3d str1, DPoint3d end1, DPoint3d str2, DPoint3d end2);

	//// ����Ԫ�κ�������Ӧ��ֵ
	//static double UseBilinear_interpolationGetVlue(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1 = false);//����Ԫ�κ���
	// �������Ȩ������Ӧ��ֵ
	static void UseIDW_interpolationGetVlue(PointOutData& mid_Point, const bvector<PointOutData>&  MeshPoints);
private:
	static bool Intersect(DSegment3d L1, DSegment3d L2);
	static double Multiply(DPoint3d p1, DPoint3d p2, DPoint3d p0);

	//static double UseBilinear_interpolationGetVlue_56(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//����Ԫ�κ�����5/6����������

	//static double UseBilinear_interpolationGetVlue_4(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//����Ԫ�κ�����4����������

	//static double UseBilinear_interpolationGetVlue_3(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//����Ԫ�κ�����3����������
};