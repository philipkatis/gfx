// TODO(philip): Combine shader types in a single file.

function GLuint
GL_LoadShaderModule(GLenum Type, char *Path)
{
    GLuint Module = 0;

    // TODO(philip): Move file loading out of here.
    buffer FileData;
    if (OS_ReadEntireFile(Path, &FileData))
    {
        Module = glCreateShader(Type);

        glShaderSource(Module, 1, (GLchar **)&FileData.Data, (GLint *)&FileData.Size);
        glCompileShader(Module);

        GLint Status;
        glGetShaderiv(Module, GL_COMPILE_STATUS, &Status);

        if (Status == GL_FALSE)
        {
            // TODO(philip): Maybe remove this.
            GLint InfoLogLength;
            glGetShaderiv(Module, GL_INFO_LOG_LENGTH, &InfoLogLength);

            Assert(InfoLogLength < 4096);

            GLchar InfoLog[4096];
            glGetShaderInfoLog(Module, 4096, &InfoLogLength, InfoLog);

            // TODO(philip): Replace this with something like a message box?
            // TODO(philip): Print correct name based on shader type.
            OutputDebugStringA("Shader module compilation failed!\n");
            OutputDebugStringA(InfoLog);
            OutputDebugStringA("\n");

            glDeleteShader(Module);
            Module = 0;
        }

        OS_FreeFileMemory(&FileData);
    }

    return Module;
}
