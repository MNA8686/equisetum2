#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetLabel;
class Dashboard
{
public:

	static std::shared_ptr<Dashboard> CreateWithView(std::shared_ptr<SystemView> pView);

	Dashboard() = default;
	virtual ~Dashboard() = default;

	void SetPos(const PointF& pos);
	PointF GetPos() const;

	virtual int Do();
	virtual int Render();

protected:

	std::shared_ptr<SystemWidgetLabel> m_labelBreadcrumb;
	std::vector<std::shared_ptr<SystemView>> m_vView;

	void RebuildBreadcrumb();
	bool IsTransition() const;

	bool m_next = false;
	bool m_prev = false;
	PointF m_nowPos;	// ��ԍ��̃r���[�̍��W
	PointF m_destPos;	// ��ԍ��̃r���[�̖ڕW���W
	PointF m_pos;
};

