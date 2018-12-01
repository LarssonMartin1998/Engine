#include "Input.h"

Input::Input()
{

}

Input::Input(const Input& /*input*/)
{

}

Input::~Input()
{

}

bool Input::Initialize()
{
	// Sets all keys to false (not being pressed);
	for (int keyIndex = 0; keyIndex < MAX_NUM_KEYS; ++keyIndex)
	{
		keys[keyIndex] = false;
	}

	return true;
}

void Input::Shutdown()
{

}

void Input::KeyDown(unsigned key)
{
	keys[key] = true;
}

void Input::KeyUp(unsigned key)
{
	keys[key] = false;
}

bool Input::IsKeyDown(unsigned key)
{
	return keys[key];
}