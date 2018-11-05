#if !defined(_EQSYSTEMVIEW_H_)
#define _EQSYSTEMVIEW_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidget.hpp"

class SystemView
{
public:
	enum class Stat : int32_t
	{
		Idle,
		Push,
		Pop,
	};

	SystemView();
	virtual ~SystemView();

	void SetPos(const PointF& pos);
	PointF GetPos() const;
	Point PosNormalToPixel() const;

	virtual int Enter();
	virtual int Leave();
	virtual int Do();
	virtual int Render();

	const String& GetName() const;

	void DoView();
	void RenderView();

	void Push(std::shared_ptr<SystemView> pView);
	std::shared_ptr<SystemView> GetNextView();
	void Pop();

	void SetFocus(bool focus);
	bool GetFocus() const;
	Stat GetStat() const;

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	String m_name;
	std::shared_ptr<SystemView> m_nextView;
	PointF m_pos;
	bool m_focus = false;
	Stat m_stat = Stat::Idle;
};

class AssetMenu : public SystemView
{
public:
	AssetMenu() = default;
	virtual ~AssetMenu() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<AssetMenu> Create(const String & name);

protected:
	int32_t m_rate = 100;
	int32_t m_ptr = 0;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
};

#endif
