
#include "ScriptMapper.hpp"

void ScriptMapper::Register(const Value& value)
{
	m_vValue.push_back(value);
}

int32_t ScriptMapper::ToIndex(const String& id) const
{
	int32_t index = 0;

	for (auto& val : m_vValue)
	{
		if (id == val.id)
		{
			return index;
		}

		index++;
	}

	return -1;
}

const ScriptMapper::Value* ScriptMapper::Get(int32_t index) const
{
	if (index >= 0 && index < static_cast<int32_t>(m_vValue.size()))
	{
		return &m_vValue[index];
	}

	return nullptr;
}

