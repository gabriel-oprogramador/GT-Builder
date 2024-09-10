# GT-Builder
GT-Builder is a tool I created to meet the needs of the small utilities I use in my engine.  
To use, simply compile with a C compiler such as clang/gcc, preferably called GT executable so that it is universally compatible.  

## Platform Support Status
![Windows](https://img.shields.io/badge/Windows-OK-green) =>
![Windows](https://img.shields.io/badge/mkdir-Suported-green)
![Windows](https://img.shields.io/badge/rmdir-Suported-green)
![Windows](https://img.shields.io/badge/cat-Suported-green)
![Windows](https://img.shields.io/badge/echo-Suported-green)  

![Linux](https://img.shields.io/badge/Linux-Not-red) =>
![Linux](https://img.shields.io/badge/mkdir-Unsupported-red)
![Linux](https://img.shields.io/badge/rmdir-Unsupported-red)
![Linux](https://img.shields.io/badge/cat-Unsupported-red)
![Linux](https://img.shields.io/badge/echo-Unsupported-red)  

## Compiling
gcc GameTemplate.c -o GT
## Using
./GT --echo My GT-Builder

## Examples
./GT --mkdir MyFolder/SubFolder/EndFolder  
./GT --rmdir MyFolder/SubFolder  
./GT --cat File1.txt print on screen  
./GT --cat File1.txt > OutFile.txt Write to File  
./GT --cat File1.txt >> OutFile.txt Append to File  
./GT --echo My Name is Gabriel print on screen 
./GT --echo My Name is Gabriel > OutFile.txt Write to File  
./GT --echo My Name is Gabriel > OutFile.txt Append to File  

### --mkdir or --rmdir
Used to recursively create or remove folders.  
Combines with subfolders will cause them to be created.  
If you have files and subfolders, everything will be deleted.  

### --cat
Pass one or more input files to the terminal or use the ">" or ">>" operators for an output file.
### --echo
Pass one or more input strings to the terminal, or use the ">" or ">>" operators for file.  

