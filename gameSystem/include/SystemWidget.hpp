#if !defined(_EQSYSTEMWIDGET_H_)
#define _EQSYSTEMWIDGET_H_

#include "Equisetum2.h"
using namespace Equisetum2;

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
	virtual int Do() = 0;
	virtual int Render() = 0;
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

class SystemWidgetEnterView : public SystemWidget
{
public:
	SystemWidgetEnterView() = default;
	~SystemWidgetEnterView() = default;
	int Do() override;
	int Render() override;
	Rect GetBox() const override;

	static std::shared_ptr<SystemWidgetEnterView> Create(const String& label);

private:
	std::shared_ptr<SystemWidgetLabel> m_label;

};

class SystemWidgetReturnView : public SystemWidget
{
public:
	SystemWidgetReturnView() = default;
	~SystemWidgetReturnView() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetReturnView> Create(const String& label);
	Rect GetBox() const override;

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif