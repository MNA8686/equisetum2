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

	const String& GetName() const;

	void Do()
	{
	//	ウィジェットの操作
	}

//	std::shared_ptr<SystemView> CreateSystemView();

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	String m_name;
	int m_focus = 0;
};

#endif
