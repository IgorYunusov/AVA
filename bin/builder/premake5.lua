require "ninja"

-- sln

solution "Project"
	location "../../.ide"
	configurations {"debug", "release", "shipping"}
	platforms { "x64" } -- "native", "universal64" }

-- app

project "launcher"
	kind "ConsoleApp" --"WindowedApp"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../app/launcher/**.c*", "../../app/launcher/**.h*" } -- "../../app/launcher/dialog.rc" }
	includedirs {"../../bin/editor/", "../../src/"}
	links {"runtimes", "editor", "game" }

	configuration "windows"
		links { "user32", "gdi32" }

	configuration "linux"
		links { "pthread" }

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

--[[
	kind "WindowedApp"
	filter "system:windows"
		flags {"WinMain"}
--]]

-- dlls

project "runtimes"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

--	files {"../../#sdk/*.c", "../../#sdk/*.cpp", "../../#sdk/*.h", "../../#sdk/*.inl"}
--	files {"../../src/**.c", "../../src/**.cpp", "../../src/**.h", "../../src/**.inl"}
	files {"../../src/ava.c"}
	includedirs {"../../src/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "editor"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../app/editor/**.c*", "../../app/editor/**.h*"}
	includedirs {"../../app/editor/", "../../src/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "game"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files { "../../app/template/**.c", "../../app/template/**.cpp", "../../app/template/**.cc", "../../app/template/**.cxx", "../../app/template/**.h", "../../app/template/**.hpp", "../../app/template/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../app/editor/", "../../src/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"
