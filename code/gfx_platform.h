#ifndef GFX_PLATFORM_H
#define GFX_PLATFORM_H

function void *OS_AllocateMemory(u64 Size);
function void OS_FreeMemory(void *Memory);

function b32 OS_ReadEntireFile(char *FilePath, buffer *FileData);
function void OS_FreeFileMemory(buffer FileData);

#endif
