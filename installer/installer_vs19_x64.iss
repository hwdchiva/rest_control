;========================================================================================
;  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
; |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
; |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
; |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
; |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
; |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
;
;========================================================================================

#define MyAppName "Rest Controller"
#define MyAppPublisher "hwdchiva"
#define MyAppURL "https://github.com/hwdchiva"
#define MyAppExeName "rest_control.exe"
#define MyAppVersion ReadIni(SourcePath + "version.ini", "Version", "Ver", "unknown")
#define BuildDir = SourcePath + "..\build"
#define BuildObjDir = SourcePath + "..\build\Release"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{7CD69287-3C01-41CC-93B7-D4D84B278CBD}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf64}\hwdchiva\{#MyAppName}
DefaultGroupName=hwdchiva\{#MyAppName}
;LicenseFile={#BuildDir}\license.rtf
OutputBaseFilename=SetupRestControl-{#MyAppVersion}-x64
OutputDir=Installers
SetupIconFile={#SourcePath}\..\img\hwdchiva.ico
Compression=lzma
SolidCompression=yes
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64
DisableWelcomePage=no
DisableDirPage=no
UsePreviousAppDir=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "{#BuildObjDir}\rest_control.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#BuildObjDir}\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#BuildObjDir}\plugins\platforminputcontexts\*"; DestDir: "{app}\bin\platforminputcontexts"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#BuildObjDir}\plugins\platforms\*"; DestDir: "{app}\bin\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SourcePath}\dep\vc15-19_redist_x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}";
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon;

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{tmp}\vc15-19_redist_x64.exe"; StatusMsg: "Installing Microsoft Visual C++ Redistributable..."; Check: not VCinstalledArch('x64')

#include "common.iss"
