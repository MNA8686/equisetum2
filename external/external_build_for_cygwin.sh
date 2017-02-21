#!/usr/bin/sh

# Target Directory
TARGET_DIR=win32

# Source Directory
SRC_DIR=src

# Build Directory
BUILD_DIR=build

# Deploy Directory
DEP_DIR=dep

#Current Directory
CUR_DIR=`pwd`

# Make Directory
mkdir ${BUILD_DIR}
mkdir ${BUILD_DIR}/${TARGET_DIR}
mkdir ${DEP_DIR}
mkdir ${DEP_DIR}/${TARGET_DIR}

################################
# SDL                          #
################################
LIB_NAME=SDL2
LIB_DIR=SDL2-2.0.5

# Make Deploy Directory
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll

# Build
if [ ! -e ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR} ]; then
	unzip ${SRC_DIR}/${LIB_DIR}.zip -d ${BUILD_DIR}/${TARGET_DIR}
	patch -p1 -d ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR} < ${SRC_DIR}/${LIB_DIR}_for_win32.patch
fi
cd ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}/VisualC
MSBuild.exe SDL.sln /t:Build /p:Configuration=Release

# Deploy
cd ${CUR_DIR}/${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}
cp include/*.h ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
cp VisualC/Win32/Release/SDL2.lib ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
cp VisualC/Win32/Release/SDL2main.lib ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
cp VisualC/Win32/Release/SDL2.dll ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll
cd ${CUR_DIR}

################################
# SDL_mixer                    #
################################
LIB_NAME=SDL2_mixer
LIB_DIR=SDL2_mixer-2.0.1

# Make Deploy Directory
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll

# Build
if [ ! -e ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR} ]; then
	unzip ${SRC_DIR}/${LIB_DIR}.zip -d ${BUILD_DIR}/${TARGET_DIR}
	patch -p1 -d ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR} < ${SRC_DIR}/${LIB_DIR}_for_win32.patch
fi
cd ${CUR_DIR}/${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}/VisualC
MSBuild.exe SDL_mixer.sln /t:Build /p:Configuration=Release

# Deploy
cd ${CUR_DIR}/${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}
cp SDL_mixer.h ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
cp VisualC/Win32/Release/${LIB_NAME}.lib ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
cp VisualC/Win32/Release/${LIB_NAME}.dll ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll
cp VisualC/Win32/Release/libogg-0.dll ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll
cp VisualC/Win32/Release/libvorbis-0.dll ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll
cp VisualC/Win32/Release/libvorbisfile-3.dll ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/dll
cd ${CUR_DIR}

################################
# AngelScript                  #
################################
LIB_NAME=angelscript
LIB_DIR=angelscript_2.31.0

# Make Deploy Directory
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
mkdir ${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib

# Build
if [ ! -e ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR} ]; then
	unzip ${SRC_DIR}/${LIB_DIR}.zip -d ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}
fi
cd ${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}/sdk/angelscript/projects/msvc2015
MSBuild.exe angelscript.sln /t:Build /p:Configuration=Release

# Deploy
cd ${CUR_DIR}/${BUILD_DIR}/${TARGET_DIR}/${LIB_DIR}
cp sdk/angelscript/include/*.h ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/include
cp sdk/angelscript/lib/angelscript.lib ${CUR_DIR}/${DEP_DIR}/${TARGET_DIR}/${LIB_NAME}/lib
cd ${CUR_DIR}
