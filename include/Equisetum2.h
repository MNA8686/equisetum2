#if !defined(_EQUISETUM2_H_)
#define _EQUISETUM2_H_

#include "type/Optional.h"
#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "type/FixedDec.hpp"

#include "util/Step.hpp"
#include "util/Regex.hpp"
#include "util/SHA256.hpp"
#include "util/SharedPool.hpp"
#include "util/ArchiveAccessor.hpp"
#include "util/ArchivePacker.hpp"
#if !defined(EQUISETUM2_STATIC)
#include "util/AssetManager.hpp"
#endif

#include "math/Math.hpp"

#include "system/Exception.hpp"
#include "system/Logger.h"
#include "system/Argument.hpp"
#if !defined(EQUISETUM2_STATIC)
#include "system/System.h"
#endif

#include "fs/File.hpp"
#include "fs/Directory.hpp"
#include "fs/Path.hpp"

#include "stream/IStream.h"
#include "stream/FileStream.h"
#include "stream/MemoryStream.h"
#include "stream/VariableMemoryStream.h"
#include "stream/PartialStream.hpp"
#include "stream/TextReader.h"
#include "stream/TextWriter.h"

#if !defined(EQUISETUM2_STATIC)
#include "graphic/Window.h"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Sprite.hpp"
#include "graphic/BitmapFont.hpp"
#include "graphic/Renderer.hpp"
#include "graphic/RenderObject.hpp"

#include "input/Key.h"
#include "input/Mouse.h"
#include "input/Touch.h"
#include "input/Joystick.h"

#include "audio/Audio.hpp"
#include "audio/BGM.hpp"
#include "audio/SE.hpp"

#include "timer/ITimer.h"
#include "timer/SystemTimer.h"
#include "timer/TickCounter.h"
#include "timer/FpsCounter.h"
#include "timer/FpsPaceMaker.h"
#endif

//#include "util/Serialization.hpp"

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

#endif
