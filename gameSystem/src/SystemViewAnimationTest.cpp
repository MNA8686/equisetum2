#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewAnimationTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<SystemViewAnimationTest> SystemViewAnimationTest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewAnimationTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

void SystemViewAnimationTest::LoadAnimation()
{
	if (m_item.empty())
	{
		return;
	}

	// 予め解放しておかないとロード済みアセットのコピーが生成されてしまうので潰す
	m_spriteRenderer = nullptr;
	m_animation = nullptr;

	// コントロールを初期化
	m_animationTagIndex = -1;
	m_animationPtr = 0;
	m_spinTag->SetRange(-1, -1, 1);	
	m_spinTag->SetValue(-1);

	// アニメーション作成
	std::shared_ptr<Animation> animation = Singleton<AssetManager>::GetInstance()->Load<Animation>(m_item);
	if (animation)
	{
		m_animation = animation;

		// スプライト関連設定
		const stAnimationElement* elem = animation->GetElement(0, 0);
		if (elem)
		{
			// スプライトレンダラ作成
			m_spriteRenderer = GetApplication()->GetRenderer()->CreateRenderObject<SpriteRenderer>();
			if (m_spriteRenderer)
			{
				m_spriteRenderer->SetSprite(elem->m_sprite);

				m_spriteRenderer->SetBlendMode(BlendMode::Blend);
				m_spritePos = Window::Size() / 2;
				m_spriteRenderer->SetPos(m_spritePos);
			}
		}

		// タグ選択スピン設定
		// アニメーションパターンが無い場合は-1～-1を設定範囲とし、-1はempty扱いとする。
		size_t tagsSize = animation->GetTagSize();
		int32_t minIndex = tagsSize <= 0 ? -1 : 0;
		int32_t maxIndex = tagsSize > 0 ? tagsSize - 1 : -1;
		m_animationTagIndex = minIndex;
		m_spinTag->SetRange(minIndex, maxIndex, 1);	
		m_spinTag->SetValue(minIndex);

		// アニメーションパターン数設定
		int32_t animSize = animation->GetTimelineSize(0);
		m_spinAnim->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);
		m_animationPtr = 0;
	}
}

int SystemViewAnimationTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// アニメーションリストを取得する
		auto spriteList = Singleton<AssetManager>::GetInstance()->GetIdList("animation");
		for (auto sprite : spriteList)
		{
			if (Path::GetExtension(sprite) == u8".json")
			{
				test.push_back(Path::ChangeExtension(sprite, ""));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		// リスト選択時に呼び出される
		if (index >= 0)
		{
			m_item = item;
			LoadAnimation();
		}
	});
	menu->SetWidget(choice);

	auto reload = SystemWidgetCustom::Create(u8"リロード", [this]()->bool {
		LoadAnimation();
		return false;	// 排他制御しない
	});
	menu->SetWidget(reload);

	auto rate = SystemWidgetSpin::Create(u8"拡大率", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
	rate->SetFormatCallBack([](int32_t val)->String {
		return String::Sprintf("%d%%", val);
	});
	menu->SetWidget(rate);

	auto tag = SystemWidgetSpin::Create(u8"タグ", [this](int32_t val) {
		// タグ切替時のコールバック
		m_animationTagIndex = val;
		m_animationPtr = 0;	// タグを切り替えたら表示アニメーションパターンを0に戻す

		// アニメーションサイズなどを設定
		int32_t animSize = m_animation->GetTimelineSize(m_animationTagIndex);
		m_spinAnim->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);
		m_spinAnim->SetValue(m_animationPtr);
	});
	tag->SetCyclic(true);
	tag->SetEnable(true);
	tag->SetRange(-1, -1, 1);
	tag->SetValue(-1);
	tag->SetFormatCallBack([this](int32_t val)->String {
		if (val < 0)
		{
			return "empty";
		}

		return m_animation->IntToTag(val);
	});
	m_spinTag = tag;
	menu->SetWidget(tag);

	auto ptr = SystemWidgetSpin::Create(u8"アニメーションパターン", [this](int32_t val) {
		m_animationPtr = val;
	});
	m_spinAnim = ptr;
	ptr->SetCyclic(true);
	ptr->SetEnable(false);
	ptr->SetFormatCallBack([this](int32_t val)->String {
		return String::Sprintf("%d / %d", val, m_spinAnim->GetMax() + 1);
	});
	menu->SetWidget(ptr);

	auto play = SystemWidgetCustom::Create(u8"再生/停止", [this]()->bool {
		if (!m_playing)
		{
			m_playing = true;
			m_frame = 0;
			m_animationPtr = 0;
		}
		else
		{
			m_playing = false;
		}
		return false;	// 排他制御しない
	});
	menu->SetWidget(play);

	auto r = SystemWidgetSpin::Create(u8"色成分 R", [this](int32_t val) {
		m_color.rgba8888.r = static_cast<uint8_t>(val + 128);
	});
	r->SetRange(-128, 127, 1);
	r->SetValue(0);
	menu->SetWidget(r);

	auto g = SystemWidgetSpin::Create(u8"色成分 G", [this](int32_t val) {
		m_color.rgba8888.g = static_cast<uint8_t>(val + 128);
	});
	g->SetRange(-128, 127, 1);
	g->SetValue(0);
	menu->SetWidget(g);

	auto b = SystemWidgetSpin::Create(u8"色成分 B", [this](int32_t val) {
		m_color.rgba8888.b = static_cast<uint8_t>(val + 128);
	});
	b->SetRange(-128, 127, 1);
	b->SetValue(0);
	menu->SetWidget(b);

	auto a = SystemWidgetSpin::Create(u8"色成分 A", [this](int32_t val) {
		m_color.rgba8888.a = static_cast<uint8_t>(val + 128);
	});
	a->SetRange(-128, 127, 1);
	a->SetValue(0);
	menu->SetWidget(a);

	auto move = SystemWidgetCustom::Create(u8"移動", [this]()->bool {
		int32_t amount = KB::KeyTab.IsPress() ? 4 : 1;

		if (KB::KeyLeft.IsPress())
		{
			m_spritePos.x -= amount;
		}
		else if (KB::KeyRight.IsPress())
		{
			m_spritePos.x += amount;
		}

		if (KB::KeyUp.IsPress())
		{
			m_spritePos.y -= amount;
		}
		else if (KB::KeyDown.IsPress())
		{
			m_spritePos.y += amount;
		}

		return !KB::KeyEscape.IsDown();
	});
	menu->SetWidget(move);

	menu->SetFocus(true);

	return 0;
}

int SystemViewAnimationTest::Do()
{
	if (m_playing)
	{
		// 現在のフレームに対応するアニメーションパターンを取得
		auto index = m_animation->GetIndexByTime(m_animationTagIndex, m_frame * 1000);
		if (index >= 0)
		{
			// アニメーションパターンをセット
			m_animationPtr = index;
			// UIにも反映する
			m_spinAnim->SetValue(index);
		}
		else
		{
			m_playing = false;
		}

		// 1フレーム進める
		m_frame++;
	}

	return 0;
}

int SystemViewAnimationTest::Render()
{
	if (m_spriteRenderer)
	{
		// 現在表示すべきアニメーションパターンを取得する
		const stAnimationElement* elem = m_animation->GetElement(m_animationTagIndex, m_animationPtr);
		if (elem)
		{
			m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
			m_spriteRenderer->SetSprite(elem->m_sprite);
			m_spriteRenderer->SetAtlasNum(elem->m_sprite->ToAtlasNumWithTagIndex(elem->m_tagIndex, elem->m_ptr));
			m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);
			m_spriteRenderer->SetColor(m_color);

			GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());
		}
	}

	return 0;
}

