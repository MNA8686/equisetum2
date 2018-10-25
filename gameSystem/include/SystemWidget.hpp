#if !defined(_EQSYSTEMWIDGET_H_)
#define _EQSYSTEMWIDGET_H_

#include "Equisetum2.h"
using namespace Equisetum2;

class SystemWidget
{
public:
	SystemWidget();
	virtual ~SystemWidget();
	virtual int Do() = 0;
};

class SystemWidgetNext : public SystemWidget
{
public:
	SystemWidgetNext() = default;
	~SystemWidgetNext() = default;

	static std::shared_ptr<SystemWidgetNext> Create();
};

class SystemWidgetPrev : public SystemWidget
{
};

class SystemWidgetCustom : public SystemWidget
{
};

class SystemWidgetSpin : public SystemWidget
{
public:
	SystemWidgetSpin() = default;
	~SystemWidgetSpin() = default;

	static std::shared_ptr<SystemWidgetSpin> Create(int32_t min, int32_t max, int32_t step, const std::function<void (int32_t)> cb);

	void SetRange(int32_t min, int32_t max);
	void SetValue(int32_t val);
	int Do() override;

private:
	std::function<void(int32_t)> m_cb;
	int32_t m_min = 0;
	int32_t m_max = 0;
	int32_t m_val = 0;
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif