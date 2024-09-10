#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE          1024
#define BUILDER_MAX_PATH     256
#define BUILDER_UUID_SIZE    40
#define BUILDER_INITIAL_SIZE 10
#define VB_LOG(Format, ...)  fprintf(stderr, Format, ##__VA_ARGS__)

typedef void Void;
typedef bool Bool;
typedef char Char;
typedef short Int16;
typedef int Int32;
typedef long long Int64;
typedef unsigned char UChar;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;
typedef float Float;
typedef double Double;
typedef const char* String;

typedef struct PDir PDir;
typedef struct PFile PFile;

Bool FBuilderProcess(UInt64 Flag, String* Args);
Bool PDirMake(String Path);
Bool PDirRemove(String Path);

int main(int argc, const char** argv) {
  VB_LOG("Init GT-Builder\n");
  if(FBuilderProcess(0, argv)) {
    return 0;
  }

  return 0;
}

// Builder Framework /=======================================/

Bool PGenerateUniqueID(Char* Buffer);
Bool FBuilderEcho(String* Args);
Bool FBuilderCat(String* Args);

Bool FBuilderProcess(UInt64 Flag, String* Args) {
  Bool retVal = false;
  while(*Args) {
    if(strcmp(*Args, "--mkdir") == 0) {
      VB_LOG("Builder => GT-DirMake\n");
      PDirMake(*(++Args));
      retVal = true;
      break;
    }
    if(strcmp(*Args, "--rmdir") == 0) {
      VB_LOG("Builder => GT-RmDir\n");
      PDirRemove(*(++Args));
      retVal = true;
      break;
    }
    if(strcmp(*Args, "--echo") == 0) {
      VB_LOG("Builder => GT-Echo\n");
      FBuilderEcho(++Args);
      retVal = true;
      break;
    }
    if(strcmp(*Args, "--cat") == 0) {
      VB_LOG("Builder => GT-Cat\n");
      FBuilderCat(++Args);
      retVal = true;
      break;
    }

    Args++;
  }

  return retVal;
}

Bool FBuilderEcho(String* Args) {
  FILE* fileOutput = NULL;
  struct {
    Char* data;
    UInt32 count;
    UInt32 size;
  } bufferMessage;

  bufferMessage.size = BUFFER_SIZE;
  bufferMessage.data = (Char*)calloc(1, bufferMessage.size);
  bufferMessage.count = 0;
  if(bufferMessage.data == NULL) {
    VB_LOG("Buffer Message Not Allocated\n");
    return false;
  }

  while(*Args) {
    if(strcmp(*Args, ">") == 0) {
      fileOutput = fopen(*(++Args), "w");
      if(fileOutput == NULL) {
        VB_LOG("File %s not Opened\n", *Args);
        return false;
      }
      break;

    } else if(strcmp(*Args, ">>") == 0) {
      fileOutput = fopen(*(++Args), "a");
      if(fileOutput == NULL) {
        VB_LOG("File %s not Opened\n", *Args);
        return false;
      }
      break;

    } else {
      UInt64 size = strlen(*Args) + 1;
      if(bufferMessage.count + size >= bufferMessage.size) {
        bufferMessage.size *= 2;
        Char* newData = (Char*)realloc(bufferMessage.data, bufferMessage.size);
        if(newData == NULL) {
          VB_LOG("Buffer Message not Reallocated\n");
          free(bufferMessage.data);
          return false;
        }
        bufferMessage.data = newData;
      }
      memcpy(bufferMessage.data + bufferMessage.count, *Args, size);
      bufferMessage.count += size;
      bufferMessage.data[bufferMessage.count - 1] = ' ';
    }
    Args++;
  }

  bufferMessage.data[bufferMessage.count - 1] = '\n';
  bufferMessage.data[bufferMessage.count] = '\0';
  if(fileOutput != NULL) {
    fprintf(fileOutput, "%s", bufferMessage.data);
    fclose(fileOutput);
  } else {
    printf("%s", bufferMessage.data);
  }

  free(bufferMessage.data);
  return true;
}

Bool FBuilderCat(String* Args) {
  FILE* fileOutput = NULL;
  struct {
    Char** data;
    UInt32 count;
    UInt32 size;
  } bufferList;

  bufferList.size = BUILDER_INITIAL_SIZE;
  bufferList.data = (Char**)calloc(bufferList.size, sizeof(Char*));
  bufferList.count = 0;

  if(bufferList.data == NULL) {
    VB_LOG("bufferList Not Valid\n");
    return false;
  }

  while(*Args) {
    if(strcmp(*Args, ">>") == 0) {
      VB_LOG("Cat Append\n");
      fileOutput = fopen(*(++Args), "a");
      if(fileOutput == NULL) { return false; }

    } else if(strcmp(*Args, ">") == 0) {
      VB_LOG("Cat Write\n");
      fileOutput = fopen(*(++Args), "w");
      if(fileOutput == NULL) { return false; }

    } else {
      FILE* file = fopen(*Args, "r");
      UInt64 size = 0;
      if(file == NULL) {
        VB_LOG("File not exist -> %s\n", *Args);
        return false;
      }

      fseek(file, 0, SEEK_END);
      size = ftell(file);
      fseek(file, 0, SEEK_SET);

      if(bufferList.count >= bufferList.size) {
        bufferList.size *= 2;
        Char** tempData = (Char**)realloc(bufferList.data, bufferList.size);
        bufferList.data = (tempData != NULL) ? tempData : bufferList.data;
        break;
      }

      Char* data = (Char*)calloc(1, size + 1);
      if(!data) { return false; }
      fread(data, 1, size, file);
      fclose(file);
      bufferList.data[bufferList.count++] = data;
      Args++;
    }
  }

  if(fileOutput != NULL) {
    for(UInt32 c = 0; c < bufferList.count; c++) {
      fprintf(fileOutput, "%s", bufferList.data[c]);
      fflush(fileOutput);
    }
    fclose(fileOutput);
  } else {
    for(UInt32 c = 0; c < bufferList.count; c++) {
      printf("%s", bufferList.data[c]);
    }
  }

  for(UInt32 c = 0; c < bufferList.count; c++) {
    free(bufferList.data[c]);
  }
  free(bufferList.data);
  return true;
}

// Platform /=======================================/

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

typedef HRESULT (*FOnCoCreateGuid)(GUID* pguid);

Bool PGenerateUniqueID(Char* Buffer) {
  if(!Buffer) {
    VB_LOG("Buffer is NULL\n");
    return false;
  }
  HMODULE libOle32 = LoadLibraryA("ole32.dll");
  FOnCoCreateGuid onCoCreateGuid = (FOnCoCreateGuid)GetProcAddress(libOle32, "CoCreateGuid");
  if(!onCoCreateGuid) {
    VB_LOG("CoCreateGuid is NULL\n");
    return false;
  }

  GUID guid;
  if(onCoCreateGuid(&guid) != S_OK) {
    VB_LOG("CoCreateGuid failed\n");
    return false;
  }

  UInt64 data4 = 0;
  data4 |= ((UInt64)guid.Data4[0]) << 8;
  data4 |= ((UInt64)guid.Data4[1]);
  data4 <<= 48;
  data4 |= ((UInt64)guid.Data4[2]) << 40;
  data4 |= ((UInt64)guid.Data4[3]) << 32;
  data4 |= ((UInt64)guid.Data4[4]) << 24;
  data4 |= ((UInt64)guid.Data4[5]) << 16;
  data4 |= ((UInt64)guid.Data4[6]) << 8;
  data4 |= ((UInt64)guid.Data4[7]);

  snprintf(Buffer, BUILDER_UUID_SIZE, "{%08lX-%04X-%04X-%04X-%012llX}", guid.Data1, guid.Data2, guid.Data3, (guid.Data4[0] << 8) | guid.Data4[1], data4);

  return true;
}

Bool PDirMake(String Path) {
  Char pathBuffer[BUILDER_MAX_PATH] = {0};
  UInt32 count = 0;
  while(*Path) {
    if(*Path == '/' || *Path == '\\') {
      CreateDirectoryA(pathBuffer, NULL);
    }
    pathBuffer[count++] = *Path;
    Path++;
  }

  return CreateDirectoryA(pathBuffer, NULL);
}

Bool PDirRemove(String Path) {
  WIN32_FIND_DATAA findData;
  HANDLE hFind;
  Char pathBuffer[BUILDER_MAX_PATH] = {0};

  snprintf(pathBuffer, sizeof(pathBuffer), "%s\\*", Path);
  hFind = FindFirstFileA(pathBuffer, &findData);

  if(hFind == INVALID_HANDLE_VALUE) {
    return false;
  }

  do {
    if(strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
      continue;
    }

    Char fullPath[BUILDER_MAX_PATH] = {0};
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", Path, findData.cFileName);
    VB_LOG("FullPath:%s\n", findData.cFileName);

    if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      PDirRemove(fullPath);
      RemoveDirectoryA(fullPath);
    } else {
      DeleteFileA(fullPath);
    }

  } while(FindNextFileA(hFind, &findData));

  FindClose(hFind);
  return RemoveDirectoryA(Path);
}

#elif defined(__linux__)

#endif
