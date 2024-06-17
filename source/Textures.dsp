# Microsoft Developer Studio Project File - Name="Textures" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Textures - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Textures.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Textures.mak" CFG="Textures - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Textures - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Textures - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Textures - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /W3 /Gi /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD -QaxW -O3 -Qip /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"Release/FSAATest.exe"

!ELSEIF  "$(CFG)" == "Textures - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Textures - Win32 Release"
# Name "Textures - Win32 Debug"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=C:\DX8SDK\common\src\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=C:\DX8SDK\common\src\d3dfile.cpp
# End Source File
# Begin Source File

SOURCE=C:\DX8SDK\common\src\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=C:\DX8SDK\common\src\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=C:\DX8SDK\common\src\dxutil.cpp
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=C:\DXSDK\Lib\strmiids.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\amstrmid.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\d3dxof.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\dmoguids.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\dxguid.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\encapi.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\msdmo.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\quartz.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\dxtrans.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\d3dx8.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\Lib\d3d8.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.res
# End Source File
# Begin Source File

SOURCE=.\Textures.cpp

!IF  "$(CFG)" == "Textures - Win32 Release"

!ELSEIF  "$(CFG)" == "Textures - Win32 Debug"

# ADD CPP /Gm-
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# End Target
# End Project
