:�J���҃R�}���h �v�����v�g for VS2013 �̃p�X��ݒ�
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
:MSBuild.exe�̃p�X��ݒ�
set PATH=%PATH%;C:\Windows\Microsoft.NET\Framework\v4.0.30319\

:cygwin�N��
set HOME=%~dp0
c:\cygwin\bin\bash --login "./external_build_for_cygwin.sh"
REM c:\cygwin\bin\bash --login

pause