#include "system/Logger.h"
#include "system/Exception.hpp"
#include "audio/Audio.hpp"
#include "audio/AudioCompat.hpp"
#include "stream/SDLBinderRWops.inl"
#include "SDL_mixer.h"

namespace Equisetum2
{
	static const int g_maxBGM = 8;
	static const int g_maxChannel = 32;

	class SDLMixer
	{
	public:
		SDLMixer()
		{
			if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
			{
				SDL_InitSubSystem(SDL_INIT_AUDIO);
			}

			if (SDL_WasInit(SDL_INIT_AUDIO) != 0)
			{
				Mix_Init(MIX_INIT_OGG);

				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0)
				{
					Mix_AllocateChannels(g_maxChannel);
					m_enable = true;
				}
			}
		}

		virtual ~SDLMixer()
		{
			Mix_CloseAudio();
			Mix_Quit();
			SDL_QuitSubSystem(SDL_INIT_AUDIO);
		}

	protected:
		bool IsEnable()
		{
			return m_enable;
		}

	private:
		bool m_enable = false;
	};

	class AudioCompat::Impl final : public SDLMixer
	{
	public:

		Impl()
		{
		}

		~Impl()
		{
		}

		AudioHandlerID CreateBGMFromStream(const std::shared_ptr<IStream> stream)
		{
			AudioHandlerID id = -1;

			EQ_DURING
			{
				if (!IsEnable())
				{
					EQ_THROW(u8"オーディオが使用不可能です。");
				}

				auto rwops = SDLBinderRWops::CreateFromStream(stream);
				if (!rwops)
				{
					EQ_THROW(u8"rwopsの作成に失敗しました。");
				}
			
				auto pMusic = Mix_LoadMUS_RW(rwops, 1);
				if (!pMusic)
				{
					EQ_THROW(u8"BGMの作成に失敗しました。");
				}

				auto spMusic = std::shared_ptr<Mix_Music>(pMusic, [](Mix_Music* pMusic) {
					Mix_FreeMusic(pMusic);
				});
				if (!spMusic)
				{
					EQ_THROW(u8"デリーターの作成に失敗しました。");
				}

				// 空いてる登録領域を探す
				AudioHandlerID count = 0;
				for (auto& ref : m_bgm)
				{
					if (!ref.m_music)
					{
						ref.m_music = spMusic;
						ref.m_stream = stream;
						ref.m_pauseRefCount = 0;
						id = count;
						break;
					}

					count++;
				}

				if (id < 0)
				{
					EQ_THROW(u8"BGMの登録領域がありません。");
				}
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER
				
			return id;
		}

		void DeleteBGM(AudioHandlerID id)
		{
			if (id >= 0 && id < g_maxBGM)
			{
				m_bgm[id].m_music = nullptr;
			}
		}

		bool PlayBGM(AudioHandlerID id, bool loop)
		{
			StopBGM(m_nowPlaying);
			if (id >= 0 && id < g_maxBGM)
			{
				if (Mix_PlayMusic(m_bgm[id].m_music.get(), loop ? -1 : 0) == 0)
				{
					m_nowPlaying = id;
					SetVolumeBGM(id, m_bgm[id].m_volume);
					return true;
				}
			}
			return false;
		}

		bool StopBGM(AudioHandlerID id)
		{
			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				Mix_HaltMusic();

				m_bgm[id].m_pauseRefCount = 0;
				m_nowPlaying = -1;
			}
			return true;
		}

		void PauseBGM(AudioHandlerID id)
		{
			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				auto& bgm = m_bgm[id];

				if (bgm.m_pauseRefCount == 0)
				{
					Mix_PauseMusic();
				}
				bgm.m_pauseRefCount++;
			}
		}

		void ResumeBGM(AudioHandlerID id)
		{
			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				auto& bgm = m_bgm[id];

				if (bgm.m_pauseRefCount > 0)
				{
					bgm.m_pauseRefCount--;
				}

				if (bgm.m_pauseRefCount == 0)
				{
					Mix_ResumeMusic();
				}
			}
		}

		bool IsPlayingBGM(AudioHandlerID id) const
		{
			auto ret = false;

			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				ret = (Mix_PlayingMusic() == 1);
			}

			return ret;
		}

		bool IsPausedBGM(AudioHandlerID id) const
		{
			auto ret = false;

			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				ret = (Mix_PausedMusic() == 1);
			}

			return ret;
		}

		bool SetVolumeBGM(AudioHandlerID id, double volume)
		{
			if (id >= 0 && id < g_maxBGM)
			{
				auto& bgm = m_bgm[id];

				bgm.m_volume = SatVolume(volume);

				if (m_nowPlaying == id)
				{
					Mix_VolumeMusic(ToSDLVolume(m_volume * bgm.m_volume));
				}
			}

			return true;
		}

		double VolumeBGM(AudioHandlerID id) const
		{
			if (id >= 0 && id < g_maxBGM)
			{
				return m_bgm[id].m_volume;
			}

			return -1;
		}

		bool SetPos(AudioHandlerID id, double pos)
		{
			if (m_nowPlaying >= 0 &&
				m_nowPlaying == id)
			{
				Mix_RewindMusic();
				return Mix_SetMusicPosition(pos) == 0;
			}

			return false;
		}

		AudioHandlerID CreateSEFromStream(const std::shared_ptr<IStream> stream)
		{
			AudioHandlerID id = -1;

			EQ_DURING
			{
				if (!IsEnable())
				{
					EQ_THROW(u8"オーディオが使用不可能です。");
				}
			
				auto rwops = SDLBinderRWops::CreateFromStream(stream);
				if (!rwops)
				{
					EQ_THROW(u8"rwopsの作成に失敗しました。");
				}

				auto pChunk = Mix_LoadWAV_RW(rwops, 1);
				if (!pChunk)
				{
					EQ_THROW(u8"SEの作成に失敗しました。");
				}

				// デリーター作成
				auto spChunk = std::shared_ptr<Mix_Chunk>(pChunk, [](Mix_Chunk* pChunk) {
					Mix_FreeChunk(pChunk);
				});
				if (!spChunk)
				{
					EQ_THROW(u8"デリーターの作成に失敗しました。");
				}

				// 空いてる登録領域を探す
				AudioHandlerID count = 0;
				for (auto& ref : m_se)
				{
					if (!ref.m_chunk)
					{
						ref.m_chunk = spChunk;
						ref.m_pauseRefCount = 0;
						id = count;
						break;
					}

					count++;
				}

				if(id < 0)
				{
					EQ_THROW(u8"SEの登録領域がありません。");
				}
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return id;
		}

		bool Replace(AudioHandlerID dstId, AudioHandlerID srcId)
		{
			// 再生状態復元用
			auto isPlaying = IsPlayingSE(dstId);
			auto isPaused = IsPausedSE(dstId);
				
			// 置き換え先データを上書き
			m_se[dstId].m_chunk = m_se[srcId].m_chunk;

			// 再生状態を復元する
			if (isPlaying)
			{
				PlaySE(dstId, m_se[dstId].m_loop);
			}
			if (isPaused)
			{
				PauseSE(dstId);
			}

			return true;
		}

		void DeleteSE(AudioHandlerID id)
		{
			if (id >= 0 && id < g_maxChannel)
			{
				m_se[id].m_chunk = nullptr;
			}
		}

		bool PlaySE(AudioHandlerID id, bool loop)
		{
			auto ret = false;

			if (id >= 0 && id < g_maxChannel)
			{
				StopSE(id);

				// SDL環境ではid==channelとなるようにしている
				auto channel = Mix_PlayChannel(id, m_se[id].m_chunk.get(), loop ? -1 : 0);
				if (channel == id)
				{
					m_se[id].m_loop = loop;
					ret = true;
				}
				else
				{
					const char* error = Mix_GetError();
					Logger::OutputError(error);
				}
			}

			return ret;
		}

		bool StopSE(AudioHandlerID id)
		{
			auto ret = false;

			if (id >= 0 && id < g_maxChannel)
			{
				Mix_HaltChannel(id);
				m_se[id].m_pauseRefCount = 0;
				ret = true;
			}

			return ret;
		}

		void PauseSE(AudioHandlerID id)
		{
			if (id >= 0 && id < g_maxChannel)
			{
				auto& se = m_se[id];

				if (se.m_pauseRefCount == 0)
				{
					Mix_Pause(id);
				}
				se.m_pauseRefCount++;
			}
		}

		void ResumeSE(AudioHandlerID id)
		{
			if (id >= 0 && id < g_maxChannel)
			{
				auto& se = m_se[id];

				if (se.m_pauseRefCount > 0)
				{
					se.m_pauseRefCount--;
				}

				if (se.m_pauseRefCount == 0)
				{
					Mix_Resume(id);
				}
			}
		}

		bool IsPlayingSE(AudioHandlerID id) const
		{
			auto ret = false;

			if (id >= 0 && id < g_maxChannel)
			{
				ret = (Mix_Playing(id) == 1);
			}

			return ret;
		}

		bool IsPausedSE(AudioHandlerID id) const
		{
			auto ret = false;

			if (id >= 0 && id < g_maxChannel)
			{
				ret = (Mix_Paused(id) == 1);
			}

			return ret;
		}

		bool SetVolumeSE(AudioHandlerID id, double volume)
		{
			auto ret = false;

			if (id >= 0 && id < g_maxChannel)
			{
				auto& se = m_se[id];

				se.m_volume = AudioCompat::SatVolume(volume);
				Mix_VolumeChunk(se.m_chunk.get(), ToSDLVolume(se.m_volume));
				ret = true;
			}

			return ret;
		}

		double VolumeSE(AudioHandlerID id) const
		{
			auto volume = 0.;

			if (id >= 0 && id < g_maxChannel)
			{
				volume = m_se[id].m_volume;
			}

			return volume;
		}

		void PauseMaster()
		{
			if (m_pauseRefCount == 0)
			{
				for (AudioHandlerID i = 0; i < g_maxBGM; i++)
				{
					PauseBGM(i);
				}

				for (AudioHandlerID i = 0; i < g_maxChannel; i++)
				{
					PauseSE(i);
				}
			}
		}

		void ResumeMaster()
		{
			if (m_pauseRefCount > 0)
			{
				m_pauseRefCount--;
			}

			if (m_pauseRefCount == 0)
			{
				for (AudioHandlerID i = 0; i < g_maxBGM; i++)
				{
					ResumeBGM(i);
				}

				for (AudioHandlerID i = 0; i < g_maxChannel; i++)
				{
					ResumeSE(i);
				}
			}
		}

		bool IsPausedMaster()
		{
			return m_pauseRefCount > 0;
		}

		bool SetVolumeMaster(double volume)
		{
			// マスターボリューム設定
			m_volume = AudioCompat::SatVolume(volume);

			//BGMのボリュームを設定する(マスターボリューム * BGMのボリューム)
			Mix_VolumeMusic(ToSDLVolume(m_volume * m_bgm[0].m_volume));

			for (AudioHandlerID i = 0; i < g_maxChannel; i++)
			{
				// チャネルのボリュームを設定する
				// チャネルとチャンクは別々に音量設定できるのでマスターボリュームは
				// チャネルの音量を変更することで実現する
				Mix_Volume(i, ToSDLVolume(m_volume));
			}

			return true;
		}

		double VolumeMaster()
		{
			return m_volume;
		}

		void Prepare()
		{
		}

		void Update()
		{
		}

	private:

		struct BGM
		{
			std::shared_ptr<Mix_Music> m_music;
			int m_pauseRefCount = 0;
			double m_volume = Audio::MaxVolume;
			std::shared_ptr<IStream> m_stream;
		};

		struct SE
		{
			std::shared_ptr<Mix_Chunk> m_chunk;
			int m_pauseRefCount = 0;
			double m_volume = Audio::MaxVolume;
			bool m_loop = false;
		};

		BGM m_bgm[g_maxBGM];
		AudioHandlerID m_nowPlaying = -1;
		SE m_se[g_maxChannel];

		int m_pauseRefCount = 0;		/// マスターの再生状態
		double m_volume = Audio::MaxVolume;			/// マスターのボリューム

		inline int ToSDLVolume(double vol)
		{
			return static_cast<int>(static_cast<double>(MIX_MAX_VOLUME) * vol);
		}
	};
}
