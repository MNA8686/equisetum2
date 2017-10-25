#if !defined(_EQSERIALIZATION_H_)
#define _EQSERIALIZATION_H_

#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/bitset.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/common.hpp"
#include "cereal/types/complex.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/forward_list.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/queue.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/stack.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/unordered_set.hpp"
#include "cereal/types/utility.hpp"
#include "cereal/types/valarray.hpp"
#include "cereal/types/vector.hpp"

//#include "system/Logger.h"
//#include "system/Exception.hpp"
#include "stream/IStream.h"
//#include "graphic/Image.hpp"
//#include "util/SharedPool.hpp"
//#include "type/Point.h"
//#include "type/Rect.hpp"

namespace Equisetum2
{
	class Serializer : public cereal::OutputArchive<Serializer, cereal::AllowEmptyClassElision>
	{
	private:

		std::shared_ptr<IStream> m_writer;

	public:

		Serializer(std::shared_ptr<IStream> inStream)
			: OutputArchive<Serializer, cereal::AllowEmptyClassElision>(this),
			m_writer(inStream)
		{

		}

		void SaveBinary(const void* data, size_t size)
		{
			m_writer->Write(reinterpret_cast<const uint8_t*>(data), size);
		}
	};

	class Deserializer : public cereal::InputArchive<Deserializer, cereal::AllowEmptyClassElision>
	{
	private:

		std::shared_ptr<IStream> m_reader;

	public:

		Deserializer(std::shared_ptr<IStream> inStream)
			: InputArchive<Deserializer, cereal::AllowEmptyClassElision>(this),
			m_reader(inStream)
		{

		}

		void LoadBinary(void* const data, size_t size)
		{
			m_reader->Read(reinterpret_cast<uint8_t*>(data), size);
		}
	};

	template<class T>
	inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
		CEREAL_SAVE_FUNCTION_NAME(Serializer& archive, T const & t)
	{
		archive.SaveBinary(std::addressof(t), sizeof(t));
	}

	template <class T>
	inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
		CEREAL_LOAD_FUNCTION_NAME(Deserializer& archive, T & t)
	{
		archive.LoadBinary(std::addressof(t), sizeof(t));
	}

	/*****************************************************************
	*	NameValuePair
	******************************************************************/
	template <class Archive, class T>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& archive, cereal::NameValuePair<T> & t)
	{
		archive(t.value);
	}

	/*****************************************************************
	*	SizeTag
	******************************************************************/
	template <class Archive, class T>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& archive, cereal::SizeTag<T> & t)
	{
		archive(t.size);
	}

	/*****************************************************************
	*	BinaryData
	******************************************************************/
	template <class T>
	inline void CEREAL_SAVE_FUNCTION_NAME(Serializer& archive, cereal::BinaryData<T> const & bin)
	{
		archive.SaveBinary(bin.data, static_cast<std::size_t>(bin.size));
	}

	template <class T>
	inline void CEREAL_LOAD_FUNCTION_NAME(Deserializer& archive, cereal::BinaryData<T> & bin)
	{
		archive.LoadBinary(bin.data, static_cast<std::size_t>(bin.size));
	}

#if !defined(EQUISETUM2_STATIC)
	/*****************************************************************
	*	Image
	******************************************************************/
	template <class Archive>
	inline void save(Archive & archive, const Image& image)
	{
		auto mem = VariableMemoryStream::Create();
		image.SaveToStream(mem);

		// ビットマップデータ
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(mem->Position())));
		archive(cereal::binary_data(mem->Data(), static_cast<size_t>(mem->Position())));

		// 名前
		archive(image.Identify());
	}

	template <class Archive>
	inline void load(Archive& archive, Image& image)
	{
		// ビットマップデータ
		cereal::size_type binarySize;
		archive(cereal::make_size_tag(binarySize));

		std::vector<uint8_t> buf(static_cast<size_t>(binarySize));
		archive(cereal::binary_data(buf.data(), buf.size()));

		auto mem = MemoryStream::CreateFromBuf(buf.data(), buf.size());
		auto tmpImage = Image::CreateFromStream(mem);
		image.MoveFrom(std::move(tmpImage));

		// 名前
		std::string id;
		archive(id);
		image.SetIdentify(id);
	}
#endif

	/*****************************************************************
	*	Point_t
	******************************************************************/
	template <class Archive, typename T>
	inline void serialize(Archive & archive, Point_t<T> point)
	{
		archive(point.x, point.y);
	}

	/*****************************************************************
	*	Rect_t
	******************************************************************/
	template <class Archive, typename T>
	inline void serialize(Archive & archive, Rect_t<T> rect)
	{
		archive(rect.x, rect.y, rect.width, rect.height);
	}

	/*****************************************************************
	*	Optional
	******************************************************************/
	template <class Archive, class T>
	inline void save(Archive& archive, const Optional<T>& opt)
	{
		const bool hasValue = opt.has_value();
		archive(hasValue);

		if (hasValue)
		{
			archive(opt.value());
		}
	}

	template <class Archive, class T>
	inline void load(Archive& archive, Optional<T>& opt)
	{
		bool hasValue;
		archive(hasValue);

		opt.reset();

		if (hasValue)
		{
			archive(opt.value());
		}
	}

	/*****************************************************************
	*	SharedPool
	******************************************************************/
	template <class Archive, class T>
	inline void save(Archive& archive, const SharedPool<T>& sharedPool)
	{
		cereal::size_type binarySize = 0;
		auto pool = sharedPool.GetPool();

		// 要素数
		for (auto& e : pool)
		{
			if (!e.m_weakPtr.expired())
			{
				binarySize++;
			}
		}
		archive(cereal::make_size_tag(binarySize));

		// 要素
		for (auto& e : pool)
		{
			if (!e.m_weakPtr.expired())
			{
				archive(e.m_weakPtr, e.m_id, e.m_shared);
			}
		}
	}

	template <class Archive, class T>
	inline void load(Archive& archive, SharedPool<T>& sharedPool)
	{
		sharedPool.Clear();

		cereal::size_type binarySize = 0;
		archive(cereal::make_size_tag(binarySize));

		for (cereal::size_type i = 0; i < binarySize; i++)
		{
			std::weak_ptr<T> ptr;
			archive(ptr);

			std::string id;
			archive(id);

			bool shared = false;
			archive(shared);

			sharedPool.Register(id, ptr.lock(), shared);
		}
	}
}

CEREAL_REGISTER_ARCHIVE(Equisetum2::Serializer)
CEREAL_REGISTER_ARCHIVE(Equisetum2::Deserializer)

#endif
