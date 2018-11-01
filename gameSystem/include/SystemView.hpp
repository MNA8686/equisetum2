#if !defined(_EQSYSTEMVIEW_H_)
#define _EQSYSTEMVIEW_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidget.hpp"

class SystemView
{
public:
	SystemView();
	virtual ~SystemView();

	void SetPos(const Point& pos);

	virtual int Enter();
	virtual int Leave();
	virtual int Do();
	virtual int Render();

	const String& GetName() const;

	void DoView();
	void RenderView();

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	String m_name;
	//int m_focus = 0;
	//int m_cursolMoveCounter = 0;
	//std::shared_ptr<RectRenderer> m_rectRenderer;
	//Rect m_cursorNow;
	//Rect m_cursorSrc;
	//Rect m_cursorDest;
};

class AssetMenu : public SystemView
{
public:
	AssetMenu() = default;
	virtual ~AssetMenu() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<AssetMenu> Create();

protected:
	int32_t m_rate = 100;
	int32_t m_ptr = 0;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
};

#endif
