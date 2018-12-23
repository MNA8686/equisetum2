#if !defined(_EQWINDOW_H_)
#define _EQWINDOW_H_

#include "type/Point.h"
#include "type/String.h"
#include <cstdint>

namespace Equisetum2
{
	enum class WindowStyle
	{
		// サイズが固定のウィンドウ
		Fixed,
		// サイズが変更可能なウィンドウ
		Sizeable,
		// フレームが無いウィンドウ
//		NonFrame,
	};

	class Window
	{
	public:
		static void SetTitle(const String& title);
		static void SetMinimumSize(int width, int height);
		static void SetSize(int width, int height);
		static void SetStyle(WindowStyle style);
		static void SetPos(int x, int y);
		static void Centering();
		static void ShowAlert(const String& msg);
		static void SetFullscreen(bool fullScreen);

		static const String Title();
		static Size Size();
		static Point Pos();
		static WindowStyle Style();
	};
}

#endif
