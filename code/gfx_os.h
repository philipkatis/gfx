#ifndef GFX_OS_H
#define GFX_OS_H

//
// NOTE(philip): Memory
//

function void *OS_AllocateMemory(u64 Size);
function void OS_FreeMemory(void *Memory);

//
// NOTE(philip): File IO
//

function b32 OS_ReadEntireFile(char *Path, buffer *Buffer);
function void OS_FreeFileMemory(buffer *Buffer);

#endif
