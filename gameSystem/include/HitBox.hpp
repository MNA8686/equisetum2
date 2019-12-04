#pragma once

#include "type/Rect.hpp"
#include "type/FixedDec.hpp"
#include "EqVector.hpp"

class HitBox
{
public:
	using iterator = Rect_t<FixedDec>*;
	using const_iterator = const Rect_t<FixedDec>*;

	HitBox() = default;
	~HitBox() = default;

	// 矩形を追加する
	void Push(const Rect_t<FixedDec>& rect);
	// ここまで追加した矩形を一纏めにする
	bool Packaging();

	// どの当たり判定を対象とするかを設定する
	bool SetCurrectRects(int32_t current);

	// 矩形イテレータ
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

private:

	// 現在対象としている当たり判定
	int32_t m_current = -1;

	// 登録されている矩形
	EqVector<Rect_t<FixedDec>> m_vRect;

	// 当たり判定管理構造体
	struct Manager
	{
		int32_t begin = 0;
		int32_t size = 0;
	};
	EqVector<Manager> m_vManager;
};

