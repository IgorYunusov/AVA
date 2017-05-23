require "ninja"

-- sln

solution "Project"
	location ".ide"
	configurations {"debug", "release", "shipping"}

-- dll

project "Editor"
	kind "SharedLib"
	location ".ide"
	language "C++"
	targetdir ".bin/%{cfg.buildcfg}"

--[[
	files {"src/editor/**.c*", "src/editor/**.h*"}
	includedirs {"src/editor/"}
	defines {"DLL_EXPORT"}
]]--

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "Runtime"
	kind "SharedLib"
	location ".ide"
	language "C++"
	targetdir ".bin/%{cfg.buildcfg}"

	files {"runtime/*.c", "runtime/*.cpp", "runtime/*.h", "runtime/*.inl"}
	excludes { "runtime/sprite.cpp" }
	includedirs {"runtime/"}
	defines {"DLL_EXPORT2"}

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
	location ".ide"
	language "C++"
	targetdir ".bin/%{cfg.buildcfg}"

	files {"runtime/sprite.cpp"}
	includedirs {"runtime/"}
	defines {"DLL_EXPORT3"}

	filter "configurations:debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

-- app

project "Launcher"
	kind "ConsoleApp"
	location ".ide"
	language "C++"
	targetdir ".bin/%{cfg.buildcfg}"

	files {"src/launcher/**.c*", "src/launcher/**.h*"}
	includedirs {"src/launcher/"}
	links {"Player", "Editor", "Game"}

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
