#pragma once

struct nodeViewer : public DgnElementSetTool
{
	nodeViewer(int toolID, bool plotModel);

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt _OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		__super::_OnPostInstall();

		mdlAccuSnap_enableLocate(FALSE);
		mdlAccuDraw_setEnabledState(FALSE);

		mdlLocate_allowLocked();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnPostLocate(HitPathCP path, WStringR cantAcceptReason);

	virtual bool _OnDataButton(DgnButtonEventCR ev);

	virtual bool _OnResetButton(DgnButtonEventCR ev) override
	{
		_ExitTool();
		return true;
	}

	virtual void _OnCleanup();

	//virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId, bool plotModel)
	{
		nodeViewer* tool = new nodeViewer(toolId, plotModel);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId(), m_bPlotModel); }

	DPoint3d m_CurNodePt;

	double	 m_dCurNodeInfo;

	//��ʾ���ع���ʱ�Ƿ�Ϊ��ͼģʽ������������Ҫ������ѡ���������ݿ��ж�ȡ����
	bool m_bPlotModel;

	//�������ж�ȡ��ָ����������±�+1��Ϊ��Ӧ��nodeID
	vector<double> m_vecResults;
};

void nodeViewerCmd(WCharP unparsed);