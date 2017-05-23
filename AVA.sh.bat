#/bin/bash 2>nul || goto :windows



# bash
echo [bash env]

#etc/builder/premake5.linux codelite
#etc/builder/premake5.linux gmake
#etc/builder/premake5.linux vs2013
#etc/builder/premake5.linux xcode4
#etc/builder/premake5.linux ninja
#etc/builder/ninja.linux -C ide

#etc/builder/premake5.osx codelite
#etc/builder/premake5.osx gmake
#etc/builder/premake5.osx vs2013
#etc/builder/premake5.osx xcode4
#etc/builder/premake5.osx ninja
#etc/builder/ninja.osx   -C ide

exit





:windows
@echo off
echo [windows env]

    cd /d "%~dp0"

    REM MSVC compiler setup

           if exist "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
              @call "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
    ) else if exist "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
              @call "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
    ) else (
        echo Warning: Could not find x64 environment variables for Visual Studio 2013/2015
    )

    REM project generation

    #etc\builder\premake5.exe codelite
    #etc\builder\premake5.exe gmake
    #etc\builder\premake5.exe vs2013
    #etc\builder\premake5.exe xcode4
    #etc\builder\premake5.exe ninja

    REM actual build

    set NINJA_STATUS="[%%e] [%%r/%%f]"
    #etc\builder\ninja.exe -C ide

if not "0"=="%ERRORLEVEL%" (

    echo  && rem beep
    pause
    exit /b

)

exit /b
