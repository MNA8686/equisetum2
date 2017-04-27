/**
* @file Path.hpp
* @brief パス操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/
#if !defined(_EQPATH_H_)
#define _EQPATH_H_

#include "type/String.h"
#include "type/Optional.h"

namespace Equisetum2
{
	/**
	* パス操作に関するクラス
	*/
	class Path
	{
	public:

		static String GetDirectoryName(const String& path);

		/**
		* @brief 拡張子を取得する
		* @param path ファイルパス
		* @return 拡張子 <br>
		* @return 例: "test.txt" -> ".txt" <br>
		* @return     "test.dat.txt" → ".txt" <br>
		* @return     "test" → ""
		*/
		static String GetExtension(const String& path);

		/**
		* @brief ファイル名を取得する
		* @param path ファイルパス
		* @return ファイル名
		*/
		static String GetFileName(const String& path);

		/**
		* @brief 拡張子を除いたファイル名を取得する
		* @param path ファイルパス
		* @return 拡張子を除いたファイル名 例: "test.txt" -> "test"
		*/
		static String GetFileNameWithoutExtension(const String& path);

		/**
		* @brief フルパス取得する
		* @param path ファイルまたはディレクトリのパス
		* @return フルパス
		*
		* @detail フルパスを入力した場合、入力と出力の文字列は同じ内容となる。<br>
		* @detail 相対パスを入力した場合、GetBasePath()で取得できるパスを基準としたフルパスを返す。<br>
		*/
		static String GetFullPath(const String& path);

		/**
		* @brief 拡張子を変更する
		* @param path ファイルパス
		* @param ext 変更後の拡張子
		* @return 拡張子を変更後のファイル名 <br>
		* @return 例: "test.txt" ".dat" -> "test.dat" 
		*/
		static String ChangeExtension(const String& path, const String& ext);

		/**
		* @brief 拡張子の有無を判定する
		* @param path ファイルパス
		* @return 拡張子有無
		*/
		static bool HasExtension(const String& path);

		/**
		* @brief ルートからのパス(フルパス)かどうかを判定する
		* @param path ファイルパス
		* @return フルパス有無
		*
		* @detail Win32: ドライブレターやネットワークホスト名が含まれていると真となる。<br>
		*/
		static bool IsPathRooted(const String& path);

		/**
		* @brief アプリケーションの基準となるパスを取得する
		* @return 基準パス
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている。<br>
		* @detail Win32: 実行ファイルのあるディレクトリを返す。<br>
		*/
		static String GetBasePath();

		/**
		* @brief テンポラリパスを取得する
		* @param org サークル名
		* @param app アプリケーション名
		* @return テンポラリパス
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている。<br>
		* @detail サークル名とアプリケーション名のフォルダは自動的に作成し、そのパスを返す。<br>
		*/
		static Optional<String> GetTempPath(const String& org, const String& app);

		/**
		* @brief キャッシュパスを取得する
		* @param org サークル名
		* @param app アプリケーション名
		* @return キャッシュパス
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている。<br>
		* @detail サークル名とアプリケーション名のフォルダは自動的に作成し、そのパスを返す。<br>
		*/
		static Optional<String> GetCachePath(const String& org, const String& app);

		/**
		* @brief 非公開ドキュメント(設定ファイルなど、ユーザーに見せたくないもの)パスを取得する
		* @param org サークル名
		* @param app アプリケーション名
		* @return 非公開ドキュメントパス
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている。<br>
		* @detail サークル名とアプリケーション名のフォルダは自動的に作成し、そのパスを返す。<br>
		*/
		static Optional<String> GetPrivateDocumentPath(const String& org, const String& app);

		/**
		* @brief 公開ドキュメント(スクリーンショットなど、ユーザーに見せてもよいもの)パスを取得する
		* @param org サークル名
		* @param app アプリケーション名
		* @return 公開ドキュメントパス
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている。<br>
		* @detail サークル名とアプリケーション名のフォルダは自動的に作成し、そのパスを返す。<br>
		*/
		static Optional<String> GetPublicDocumentPath(const String& org, const String& app);

		static const String DirectorySeparatorChar;	/// 使用している環境でのセパレータ
	};
}

#endif
