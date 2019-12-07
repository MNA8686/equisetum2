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

class HitBoxIterator
{
public:
	class iterator
	{
	private:
		Point_t<FixedDec> m_pos;		// オブジェクトの座標
		HitBox::const_iterator m_it;
		Rect_t<FixedDec> m_rect;	// ワールド座標に変換された当たり判定

	public:
		iterator(const Point_t<FixedDec>& pos, const HitBox::const_iterator& it);
		void calc();
		iterator& operator++();
		iterator operator++(int);
		const Rect_t<FixedDec>& operator*();
		const Rect_t<FixedDec>* operator->();
		bool operator== (const iterator& r) const;
		bool operator!= (const iterator& r) const;
	};

	HitBoxIterator(const Point_t<FixedDec>& pos, const HitBox* pHitBox);
	iterator begin() const;
	iterator end() const;
	bool HitTest(const Rect_t<FixedDec>& rect) const;

private:
	Point_t<FixedDec> m_pos;
	const HitBox* m_pHitBox = nullptr;
};

