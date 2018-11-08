#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"
#include "SystemWidgetLabel.hpp"

class SystemChoiceDialog
{
public:
	static std::shared_ptr<SystemChoiceDialog> Create(const String& title, const std::vector<String>& vItem, const std::function<void(int index, const String& item)> cb);

	SystemChoiceDialog() = default;
	virtual ~SystemChoiceDialog() = default;

	int Do(SystemView* pView);
	int Render(const SystemView* pView);
	bool IsActive() const;

protected:
	std::function<void(int index, const String& item)> m_cb;
	std::vector<String> m_vItem;
	String m_title;
	bool m_isActive = false;
	int32_t m_maxRow = 0;

	std::shared_ptr<RectRenderer> m_rectRendererBorder;
	std::shared_ptr<RectRenderer> m_rectRendererInnerFrame;
	std::shared_ptr<SystemWidgetLabel> m_label;
	
	const float textSize = 0.05f;
	const float textMargin = textSize * 1.5f;

	// 外枠
	const SizeF sizeBorderMargin{ 0.05f, 0.05f };
	RectF m_rectBorder{ sizeBorderMargin.x, sizeBorderMargin.y, 1.f - (sizeBorderMargin.x) * 2, 1.f - (sizeBorderMargin.y) * 2 };
	Rect m_rectBorderPixel;			// ピクセル版 

	// 内枠
	const SizeF sizeInnerFrameMargin{ 0.04f, 0.04f };
	RectF m_rectInnerFrame{ m_rectBorder.x + sizeInnerFrameMargin.x, m_rectBorder.y + textMargin,
							m_rectBorder.width - (sizeInnerFrameMargin.x) * 2, m_rectBorder.height - (sizeInnerFrameMargin.y + textMargin) };
	Rect m_rectInnerFramePixel;		// ピクセル版
};


