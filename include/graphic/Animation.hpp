#pragma once

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Texture.hpp"
#include <memory>
//#include <unordered_map>

namespace Equisetum2
{
	class Sprite;

	/// アニメーションのループ設定
	enum class AnimationLoopType : int32_t
	{
		none,	/// 無し
		loop,	/// 片方向
		pingPong	/// 往復
	};

	// アニメーションタイムライン設定
	typedef struct
	{
	public:
		std::shared_ptr<Sprite> m_sprite;
		int32_t m_tagIndex;
		int32_t m_ptr;
		int32_t m_timing;
		int32_t m_length;
	}stAnimationElement;

	class AnimationTimeline
	{
	public:
		typedef struct
		{
			int32_t step = 1;
			int32_t offset = 1;
		}stRotate;

		AnimationTimeline() = default;
		virtual ~AnimationTimeline() = default;

		static std::shared_ptr<AnimationTimeline> Create();
		void Begin();
		bool AppendTimeline(const std::shared_ptr<Sprite>& sprite, const String& tag, int32_t ptr, int32_t delay);
		void End();
		void SetLoopType(AnimationLoopType type);
		void SetRotate(stRotate rotate);
		stRotate GetRotate(void) const;
		int32_t GetTotalTime(void) const;
		AnimationLoopType GetLoopType(void) const;

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		virtual bool MoveFrom(std::shared_ptr<AnimationTimeline>&& src);

		int32_t GetIndexByTime(int32_t time) const;
		const stAnimationElement* GetElement(int32_t index) const;
		int32_t GetSize() const;

	private:

		std::vector<stAnimationElement> m_vTimeline;
		AnimationLoopType m_loop = AnimationLoopType::none;
		int32_t m_totalTime = 0;

		stRotate m_rotate;
		bool m_isAppending = false;
	};

	class Animation
	{
	public:

		Animation() = default;
		virtual ~Animation() = default;

		static std::shared_ptr<Animation> Create();

		/**
		* @brief このオブジェクトのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief このオブジェクトのIDを取得する
		* @return このオブジェクトのID
		*/
		virtual String Identify() const;

		int32_t GetIndexByTime(int32_t tagIndex, int32_t time) const;
		const stAnimationElement* GetElement(int32_t tagIndex, int32_t index) const;
		int32_t TagToInt(const String& tag) const;
		String IntToTag(int32_t tagIndex) const;
		void AppendAnimation(const String& tag, const std::shared_ptr<AnimationTimeline>& timeline);
		int32_t GetTimelineSize(int32_t tagIndex) const;
		int32_t GetTotalTime(int32_t tagIndex) const;
		int32_t GetTagSize() const;	/// タグの種類を返す
		int32_t GetRotateOffset(int32_t tagIndex, int32_t degree) const;
		
	private:

		typedef struct
		{
			String tag;
			std::shared_ptr<AnimationTimeline> timeline;
		}stTimelineWithTag;

		std::vector<stTimelineWithTag> m_vTimeline;
		String m_id;		/// このオブジェクトのID
	};
}

