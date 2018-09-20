configuration {}
	includedirs {
		"../../../Thirdparty/FBX/Include/"
	}
	links {
		"libfbxsdk"
	}

configuration { "Win32", "Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x86/Debug/"
	}

configuration { "Win32", "not Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x86/Release/"
	}

configuration { "x64", "Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x64/Debug/"
	}

configuration { "x64", "not Debug" }

	libdirs {
		"../../../Thirdparty/FBX/Lib/x64/Release/"
	}