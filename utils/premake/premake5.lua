-- Please view the premake directory for more configuration options.
require "Settings"
local modules = require "Modules"

solution(PRODUCT_NAME)
	location "../../Source/"
	startproject(PRODUCT_NAME)
	systemversion "latest"
	platforms {
		"x86",
		"x64"
	}
	configurations {
		"Debug",
		"Release"
	}

group "Modules"

-- Add any modules under the group "Modules", add dependencies directly below a module. Remember to also add the modules to the main project below if it's necessary.
modules.module "System"

group "Executables"

project(PRODUCT_NAME)
	location "../../Build/%{prj.name}"
	symbolspath "../../Build/DebugSymbols/%{prj.name}/%{cfg.platform}_%{cfg.buildcfg}.pdb"
	kind "WindowedApp"
	language "C++"
	characterset "MBCS"
	files { "../../Source/Code/Main.cpp" }
	targetname "%{prj.name}_%{cfg.platform}_%{cfg.buildcfg}"
	systemversion "latest"
	toolset "v142"

	warnings "Extra"
	editandcontinue "Off"

	-- Include any modules for the main project.
	modules.links "System"

	-- Include third-party libraries for this project.
	-- dofile(libraries/thirpartylibrary.lua)
	dofile("Libraries/Assimp.lua")

	-- Global per-configuration settings.

	filter { "configurations:Debug" }

		defines {
			"DEBUG",
			"NRELEASE"
		}

		symbols "On"
		optimize "Debug"

	filter { "configurations:Release" }

		defines {
			"NDEBUG",
			"RELEASE"
		}

		symbols "Off"
		optimize "Speed"

	-- x86

	filter { "platforms:x86" }

		defines {
			"WIN32",
			"_WINDOWS"
		}

		flags {
			"NoMinimalRebuild",
		}

		vectorextensions "SSE2"

	filter { "platforms:x86", "configurations:not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}

	filter { "platforms:x86", "configurations:Debug" }

		targetdir "../../Binaries/x86/Debug/"
		targetdir "../../Binaries/x86/Debug/"

	filter { "platforms:x86", "configurations:Release" }

		targetdir "../../Binaries/x86/Release/"

	-- x64

	filter { "platforms:x64" }

		defines {
			"WIN64",
			"_WINDOWS"
		}

		flags {
			"NoMinimalRebuild",
		}

	filter { "platforms:x64", "configurations:not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}

	filter { "platforms:x64", "configurations:Debug" }

		targetdir "../../Binaries/x64/Debug/"
		targetdir "../../Binaries/x64/Debug/"

	filter { "platforms:x64", "configurations:Release" }

		targetdir "../../Binaries/x64/Release/"
