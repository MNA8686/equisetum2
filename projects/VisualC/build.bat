@echo off
:開発者コマンド プロンプト for VS2013 のパスを設定
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsMSBuildCmd.bat"
:MSBuild.exeのパスを設定
set PATH=%PATH%;C:\Windows\Microsoft.NET\Framework\v4.0.30319\

chcp 65001
echo ==============================
echo =    libEquisetum2 Debug     =
echo ==============================
MSBuild.exe libEquisetum2.sln /t:Build /p:Configuration=Debug
if not %errorlevel% == 0 (
echo errorlevel %errorlevel%
exit /b %errorlevel%
)

echo ==============================
echo =    libEquisetum2 Release   =
echo ==============================
MSBuild.exe libEquisetum2.sln /t:Build /p:Configuration=Release
if not %errorlevel% == 0 (
echo errorlevel %errorlevel%
exit /b %errorlevel%
)

exit /b
