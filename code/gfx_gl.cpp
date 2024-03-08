function GLuint
GL_CreateShaderModule(GLenum Type, buffer Source)
{
    GLuint Module = glCreateShader(Type);

    glShaderSource(Module, 1, (GLchar **)&Source.Data, (GLint *)&Source.Size);
    glCompileShader(Module);

    GLint Status;
    glGetShaderiv(Module, GL_COMPILE_STATUS, &Status);

    if (Status == GL_FALSE)
    {
        GLchar InfoLog[4096];
        GLsizei Len;

        glGetShaderInfoLog(Module, 4096, &Len, InfoLog);

        OutputDebugStringA(InfoLog);

        glDeleteShader(Module);
        Module = 0;
    }

    return Module;
}

function shader
GL_LoadShader(char *VertexModuleFilePath, char *PixelModuleFilePath)
{
    shader Shader = { };

    GLuint VertexModule = 0;
    GLuint PixelModule = 0;

    buffer VertexModuleSource;
    if (OS_ReadEntireFile(VertexModuleFilePath, &VertexModuleSource))
    {
        VertexModule = GL_CreateShaderModule(GL_VERTEX_SHADER, VertexModuleSource);
        OS_FreeFileMemory(VertexModuleSource);
    }

    buffer PixelModuleSource;
    if (OS_ReadEntireFile(PixelModuleFilePath, &PixelModuleSource))
    {
        PixelModule = GL_CreateShaderModule(GL_FRAGMENT_SHADER, PixelModuleSource);
        OS_FreeFileMemory(PixelModuleSource);
    }

    Shader.Program = glCreateProgram();

    glAttachShader(Shader.Program, VertexModule);
    glAttachShader(Shader.Program, PixelModule);

    GLint Status;
    glLinkProgram(Shader.Program);

    glGetProgramiv(Shader.Program, GL_LINK_STATUS, &Status);
    if (Status == GL_FALSE)
    {
        GLchar InfoLog[4096];

        GLsizei Len;
        glGetProgramInfoLog(Shader.Program, 4096, &Len, InfoLog);

        OutputDebugStringA(InfoLog);

        glDeleteProgram(Shader.Program);
        Shader.Program = 0;
    }

    glValidateProgram(Shader.Program);

    glGetProgramiv(Shader.Program, GL_VALIDATE_STATUS, &Status);
    if (Status == GL_FALSE)
    {
        GLchar InfoLog[4096];

        GLsizei Len;
        glGetProgramInfoLog(Shader.Program, 4096, &Len, InfoLog);

        OutputDebugStringA(InfoLog);

        glDeleteProgram(Shader.Program);
        Shader.Program = 0;
    }

    glDetachShader(Shader.Program, VertexModule);
    glDetachShader(Shader.Program, PixelModule);

    glDeleteShader(VertexModule);
    glDeleteShader(PixelModule);

    Shader.ViewProjectionUniform = glGetUniformLocation(Shader.Program, "ViewProjection");
    Shader.CameraPositionUniform = glGetUniformLocation(Shader.Program, "CameraPosition");

    Shader.AmbientLightColorUniform = glGetUniformLocation(Shader.Program, "AmbientLightColor");
    Shader.AmbientLightIntensityUniform = glGetUniformLocation(Shader.Program, "AmbientLightIntensity");
    Shader.LightPositionUniform = glGetUniformLocation(Shader.Program, "Light.Position");
    Shader.LightColorUniform = glGetUniformLocation(Shader.Program, "Light.Color");
    Shader.LightIntensityUniform = glGetUniformLocation(Shader.Program, "Light.Intensity");

    Shader.TransformUniform = glGetUniformLocation(Shader.Program, "Transform");

    Shader.MaterialUseDiffuseMapUniform = glGetUniformLocation(Shader.Program, "Material.UseDiffuseMap");
    Shader.MaterialBaseColorUniform = glGetUniformLocation(Shader.Program, "Material.BaseColor");
    Shader.MaterialIsLitUniform = glGetUniformLocation(Shader.Program, "Material.IsLit");
    Shader.MaterialAmbientUniform = glGetUniformLocation(Shader.Program, "Material.Ambient");
    Shader.MaterialDiffuseUniform = glGetUniformLocation(Shader.Program, "Material.Diffuse");
    Shader.MaterialSpecularUniform = glGetUniformLocation(Shader.Program, "Material.Specular");

    return Shader;
}

function void
GL_FreeShader(shader Shader)
{
    glDeleteProgram(Shader.Program);
}

function mesh
GL_UploadMesh(mesh_asset MeshAsset, u64 MaterialAssetCount, material_asset *MaterialAssets)
{
    mesh Mesh = { };

    if (MeshAsset.SubmeshCount)
    {
        Mesh.SubmeshCount = MeshAsset.SubmeshCount;
        Mesh.Submeshes = (submesh *)OS_AllocateMemory(Mesh.SubmeshCount * sizeof(submesh));
        memcpy(Mesh.Submeshes, MeshAsset.Submeshes, Mesh.SubmeshCount * sizeof(submesh));
    }
    else
    {
        Mesh.SubmeshCount = 1;
        Mesh.Submeshes = (submesh *)OS_AllocateMemory(Mesh.SubmeshCount * sizeof(submesh));

        submesh *Submesh = Mesh.Submeshes;
        Submesh->IndexCount = MeshAsset.IndexCount;
        Submesh->IndexDataOffset = 0;
        Submesh->MaterialIndex = -1;
    }

    Mesh.MaterialCount = MaterialAssetCount;
    Mesh.Materials = (material *)OS_AllocateMemory(Mesh.MaterialCount * sizeof(material));

    for (u64 Index = 0;
         Index < Mesh.MaterialCount;
         ++Index)
    {
        material_asset *MaterialAsset = MaterialAssets + Index;
        material *Material = Mesh.Materials + Index;

        Material->Properties = MaterialAsset->Properties;

        if (MaterialAsset->DiffuseMap)
        {
            texture_asset TextureAsset = LoadTextureAsset(MaterialAsset->DiffuseMap);

            glGenTextures(1, &Material->DiffuseMap);
            glBindTexture(GL_TEXTURE_2D, Material->DiffuseMap);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            switch (TextureAsset.Format)
            {
                case TextureFormat_BGR:
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TextureAsset.Width, TextureAsset.Height, 0, GL_BGR,
                                 GL_UNSIGNED_BYTE, TextureAsset.Data);
                } break;

                case TextureFormat_BGRA:
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureAsset.Width, TextureAsset.Height, 0, GL_BGRA,
                                 GL_UNSIGNED_BYTE, TextureAsset.Data);
                } break;
            }

            FreeTextureAsset(TextureAsset);

            Material->UseDiffuseMap = true;
        }
        else
        {
            Material->UseDiffuseMap = false;
        }

        Material->BaseColor = MaterialAsset->BaseColor;

        Material->Ambient = MaterialAsset->Ambient;
        Material->Diffuse = MaterialAsset->Diffuse;
        Material->Specular = MaterialAsset->Specular;
    }

    glGenVertexArrays(1, &Mesh.VertexArray);
    glBindVertexArray(Mesh.VertexArray);

    glGenBuffers(1, &Mesh.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, MeshAsset.VertexDataSize, MeshAsset.VertexData, GL_STATIC_DRAW);

    u64 VertexSize = GetVertexSize(MeshAsset.VertexAttributes);
    u64 AttributeIndex = 0;
    u64 AttributeOffset = 0;

    if (MeshAsset.VertexAttributes & VertexAttribute_HasPositions)
    {
        glEnableVertexAttribArray(AttributeIndex);
        glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, VertexSize, (void *)AttributeOffset);

        ++AttributeIndex;
        AttributeOffset += sizeof(v3);
    }

    if (MeshAsset.VertexAttributes & VertexAttribute_HasTextureCoordinates)
    {
        glEnableVertexAttribArray(AttributeIndex);
        glVertexAttribPointer(AttributeIndex, 2, GL_FLOAT, GL_FALSE, VertexSize, (void *)AttributeOffset);

        ++AttributeIndex;
        AttributeOffset += sizeof(v2);
    }

    if (MeshAsset.VertexAttributes & VertexAttribute_HasNormals)
    {
        glEnableVertexAttribArray(AttributeIndex);
        glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, VertexSize, (void *)AttributeOffset);

        ++AttributeIndex;
        AttributeOffset += sizeof(v3);
    }

    glGenBuffers(1, &Mesh.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MeshAsset.IndexCount * sizeof(u32), MeshAsset.IndexData, GL_STATIC_DRAW);

    return Mesh;
}

function void
GL_DrawMesh(shader Shader, mesh Mesh, m4 Transform)
{
    glUseProgram(Shader.Program);
    glBindVertexArray(Mesh.VertexArray);

    glUniformMatrix4fv(Shader.TransformUniform, 1, GL_TRUE, (GLfloat *)&Transform);

    for (u64 Index = 0;
         Index < Mesh.SubmeshCount;
         ++Index)
    {
        submesh *Submesh = Mesh.Submeshes + Index;
        material *Material = 0;

        if (Submesh->MaterialIndex != -1)
        {
            Material = Mesh.Materials + Submesh->MaterialIndex;
        }
        else
        {
            Material = Mesh.Materials;
        }

        if (Material)
        {
            f32 UseDiffuseMap = 1.0f;
            if (!Material->UseDiffuseMap)
            {
                UseDiffuseMap = 0.0f;
            }

            glUniform1fv(Shader.MaterialUseDiffuseMapUniform, 1, &UseDiffuseMap);
            glUniform4fv(Shader.MaterialBaseColorUniform, 1, (GLfloat *)&Material->BaseColor);

            f32 IsLit = 1.0f;
            if (Material->Properties & MaterialProperty_Unlit)
            {
                IsLit = 0.0f;
            }

            glUniform1fv(Shader.MaterialIsLitUniform, 1, &IsLit);
            glUniform1fv(Shader.MaterialAmbientUniform, 1, &Material->Ambient);
            glUniform1fv(Shader.MaterialDiffuseUniform, 1, &Material->Diffuse);
            glUniform1fv(Shader.MaterialSpecularUniform, 1, &Material->Specular);

            if (Material->Properties & MaterialProperty_Wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            glBindTexture(GL_TEXTURE_2D, Material->DiffuseMap);
        }

        glDrawElements(GL_TRIANGLES, Submesh->IndexCount, GL_UNSIGNED_INT,
                       (GLvoid *)(Submesh->IndexDataOffset * sizeof(u32)));
    }
}

function void
GL_FreeMesh(mesh Mesh)
{
    for (u64 Index = 0;
         Index < Mesh.MaterialCount;
         ++Index)
    {
        material *Material = Mesh.Materials + Index;
        glDeleteTextures(1, &Material->DiffuseMap);
    }

    glDeleteBuffers(1, &Mesh.IndexBuffer);
    glDeleteBuffers(1, &Mesh.VertexBuffer);
    glDeleteVertexArrays(1, &Mesh.VertexArray);

    OS_FreeMemory(Mesh.Submeshes);
    OS_FreeMemory(Mesh.Materials);
}
