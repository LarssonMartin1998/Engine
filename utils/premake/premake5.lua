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
-- modules.module "DirectX11"
modules.module "System"
	modules.links "DirectX11"
modules.module "DirectX11"

group "Executables"

project(PRODUCT_NAME)
	location "../../build/%{prj.name}"
	kind "WindowedApp"
	language "C++"
	characterset "MBCS"
	files { "../../source/code/main.cpp" }

	targetdir "../../game/"
	targetname "%{prj.name}_%{cfg.platform}_%{cfg.buildcfg}"
	debugdir "../../game/"
	
	warnings "Extra"
	editandcontinue "Off"

	-- Include third-party libraries for this project.
	--modules.dependson "DirectX11"
	modules.links "System"

	-- Global per-configuration settings.

	configuration { "Debug" }
		defines {
			"DEBUG",
			"NFINAL"
		}
		symbols "On"

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