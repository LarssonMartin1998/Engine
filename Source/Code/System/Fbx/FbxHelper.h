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
	FbxIOSettings* GetIOSettings() { return IOSettings; }
	FbxImporter* GetImporter() { return importer; }

	bool Initialize();
	void Shutdown();

private:

	FbxManager* fbxManager;
	FbxIOSettings* IOSettings;
	FbxImporter* importer;

};

#endif