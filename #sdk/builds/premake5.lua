require "ninja"

-- sln

solution "Project"
	location "../../.ide"
	configurations {"debug", "release", "shipping"}
	platforms { "x64" } -- "native", "universal64" }

-- app

project "Launch"
	kind "ConsoleApp"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/launch/**.c*", "../../#sdk/launch/**.h*"}
	includedirs {"../../#sdk/editor/", "../../#sdk/source/"}
	links {"Source", "Editor", "Game"}

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

project "Source"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/source/*.c", "../../#sdk/source/*.cpp", "../../#sdk/source/*.h", "../../#sdk/source/*.inl"}
	includedirs {"../../#sdk/source/"}
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
	includedirs {"../../#sdk/editor/", "../../#sdk/source/"}
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
	includedirs {"../../#sdk/editor/", "../../#sdk/source/"}
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
