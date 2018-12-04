#include "FileSystem.h"

#include <windows.h>
#include <assert.h>

FileSystem::FileSystem()
: currentStream (nullptr)
{

}

FileSystem::FileSystem(const FileSystem& /*fileSystem*/)
{

}

FileSystem::~FileSystem()
{

}

bool FileSystem::Initialize()
{
	assert(!isInitialized);

	bool result = CalculateCurrentProgramPath();
	if (!result)
	{
		return false;
	}

	CalculateIsRunningFromExecutable();

	isInitialized = true;
	return isInitialized;
}

void FileSystem::Shutdown()
{
	assert(isInitialized);

	CloseCurrentFile();

	isInitialized = false;
}

bool FileSystem::CalculateCurrentProgramPath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	int lastPositionWithBackslash = 0;
	for (int charIndex = 0; charIndex < MAX_PATH; ++charIndex)
	{
		char& c = buffer[charIndex];

		if (c == '\\')
		{
			lastPositionWithBackslash = charIndex;
		}

		if (c == '\0')
		{
			programsPath = std::string(buffer).substr(0, lastPositionWithBackslash);
			return true;
		}
	}

	return false;
}

void FileSystem::CalculateIsRunningFromExecutable()
{
	// This method is only necessary since my solution is in a different dir than my output .exe file.
	// When trying to say load file from a program run directly from VS, it tries to load files from the solutions dir.
	for (int cIndex = programsPath.length(); cIndex > 0; cIndex--)
	{
		char& c = programsPath[cIndex];
		if (c == '.' && (programsPath[cIndex + 1] == '.' || programsPath[cIndex - 1] == '.'))
		{
			isRunningFromExecutable = false;
			return;
		}
	}

	isRunningFromExecutable = true;
}

char* FileSystem::LoadFileToBuffer(std::string path)
{
	assert(isInitialized);

	std::string newPath = GetCorrectPath(path);

	std::ifstream file;
	file.open(newPath);

	assert(!file.fail());

	file.seekg(0, file.end);
	int fileLength = file.tellg();

	char* buffer = new char[fileLength];
	file.read(buffer, fileLength);

	file.close();

	return buffer;
}

// Needs to call close when done with the stream.
std::ifstream FileSystem::LoadFileToStream(std::string path)
{
	assert(isInitialized);

	std::string newPath = GetCorrectPath(path);

	std::ifstream file;
	file.open(newPath);

	assert(!file.fail());

	currentStream = &file;

	return file;
}

void FileSystem::CloseCurrentFile()
{
	assert(isInitialized);

	if (currentStream && currentStream->is_open())
	{
		currentStream->close();
		currentStream = nullptr;
	}
}

std::string FileSystem::GetCorrectPath(std::string path)
{
	std::string newPath = "../../";

	if (!isRunningFromExecutable)
	{
		newPath.append("Binaries/");
	}

	newPath.append("data/" + path);

	return newPath;
}

WCHAR* FileSystem::GetCorrectPathShader(WCHAR* filename)
{
	WCHAR* prepend = isRunningFromExecutable ? L"../../../Source/Code/Shaders/" : L"../../Source/Code/Shaders/";
	const int length = wcslen(prepend) + wcslen(filename);
	WCHAR* correctPath = new WCHAR[length];
	wcscpy(correctPath, prepend);
	wcscat(correctPath, filename);
	return correctPath;
}