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
		"../../../game/dll/fbx/x86/debug/libfbxsdk.dll"
	}

filter { "Win32", "not Debug" }

	libdirs {
		"../../../thirdparty/fbx/lib/x86/release/"
	}

	links {
		"../../../game/dll/fbx/x86/release/libfbxsdk.dll"
	}

filter { "x64", "Debug" }

	libdirs {
	"../../../thirdparty/fbx/lib/x64/debug/"
	}

	links {
		"../../../game/dll/fbx/x64/debug/libfbxsdk.dll"
	}

filter { "x64", "not Debug" }

	libdirs {
	"../../../thirdparty/fbx/lib/x64/release/"
	}
	
	links {
		"../../../game/dll/fbx/x64/release/libfbxsdk.dll"
	}