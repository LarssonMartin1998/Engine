-- Please view the premake directory for more configuration options.
require "settings"
local modules = require "modules"

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
		"Release",
		"Final"
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
	files { "../../Source/Code/main.cpp" }
	targetname "%{prj.name}_%{cfg.platform}_%{cfg.buildcfg}"

	warnings "Extra"
	editandcontinue "Off"

	-- Include any modules for the main project.
	modules.links "System"

	-- Include third-party libraries for this project.
	-- dofile(libraries/thirpartylibrary.lua)
	dofile("libraries/fbx.lua")
	dofile("libraries/assimp.lua")

	-- Global per-configuration settings.

	filter { "configurations:Debug" }

		defines {
			"DEBUG",
			"NFINAL"
		}

		symbols "On"
		optimize "Debug"

	filter { "configurations:Release" }

		defines {
			"NDEBUG",
			"RELEASE",
			"NFINAL"
		}

		symbols "On"
		optimize "Speed"

	filter { "configurations:Final" }

		defines {
			"NDEBUG",
			"FINAL",
		}

		symbols "Off"
		optimize "Speed"

	-- x86

	filter { "platforms:x86" }

		defines {
			"WIN32",
			"_WINDOWS",
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

	filter { "platforms:x86", "configurations:Final" }

		targetdir "../../Binaries/x86/Final/"

	-- x64

	filter { "platforms:x64" }

		defines {
			"WIN64",
			"_WINDOWS",
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

	filter { "platforms:x64", "configurations:Final" }

		targetdir "../../Binaries/x64/Final/"
