# Microsoft Developer Studio Generated NMAKE File, Based on RasDialPro.dsp
!IF "$(CFG)" == ""
CFG=RasDial95 - Win32 Release
!MESSAGE No configuration specified. Defaulting to RasDial95 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "RasDial95 - Win32 Release" && "$(CFG)" != "RasDial95 - Win32 Debug" && "$(CFG)" != "RasDial95 - Win32 Release Registered"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RasDialPro.mak" CFG="RasDial95 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RasDial95 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RasDial95 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "RasDial95 - Win32 Release Registered" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RasDial95 - Win32 Release"

OUTDIR=.\WinRel
INTDIR=.\WinRel
# Begin Custom Macros
OutDir=.\WinRel
# End Custom Macros

ALL : "$(OUTDIR)\RDialPro.exe"


CLEAN :
	-@erase "$(INTDIR)\RasDialPro.obj"
	-@erase "$(INTDIR)\RasDialPro.res"
	-@erase "$(INTDIR)\RegCodeCommon.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\RDialPro.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D WINVER=0x0400 /Fp"$(INTDIR)\RasDialPro.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RasDialPro.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RasDialPro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rasapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\RDialPro.pdb" /machine:I386 /out:"$(OUTDIR)\RDialPro.exe" 
LINK32_OBJS= \
	"$(INTDIR)\RasDialPro.obj" \
	"$(INTDIR)\RegCodeCommon.obj" \
	"$(INTDIR)\RasDialPro.res"

"$(OUTDIR)\RDialPro.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Debug"

OUTDIR=.\WinDebug
INTDIR=.\WinDebug
# Begin Custom Macros
OutDir=.\WinDebug
# End Custom Macros

ALL : "$(OUTDIR)\RDialPro.exe" "$(OUTDIR)\RasDialPro.bsc"


CLEAN :
	-@erase "$(INTDIR)\RasDialPro.obj"
	-@erase "$(INTDIR)\RasDialPro.res"
	-@erase "$(INTDIR)\RasDialPro.sbr"
	-@erase "$(INTDIR)\RegCodeCommon.obj"
	-@erase "$(INTDIR)\RegCodeCommon.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\RasDialPro.bsc"
	-@erase "$(OUTDIR)\RDialPro.exe"
	-@erase "$(OUTDIR)\RDialPro.ilk"
	-@erase "$(OUTDIR)\RDialPro.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D WINVER=0x0400 /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\RasDialPro.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RasDialPro.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RasDialPro.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\RasDialPro.sbr" \
	"$(INTDIR)\RegCodeCommon.sbr"

"$(OUTDIR)\RasDialPro.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=rasapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\RDialPro.pdb" /debug /machine:I386 /out:"$(OUTDIR)\RDialPro.exe" 
LINK32_OBJS= \
	"$(INTDIR)\RasDialPro.obj" \
	"$(INTDIR)\RegCodeCommon.obj" \
	"$(INTDIR)\RasDialPro.res"

"$(OUTDIR)\RDialPro.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Release Registered"

OUTDIR=.\WinRelRegistered
INTDIR=.\WinRelRegistered
# Begin Custom Macros
OutDir=.\WinRelRegistered
# End Custom Macros

ALL : "$(OUTDIR)\RDialPro.exe"


CLEAN :
	-@erase "$(INTDIR)\RasDialPro.obj"
	-@erase "$(INTDIR)\RasDialPro.res"
	-@erase "$(INTDIR)\RegCodeCommon.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\RDialPro.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "REGISTEREDTEMPLATE" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D WINVER=0x0400 /Fp"$(INTDIR)\RasDialPro.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RasDialPro.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RasDialPro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rasapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\RDialPro.pdb" /machine:I386 /out:"$(OUTDIR)\RDialPro.exe" 
LINK32_OBJS= \
	"$(INTDIR)\RasDialPro.obj" \
	"$(INTDIR)\RegCodeCommon.obj" \
	"$(INTDIR)\RasDialPro.res"

"$(OUTDIR)\RDialPro.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("RasDialPro.dep")
!INCLUDE "RasDialPro.dep"
!ELSE 
!MESSAGE Warning: cannot find "RasDialPro.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "RasDial95 - Win32 Release" || "$(CFG)" == "RasDial95 - Win32 Debug" || "$(CFG)" == "RasDial95 - Win32 Release Registered"
SOURCE=.\RasDialPro.cpp

!IF  "$(CFG)" == "RasDial95 - Win32 Release"


"$(INTDIR)\RasDialPro.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Debug"


"$(INTDIR)\RasDialPro.obj"	"$(INTDIR)\RasDialPro.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Release Registered"


"$(INTDIR)\RasDialPro.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\RegCode\RegCodeCommon.cpp

!IF  "$(CFG)" == "RasDial95 - Win32 Release"


"$(INTDIR)\RegCodeCommon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Debug"


"$(INTDIR)\RegCodeCommon.obj"	"$(INTDIR)\RegCodeCommon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "RasDial95 - Win32 Release Registered"


"$(INTDIR)\RegCodeCommon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RasDialPro.rc

"$(INTDIR)\RasDialPro.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

