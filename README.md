# GT-Builder
GT-Builder is a tool I created to meet the needs of the small utilities I use in my engine.  
To use, simply compile with a C compiler such as clang/gcc, preferably called GT executable so that it is universally compatible.  

## Platform Support Status
![Windows](https://img.shields.io/badge/Windows-Supported-green) =>
![Windows](https://img.shields.io/badge/guid-OK-green)
![Windows](https://img.shields.io/badge/mkdir-OK-green)
![Windows](https://img.shields.io/badge/rmdir-OK-green)
![Windows](https://img.shields.io/badge/rm-NOT-red)
![Windows](https://img.shields.io/badge/mk-NOT-red)
![Windows](https://img.shields.io/badge/cat-OK-green)
![Windows](https://img.shields.io/badge/echo-OK-green)  

![Linux](https://img.shields.io/badge/Linux-Supported-green) =>
![Linux](https://img.shields.io/badge/uuid-OK-green)
![Linux](https://img.shields.io/badge/mkdir-OK-green)
![Linux](https://img.shields.io/badge/rmdir-OK-green)
![Linux](https://img.shields.io/badge/rm-OK-green)
![Linux](https://img.shields.io/badge/mk-OK-green)
![Linux](https://img.shields.io/badge/cat-OK-green)
![Linux](https://img.shields.io/badge/echo-OK-green)  

## Compiling
gcc GameTemplate.c -o GT
## Using
./GT --echo My GT-Builder

## Examples
./GT --mkdir MyFolder/SubFolder/EndFolder  
./GT --rmdir MyFolder/SubFolder  
./GT --rm MyFolder/SubFolder/File1.h MyFolder2/SubFolder2/File.c  
./GT --rm -d MyFolder/SubFolder  
./GT --mk  MyFolder/SubFolder/File1.h MyFolder2/SubFolder2/File.c  
./GT --mk  -d Source/Game File1.h File.c  
./GT --cat File1.txt print on screen  
./GT --cat File1.txt -write OutFile.txt Write to File  
./GT --cat File1.txt -append OutFile.txt Append to File  
./GT --echo My Name is Gabriel print on screen  
./GT --echo My Name is Gabriel -write OutFile.txt Write to File  
./GT --echo My Name is Gabriel -append OutFile.txt Append to File  

### --mkdir or --rmdir
Used to recursively create or remove folders.  
Combines with subfolders will cause them to be created.  
If you have files and subfolders, everything will be deleted.  

## --rm or --mk
Remove one or several files passed with '-rm'.  
Passing '-d' with '--rm' all files in the specified folder will be deleted, subfolders will not be affected!  
Create blank files, but you can create several at once.  
It also creates subfolders if there is no file path.  
Passing '-d' followed by a path '--mk' creates a folder and adds all the files inside it.  

### --cat
Pass one or more input files to the terminal or use the "-write" or "-append" operators for an output file.
### --echo
Pass one or more input strings to the terminal, or use the "-write" or "-append" operators for file.  

