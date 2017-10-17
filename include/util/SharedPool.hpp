#if !defined(_EQSHAREDPOOL_H_)
#define _EQSHAREDPOOL_H_

#include "system/Singleton.h"
#include <memory>
#include <list>
#include <algorithm>

namespace Equisetum2
{
	/**
	* リソース共有クラス<br>
	* 同じ名前のリソースをロード済みの場合、そのリソースへのshared_ptrを返す。<br>
	* シャローコピーなのでリソースに対して変更を加えると、参照先全てに影響があるので注意。<br>
	* <br>
	* インスタンスは以下のように呼び出す。<br>
	* Singleton<SharedPool<T>>::GetInstance()->Register(ptr);
	*/
	template<typename T>
	class SharedPool final
	{
	public:
		friend Singleton<SharedPool<T>>;	// シングルトンからインスタンスを作成してもらえるようにする

		typedef struct
		{
			String m_id;	/// ID比較用
			std::weak_ptr<T> m_weakPtr;		/// リソースへの参照
			bool m_shared = false;
		}Info;

		/**
		* @brief リソースを登録する
		* @param id リソースのID
		* @param ptr リソースのshared_ptr
		* @return 成否
		*
		* @detail 共有時、すでに同名のIDが存在する場合やIDが設定されていない場合、このメソッドは失敗する。<br>
		* @detail 共有ではない時、必ず登録が行われる。
		*/
		bool Register(const String& id, std::shared_ptr<T> ptr, bool shared=true)
		{
			if (!ptr || id.empty())
			{
				return false;
			}

			// 未使用領域削除
			m_pool.remove_if([](Info& info)->bool {
				return info.m_weakPtr.expired();
			});

			// 新規リソース情報作成
			Info info;
			info.m_weakPtr = ptr;
			info.m_id = id;
			info.m_shared = shared;

			if (shared)
			{
				// すでに同名のIDが無いか調べる
				auto result = std::find_if(m_pool.begin(), m_pool.end(), [&id](Info& info)->bool {
					return info.m_shared && (id == info.m_id);
				});
				if (result != m_pool.end())
				{
					return false;
				}
			}

			m_pool.push_back(info);

			return true;
		}

		/**
		* @brief IDを元にすでにロードされたリソースを取得する(shared属性のもののみ)
		* @param id リソースのID
		* @return 成功時 ロード済みのリソースのshared_ptr、失敗時 nullptr
		*/
		std::shared_ptr<T> GetByIdentify(const String& id)
		{
			std::shared_ptr<T> p;

			auto result = std::find_if(m_pool.begin(), m_pool.end(), [&id](Info& info)->bool {
				return (!info.m_weakPtr.expired() &&
						info.m_shared && 
						info.m_id == id);
			});
			if (result != m_pool.end())
			{
				p = result->m_weakPtr.lock();
			}

			return p;
		}

		/**
		* @brief ロード済みリソースのリストをクリアする
		*/
		void Clear()
		{
			m_pool.clear();
		}

		/**
		* @brief ロード済みリソースのリストを返す
		* @return ロード済みのリソースのリスト
		*/
		const std::list<Info>& GetPool() const
		{
			return m_pool;
		}

	private:
		std::list<Info> m_pool;			/// 共有リソース以外(リソースのリロードの為に共有リソースではないものもリストを保持しておく)

		SharedPool() = default;
		~SharedPool() = default;
		SharedPool(const SharedPool&) = delete;				// コピーコンストラクタ封じ
		SharedPool& operator= (const SharedPool&) = delete;	// コピーコンストラクタ封じ
		SharedPool& operator= (const SharedPool&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};
}

#endif
