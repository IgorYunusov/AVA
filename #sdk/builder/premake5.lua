require "ninja"

-- sln

solution "Project"
	location "../../.ide"
	configurations {"debug", "release", "shipping"}
	platforms { "x64" } -- "native", "universal64" }

-- app

project "Launcher"
	kind "ConsoleApp" --"WindowedApp"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/launcher/**.c*", "../../#sdk/launcher/**.h*" } -- "../../#sdk/launcher/dialog.rc" }
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
	links {"runtime", "editor", "game" }

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

project "runtime"
	kind "SharedLib"
	location "../../.ide"
	language "C++"
	targetdir "../../.bin/%{cfg.buildcfg}"

	files {"../../#sdk/runtime/*.c", "../../#sdk/runtime/*.cpp", "../../#sdk/runtime/*.h", "../../#sdk/runtime/*.inl"}
	includedirs {"../../#sdk/runtime/"}
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
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
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
	includedirs {"../../#sdk/editor/", "../../#sdk/runtime/"}
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
