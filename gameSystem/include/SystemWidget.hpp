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
	virtual void SetPos(const PointF& pos);
	PointF GetPos() const;
	void SetEnable(bool enable);
	virtual Rect GetBox() const = 0;

	/*
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
	*/

protected:
	String m_text;
	Stat m_stat = Stat::Idle;
	bool m_focus = false;
	PointF m_pos;
	bool m_enable = true;

};

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
	int Render();
	// ラベルのサイズ
	Size GetBoxSize() const;
	// pivotを設定する
	void SetPivot(PointF pivot);
	// ポストエフェクトコールバックを設定する
	void SetPostEffect(const std::function<void(TextRenderer*)> cb);
	Rect GetBox() const override;

protected:
	std::shared_ptr<TextRenderer>& GetRenderer();

private:
	//virtual void RenderPostEffect(SystemWidget* pWidget) {}
	std::shared_ptr<TextRenderer> m_renderer;
	std::function<void(TextRenderer*)> m_cb;

	// SystemWidget を介して継承されました
	virtual int Do() override;
};

class SystemWidgetMenu : public SystemWidget
{
public:
	SystemWidgetMenu () = default;
	virtual ~SystemWidgetMenu() = default;

	int SetWidget(std::shared_ptr<SystemWidget> pWidget);
	virtual int Do();
	virtual int Render();

	static std::shared_ptr<SystemWidgetMenu> Create(const String& label);
	Rect GetBox() const override;

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	int m_focus = 0;
	int m_cursolMoveCounter = 0;
	std::shared_ptr<RectRenderer> m_rectRenderer;
	Rect m_cursorNow;
	Rect m_cursorSrc;
	Rect m_cursorDest;
};

class SystemWidgetEnterView : public SystemWidget
{
public:
	SystemWidgetEnterView() = default;
	~SystemWidgetEnterView() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetEnterView> Create(const String& label);

private:
	std::shared_ptr<SystemWidgetLabel> m_label;

	// SystemWidget を介して継承されました
	virtual Rect GetBox() const override;
};

class SystemWidgetReturnView : public SystemWidget
{
public:
	SystemWidgetReturnView() = default;
	~SystemWidgetReturnView() = default;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemWidgetReturnView> Create(const String& label);
	Rect GetBox() const override;

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
};

class SystemWidgetCustom : public SystemWidget
{
public:
	SystemWidgetCustom() = default;
	~SystemWidgetCustom() = default;
	int Do() override;
	int Render() override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;
	
	static std::shared_ptr<SystemWidgetCustom> Create(const String& label, const std::function<bool (void)> cb);

private:
	std::function<bool(void)> m_cb;
	bool m_exclusive = false;
	std::shared_ptr<SystemWidgetLabel> m_label;
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
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;

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

	std::shared_ptr<SystemWidgetLabel> m_label;
};

class SystemWidgetChoice : public SystemWidget
{
};

#endif