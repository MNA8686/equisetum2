#if !defined(_EQSYSTEMWIDGET_H_)
#define _EQSYSTEMWIDGET_H_

#include "Equisetum2.h"
using namespace Equisetum2;


class SystemWidget
{
public:
	enum class Stat : int32_t
	{
		Idle,
		Next,
		Prev,
		Exclusive,
	};

	SystemWidget();
	virtual ~SystemWidget();
	virtual int Do() = 0;
	virtual int Render() = 0;
	void Prev();
	void Next();
	void Prepare();
	void SetFocus(bool focus);
	bool GetFocus() const;
	Stat GetStat() const;
	void SetPos(const PointF& pos);
	PointF GetPos() const;
	void SetEnable(bool enable);
	Rect GetBox() const;

	class Label
	{
	public:
		Label();
		~Label() = default;

		// 使用する文字一覧をセットする
		bool SetPreset(const String& preset);
		// テキストを設定する
		bool SetText(const String& label);
		// ラベルのレンダリング
		bool Render(SystemWidget* pWidget);
		// ラベルのサイズ
		Size GetBoxSize() const;

	protected:
		std::shared_ptr<TextRenderer>& GetRenderer();
	
	private:
		virtual void RenderPostEffect(SystemWidget* pWidget) {}
		std::shared_ptr<TextRenderer> m_renderer;
	};

protected:
	String m_text;
	Stat m_stat = Stat::Idle;
	bool m_focus = false;
	PointF m_pos;
	bool m_enable = true;

	std::shared_ptr<Label> m_label;
};

class SystemWidgetEnterView : public SystemWidget
{
public:
	SystemWidgetEnterView() = default;
	~SystemWidgetEnterView() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetEnterView> Create(const String& label);
};

class SystemWidgetReturnView : public SystemWidget
{
public:
	SystemWidgetReturnView() = default;
	~SystemWidgetReturnView() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetReturnView> Create(const String& label);
};

class SystemWidgetCustom : public SystemWidget
{
public:
	SystemWidgetCustom() = default;
	~SystemWidgetCustom() = default;
	int Do() override;
	int Render() override;
	
	static std::shared_ptr<SystemWidgetCustom> Create(const String& label, const std::function<bool (void)> cb);

private:
	std::function<bool(void)> m_cb;
	bool m_exclusive = false;
	std::shared_ptr<RectRenderer> m_rectRenderer;
};

class SystemWidgetSpin : public SystemWidget
{
public:
	SystemWidgetSpin() = default;
	~SystemWidgetSpin() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetSpin> Create(const String& label, const std::function<void (int32_t)> cb);

	void SetRange(int32_t min, int32_t max, int32_t step = 1);
	void SetValue(int32_t val);
	int32_t GetValue() const;
	void SetCyclic(bool val);

	class LabelEx : public Label
	{
	public:
		LabelEx() = default;
		~LabelEx() = default;

	private:
		virtual void RenderPostEffect(SystemWidget* pWidget) override;
	};

private:
	std::function<void(int32_t)> m_cb;
	int32_t m_min = 0;
	int32_t m_max = 100;
	int32_t m_step = 1;
	int32_t m_val = 0;
	int32_t m_direction = 0;
	bool m_cyclic = false;

	const String MakeString();

	const String leftArrow = u8"◀";
	const String rightArrow = u8"▶";
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif