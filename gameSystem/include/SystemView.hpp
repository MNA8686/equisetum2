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

	virtual int Init() = 0;

	const String& GetName() const;

	void Do()
	{
	//	ウィジェットの操作
	}

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	String m_name;
	int m_focus = 0;
};

#endif
