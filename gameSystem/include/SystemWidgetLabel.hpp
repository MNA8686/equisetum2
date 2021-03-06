﻿#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidget.hpp"

// システムフォントを使用したラベル
// 画面サイズの変更に追従する
class SystemWidgetLabel : public SystemWidget
{
public:
	static std::shared_ptr<SystemWidgetLabel> Create(const String& label);

	SystemWidgetLabel();
	~SystemWidgetLabel() = default;

	// 使用する文字一覧をセットする
	bool SetPreset(const String& preset);
	// テキストを設定する
	bool SetText(const String& label);
	// ラベルのレンダリング
	int Render(const SystemView* pView) override;
	// ラベルのサイズ
	Size GetBoxSize() const;
	// pivotを設定する
	void SetPivot(PointF pivot);
	// ポストエフェクトコールバックを設定する
	void SetPostEffect(const std::function<void(TextRenderer*)> cb);
	Rect GetBox() const override;
	int Do(SystemView* pView) override;

protected:
	std::shared_ptr<TextRenderer>& GetRenderer();

private:
	std::shared_ptr<TextRenderer> m_renderer;
	std::function<void(TextRenderer*)> m_cb;
	std::shared_ptr<FontManager> m_font;
	String m_preset;
};

