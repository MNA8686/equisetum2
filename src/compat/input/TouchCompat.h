#if !defined(_EQTOUCHCOMPAT_H_)
#define _EQTOUCHCOMPAT_H_

#include "system/Singleton.h"
#include "input/IKeyState.h"
#include "input/Touch.h"
#include <memory>

namespace Equisetum2
{
	class TouchCompat final
	{
	public:
		friend Singleton<TouchCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		void Prepare();
		void Update();

		const Point<float>& Pos(int fingerIndex) const;
		const Point<float>& Delta(int fingerIndex) const;
		const stKeyState& Button(int fingerIndex) const;
		int64_t FingerId(int fingerIndex) const;

		int NumSupportedFinger() const;		

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		TouchCompat();	// インスタンス作成封じ
	};
}

#endif
