#if !defined(_EQWINDOW_H_)
#define _EQWINDOW_H_

#include "type/Point.h"
#include <cstdint>
#include <string>

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
		static void SetTitle(const std::string& title);
		static void SetSize(int width, int height);
		static void SetStyle(WindowStyle style);
		static void SetPos(int x, int y);
		static void Centering();

		static const std::string Title();
		static Size Size();
		static Point<int> Pos();
		static WindowStyle Style();
	};
}

#endif
