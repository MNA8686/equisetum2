#include "system/Exception.hpp"
#include "system/Logger.h"
#include "graphic/Animation.hpp"
#include "graphic/Sprite.hpp"
#include <algorithm>
#include <numeric>

namespace Equisetum2
{
	std::shared_ptr<AnimationTimeline> AnimationTimeline::Create()
	{
		return std::make_shared<AnimationTimeline>();
	}

	bool AnimationTimeline::AppendTimeline(const std::shared_ptr<Sprite>& sprite, const String & tag, int32_t ptr, int32_t delay)
	{
		if (!sprite)
		{
			return false;
		}

		stAnimationElement elem{};
		elem.m_sprite = sprite;
		elem.m_tagIndex = tag.empty() ? -1 : sprite->TagToInt(tag);
		elem.m_ptr = ptr;
		elem.m_timing = m_totalTime;
		elem.m_length = delay;

		m_vTimeline.emplace_back(elem);

		m_totalTime += delay;

		return true;
	}

	void AnimationTimeline::SetLoopType(AnimationLoopType type)
	{
		m_loop = type;
	}

	bool AnimationTimeline::MoveFrom(std::shared_ptr<AnimationTimeline>&& src)
	{
		m_vTimeline = std::move(src->m_vTimeline);
		m_loop = src->m_loop;
		m_totalTime = src->m_totalTime;

		return true;
	}

	int32_t AnimationTimeline::GetIndexByTime(int32_t time) const
	{
		if (time < 0 || 
			(m_loop == AnimationLoopType::none &&
			time >= m_totalTime))
		{
			return -1;
		}

		int32_t timeMod = time % m_totalTime;
		int32_t index = -1;
		auto findElem = std::find_if(m_vTimeline.begin(), m_vTimeline.end(), [&index, timeMod](const stAnimationElement& elem) {
			index++;

			// Œ»Ý‚Ç‚ÌˆÊ’u‚ðÄ¶’†‚©”»’è‚·‚é
			return timeMod >= elem.m_timing && timeMod < elem.m_timing + elem.m_length;
		});

		if (findElem == m_vTimeline.end())
		{
			return -1;
		}

		return index;
	}

	const stAnimationElement* AnimationTimeline::GetElement(int32_t index) const
	{
		if (index < 0 || index >= static_cast<int32_t>(m_vTimeline.size()))
		{
			return nullptr;
		}

		return &m_vTimeline[index];
	}

	int32_t AnimationTimeline::GetSize() const
	{
		return static_cast<int32_t>(m_vTimeline.size());
	}

	std::shared_ptr<Animation> Animation::Create()
	{
		return std::make_shared<Animation>();
	}

	void Animation::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String Animation::Identify() const
	{
		return m_id;
	}

	int32_t Animation::GetIndexByTime(int32_t tagIndex, int32_t time) const
	{
		if (tagIndex < 0 || tagIndex >= static_cast<int32_t>(m_vTimeline.size()))
		{
			return -1;
		}

		return m_vTimeline[tagIndex].timeline->GetIndexByTime(time);
	}

	const stAnimationElement * Animation::GetElement(int32_t tagIndex, int32_t index) const
	{
		if (tagIndex < 0 || tagIndex >= static_cast<int32_t>(m_vTimeline.size()))
		{
			return nullptr;
		}

		return m_vTimeline[tagIndex].timeline->GetElement(index);
	}

	int32_t Animation::TagToInt(const String & tag) const
	{
		int32_t tagIndex = -1;

		auto findTag = std::find_if(m_vTimeline.begin(), m_vTimeline.end(), [&tag, &tagIndex](const stTimelineWithTag& timeline) {
			tagIndex++;
			return timeline.tag == tag;
		});
		
		if (findTag == m_vTimeline.end())
		{
			return -1;
		}

		return tagIndex;
	}

	String Animation::IntToTag(int32_t tagIndex) const
	{
		if (tagIndex < 0 || tagIndex >= static_cast<int32_t>(m_vTimeline.size()))
		{
			return "???";
		}

		return m_vTimeline[tagIndex].tag;
	}

	void Animation::AppendAnimation(const String & tag, const std::shared_ptr<AnimationTimeline>& timeline)
	{
		m_vTimeline.push_back({ tag, timeline });
	}

	int32_t Animation::GetTimelineSize(int32_t tagIndex) const
	{
		if (tagIndex < 0 || tagIndex >= static_cast<int32_t>(m_vTimeline.size()))
		{
			return 0;
		}

		return m_vTimeline[tagIndex].timeline->GetSize();
	}

	int32_t Animation::GetTagSize() const
	{
		return static_cast<int32_t>(m_vTimeline.size());
	}
}
