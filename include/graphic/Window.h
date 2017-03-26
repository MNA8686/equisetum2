#if !defined(_EQWINDOW_H_)
#define _EQWINDOW_H_

#include "type/Point.h"
#include "type/String.h"
#include <cstdint>

namespace Equisetum2
{
	enum class WindowStyle
	{
		// �T�C�Y���Œ�̃E�B���h�E
		Fixed,
		// �T�C�Y���ύX�\�ȃE�B���h�E
		Sizeable,
		// �t���[���������E�B���h�E
//		NonFrame,
	};

	class Window
	{
	public:
		static void SetTitle(const String& title);
		static void SetSize(int width, int height);
		static void SetStyle(WindowStyle style);
		static void SetPos(int x, int y);
		static void Centering();
		static void ShowAlert(const String& msg);

		static const String Title();
		static Size Size();
		static Point<int> Pos();
		static WindowStyle Style();
	};
}

#endif
