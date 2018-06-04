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
		friend Singleton<WindowCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		void Prepare();
		void Update();

		void SetTitle(const String& title);
		void SetSize(int width, int height);
		void SetStyle(WindowStyle style);
		void SetPos(int x, int y);
		void Centering();
		void ShowAlert(const String& msg);
		void SetFullscreen(bool fullScreen);

		const String Title();
		Equisetum2::Size Size();
		Point Pos();
		WindowStyle Style();

		friend class Renderer;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		WindowCompat();	// インスタンス作成封じ
	};
}

#endif
