#/bin/bash 2>nul || goto :windows



# bash
echo [bash env]

#sdk/builds/premake5.linux codelite
#sdk/builds/premake5.linux gmake
#sdk/builds/premake5.linux vs2013
#sdk/builds/premake5.linux xcode4
#sdk/builds/premake5.linux ninja
#sdk/builds/ninja.linux -C .ide

#sdk/builds/premake5.osx codelite
#sdk/builds/premake5.osx gmake
#sdk/builds/premake5.osx vs2013
#sdk/builds/premake5.osx xcode4
#sdk/builds/premake5.osx ninja
#sdk/builds/ninja.osx   -C .ide

exit





:windows
@echo off

    REM setup

        REM MSVC
        if not "%Platform%"=="x64" ( 
            echo [win][msc]
                   if exist "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else if exist "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else (
                echo Warning: Could not find x64 environment variables for Visual Studio 2013/2015
                exit /b
            )
        )

        REM Luajit, %AVASDK%
        if ""=="" ( 
            echo [win][set]
            set AVASDK="%~dp0%\#sdk\"
            REM set      path="%path%;%~dp0%\#sdk\;"
            REM      endlocal &&  && set AVASDK=%AVASDK%
        )

    REM build

        pushd "%AVASDK%\builds"

            REM project generation
            premake5.exe codelite
            premake5.exe gmake
            premake5.exe vs2013
            premake5.exe xcode4
            premake5.exe ninja

            REM actual build
            set NINJA_STATUS="[%%e] [%%r/%%f]"
            ninja.exe -C ..\..\.ide

        popd

    REM launch

    pushd "!AVASDK!\.."

        if "0"=="%ERRORLEVEL%" (
            echo ^>^> launch
            .bin\debug\launch.exe
            echo ^<^< launch
        ) else (
            echo  && rem beep
        )

    popd

    REM exit

        echo Press any key to continue... && pause > nul
        exit /b
