# Stub Loader
### Getting Started
1) Place the stub files into the `Stubs` folder.
   - See the `Stub File Format` below
3) Run the `StubLoader.exe` executable.
4) The newly created files will be placed in the root folder.
5) Add each of the user names into the `Users.txt` file under the `Build` folder
    - Each user's name should be placed on a new line

### Stub File Format
- The file **must** contain the keyword `Stub` in the name
    - The program will prompt for a new token to replace the `Stub` token inside the filename
    - Examples: StubScene, StubComponent
- The file extension can be anything
- You can have any number of extensions with the same stem (e.g. StubBehavior.cpp, StubBehavior.h, StubBehavior.lua, etc.)

### Search and Replace Tokens
- `NAME` will be replaced with the author's full name
- `MM/DD/YYYY` will be replaced with the current date
- `Insert description here...` will be replaced with the description
- All instances of `Stub` will be replaced with the user's selected name (in the program)
      - This is a general search and replace, meaning the case-sensitive token `Stub` can be a standalone token, in the middle of a larger token, or at the end of a word/token

### Example Header Comment
```cpp
/*
	Author:		NAME
	Date:		MM/DD/YYYY
	File:		Stub.h
	----------------------------------
	Brief:		Insert description here...
*/
```
