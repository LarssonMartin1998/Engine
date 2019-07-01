filter {}
	includedirs {
		"../../../Thirdparty/FBX/Include/"
	}
	links {
		"libfbxsdk"
	}
	defines {
		"FBXSDK_SHARED"
	}

filter { "platforms:x86", "configurations:Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x86/Debug/"
	}

filter { "platforms:x86", "configurations:not Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x86/Release/"
	}

filter { "platforms:x64", "configurations:Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x64/Debug/"
	}

filter { "platforms:x64", "configurations:not Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x64/Release/"
	}