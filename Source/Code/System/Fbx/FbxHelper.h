#ifndef _FBXHELPER_H_
#define _FBXHELPER_H_

#include "fbxsdk.h"

class FbxHelper
{
public:

	FbxHelper();
	FbxHelper(const FbxHelper&);
	~FbxHelper();

	FbxManager* GetFbxManager() { return fbxManager; }

	void ConvertSceneToLeftHandAndMeters(FbxScene*);

	bool Initialize();
	void Shutdown();

private:

	FbxManager* fbxManager;
	FbxIOSettings* IOSettings;

};

#endif