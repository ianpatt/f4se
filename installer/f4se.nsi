!include Library.nsh
!include FileFunc.nsh
!include MUI2.nsh
!include LogicLib.nsh

!ifndef IPersistFile
!define IPersistFile {0000010b-0000-0000-c000-000000000046}
!endif
!ifndef CLSID_ShellLink
!define CLSID_ShellLink {00021401-0000-0000-C000-000000000046}
!define IID_IShellLinkA {000214EE-0000-0000-C000-000000000046}
!define IID_IShellLinkW {000214F9-0000-0000-C000-000000000046}
!define IShellLinkDataList {45e2b4ae-b1c3-11d0-b92f-00a0c90312e1}
	!ifdef NSIS_UNICODE
	!define IID_IShellLink ${IID_IShellLinkW}
	!else
	!define IID_IShellLink ${IID_IShellLinkA}
	!endif
!endif

; version defines
!define VERSION_RUNTIME	"1.10.82.0"
!define VERSION_F4SE	"0.6.7"

; config installer
OutFile "f4se_${VERSION_F4SE}_installer.exe"
InstallDirRegKey HKLM "SOFTWARE\Bethesda Softworks\Fallout4" "Installed Path"
RequestExecutionLevel admin
DirText "F4SE ${VERSION_F4SE} Installer" "Fallout 4 installation directory" "" ""
SetCompressor /SOLID /FINAL lzma

Var StartMenuFolder

Name "F4SE"

BrandingText `Fallout 4 Script Extender`

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit
!insertmacro MUI_PAGE_LICENSE "License.txt"
!define MUI_PAGE_CUSTOMFUNCTION_PRE DirectoryPage_Create
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE DirectoryPage_Leave
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_NOAUTOCLOSE
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


!insertmacro MUI_LANGUAGE English

Function onGUIInit
  Aero::Apply
FunctionEnd

; main section
Section "!F4SE Core"
	SectionIn 1 RO
	SetOutPath $INSTDIR

	# Main files
	!insertmacro InstallLib DLL NOTSHARED NOREBOOT_NOTPROTECTED f4se_1_10_82.dll $INSTDIR\f4se_1_10_82.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED NOREBOOT_NOTPROTECTED f4se_steam_loader.dll $INSTDIR\f4se_steam_loader.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED NOREBOOT_NOTPROTECTED f4se_loader.exe $INSTDIR\f4se_loader.exe $INSTDIR
	
	# Papyrus scripts
	File /r Data
	
	# Docs
	File f4se_readme.txt
	File f4se_whatsnew.txt

	File CustomControlMap.txt

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Fallout 4 (F4SE).lnk" "$INSTDIR\f4se_loader.exe" "" "$INSTDIR\Fallout4Launcher.exe" 0
;	push "$SMPROGRAMS\$StartMenuFolder\Fallout 4 (F4SE).lnk"
;	call ShellLinkSetRunAs
;	pop $0
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\f4se_uninstall.exe"
	!insertmacro MUI_STARTMENU_WRITE_END

	WriteUninstaller "$INSTDIR\f4se_uninstall.exe"
SectionEnd

; desktop shortcut
Section "Desktop Shortcut"
	CreateShortCut "$DESKTOP\Fallout 4 (F4SE).lnk" "$INSTDIR\f4se_loader.exe" "" "$INSTDIR\Fallout4Launcher.exe" 0
;	push "$DESKTOP\Fallout 4 (F4SE).lnk"
;	call ShellLinkSetRunAs
;	Pop $0
SectionEnd

;Section "Start Menu"
		
;SectionEnd

; basic f4 install directory validation
Function DirectoryPage_Create
	IfFileExists "$INSTDIR\Fallout4.exe" done
	MessageBox MB_OK "The installer was unable to determine your Fallout 4 install directory. Please set the install directory to the folder containing Fallout4.exe."
done:
FunctionEnd

Function DirectoryPage_Leave
	IfFileExists "$INSTDIR\Fallout4.exe" checkVersion
	MessageBox MB_YESNO "The Fallout 4 EXE file was not found in the install directory. You have probably not selected the correct folder. Continue?" /SD IDYES IDYES done
	Abort
checkVersion:
	${GetFileVersion} "$INSTDIR\Fallout4.exe" $R0
	StrCmp $R0 ${VERSION_RUNTIME} done
	MessageBox MB_YESNO "This F4SE release requires Fallout 4 runtime ${VERSION_RUNTIME}, but yours is $R0. Continue?" /SD IDYES IDYES done
	Abort
done:
FunctionEnd

Section "Uninstall"
	Delete "$DESKTOP\Fallout 4 (F4SE).lnk"
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	RMDir "$SMPROGRAMS\$StartMenuFolder"

	Delete "$INSTDIR\f4se_loader.exe"
	Delete "$INSTDIR\f4se_1_10_82.dll"
	Delete "$INSTDIR\f4se_steam_loader.dll"
	Delete "$INSTDIR\f4se_readme.txt"
	Delete "$INSTDIR\f4se_whatsnew.txt"
	Delete "$INSTDIR\f4se_uninstall.exe"
SectionEnd

;Function ShellLinkSetRunAs
;System::Store S
;pop $9
;System::Call "ole32::CoCreateInstance(g'${CLSID_ShellLink}',i0,i1,g'${IID_IShellLink}',*i.r1)i.r0"
;${If} $0 = 0
;	System::Call "$1->0(g'${IPersistFile}',*i.r2)i.r0" ;QI
;	${If} $0 = 0
;		System::Call "$2->5(w '$9',i 0)i.r0" ;Load
;		${If} $0 = 0
;			System::Call "$1->0(g'${IShellLinkDataList}',*i.r3)i.r0" ;QI
;			${If} $0 = 0
;				System::Call "$3->6(*i.r4)i.r0" ;GetFlags
;				${If} $0 = 0
;					System::Call "$3->7(i $4|0x2000)i.r0" ;SetFlags ;SLDF_RUNAS_USER
;					${If} $0 = 0
;						System::Call "$2->6(w '$9',i1)i.r0" ;Save
;					${EndIf}
;				${EndIf}
;				System::Call "$3->2()" ;Release
;			${EndIf}
;		System::Call "$2->2()" ;Release
;		${EndIf}
;	${EndIf}
;	System::Call "$1->2()" ;Release
;${EndIf}
;push $0
;System::Store L
;FunctionEnd