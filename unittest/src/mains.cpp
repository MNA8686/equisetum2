#include "UnitTest.hpp"
#include "Equisetum2.h"

#ifdef _MSC_VER
#include <crtdbg.h>  // debugging routines
#endif

using namespace Equisetum2;

class SingletonDeleter
{
public:
	~SingletonDeleter()
	{
		SingletonFinalizer::Finalize();
	}
};
static SingletonDeleter g_SingletonDeleter;

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#if 0
	template <class Archive, typename T>
	inline void serialize(Archive & archive, Point_t<T> point)
	{
		archive(point.x, point.y);
	}

	template <class Archive>
	void save(Archive & archive, const String& str)
	{
		const std::string& stdStr = str;

		archive(stdStr);
	}

	template <class Archive>
	void load(Archive& archive, String& str)
	{
		std::string stdStr;
		archive(stdStr);

		str = stdStr;
	}
#endif

#if 0
	CEREAL_REGISTER_POLYMORPHIC_RELATION(std::string, String)

		struct Pokemon {
		//std::string name;
		String name;
		int hp = 0;
		std::vector<int> vec;
		Point point;

#if 0
		template<class Archive>
		void save(Archive & archive) const
		{
			//			archive(CEREAL_NVP(name));
			archive(CEREAL_NVP(hp));
			archive(CEREAL_NVP(vec));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			//			archive(CEREAL_NVP(name));
			archive(CEREAL_NVP(hp));
			archive(CEREAL_NVP(vec));
		}
#endif
		template<class Archive>
		void serialize(Archive & archive)
		{
			//		archive(cereal::virtual_base_class<std::string>(&name), CEREAL_NVP(name));
			archive(CEREAL_NVP(name));
			archive(CEREAL_NVP(hp));
			archive(CEREAL_NVP(vec));
			//archive(CEREAL_NVP(point));
		}
	};
#else

#if 0
	namespace cereal
	{
		//	template<class CharT, class Traits, class Alloc> inline
		void prologue(JSONOutputArchive & ar, String const &)
		{
			ar.writeName();
		}

		//! Prologue for strings for JSON archives
	//	template<class CharT, class Traits, class Alloc> inline
		void prologue(JSONInputArchive &, String const &)
		{ }

		// ######################################################################
		//! Epilogue for strings for JSON archives
	//	template<class CharT, class Traits, class Alloc> inline
		void epilogue(JSONOutputArchive &, String const &)
		{ }

		//! Epilogue for strings for JSON archives
	//	template<class CharT, class Traits, class Alloc> inline
		void epilogue(JSONInputArchive &, String const &)
		{ }

		//! saving string to JSON
	//	template<class CharT, class Traits, class Alloc> inline
		void CEREAL_SAVE_FUNCTION_NAME(JSONOutputArchive & ar, String const & str)
		{
			ar.saveValue(static_cast<std::string>(str));
		}

		//! loading string from JSON
	//	template<class CharT, class Traits, class Alloc> inline
		void CEREAL_LOAD_FUNCTION_NAME(JSONInputArchive & ar, String & str)
		{
			std::string tmp;
			ar.loadValue(tmp);

			str = tmp;
		}
	}
#endif
#if 0
	template <class Archive>
	inline void save(Archive & archive, const String& str)
	{
		const std::string& stdStr = str;

		archive(stdStr);
	}

	template <class Archive>
	inline void load(Archive& archive, String& str)
	{
		std::string stdStr;
		archive(stdStr);

		str = stdStr;
	}
#endif
	struct Tamago {
		std::string name;
		int hp = 555;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(name));
			archive(CEREAL_NVP(hp));
		}
	};

	struct Pokemon {
		std::string name;
		//String name;
		int hp = 0;

		//	std::shared_ptr<Image> image;
		//	std::weak_ptr<Image> weak;
		Point point;
		Optional<int> opt;
		std::function<void()> cb;
		std::vector<int> vec;

		Tamago tamago;

		template<class Archive>
		void serialize(Archive & archive)
		{
			//archive(opt);
			//archive(point);
	//		archive(g_test);

	//		archive(weak);
			archive(CEREAL_NVP(tamago));

			//		archive(::cereal::make_nvp("name", static_cast<std::string>(name)));
			archive(CEREAL_NVP(name));
			//		archive(name);
			archive(CEREAL_NVP(hp));
			archive(CEREAL_NVP(vec));
			//archive(image);

			//		archive(cb);
			//		archive(CEREAL_NVP(name), CEREAL_NVP(hp), CEREAL_NVP(image));
			//		archive(CEREAL_NVP(name), CEREAL_NVP(hp));
		}
	};
#endif

#if 0
	・プロジェクトフォルダにリソースが置かれている
		VCプロジェクトはどこに置く？
		SDLを毎回ビルドしていたらDLLのハッシュが変わってしまうのでは？
		depフォルダはコミットに含めるようにする？
		externalは切り離すべきかねぇ
		equisetumのexternalはdepされたものだけ置くようにすればよいのでは？
		・最新版のデプロイ
		リソースの使用状況を判定し、デプロイフォルダにコピー
		リソースのアーカイブを作る
		exeのビルドを行う
		各ファイルのハッシュとアーカイブのハッシュをjsonにする
		zipとインストーラ作成
		・デプロイフォルダから差分を抽出する
		diffフォルダ Ver1の完全なファイルとそれ以降の差分が格納されている
		diffフォルダは一つ前のバージョンとの差分が格納されている ← 常にVer1との差分で良くね？でも容量食うかな……
		アップデート用のファイルの中身は常にVer1と最新版との差分となる
		どのバージョンからでも最新版にアップデートできるはずである
		バージョン判定のために各バージョンのハッシュを算出する
		全てのバージョンにアップデートを適用してハッシュが合っているかを確認する
		過去バージョンの完全体はリポジトリに持つべきか
		それともハッシュだけリポジトリに持っておいて毎回組み立てる？

		リソースのアーカイブはどの時点で組み立てるか？
		あるいは併用？
		デバッグ版では「アーカイブよりも単体のファイルが優先される」とか

		eqdiff - i newver_path - c ver_folder_path - o path_out_path
#endif

#if 1
	struct FileInfo
	{
		std::string id;			// ファイル名
		size_t size;			// ファイルサイズ
		std::string hash;		// ハッシュ(SHA256)
		std::string version;	// ファイルのバージョン

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(id));
			archive(CEREAL_NVP(size));
			archive(CEREAL_NVP(hash));
		}
	};

	// バージョンフォルダごとに持たせるjson
	// そのバージョンの全てのファイル情報とアセットアーカイブのハッシュを持つ
	struct FolderInfo
	{
		std::string version;					// フォルダ内で扱っているバージョン
		std::vector<FileInfo> fileInfoArray;	// ファイル情報配列
		std::string assetHash;					// アセットアーカイブのハッシュ(HMAC)

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(version));
			archive(CEREAL_NVP(fileInfoArray));
			archive(CEREAL_NVP(assetHash));
		}

		void Save()
		{
			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);

				o_archive(CEREAL_NVP(version));
				o_archive(CEREAL_NVP(fileInfoArray));
				o_archive(CEREAL_NVP(assetHash));
			}
			std::cout << ss.str() << std::endl;
		}
	};

	struct VersionInfo
	{
		std::string version;	// バージョン番号
		std::string date;		// 作成日時

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(version));
			archive(CEREAL_NVP(date));
		}
	};
#endif


	class ArgumentImpl final
	{
	public:
		friend Singleton<ArgumentImpl>;	// シングルトンからインスタンスを作成してもらえるようにする

		ArgumentImpl() = default;
		~ArgumentImpl() = default;

		void Set(int argc, char *argv[])
		{
			if (argc > 1)
			{
				for (auto& i : Step( 1, argc - 1))
				{
					m_vArgv.emplace_back(argv[i.x]);
				}
			}
		}

		const std::vector<String>& Get()
		{
			return m_vArgv;
		}

	private:
		std::vector<String> m_vArgv;
	};

	class Argument final
	{
	public:
		Argument() = default;
		~Argument() = default;

		std::vector<String>::const_iterator begin()
		{
			return Singleton<ArgumentImpl>::GetInstance()->Get().begin();
		}

		std::vector<String>::const_iterator end()
		{
			return Singleton<ArgumentImpl>::GetInstance()->Get().end();
		}

		size_t size()
		{
			return Singleton<ArgumentImpl>::GetInstance()->Get().size();
		}
	};


	int main(int argc, char *argv[])
	{
#ifdef _MSC_VER
		// Detect memory leaks
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

		Logger::SetPriority(LogLevel::Info);
		Logger::SetCallback([](LogLevel level, const char* str)->bool {
			printf("%s\n", str);
			return false;
		});

		Singleton<ArgumentImpl>::GetInstance()->Set(argc, argv);

		int i = 0;
		for (auto& arg : Argument())
		{
			Logger::OutputInfo("%d: %s", i, arg.c_str());
			i++;
		}

#if 1
		{
			auto outFile = FileStream::CreateFromPath(Path::GetFullPath("seri.bin"), FileStream::Method::Write);
			Serializer seri(outFile);

			int testdayo = 123;
			Pokemon poke;
			poke.name = "testdayomon";
			poke.hp = 65535;
			//		poke.image = image1;
			//		poke.weak = poke.image;
			poke.cb = [testdayo]() {
				Logger::OutputInfo("cb test %d", testdayo);
			};

			poke.cb();
			//		seri(poke);
#if 1
			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("root", poke));
			}
			std::cout << ss.str() << std::endl;
#endif
		}
#else 
		{
			auto inFile = FileStream::CreateFromPath(Path::GetFullPath("seri.bin"));
			Deserializer seri(inFile);

			Pokemon poke;
			seri(poke);

			Logger::OutputInfo("%s %d", poke.name.c_str(), poke.hp);
		}
#endif

#if 0
		Pokemon pokemon;
		pokemon.name = "PIKACHU";
		pokemon.hp = 100;
		pokemon.vec.push_back(1);
		pokemon.vec.push_back(5);
		pokemon.vec.push_back(10);

		int test = 123;

		std::stringstream ss;
		{
			cereal::JSONOutputArchive o_archive(ss);
			o_archive(CEREAL_NVP(test));
			o_archive(cereal::make_nvp("root", pokemon));
		}
		std::cout << ss.str() << std::endl;

		Pokemon pokemon_i;
		cereal::JSONInputArchive i_archive(ss);
		i_archive(cereal::make_nvp("root", pokemon_i));

		std::cout << pokemon_i.name << std::endl;
		std::cout << pokemon_i.hp << std::endl;
#endif
		return 0;

		// 単体テスト実行
		return UnitTest::GetInstance()->Do();
	}
