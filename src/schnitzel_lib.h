#pragma once
 
#include <stdio.h>

// this is to get malloc
#include <stdlib.h>

// this is to get memset
#include <string.h>

// used to get the edit timestamp of files
#include <sys/stat.h>

// ########################################################
//                          Defines
// ########################################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(x)  1 << (x)
#define KB(x)  ((unsigned long long)1024 * x)
#define MB(x)  ((unsigned long long)1024 * KB(x))
#define GB(X)  ((unsigned long long)1024 * MB(x))


// ########################################################
//                          Logging
// ########################################################

enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

template<typename ...Args>
void _log(char* prefix, char* msg, TextColor textcolor, Args... args)
{
    static char* TextColorTable[TEXT_COLOR_COUNT] =
    {
        "\x1b[30m", // TEXT_COLOR_BLACK
        "\x1b[31m", // TEXT_COLOR_RED
        "\x1b[32m", // TEXT_COLOR_GREEN
        "\x1b[33m", // TEXT_COLOR_YELLOW
        "\x1b[34m", // TEXT_COLOR_BLUE
        "\x1b[35m", // TEXT_COLOR_MAGENTA
        "\x1b[36m", // TEXT_COLOR_CYAN
        "\x1b[37m", // TEXT_COLOR_WHITE
        "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
        "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
        "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
        "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
        "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
        "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
        "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
        "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
    };

    char formatBuffer[8192] = {};  
    sprintf(formatBuffer, "%s %s %s \033[0m",TextColorTable[textcolor],prefix,msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer,formatBuffer,args...);

    puts(textBuffer);
}

#define SM_TRACE(msg,...) _log("TRACE: ",msg,TEXT_COLOR_GREEN,##__VA_ARGS__);
#define SM_WARN(msg,...) _log("WARN: ",msg,TEXT_COLOR_YELLOW,##__VA_ARGS__);
#define SM_ERROR(msg,...) _log("ERROR: ",msg,TEXT_COLOR_RED,##__VA_ARGS__);

#define SM_ASSERT(x,msg,...)          \
{                                     \
    if(!(x))                          \
    {                                 \
        SM_ERROR(msg,##__VA_ARGS__);  \
        DEBUG_BREAK();                \
        SM_ERROR("Assertion HIT!")    \
    }                                 \
}


// ########################################################
//                          Bump Allocator
// ########################################################
struct BumpAllocator
{
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size)
{
    BumpAllocator ba = {};

    ba.memory = (char*)malloc(size);

    if(ba.memory)
    {
        ba.capacity = size;
        memset(ba.memory,0,size); // set memory to 0
    }
    else{
        SM_ASSERT(false,"Failed to allocate Memory");
    }

    return ba;
}

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size)
{
    char* result = nullptr;

    size_t allignedSize = (size + 7) & ~ 7;

    if(bumpAllocator->used + allignedSize <= bumpAllocator->capacity)
    {
        result = bumpAllocator->memory + bumpAllocator->used;
        bumpAllocator->used += allignedSize;
    }
    else
    {
        SM_ASSERT(false,"BumpAllocator is full");
    }
    return result;
}

// ########################################################
//                          File I/O
// ########################################################

long long get_timestamp(char* file)
{
    struct _stat64 file_stat = {};
    _stat64(file, &file_stat);
    return file_stat.st_mtime;
}

bool file_exist(const char* Filename)
{
    SM_ASSERT(Filename,"No fileName(filePath) supplied!");

    auto file = fopen(Filename,"rb");
    if(!file)
    {
        return false;
    }
    fclose(file);

    return true;
}

long get_file_size(const char* FilePath)
{
    SM_ASSERT(FilePath,"No filePath supplied!");
    
    long filesize = 0;
    auto file = fopen(FilePath,"rb");
    if(!file)
    {
        SM_ERROR("Failed To Open File: %s", FilePath);
        return 0;
    }

    fseek(file,0,SEEK_END);
    filesize = ftell(file);
    fseek(file,0,SEEK_SET);
    fclose(file);

    return filesize;
}

/*
* Reads a file into a supplied buffer. We manage our own
* memory and therefore want more control over where it 
* is allocated
*/

char* read_file(const char* filePath, int* filesize, char* buffer)
{
    SM_ASSERT(filePath,"No filePath supplied!");
    SM_ASSERT(filesize,"No filesize supplied!");
    SM_ASSERT(buffer,"No buffer supplied!");

    *filesize = 0;
    auto file = fopen(filePath,"rb");
    if(!file)
    {
        SM_ERROR("Failed To Open File: %s", filePath);
        return 0;
    }

    fseek(file,0,SEEK_END);
    *filesize = ftell(file);
    fseek(file,0,SEEK_SET);

    memset(buffer,0,*filesize + 1);
    fread(buffer,sizeof(char),*filesize,file);

    fclose(file);
    
    return buffer;
}

char* read_file(const char* filePath, int* filesize, BumpAllocator* bumpAllocator)
{
    char* file = nullptr;
    long fileSize2 = get_file_size(filePath);

    if(fileSize2)
    {
        char* buffer = bump_alloc(bumpAllocator, fileSize2+1);
        
        file = read_file(filePath,filesize,buffer);
    }

    return file;
}

void write_file(const char* filePath, char* buffer, int size)
{
    SM_ASSERT(filePath,"No filePath supplied!");
    SM_ASSERT(buffer,"No buffer supplied!");

    auto file = fopen(filePath,"wb");
    if(!file)
    {
        SM_ERROR("Failed To Open File: %s", filePath);
        return;
    }

    fwrite(buffer,sizeof(char),size,file);
    fclose(file);
}

bool copy_file(const char* fileName, const char* outputName, char* buffer)
{
    int filesize = 0;
    char* data = read_file(fileName, &filesize, buffer);

    auto outputfile = fopen(outputName,"wb");
    if(!outputfile)
    {
        SM_ERROR("Failed To Open File: %s",outputName);
        return false;
    }

    int result = fwrite(data,sizeof(char),filesize,outputfile);
    if(!result)
    {
        SM_ERROR("Failed To Write File: %s",outputName);
        return false;
    }

    fclose(outputfile);

    return true;
}

bool copy_file(const char* fileName,const char* outputName, BumpAllocator* bumpAllocator)
{
    char* file = 0;
    long filesize = get_file_size(fileName);
    
    if(filesize)
    {
        char* buffer = bump_alloc(bumpAllocator,filesize+1);

        return copy_file(fileName,outputName,buffer);
    }

    return false;
}


// ########################################################
//                          Math Stuff
// ########################################################

struct Vec2
{
    float x;
    float y;
};

struct IVec2
{
    int x;
    int y;
};
