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
typedef struct TArray TArray;
Bool FBuilderProcess(UInt64 Flag, String* Args);
Bool PGenerateUniqueID(Char* Buffer);
Bool PFileMake(String Path);
Bool PFileRemove(String Path);
Bool PDirMake(String Path);
Bool PDirRemove(String Path);
UInt32 TArrayGetSize(TArray* Array);
UInt32 TArrayGetMaxSize(TArray* Array);
TArray* TArrayNew(UInt32 InitialSize);
Void TArrayFree(TArray* Array);
Char* TArrayPushSize(TArray* Array, UInt64 Size);
Void TArrayRemove(TArray* Array, Char* Buffer);
Char* TArrayGet(TArray* Array, UInt32 Index);

int main(int argc, const char** argv) {
  VB_LOG("Init GT-Builder\n");
  if(FBuilderProcess(0, argv)) {
    return 0;
  }

  Char UUID[BUILDER_UUID_SIZE] = {0};
  PGenerateUniqueID(UUID);
  printf("UUID:%s\n", UUID);

  return 0;
}

// Builder Framework /=======================================/
typedef struct {
  Char* buffData;
  UInt64 buffSize;
} FEntry;

struct TArray {
  FEntry** entries;
  UInt32 size;
  UInt32 maxSize;
};

UInt32 TArrayGetSize(TArray* Array) {
  return Array->size;
}
UInt32 TArrayGetMaxSize(TArray* Array) {
  return Array->maxSize;
}
TArray* TArrayNew(UInt32 InitialSize) {
  TArray* arr = (TArray*)malloc(sizeof(TArray));
  if(arr == NULL) { return NULL; }
  arr->size = 0;
  arr->maxSize = InitialSize;
  arr->entries = (FEntry**)calloc(arr->maxSize, sizeof(FEntry));
  return arr;
}
Void TArrayFree(TArray* Array) {
  for(UInt32 c = 0; c < Array->size; c++) {
    free(Array->entries[c]->buffData);
    free(Array->entries[c]);
  }
  free(Array->entries);
  free(Array);
}
Char* TArrayPushSize(TArray* Array, UInt64 Size) {
  if(Array->size + 1 >= Array->maxSize) {
    UInt32 newMaxSize = Array->maxSize * 2;
    FEntry** newEntries = (FEntry**)realloc(Array->entries, (Array->maxSize * sizeof(FEntry)));
    if(newEntries == NULL) { return NULL; }
    Array->entries = newEntries;
    Array->maxSize = newMaxSize;
  }
  UInt32 index = Array->size++;
  FEntry* entry = (FEntry*)calloc(1, sizeof(FEntry));
  if(entry == NULL) { return NULL; }
  entry->buffData = (Char*)calloc(1, Size);
  entry->buffSize = Size;
  Array->entries[index] = entry;

  return entry->buffData;
}
Void TArrayRemove(TArray* Array, Char* Buffer) {
  for(UInt32 c = 0; c < Array->size; c++) {
    if(Array->entries[c]->buffData == Buffer) {
      free(Array->entries[c]->buffData);
      free(Array->entries[c]);
      UInt64 size = (Array->size - c - 1) * sizeof(FEntry*);
      memmove(Array->entries[c], Array->entries[c + 1], size);
      Array->size--;
      return;
    }
  }
}
Char* TArrayGet(TArray* Array, UInt32 Index) {
  if(Index >= Array->size) {
    return Array->entries[0]->buffData;
  }
  return Array->entries[Index]->buffData;
}

Bool FBuilderMake(String* Args);
Bool FBuilderRemove(String* Args);
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
      VB_LOG("Builder => GT-DirRemove\n");
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
    if(strcmp(*Args, "--mk") == 0) {
      VB_LOG("Builder => GT-FileMake\n");
      FBuilderMake(++Args);
      retVal = true;
      break;
    }
    if(strcmp(*Args, "--rm") == 0) {
      VB_LOG("Builder => GT-FileRemove\n");
      FBuilderRemove(++Args);
      retVal = true;
      break;
    }

    Args++;
  }

  return retVal;
}

Bool FBuilderMake(String* Args) {
  TArray* filesArray = TArrayNew(10);
  Char pathBuffer[BUILDER_MAX_PATH] = {0};
  UInt64 pathSize = 0;
  while(*Args) {
    if(strcmp(*Args, "-d") == 0) {
      pathSize = strlen(*(++Args)) + 1;
      snprintf(pathBuffer, BUILDER_MAX_PATH, "%s", *(Args++));
      continue;
    }
    UInt64 size = strlen(*Args) + 1 + pathSize;
    Char* data = TArrayPushSize(filesArray, size);
    snprintf(data, size, "%s/%s", ((pathSize > 0) ? pathBuffer : ""), *Args);
    Args++;
  }

  UInt32 maxSize = TArrayGetSize(filesArray);
  for(UInt32 c = 0; c < maxSize; c++) {
    PFileMake(TArrayGet(filesArray, c));
  }

  TArrayFree(filesArray);
  return true;
}

Bool FBuilderRemove(String* Args) {
  TArray* filesArray = TArrayNew(10);
  Char pathBuffer[BUILDER_MAX_PATH] = {0};

  while(*Args) {
    if(strcmp(*(Args++), "-d") == 0) {
      UInt64 size = strlen(*Args) + 1;
      Char* data = TArrayPushSize(filesArray, size);
      snprintf(data, size, "%s", *Args);
      break;
    }
    UInt64 size = strlen(*Args) + 1;
    Char* data = TArrayPushSize(filesArray, size);
    snprintf(data, size, "%s", *Args);
    Args++;
  }

  UInt32 maxSize = TArrayGetSize(filesArray);
  for(UInt32 c = 0; c < maxSize; c++) {
    PFileRemove(TArrayGet(filesArray, c));
  }

  TArrayFree(filesArray);
  return true;
}

Bool FBuilderEcho(String* Args) {
  FILE* fileOutput = NULL;
  TArray* messageArray = TArrayNew(10);

  while(*Args) {
    if(strcmp(*Args, "-write") == 0) {
      fileOutput = fopen(*(++Args), "w");
      if(fileOutput == NULL) {
        VB_LOG("File %s not Opened\n", *Args);
        return false;
      }
      break;

    } else if(strcmp(*Args, "-append") == 0) {
      fileOutput = fopen(*(++Args), "a");
      if(fileOutput == NULL) {
        VB_LOG("File %s not Opened\n", *Args);
        return false;
      }
      break;

    } else {
      UInt64 size = strlen(*Args) + 1;
      Char* data = TArrayPushSize(messageArray, size);
      snprintf(data, size, "%.*s", (UInt32)size, *Args);
    }
    Args++;
  }

  if(fileOutput != NULL) {
    for(UInt32 c = 0; c < messageArray->size; c++) {
      fprintf(fileOutput, "%s", messageArray->entries[c]->buffData);
    }
    fprintf(fileOutput, "\n");
    fclose(fileOutput);
  } else {
    for(UInt32 c = 0; c < messageArray->size; c++) {
      printf("%s", messageArray->entries[c]->buffData);
    }
    printf("\n");
  }

  TArrayFree(messageArray);
  return true;
}

Bool FBuilderCat(String* Args) {
  FILE* fileOutput = NULL;
  TArray* fileBuffers = TArrayNew(10);

  while(*Args) {
    if(strcmp(*Args, "-append") == 0) {
      VB_LOG("Cat Append\n");
      fileOutput = fopen(*(++Args), "ab");
      if(fileOutput == NULL) {
        VB_LOG("File not exist -> %s\n", *Args);
        return false;
      }
      break;

    } else if(strcmp(*Args, "-write") == 0) {
      VB_LOG("Cat Write\n");
      fileOutput = fopen(*(++Args), "wb");
      if(fileOutput == NULL) {
        VB_LOG("File not exist -> %s\n", *Args);
        continue;
      }
      break;

    } else {
      FILE* file = fopen(*Args, "rb");
      UInt64 size = 0;
      if(file == NULL) {
        VB_LOG("File not exist -> %s\n", *Args);
        continue;
      }

      fseek(file, 0, SEEK_END);
      size = ftell(file);
      fseek(file, 0, SEEK_SET);

      Char* data = TArrayPushSize(fileBuffers, size);
      if(!data) { return false; }
      fread(data, 1, size, file);
      fclose(file);
      Args++;
    }
  }

  if(fileOutput != NULL) {
    for(UInt32 c = 0; c < fileBuffers->size; c++) {
      Char* data = fileBuffers->entries[c]->buffData;
      UInt64 size = fileBuffers->entries[c]->buffSize;
      fwrite(data, size, 1, fileOutput);
    }
    fclose(fileOutput);
  } else {
    for(UInt32 c = 0; c < fileBuffers->size; c++) {
      Char* data = fileBuffers->entries[c]->buffData;
      UInt64 size = fileBuffers->entries[c]->buffSize + 1;  // For 0/
      Char* content = (Char*)calloc(1, size);
      if(!content) {
        TArrayFree(fileBuffers);
        return false;
      }
      snprintf(content, size, "%.*s", (UInt32)size, data);
      printf("%s", content);
      free(content);
    }
  }

  TArrayFree(fileBuffers);
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
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <uuid/uuid.h>

Bool PGenerateUniqueID(Char* Buffer) {
  if(Buffer == NULL) {
    VB_LOG("Buffer for UUID is NULL\n");
    return false;
  }
  typedef Void (*FOnUuidGenerate)(uuid_t);
  typedef Void (*FOnUuidUnparseLower)(const uuid_t, Char*);
  Void* libUuid = dlopen("libuuid.so", RTLD_LAZY);
  if(libUuid == NULL) {
    printf("libuuid.so Not Loaderd!!\n");
    return false;
  }
  FOnUuidGenerate onUuidGenerate = dlsym(libUuid, "uuid_generate");
  FOnUuidUnparseLower onUuidParseLower = dlsym(libUuid, "uuid_unparse_lower");

  uuid_t uuid;
  Char uuidBuffer[BUILDER_UUID_SIZE];
  onUuidGenerate(uuid);
  onUuidParseLower(uuid, uuidBuffer);
  snprintf(Buffer, BUILDER_UUID_SIZE, "%s", uuidBuffer);
  dlclose(libUuid);
  return true;
}

Bool PFileMake(String Path) {
  UInt32 size = strrchr(Path, '/') - Path + 1;
  String path = Path;
  Char pathBuffer[BUILDER_MAX_PATH] = {0};
  UInt32 count = 0;

  while(*path) {
    if(count == size) {
      break;
    }
    if(*path == '/') {
      PDirMake(pathBuffer);
    }
    pathBuffer[count++] = *path;
    path++;
  }

  UInt32 fd = open(Path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if(fd == -1) {
    VB_LOG("File Not Opened ->%s\n", Path);
    return false;
  }
  close(fd);
  return true;
}

Bool PFileRemove(String Path) {
  Char* dot = strrchr(Path, '.');
  if(dot != NULL) {
    return unlink(Path);
  }
  //TODO: Temporary code!
  struct dirent* entry = NULL;
  struct stat info;
  DIR* dir = opendir(Path);
  Char pathBuffer[BUILDER_MAX_PATH];

  if(dir == NULL) {
    VB_LOG("Path not exist -> %s\n", Path);
    return false;
  }

  do {
    entry = readdir(dir);
    if(entry == NULL) { break; }
    if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { continue; }
    snprintf(pathBuffer, BUILDER_MAX_PATH, "%s/%s", Path, entry->d_name);
    if(stat(pathBuffer, &info) == 0) {
      if(S_ISDIR(info.st_mode)) {
        continue;
      } else {
        unlink(pathBuffer);
      }
    }
  } while(entry != NULL);

  closedir(dir);
  return true;
}

Bool PDirMake(String Path) {
  const UInt16 ATTRIBUTE = 0755;
  Char pathBuffer[BUILDER_MAX_PATH] = {0};
  UInt32 count = 0;
  while(*Path) {
    if(*Path == '/' || *Path == '\\') {
      mkdir(pathBuffer, ATTRIBUTE);
    }
    pathBuffer[count++] = *(Path++);
  }
  return mkdir(pathBuffer, ATTRIBUTE);
}

Bool PDirRemove(String Path) {
  struct dirent* entry = NULL;
  struct stat info;
  DIR* dir = opendir(Path);
  Char pathBuffer[BUILDER_MAX_PATH];

  if(dir == NULL) {
    VB_LOG("Path not exist -> %s\n", Path);
    return false;
  }

  do {
    entry = readdir(dir);
    if(entry == NULL) { break; }
    if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { continue; }
    snprintf(pathBuffer, BUILDER_MAX_PATH, "%s/%s", Path, entry->d_name);
    if(stat(pathBuffer, &info) == 0) {
      if(S_ISDIR(info.st_mode)) {
        PDirRemove(pathBuffer);
      } else {
        unlink(pathBuffer);
      }
    }
  } while(entry != NULL);

  closedir(dir);
  return rmdir(Path);
}

#endif
