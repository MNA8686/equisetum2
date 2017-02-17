#if !defined(_EQMOUSECOMPAT_H_)
#define _EQMOUSECOMPAT_H_

#include "system/Singleton.h"
#include "input/IKeyState.h"
#include "input/Mouse.h"
#include <memory>

namespace Equisetum2
{
	class MouseCompat final
	{
	public:
		friend Singleton<MouseCompat>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		void Prepare();
		void Update();

		const Point<int32_t>& Pos() const;
		const Point<int32_t>& Delta() const;
		int32_t Wheel() const;
		int32_t WheelH() const;
		const stKeyState& Button(MouseButton button) const;

		bool ShowCursor(bool toggle);

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		MouseCompat();	// �C���X�^���X�쐬����
	};
}

#endif
