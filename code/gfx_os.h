#ifndef GFX_OS_H
#define GFX_OS_H

//
// NOTE(philip): File IO
//

function b32 OS_ReadEntireFile(char *Path, buffer *Buffer);
function void OS_FreeFileMemory(buffer *Buffer);

#endif
