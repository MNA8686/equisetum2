@echo off
:開発者コマンド プロンプト for VS2019 のパスを設定
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
:MSBuild.exeのパスを設定
set PATH=%PATH%;C:\Windows\Microsoft.NET\Framework\v4.0.30319\
set VCTargetsPath=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Microsoft\VC\v160\

echo ==============================
echo =    libEquisetum2 Debug     =
echo ==============================
MSBuild.exe libEquisetum2.sln /t:Build /p:Configuration=Debug;PlatformTarget="x86"
if not %errorlevel% == 0 (
echo errorlevel %errorlevel%
exit /b %errorlevel%
)

echo ==============================
echo =    libEquisetum2 Release   =
echo ==============================
MSBuild.exe libEquisetum2.sln /t:Build /p:Configuration=Release;PlatformTarget="x86"
if not %errorlevel% == 0 (
echo errorlevel %errorlevel%
exit /b %errorlevel%
)

exit /b
