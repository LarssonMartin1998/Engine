#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <fstream>;
#include <string.h>
#include <Esent.h>

class FileSystem
{
public:

	FileSystem();
	FileSystem(const FileSystem&);
	~FileSystem();

	bool Initialize();
	void Shutdown();

	char* LoadFileToBuffer(std::string);
	std::ifstream LoadFileToStream(std::string);
	void CloseCurrentFile();
	std::string GetCorrectPath(std::string);
	WCHAR* GetCorrectPathShader(WCHAR* path);

private:

	struct FileSystemConstants
	{
	public:

		const std::string pathRunFromExecutable = "Engine\Binaries\x86\Debug";
		const std::string pathRunFromSolution = "Engine\Build\Engine\..\..\Binaries\x86\Debug";
	};

	bool CalculateCurrentProgramPath();
	void CalculateIsRunningFromExecutable();

	bool isInitialized = false;

	const FileSystemConstants constants;
	std::string programsPath;
	bool isRunningFromExecutable;

	std::ifstream* currentStream;

};

#endif