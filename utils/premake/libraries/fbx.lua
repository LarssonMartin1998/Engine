configuration {}
	includedirs {
		"../../../thirdparty/fbx/include"
	}
	links {
		"libfbxsdk"
	}

filter { "Win32", "Debug" }

	libdirs {
		"../../../thirdparty/fbx/lib/x86/debug/"
	}

	links {
		"../../../game/libfbxsdk_x86_debug.dll"
	}

filter { "Win32", "not Debug" }

	libdirs {
		"../../../thirdparty/fbx/lib/x86/release/"
	}

	links {
		"../../../game/libfbxsdk_x86_release.dll"
	}

filter { "x64", "Debug" }

	libdirs {
	"../../../thirdparty/fbx/lib/x64/debug/"
	}

	links {
		"../../../game/libfbxsdk_x64_debug.dll"
	}

filter { "x64", "not Debug" }

	libdirs {
	"../../../thirdparty/fbx/lib/x64/release/"
	}
	
	links {
		"../../../game/libfbxsdk_x64_release.dll"
	}