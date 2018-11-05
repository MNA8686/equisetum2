#if !defined(_EQSYSTEMWIDGET_H_)
#define _EQSYSTEMWIDGET_H_

#include "Equisetum2.h"
using namespace Equisetum2;

class SystemView;

class SystemWidget
{
public:
	enum class Stat : int32_t
	{
		Idle,
		Next,
		Prev,
		Exclusive,
	};

	SystemWidget();
	virtual ~SystemWidget();
	virtual int Do(SystemView* pView) = 0;
	virtual int Render(const SystemView* pView) = 0;
	void Prev();
	void Next();
	void Prepare();
	void SetFocus(bool focus);
	bool GetFocus() const;
	Stat GetStat() const;
	virtual void SetPos(const PointF& pos);
	PointF GetPos() const;
	void SetEnable(bool enable);
	virtual Rect GetBox() const = 0;

protected:
	String m_text;
	Stat m_stat = Stat::Idle;
	bool m_focus = false;
	PointF m_pos;
	bool m_enable = true;
};

#include "SystemWidgetLabel.hpp"
class SystemWidgetLabel;

class SystemWidgetPushView : public SystemWidget
{
public:
	SystemWidgetPushView() = default;
	~SystemWidgetPushView() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;

	static std::shared_ptr<SystemWidgetPushView> Create(const String& label, const std::function<void()>& cb);

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
	std::function<void()> m_cb;
};


class SystemWidgetPopView : public SystemWidget
{
public:
	SystemWidgetPopView() = default;
	~SystemWidgetPopView() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;

	static std::shared_ptr<SystemWidgetPopView> Create(const String& label);
	Rect GetBox() const override;

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif