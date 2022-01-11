#ifndef GFX_OS_H
#define GFX_OS_H

//
// NOTE(philip): File IO
//

static b32 OS_ReadEntireFile(char *Path, buffer *Buffer);
static void OS_FreeFileMemory(buffer *Buffer);

#endif
