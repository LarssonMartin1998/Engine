-- Please view the premake directory for more configuration options.
require "settings"
local modules = require "modules"

solution(PRODUCT_NAME)
	location "../../source/"
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
	location "../../build/%{prj.name}"
	symbolspath "../../build/DebugSymbols/%{prj.name}/%{cfg.platform}_%{cfg.buildcfg}.pdb"
	kind "WindowedApp"
	language "C++"
	characterset "MBCS"
	files { "../../source/code/main.cpp" }
	targetdir "../../game/"
	targetname "%{prj.name}_%{cfg.platform}_%{cfg.buildcfg}"
	debugdir "../../game/"
	
	warnings "Extra"
	editandcontinue "Off"

	-- Include any modules for the main project.
	modules.links "System"

	-- Include third-party libraries for this project.
	

	-- Global per-configuration settings.

	filter { "Debug" }

		defines {
			"DEBUG",
			"NFINAL"
		}

		symbols "On"

	filter { "Release" }

		defines {
			"NDEBUG",
			"RELEASE",
			"NFINAL"
		}

		symbols "On"
		optimize "Speed"

	filter { "Final" }

		defines {
			"NDEBUG",
			"FINAL",
		}

		symbols "Off"
		optimize "Speed"

	-- Win32

	filter { "Win32" }

		defines {
			"WIN32",
			"_WINDOWS",
		}

		flags {
			"NoMinimalRebuild",
		}
		
		vectorextensions "SSE2"

	filter { "Win32", "not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}

	-- x64

	filter { "x64" }

		defines {
			"WIN64",
			"_WINDOWS",
		}

		flags {
			"NoMinimalRebuild",
		}

	filter { "x64", "not Debug" }

		buildoptions {
			-- Whole program optimization
			"/GL"
		}

		linkoptions {
			-- Link-time code generation
			"/LTCG"
		}