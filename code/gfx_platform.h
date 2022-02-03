#ifndef GFX_PLATFORM_H
#define GFX_PLATFORM_H

//
// NOTE(philip): Memory
//

typedef void *platform_allocate_memory(u64 Size);
typedef void platform_free_memory(void *Memory);

//
// NOTE(philip): File IO
//

typedef b32 platform_read_entire_file(char *Path, buffer *Buffer);
typedef void platform_free_file_memory(buffer *Buffer);

//
// NOTE(philip): Platform API
//

struct platform_api
{
    // NOTE(philip): Memory
    platform_allocate_memory *AllocateMemory;
    platform_free_memory *FreeMemory;

    // NOTE(philip): File IO
    platform_read_entire_file *ReadEntireFile;
    platform_free_file_memory *FreeFileMemory;
};

global platform_api Platform = { };

#endif
