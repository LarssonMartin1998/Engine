local m = {}


-- Normalizes a module name to its directory name.

function get_module_directory(name)
	return "../../Source/Code/" .. string.lower(name)
end

-- Declares a module to be built.

function m.module(name)
	project(name)
	location "../../Build/%{prj.name}"
	kind "StaticLib"
	language "C++"
	characterset "MBCS"

	local source_dir = get_module_directory(name)

	files {
		source_dir .. "/**.cpp",
		source_dir .. "/**.h",
	}

	filter {}

	flags "NoMinimalRebuild"
	editandcontinue  "Off"
	warnings "Extra"

	includedirs {
		"./" .. source_dir .. "/"
	}

	filter { "configurations:Debug" }
		defines {
			"DEBUG",
			"NFINAL"
		}
		symbols "On"

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
			"FINAL"
		}
		symbols "Off"

	-- Run module.lua file if it exist within the target module.
	local module_file = './' .. source_dir .. '/module.lua'
	if os.isfile(module_file) then
		-- Reset the filter to give the module file a clean slate.
		filter {}
		dofile(module_file)
	end

	-- Reset the filter to leave behind a blank slate, too.
	filter {}
end

-- Declares a header-only dependency on a module.

-- Should be used by modules to declare intra-module dependencies, or by any
-- project type that does not require linking to the module library.

function m.dependson(name)
	dependson(name)
	includedirs {
		"./" .. get_module_directory(name) .. "/"
	}
end

-- Declares a linker dependency on a module.

-- Should be used by projects that produce an executable file, or by any
-- project type that requires linking to the module library.

function m.links(name)
	links(name)
	includedirs {
		"./" .. get_module_directory(name) .. "/"
	}
end

function dirLookup(dir)
   local p = io.popen('find "'..dir..'" -type f')  --Open directory look for files, save data in p. By giving '-type f' as parameter, it returns all files.     
   for file in p:lines() do                         --Loop through all files
       print(file)       
   end
end

return m