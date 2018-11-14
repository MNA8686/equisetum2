#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemChoiceDialog;
class SystemWidgetChoice : public SystemWidget
{
public:
	static std::shared_ptr<SystemWidgetChoice> Create(const String& label, const std::function<std::vector<String>()>& cbBegin, const std::function<void(int32_t, const String&)>& cbEnd);

	SystemWidgetChoice() = default;
	~SystemWidgetChoice() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;
	int32_t GetChooseIndex() const;

private:
	std::shared_ptr<SystemWidgetLabel> m_label;		// ラベル表示ウィジェット
	std::vector<String> m_vItem;					// ダイアログに設定するアイテム
	std::function<std::vector<String>()> m_cbBegin;		// ダイアログ表示開始時に呼び出されるコールバック
	std::function<void(int32_t, const String&)> m_cbEnd;		// 選択完了時に呼び出されるコールバック
	bool m_exclusive = false;	// ダイアログ表示中かどうか
	std::shared_ptr<SystemChoiceDialog> m_dialog;	// ダイアログオブジェクト
	int32_t m_chooseIndex = -1;		// 選択した項目
};

