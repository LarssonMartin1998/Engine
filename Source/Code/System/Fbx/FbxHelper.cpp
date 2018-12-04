#include "FbxHelper.h"

#include "fbxsdk/scene/fbxaxissystem.h"

FbxHelper::FbxHelper()
: fbxManager (nullptr)
, IOSettings (nullptr)
{

}

FbxHelper::FbxHelper(const FbxHelper& /*fbxHelper*/)
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
	IOSettings->SetBoolProp(IMP_FBX_PIVOT, true);
	IOSettings->SetBoolProp(IMP_FBX_CONSTRAINT, false);
	IOSettings->SetBoolProp(IMP_FBX_MATERIAL, false);
	IOSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
	IOSettings->SetBoolProp(IMP_FBX_MODEL, true);
	IOSettings->SetBoolProp(IMP_FBX_AUDIO, false);
	IOSettings->SetBoolProp(IMP_FBX_ANIMATION, false);
	IOSettings->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, false);
	IOSettings->SetBoolProp(IMP_FBX_LINK, false);
	IOSettings->SetBoolProp(IMP_FBX_SHAPE, true);
	IOSettings->SetBoolProp(IMP_FBX_GOBO, false);
	IOSettings->SetBoolProp(IMP_FBX_CHARACTER, false);
	IOSettings->SetBoolProp(IMP_FBX_TEMPLATE, false);

	fbxManager->SetIOSettings(IOSettings);

	return true;
}

void FbxHelper::Shutdown()
{
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

void FbxHelper::ConvertSceneToLeftHandAndMeters(FbxScene* scene)
{
	//FbxAxisSystem::DirectX.ConvertScene(scene);
	//FbxSystemUnit::m.ConvertScene(scene);
}