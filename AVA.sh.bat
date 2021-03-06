#/bin/bash 2>nul || goto :windows



# bash
echo [bash env]

bin/builder/premake5.linux codelite
bin/builder/premake5.linux gmake
bin/builder/premake5.linux vs2013
bin/builder/premake5.linux xcode4
bin/builder/premake5.linux ninja
bin/builder/ninja.linux -C .ide

bin/builder/premake5.osx codelite
bin/builder/premake5.osx gmake
bin/builder/premake5.osx vs2013
bin/builder/premake5.osx xcode4
bin/builder/premake5.osx ninja
bin/builder/ninja.osx   -C .ide

exit





:windows
@echo off

    REM cleanup

        if "%1"=="wipe" (
            if exist .bin rd /q /s .bin && if exist .bin echo "error cannot clean up .bin" && exit /b
            if exist .ide rd /q /s .ide && if exist .ide echo "error cannot clean up .ide" && exit /b
            echo Clean up && exit /b
        )

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

        REM Luajit, %AVABIN%
        if ""=="" (
            echo [win][set]
            set AVABIN="%~dp0%\bin\"
            REM set      path="%path%;%~dp0%\bin\;"
            REM      endlocal &&  && set AVABIN=%AVABIN%
        )

    REM build

        pushd "%AVABIN%\builder"

            REM project generation
            REM premake5.exe codelite
            REM premake5.exe gmake
            REM premake5.exe vs2015
            REM premake5.exe vs2013
            REM premake5.exe xcode4
            premake5.exe ninja

            REM actual build
            set NINJA_STATUS="[%%e] [%%r/%%f]"
            ninja.exe -v -C ..\..\.ide
            set OK=%ERRORLEVEL%

        popd

    REM launch

    pushd "!AVABIN!\.."

        if "0"=="%OK%" (
            echo ^>^> launcher
            .bin\debug\launcher.exe %*
            echo ^<^< launcher
        ) else (
            echo  && rem beep
        )

    popd

    REM exit

        echo Press any key to continue... && pause > nul
        exit /b
