#if !defined(_EQKEYBOARDINTERNAL_H_)
#define _EQKEYBOARDINTERNAL_H_

#include "system/Singleton.h"
#include "input/KeyCode.h"
#include "input/IKeyState.h"
#include <memory>

namespace Equisetum2
{
	class KeyboardInternal final
	{
	public:
		friend Singleton<KeyboardInternal>;	// シングルトンからインスタンスを作成してもらえるようにする

		void Prepare();
		void Update();
		const stKeyState& GetKeyState(KeyCode keyCode) const;
		const std::string GetNameFromKeyCode(KeyCode keyCode) const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		KeyboardInternal();	// インスタンス作成封じ
	};
}

#endif
