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

#endif
