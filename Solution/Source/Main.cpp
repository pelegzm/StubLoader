/*
	Author:		Peleg Zahavy-Mittelman
	Date:		11/14/2024
	File:		Main.cpp
	----------------------------------
	Brief:		The Window's Application Entry Point
*/

//----------------------------------------------
// Included Headers:
//----------------------------------------------

#include <Windows.h>	// winMain
#include <iostream>		// std::cout
#include <filesystem>	// std::filesystem
#include <string>		// std::string
#include <map>			// std::map
#include <fstream>		// std::ofstream, std::ifstream

//----------------------------------------------
// Application Entry Point:
//----------------------------------------------

FILE* ConsoleStream;
using StubMapType = std::map<std::string, std::string>;
using StubPair = StubMapType::value_type;
StubMapType StubMap;
std::string userName;
std::string userFullName;

void CreateConsole();
void ScanStubDirectory();
void StartStubLoader();
void DestroyConsole();
void PromptUserName();
void PromptStubTypes();
StubPair const& FindPairAt(const unsigned int);
void PromptStubName(StubPair const&);
void CreateStubFiles(StubPair const&, std::string const&, std::string const&);
void PromptDescription(StubPair const&, std::string const&);
std::string GetFileAsString(std::string const&);
void WriteToFile(std::string const&, std::string const&);
void ClearConsole() { system("CLS"); }
void PauseConsole() { system("pause"); }
void PromptUser();
void FindAndReplaceAll(std::string&, std::string const&, std::string const&);
void CopyAndModify(std::string const&, std::string const&, std::string const&, std::string const&);
void PromptDestination(std::string const&, std::vector<std::string> const&);
void MoveFileFromTo(std::string const&, std::string const&);
void MoveNewFiles(std::string const&, std::vector<std::string> const&);
void PromptNewFolderName(std::string const&, std::vector<std::string> const&);

int CALLBACK WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	CreateConsole();
	ScanStubDirectory();
	StartStubLoader();
	DestroyConsole();
	return 0;
}

void ScanStubDirectory()
{
	std::string pathName = "Stubs";

	// validate that the entry exists
	if (!std::filesystem::exists(pathName) || std::filesystem::is_empty(pathName))
	{
		std::cout << "Please create a Stubs folder, and place at least 1 file inside." << std::endl << std::endl;
		PauseConsole();
		exit(-1);
	}

	constexpr size_t StubStrLength = 4;
	std::filesystem::directory_iterator directory(pathName);
	for (const auto& entry : directory)
	{
		std::filesystem::path const& path(entry.path());
		std::string fileStem(path.stem().string());
		std::string stubType(fileStem);
		if (fileStem.length() != StubStrLength)
		{
			stubType.erase(stubType.find_first_of("Stub"), StubStrLength);
		}
		StubMap.emplace(std::pair{ fileStem, stubType });
	}
}

void PromptUserName()
{
	std::vector<std::string> users;
	std::ifstream usersFile;
	usersFile.open("Users.txt");
	if (!usersFile.is_open() || !usersFile.good())
	{
		std::cout << "Users.txt file is empty" << std::endl;
		PauseConsole();
		exit(-1);
	}

	std::string userText;
	while (std::getline(usersFile, userText))
	{
		users.push_back(userText);
	}

	std::cout << "Select a user:" << std::endl;
	int i = 0;
	for (auto const& user : users)
	{
		std::cout << i << ". " << user << std::endl;
		++i;
	}

	std::cout << std::endl << "Enter the number corresponding to your name: ";

	try
	{
		std::string nameIndexToken;
		std::cin >> nameIndexToken;

		int nameIndex = std::stoi(nameIndexToken);

		if (nameIndex < 0 || nameIndex >= users.size())
		{
			ClearConsole();
			PromptUserName();
			return;
		}

		std::string user = users[nameIndex];
		userFullName = user;
		userName = user;
		const size_t space = user.find(' ');
		if (space != std::string::npos)
		{
			userName = user.substr(0, space);
		}
	}
	catch (std::exception)
	{
		ClearConsole();
		PromptUserName();
		return;
	}
}

void StartStubLoader()
{
	PromptUserName();
	PromptStubTypes();

	// Ask the user which stub type to use
	std::string token;
	int stubTypeIndex = -1;
	while (std::cin >> token)
	{
		try
		{
			stubTypeIndex = std::stoi(token);
			if (stubTypeIndex == 0 || stubTypeIndex > StubMap.size())
			{
				PromptStubTypes();
				continue;
			}
			break;
		}
		catch (std::exception const& except)
		{
			if (token == "exit") return;
			(void)except;
			PromptStubTypes();
		}
	}

	PromptStubName(FindPairAt(stubTypeIndex));
}

void PromptStubName(StubPair const& stubPair)
{
	ClearConsole();
	std::string const& fileStem = stubPair.first;
	std::string const& stubType = stubPair.second;

	std::cout << "Commands: [back] / [cancel] to go back, [exit] to close the program." << std::endl << std::endl;
	std::cout << "What would you like to name your new " << stubType << " file?" << std::endl;

	PromptUser();

	std::string stubName;
	std::cin >> stubName;

	if (stubName.find("Stub") != std::string::npos || stubName.find("stub") != std::string::npos)
	{
		ClearConsole();
		std::cout << "File name cannot contain \"Stub\"." << std::endl;
		std::cout << "Please choose a different name." << std::endl << std::endl;
		PauseConsole();
		PromptStubName(stubPair);
		return;
	}

	if (stubName == "back" || stubName == "cancel")
	{
		StartStubLoader();
		return;
	}

	if (stubName == "exit")
	{
		return;
	}

	ClearConsole();

	std::cout << "You typed: " << stubName << std::endl << std::endl;
	std::cout << "Is this correct? (Y/N)" << std::endl;
	PromptUser();

	std::string yesNo;
	std::cin >> yesNo;

	if (std::tolower(yesNo[0]) == 'y')
	{
		PromptDescription(stubPair, stubName);
		return;
	}

	PromptStubName(stubPair);
}

void PromptDescription(StubPair const& stubPair, std::string const& stubName)
{
	ClearConsole();
	std::cout << "Perfect! Give me a brief description of what the " << stubName << " class does:" << std::endl << std::endl;
	PromptUser();

	// read the description from the user
	std::string brief;
	std::cin.ignore();
	std::getline(std::cin, brief);

	CreateStubFiles(stubPair, stubName, brief);
}

void CreateStubFiles(StubPair const& stubPair, std::string const& stubName, std::string const& brief)
{
	std::string pathName = "Stubs";
	std::filesystem::directory_iterator directory(pathName);

	std::vector<std::string> fileNames;

	for (const auto& entry : directory)
	{
		std::filesystem::path const& path(entry.path());
		std::string fileStem(path.stem().string());
		if (fileStem == stubPair.first)
		{
			CopyAndModify(stubPair.first, stubName, brief, path.extension().string());
			fileNames.push_back(stubName + path.extension().string());
		}
	}

	PromptDestination(".", fileNames);

	PauseConsole();
}

void PromptDestination(std::string const& path, std::vector<std::string> const& fileNames)
{
	ClearConsole();
	std::cout << "Perfect! Where would you like to place the new files?" << std::endl;

	std::filesystem::directory_iterator currentDirectory(path);

	std::cout << std::endl;
	std::cout << "> " << std::filesystem::path(path).parent_path().stem().string() << std::endl;

	int i = 0;
	std::cout << "  " << i << ") .." << std::endl;
	std::cout << "  " << ++i << ") ." << std::endl;

	for (const auto& entry : currentDirectory)
	{
		std::filesystem::path const& path(entry.path());
		if (!path.has_extension()) // is a folder
			std::cout << "  " << ++i << ") " << path.stem().string() << std::endl;
	}

	std::cout << "  " << ++i << ") Create a new folder." << std::endl;

	PromptUser();

	int userSelection;
	std::cin >> userSelection;

	if (userSelection > i)
	{
		PromptDestination(path, fileNames);
		return;
	}

	if (userSelection == 0)	// go to parent directory
	{
		PromptDestination(path + "/..", fileNames);
	}

	if (userSelection == 1) // place the files in this directory
	{
		MoveNewFiles(path, fileNames);
		return;
	}

	if (userSelection == i) // create new directory
	{
		PromptNewFolderName(path, fileNames);
		// Prompt the user to name the new folder
		// Verify the name
		// If yes, then move the files over there
		// If not, then prompt the name again
			// "Back" should call PromptDestination again
		return;
	}

	// selected an existing folder
	std::filesystem::directory_iterator directoryIter(path);
	for (int f = 2; !directoryIter._At_end(); ++directoryIter)
	{
		auto const& path = *directoryIter;
		if (!path.path().has_extension()) // is a folder
		{
			if (f == userSelection)
			{
				PromptDestination(path.path().string(), fileNames);
				return;
			}
			++f;
		}
	}
}

void PromptNewFolderName(std::string const& path, std::vector<std::string> const& fileNames)
{
	ClearConsole();
	std::cout << "Commands: [back] / [cancel] to go back, [exit] to close the program." << std::endl << std::endl;
	std::cout << "What would you like to name your new directory?" << std::endl;
	std::cout << path << "/";

	std::string folderName;
	std::cin >> folderName;

	if (folderName == "back" || folderName == "cancel")
	{
		StartStubLoader();
		return;
	}

	if (folderName == "exit")
	{
		for (auto const& fileName : fileNames)
		{
			std::wstringstream stream;
			stream << fileName.c_str();
			DeleteFile(stream.str().c_str());
		}
		return;
	}

	ClearConsole();

	std::cout << "You typed: " << folderName << std::endl << std::endl;
	std::cout << "Is this correct? (Y/N)" << std::endl;
	PromptUser();

	std::string yesNo;
	std::cin >> yesNo;

	if (std::tolower(yesNo[0]) == 'y')
	{
		std::string command = "mkdir " + folderName;
		system(command.c_str());
		MoveNewFiles(path + "/" + folderName, fileNames);
		return;
	}

	PromptNewFolderName(path, fileNames);
}

void MoveNewFiles(std::string const& path, std::vector<std::string> const& fileNames)
{
	for (auto const& fileName : fileNames)
	{
		MoveFileFromTo(fileName, path + "/" + fileName);
	}
}

void MoveFileFromTo(std::string const& location, std::string const& destination)
{
	std::wstringstream wlocation;
	wlocation << location.c_str();

	std::wstringstream wdestination;
	wdestination << destination.c_str();

	MoveFile(wlocation.str().c_str(), wdestination.str().c_str());
}

void CopyAndModify(std::string const& inPath, std::string const& outPath, std::string const& brief, std::string const& extension)
{
	std::string inContent = GetFileAsString(inPath + extension);

	// modify the file contents
	FindAndReplaceAll(inContent, inPath, outPath);

	// replace the author name
	FindAndReplaceAll(inContent, "NAME", userFullName);

	// replace the brief
	FindAndReplaceAll(inContent, "Insert description here...", brief);

	// replace the date
	std::chrono::time_point now{ std::chrono::system_clock::now() };
	std::chrono::year_month_day date{ std::chrono::floor<std::chrono::days>(now) };
	FindAndReplaceAll(inContent, "MM/DD/YYYY", std::format("{:%m/%d/%EY}", date));

	// write the modified file contents to output files
	const std::string sourcePath = outPath + extension;
	WriteToFile(sourcePath, inContent);

	std::cout << sourcePath << " created successfully." << std::endl;
}

void FindAndReplaceAll(std::string& str, std::string const& search, std::string const& replace)
{
	size_t off;
	for (off = str.find(search.data()); off != std::string::npos; off = str.find(search.data()))
	{
		str.replace(str.begin() + off, str.begin() + off + search.size(), replace.data());
	}
}

std::string GetFileAsString(std::string const& filePath)
{
	std::ifstream file;
	file.open("Stubs/" + filePath);

	std::string content;
	for (std::string line; std::getline(file, line);)
	{
		content += line += '\n';
	}

	file.close();
	return content;
}

void WriteToFile(std::string const& filePath, std::string const& fileContents)
{
	std::ofstream file;
	file.open(filePath);
	file << fileContents;
	file.close();
}

StubPair const& FindPairAt(const unsigned int index)
{
	int i = 1;
	for (StubPair const& stubEntry : StubMap)
	{
		if (i == index)
		{
			return stubEntry;
		}
		++i;
	}
	throw std::out_of_range("index out of range");
}

void PromptStubTypes()
{
	ClearConsole();
	std::cout << "Hello, " << userName << "! What can I help you with today?" << std::endl << std::endl;

	// Prompt the user with the stub types
	unsigned int i = 1;
	for (auto const& stubPair : StubMap)
	{
		std::cout << i << ". Create a " << stubPair.second << std::endl;
		++i;
	}

	PromptUser();
}

void PromptUser()
{
	std::cout << std::endl << ">> ";
}

void CreateConsole()
{
	// Allocate a console
	AllocConsole();

	// Allow write for stdin & stdout
	freopen_s(&ConsoleStream, "CONIN$", "r", stdin);
	freopen_s(&ConsoleStream, "CONOUT$", "w", stdout);
}

void DestroyConsole()
{
	FreeConsole();
	fclose(ConsoleStream);
}
