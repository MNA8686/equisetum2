#if !defined(_EQSYSTEMWIDGET_H_)
#define _EQSYSTEMWIDGET_H_

#include "Equisetum2.h"
using namespace Equisetum2;

class SystemWidget
{
public:
	SystemWidget();
	virtual ~SystemWidget();
};

class SystemWidgetNext : public SystemWidget
{
};

class SystemWidgetPrev : public SystemWidget
{
};

class SystemWidgetCustom : public SystemWidget
{
};

class SystemWidgetSpin : public SystemWidget
{
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif