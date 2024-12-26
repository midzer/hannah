!define PRODUCT_NAME "Help Hannah's Horse"
!define PRODUCT_VERSION "2.0.0.0"
!define VERSION "2.0.0.0"
!define PRODUCT_PUBLISHER "FatHorseGames; Stephen Branley"

VIProductVersion "${VERSION}"
VIAddVersionKey  "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey  "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey  "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey  "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey  "FileDescription" "${PRODUCT_NAME} ${VERSION}"
VIAddVersionKey  "LegalCopyright" "Copyright Stephen Branley 2020"

OutFile "Hannah-2.0.0.exe"

Unicode True

InstallDir $PROGRAMFILES\FatHorseGames\Hannah\

Page directory
Page components
Page instfiles

# define uninstaller name

Section "Game" game_id
	SetOutPath $INSTDIR
	File hannah.exe
	File *.dll
	File COPYING
	File README
	File /r resources
	File ico.bmp
	
	WriteUninstaller $INSTDIR\uninstaller.exe
SectionEnd

Section "Start menu icon"
	CreateShortcut "$SMPROGRAMS\Hannah.lnk" "$INSTDIR\hannah.exe"
SectionEnd

Section "Desktop icon"
	CreateShortcut "$Desktop\Hannah.lnk" "$INSTDIR\hannah.exe"
SectionEnd


Section "Uninstall"
	# Delete loose files
	Delete $INSTDIR\uninstaller.exe
	Delete $INSTDIR\test.txt
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\COPYING
	Delete $INSTDIR\README
	Delete $INSTDIR\ico.bmp
	Delete $INSTDIR\hhh.exe
	Delete $INSTDIR\hannah.exe
	 
	# Delete the directories
	RMDir /r $INSTDIR\resources
	RMDir $INSTDIR
SectionEnd

Function .oninit
	SectionSetFlags ${game_id} 17
FunctionEnd