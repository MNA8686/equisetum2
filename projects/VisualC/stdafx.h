#if !defined(EQUISETUM2_STATIC)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "glew.h"
#endif

#include "type/Optional.h"

#define NOMINMAX		// これがないとWindows.hのmin,maxマクロが原因でエラーだらけになる
#include <windows.h>
#include <winbase.h>
#include <shlobj.h>
#include <stdio.h>

#include <array>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <cmath>
#include <regex>
#include <queue>
#include <list>
#include <mutex>

#ifdef _MSC_VER
#include <crtdbg.h>  // debugging routines
#endif

#if 1
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>

#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"
#endif

#undef GetTempPath
