#if !defined(_EQWINDOWCOMPAT_H_)
#define _EQWINDOWCOMPAT_H_

#include "system/Singleton.h"
#include "graphic/Window.h"
//#include <cstdint>
//#include <string>

namespace Equisetum2
{
	class WindowCompat final
	{
	public:
		friend Singleton<WindowCompat>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		void Prepare();
		void Update();

		void SetTitle(const String& title);
		void SetSize(int width, int height);
		void SetStyle(WindowStyle style);
		void SetPos(int x, int y);
		void Centering();
		void ShowAlert(const String& msg);

		const String Title();
		Equisetum2::Size Size();
		Point<int> Pos();
		WindowStyle Style();

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		WindowCompat();	// �C���X�^���X�쐬����
	};
}

#endif
