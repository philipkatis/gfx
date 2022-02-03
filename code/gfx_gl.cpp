// TODO(philip): Combine shader types in a single file.

function GLuint
GL_LoadShaderModule(GLenum Type, char *Path)
{
    GLuint Module = 0;

    // TODO(philip): Move file loading out of here.
    buffer FileData;
    if (Platform.ReadEntireFile(Path, &FileData))
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

        Platform.FreeFileMemory(&FileData);
    }

    return Module;
}

function void
GL_FreeShader(shader *Shader)
{
    Assert(Shader);

    // TODO(philip): Only do this here in debug builds.
    glDetachShader(Shader->Program, Shader->VertexModule);
    glDetachShader(Shader->Program, Shader->PixelModule);

    // TODO(philip): Only do this here in debug builds.
    glDeleteShader(Shader->VertexModule);
    glDeleteShader(Shader->PixelModule);

    glDeleteProgram(Shader->Program);

    Shader->Program = 0;

    // TODO(philip): Only do this here in debug builds.
    Shader->VertexModule = 0;
    Shader->PixelModule = 0;
}

function shader
GL_LoadShader(char *VertexModulePath, char *PixelModulePath)
{
    shader Shader = { };
    Shader.Program = glCreateProgram();

    Shader.VertexModule = GL_LoadShaderModule(GL_VERTEX_SHADER, VertexModulePath);
    Shader.PixelModule = GL_LoadShaderModule(GL_FRAGMENT_SHADER, PixelModulePath);

    glAttachShader(Shader.Program, Shader.VertexModule);
    glAttachShader(Shader.Program, Shader.PixelModule);

    GLint Status;

    glLinkProgram(Shader.Program);
    glGetProgramiv(Shader.Program, GL_LINK_STATUS, &Status);

    if (Status == GL_FALSE)
    {
        // TODO(philip): Maybe remove this.
        GLint InfoLogLength;
        glGetProgramiv(Shader.Program, GL_INFO_LOG_LENGTH, &InfoLogLength);

        Assert(InfoLogLength < 4096);

        GLchar InfoLog[4096];
        glGetProgramInfoLog(Shader.Program, 4096, &InfoLogLength, InfoLog);

        // TODO(philip): Replace this with something like a message box?
        OutputDebugStringA("Shader program linking failed!\n");
        OutputDebugStringA(InfoLog);
        OutputDebugStringA("\n");

        GL_FreeShader(&Shader);
    }

    glValidateProgram(Shader.Program);
    glGetProgramiv(Shader.Program, GL_VALIDATE_STATUS, &Status);

    if (Status == GL_FALSE)
    {
        // TODO(philip): Maybe remove this.
        GLint InfoLogLength;
        glGetProgramiv(Shader.Program, GL_INFO_LOG_LENGTH, &InfoLogLength);

        Assert(InfoLogLength < 4096);

        GLchar InfoLog[4096];
        glGetProgramInfoLog(Shader.Program, 4096, &InfoLogLength, InfoLog);

        // TODO(philip): Replace this with something like a message box?
        OutputDebugStringA("Shader program validation failed!\n");
        OutputDebugStringA(InfoLog);
        OutputDebugStringA("\n");

        GL_FreeShader(&Shader);
    }

    // TODO(philip): Detach the shaders and delete them here in release builds.

    return Shader;
}

function mesh
GL_UploadMeshAsset(mesh_asset *Asset)
{
    Assert(Asset);

    mesh Mesh = { };

    Mesh.SubmeshCount = Asset->SubmeshCount;
    Mesh.Submeshes = (submesh *)Platform.AllocateMemory(Mesh.SubmeshCount * sizeof(submesh));

    // TODO(philip): Replace with my own function.
    memcpy(Mesh.Submeshes, Asset->Submeshes, Mesh.SubmeshCount * sizeof(submesh));

    glGenVertexArrays(1, &Mesh.VertexArray);
    glBindVertexArray(Mesh.VertexArray);

    glGenBuffers(1, &Mesh.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh.VertexBuffer);

    // TODO(philip): Don't use the vertex struct for the size. Find it from the mesh attribute flags.
    glBufferData(GL_ARRAY_BUFFER, Asset->VertexCount * sizeof(vertex), Asset->Vertices, GL_STATIC_DRAW);

    // TODO(philip): Don't hard code these. Find them from the mesh attribute flags.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(v3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(v3) + sizeof(v2)));

    glGenBuffers(1, &Mesh.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Asset->IndexCount * sizeof(u32), Asset->Indices, GL_STATIC_DRAW);

    // TODO(philip): Redundant?
    glBindVertexArray(0);

    return Mesh;
}

function void
GL_FreeMesh(mesh *Mesh)
{
    Assert(Mesh);

    Platform.FreeMemory(Mesh->Submeshes);

    glDeleteVertexArrays(1, &Mesh->VertexArray);
    glDeleteVertexArrays(1, &Mesh->VertexBuffer);
    glDeleteVertexArrays(1, &Mesh->IndexBuffer);

    Mesh->SubmeshCount = 0;
    Mesh->Submeshes = 0;
    Mesh->VertexArray = 0;
    Mesh->VertexBuffer = 0;
    Mesh->IndexBuffer = 0;
}
