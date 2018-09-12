#pragma once

#ifndef _INPUT_H_
#define _INPUT_H_

#define MAX_NUM_KEYS 256

class Input
{
public:

	Input();
	~Input();

	bool Initialize();
	void Shutdown();

	void KeyDown(unsigned);
	void KeyUp(unsigned);
	
	bool IsKeyDown(unsigned);

private:

	bool keys[MAX_NUM_KEYS];

};

#endif