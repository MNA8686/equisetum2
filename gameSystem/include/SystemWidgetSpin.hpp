#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemWidgetSpin : public SystemWidget
{
public:
	SystemWidgetSpin() = default;
	~SystemWidgetSpin() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;

	static std::shared_ptr<SystemWidgetSpin> Create(const String& label, const std::function<void (int32_t)> cb);

	void SetRange(int32_t min, int32_t max, int32_t step = 1);
	void SetValue(int32_t val);
	int32_t GetValue() const;
	void SetCyclic(bool val);
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;

private:
	std::function<void(int32_t)> m_cb;
	int32_t m_min = 0;
	int32_t m_max = 100;
	int32_t m_step = 1;
	int32_t m_val = 0;
	int32_t m_direction = 0;
	bool m_cyclic = false;

	const String MakeString();

	const String leftArrow = u8"◀";
	const String rightArrow = u8"▶";

	std::shared_ptr<SystemWidgetLabel> m_label;
};

