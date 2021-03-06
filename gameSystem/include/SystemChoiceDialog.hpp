#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"
#include "SystemWidgetLabel.hpp"
#include "SystemWidgetLabelWithFont.hpp"

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
	std::function<void(int index, const String& item)> m_cb;	// 選択されたアイテムのインデックスとアイテムを引数に設定する。何もない場合は-1が設定される。
	std::vector<String> m_vItem;
	String m_title;
	bool m_isActive = false;
	int32_t m_maxRow = 0;	// 表示可能最大行数

	std::shared_ptr<RectRenderer> m_rectRendererBorder;
	std::shared_ptr<RectRenderer> m_rectRendererInnerFrame;
	std::shared_ptr<SystemWidgetLabel> m_label;
	
	std::vector<std::shared_ptr<SystemWidgetLabelWithFont>> m_vLabel;
	int32_t m_cursor = -1;	// 選択しているアイテムの番号
	int32_t m_index = 0;	// アイテムの表示開始行

	const float textSize = 0.05f;
	const float textMargin = textSize * 1.5f;

	// 外枠
	const SizeF sizeBorderMargin{ 0.05f, 0.085f };
	RectF m_rectBorder{ sizeBorderMargin.x, sizeBorderMargin.y, 1.f - (sizeBorderMargin.x) * 2, 1.f - (sizeBorderMargin.y) * 2 };

	// 内枠
	const SizeF sizeInnerFrameMargin{ 0.04f, 0.04f };
	RectF m_rectInnerFrame{ m_rectBorder.x + sizeInnerFrameMargin.x, m_rectBorder.y + textMargin,
							m_rectBorder.width - (sizeInnerFrameMargin.x) * 2, m_rectBorder.height - (sizeInnerFrameMargin.y + textMargin) };

	std::shared_ptr<BitmapFont> MakeBitmapFont();
	std::shared_ptr<FontManager> m_font;

	void CalculateBorder();
};


