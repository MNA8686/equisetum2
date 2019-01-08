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
	void Setlayer(int layer);

protected:
	String m_text;
	Stat m_stat = Stat::Idle;
	bool m_focus = false;
	PointF m_pos;
	bool m_enable = true;
	int m_layer = 0;

	const PointF m_addPosOnPuress{ 0.004f, 0.004f };
};

#endif