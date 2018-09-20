#include "FbxHelper.h"

FbxHelper::FbxHelper()
: fbxManager (nullptr)
, IOSettings (nullptr)
, importer (nullptr)
{

}

FbxHelper::FbxHelper(const FbxHelper& fbxHelper)
{

}

FbxHelper::~FbxHelper()
{

}

bool FbxHelper::Initialize()
{
	fbxManager = FbxManager::Create();
	if (!fbxManager)
	{
		return false;
	}

	IOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	if (!IOSettings)
	{
		return false;
	}

	fbxManager->SetIOSettings(IOSettings);

	importer = FbxImporter::Create(fbxManager, "");
	if (!importer)
	{
		return false;
	}

	return true;
}

void FbxHelper::Shutdown()
{
	if (importer)
	{
		importer->Destroy();
		importer = nullptr;
	}

	if (IOSettings)
	{
		IOSettings->Destroy();
		IOSettings = nullptr;
	}

	if (fbxManager)
	{
		fbxManager->Destroy();
		fbxManager = nullptr;
	}
}