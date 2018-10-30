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

	void DoWidget();
	void RenderWidget();

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	String m_name;
	int m_focus = 0;
};

class AssetMenu : public SystemView
{
public:
	AssetMenu() = default;
	virtual ~AssetMenu() = default;

	int Enter() override;

//	void Do() override;

	static std::shared_ptr<AssetMenu> Create();

protected:
};

#endif
