#if !defined(_EQKEY_H_)
#define _EQKEY_H_

#include <vector>
#include <functional>
#include "type/String.h"
#include "input/IKeyState.h"
#include "input/KeyCode.h"

namespace Equisetum2
{
	class Key : public IKeyState
	{
	private:

		std::function<stKeyState()> m_Listener;

	public:

		virtual const String Name() const override;
		virtual bool IsDown() const override;
		virtual bool IsUp() const override;
		virtual bool IsPress() const override;
		virtual uint32_t TimeStamp() const override;
		virtual uint32_t PressedDuration() const override;

		Key();
		Key(const std::function<stKeyState()>& listener);

		// TODO 同一キー判定
	};

	class CombinationKey : public IKeyState
	{
	private:

		std::vector<Key> m_vKey;
		void RegisterTimeStamp();

	public:

		virtual const String Name() const override;
		virtual bool IsDown() const override;
		virtual bool IsUp() const override;
		virtual bool IsPress() const override;
		virtual uint32_t TimeStamp() const override;
		virtual uint32_t PressedDuration() const override;

		template <class Type, class ... Args>
		void Append(const Type& key, const Args& ... args)
		{
			Append(key);
			Append(args...);
		}

		template <class ... Args>
		explicit CombinationKey(const Args& ... args)
		{
			RegisterTimeStamp();

			Append(args...);
		}
		
		~CombinationKey();

		void Append(const Key& key);
		void Append(const CombinationKey& combKey);

		template <class Type>
		CombinationKey& operator |= (const Type& key)
		{
			Append(key);
			return *this;
		}
	};

	inline CombinationKey operator | (const Key& key1, const Key& key2)
	{
		return CombinationKey(key1, key2);
	}

	inline CombinationKey operator | (const Key& key1, const CombinationKey& key2)
	{
		return CombinationKey(key1, key2);
	}

	inline CombinationKey operator | (const CombinationKey& key1, Key key2)
	{
		return CombinationKey(key1, key2);
	}

	inline CombinationKey operator | (const CombinationKey& key1, const CombinationKey& key2)
	{
		return CombinationKey(key1, key2);
	}

	namespace KB
	{
		const std::function<stKeyState()> CreateKeyboardFunc(KeyCode keyCode);

		const Key KeyA{ CreateKeyboardFunc(KeyCode::KEY_a) };
		const Key KeyB{ CreateKeyboardFunc(KeyCode::KEY_b) };
		const Key KeyC{ CreateKeyboardFunc(KeyCode::KEY_c) };
		const Key KeyD{ CreateKeyboardFunc(KeyCode::KEY_d) };
		const Key KeyE{ CreateKeyboardFunc(KeyCode::KEY_e) };
		const Key KeyF{ CreateKeyboardFunc(KeyCode::KEY_f) };
		const Key KeyG{ CreateKeyboardFunc(KeyCode::KEY_g) };
		const Key KeyH{ CreateKeyboardFunc(KeyCode::KEY_h) };
		const Key KeyI{ CreateKeyboardFunc(KeyCode::KEY_i) };
		const Key KeyJ{ CreateKeyboardFunc(KeyCode::KEY_j) };
		const Key KeyK{ CreateKeyboardFunc(KeyCode::KEY_k) };
		const Key KeyL{ CreateKeyboardFunc(KeyCode::KEY_l) };
		const Key KeyM{ CreateKeyboardFunc(KeyCode::KEY_m) };
		const Key KeyN{ CreateKeyboardFunc(KeyCode::KEY_n) };
		const Key KeyO{ CreateKeyboardFunc(KeyCode::KEY_o) };
		const Key KeyP{ CreateKeyboardFunc(KeyCode::KEY_p) };
		const Key KeyQ{ CreateKeyboardFunc(KeyCode::KEY_q) };
		const Key KeyR{ CreateKeyboardFunc(KeyCode::KEY_r) };
		const Key KeyS{ CreateKeyboardFunc(KeyCode::KEY_s) };
		const Key KeyT{ CreateKeyboardFunc(KeyCode::KEY_t) };
		const Key KeyU{ CreateKeyboardFunc(KeyCode::KEY_u) };
		const Key KeyV{ CreateKeyboardFunc(KeyCode::KEY_v) };
		const Key KeyW{ CreateKeyboardFunc(KeyCode::KEY_w) };
		const Key KeyX{ CreateKeyboardFunc(KeyCode::KEY_x) };
		const Key KeyY{ CreateKeyboardFunc(KeyCode::KEY_y) };
		const Key KeyZ{ CreateKeyboardFunc(KeyCode::KEY_z) };

		const Key Key0{ CreateKeyboardFunc(KeyCode::KEY_0) };
		const Key Key1{ CreateKeyboardFunc(KeyCode::KEY_1) };
		const Key Key2{ CreateKeyboardFunc(KeyCode::KEY_2) };
		const Key Key3{ CreateKeyboardFunc(KeyCode::KEY_3) };
		const Key Key4{ CreateKeyboardFunc(KeyCode::KEY_4) };
		const Key Key5{ CreateKeyboardFunc(KeyCode::KEY_5) };
		const Key Key6{ CreateKeyboardFunc(KeyCode::KEY_6) };
		const Key Key7{ CreateKeyboardFunc(KeyCode::KEY_7) };
		const Key Key8{ CreateKeyboardFunc(KeyCode::KEY_8) };
		const Key Key9{ CreateKeyboardFunc(KeyCode::KEY_9) };

		const Key KeyRight{ CreateKeyboardFunc(KeyCode::KEY_RIGHT) };
		const Key KeyLeft{ CreateKeyboardFunc(KeyCode::KEY_LEFT) };
		const Key KeyDown{ CreateKeyboardFunc(KeyCode::KEY_DOWN) };
		const Key KeyUp{ CreateKeyboardFunc(KeyCode::KEY_UP) };

		const Key KeyTab{ CreateKeyboardFunc(KeyCode::KEY_TAB) };
		const Key KeyEnter{ CreateKeyboardFunc(KeyCode::KEY_RETURN) };
		const Key KeyEscape{ CreateKeyboardFunc(KeyCode::KEY_ESCAPE) };
		const Key KeyPageup{ CreateKeyboardFunc(KeyCode::KEY_PAGEUP) };
		const Key KeyPagedown{ CreateKeyboardFunc(KeyCode::KEY_PAGEDOWN) };
		const Key KeyHome{ CreateKeyboardFunc(KeyCode::KEY_HOME) };
		const Key KeyEnd{ CreateKeyboardFunc(KeyCode::KEY_END) };
	};
}

#endif
