#if !defined(_EQKEYBOARDCOMPAT_H_)
#define _EQKEYBOARDCOMPAT_H_

#include "system/Singleton.h"
#include "input/KeyCode.h"
#include "input/IKeyState.h"
#include <memory>

namespace Equisetum2
{
	class KeyboardCompat final
	{
	public:
		friend Singleton<KeyboardCompat>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		void Prepare();
		void Update();
		const stKeyState& GetKeyState(KeyCode keyCode) const;
		const std::string GetNameFromKeyCode(KeyCode keyCode) const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		KeyboardCompat();	// �C���X�^���X�쐬����
	};
}

#endif
