require "ninja"

-- sln

solution "Project"
	location "../../.ide"
	configurations {"debug", "release", "shipping"}
	platforms { "x64" } -- "native", "universal64" }

-- app

project "Launcher"
	kind "ConsoleApp"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/launcher/**.c*", "../../#sdk/launcher/**.h*"}
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
	links {"Runtime", "Editor", "Game"}

	configuration "windows"
		links { "user32", "gdi32" }

	configuration "linux"
		links { "pthread" }

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

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

project "Runtime"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/runtime/*.c", "../../#sdk/runtime/*.cpp", "../../#sdk/runtime/*.h", "../../#sdk/runtime/*.inl"}
	includedirs {"../../#sdk/runtime/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "Editor"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/editor/**.c*", "../../#sdk/editor/**.h*"}
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "Game"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files { "../../**.c", "../../**.cpp", "../../**.cc", "../../**.cxx", "../../**.h", "../../**.hpp", "../../**.inl"}
	removefiles { "../../#sdk/**" }
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"
