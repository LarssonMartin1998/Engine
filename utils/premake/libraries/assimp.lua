filter {}

	includedirs {
		"../../../Thirdparty/Assimp/Include/assimp/"
	}

filter { "configurations:Debug" }

	libdirs {
		"../../../Thirdparty/Assimp/Lib/Debug/"
	}

	links {
		"assimp-vc142-mtd"
	}

filter { "configurations:not Debug" }

	libdirs {
		"../../../Thirdparty/Assimp/Lib/Release/"
	}

	links {
		"assimp-vc142-mt"
	}
