-- Please view the premake directory for more configuration options.
require "settings"
local modules = require "modules"

solution(PRODUCT_NAME)
	location "../../Source/"
	startproject(PRODUCT_NAME)
	platforms {
		"Win32",
		"x64",
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

	-- Global per-configuration settings.

	configuration { "Debug" }

		defines {
			"DEBUG",
			"NFINAL"
		}

		symbols "On"
		optimize "Debug"

	configuration { "Release" }

		defines {
			"NDEBUG",
			"RELEASE",
			"NFINAL"
		}

		symbols "On"
		optimize "Speed"

	configuration { "Final" }

		defines {
			"NDEBUG",
			"FINAL",
		}

		symbols "Off"
		optimize "Speed"

	-- Win32

	configuration { "Win32" }

		defines {
			"WIN32",
			"_WINDOWS",
		}

		flags {
			"NoMinimalRebuild",
		}
		
		vectorextensions "SSE2"

	configuration { "Win32", "not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}

	configuration { "Win32", "Debug" }

		targetdir "../../Binaries/x86/Debug/"
		targetdir "../../Binaries/x86/Debug/"

	configuration { "Win32", "Release" }

		targetdir "../../Binaries/x86/Release/"

	configuration { "Win32", "Final" }

		targetdir "../../Binaries/x86/Final/"

	-- x64

	configuration { "x64" }

		defines {
			"WIN64",
			"_WINDOWS",
		}

		flags {
			"NoMinimalRebuild",
		}

	configuration { "x64", "not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}

	configuration { "x64", "Debug" }

		targetdir "../../Binaries/x64/Debug/"
		targetdir "../../Binaries/x64/Debug/"

	configuration { "x64", "Release" }

		targetdir "../../Binaries/x64/Release/"

	configuration { "x64", "Final" }

		targetdir "../../Binaries/x64/Final/"