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

	files {"../../#sdk/launcher/**.c*", "../../#sdk/launcher/**.h*" } -- "../../#sdk/launcher/dialog.rc" }
	includedirs {"../../#sdk/editor/", "../../#sdk/"}
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

	files {"../../#sdk/*.c", "../../#sdk/*.cpp", "../../#sdk/*.h", "../../#sdk/*.inl"}
	files {"../../#sdk/runtimes/**.c", "../../#sdk/runtimes/**.cpp", "../../#sdk/runtimes/**.h", "../../#sdk/runtimes/**.inl"}
	includedirs {"../../#sdk/"}
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

	files {"../../#sdk/editor/**.c*", "../../#sdk/editor/**.h*"}
	includedirs {"../../#sdk/editor/", "../../#sdk/"}
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

	files { "../../**.c", "../../**.cpp", "../../**.cc", "../../**.cxx", "../../**.h", "../../**.hpp", "../../**.inl"}
	removefiles { "../../#sdk/**" }
	includedirs {"../../#sdk/editor/", "../../#sdk/"}
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
