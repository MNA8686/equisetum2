#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidget.hpp"

class SystemWidgetLabelWithFont : public SystemWidget
{
public:
	static std::shared_ptr<SystemWidgetLabelWithFont> CreateWithFont(const String& label, const std::shared_ptr<FontManager> fontManager);
	static std::shared_ptr<SystemWidgetLabelWithFont> CreateWithBitmapFont(const String& label, std::shared_ptr<BitmapFont> bitmapFont);

	SystemWidgetLabelWithFont();
	~SystemWidgetLabelWithFont() = default;

	// 使用する文字一覧をセットする
	bool SetPresetWithFont(const String& preset, const std::shared_ptr<FontManager> fontManager);
	// ビットマップフォントをセットする
	bool SetBitmapFont(std::shared_ptr<BitmapFont> bitmapFont);
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
	String m_preset;
};

