# UTF-8 for MSBuild 
chcp 65001

:開発者コマンド プロンプト for VS2013 のパスを設定
:call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsMSBuildCmd.bat"
:MSBuild.exeのパスを設定
set PATH=%PATH%;C:\Windows\Microsoft.NET\Framework\v4.0.30319\

:cygwin起動
set HOME=%~dp0
c:\cygwin\bin\bash --login "./external_build_for_cygwin.sh"

echo errorlevel %errorlevel%
exit /b %errorlevel%
