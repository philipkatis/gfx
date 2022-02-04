//
// NOTE(philip): OBJ
//

function char *
OBJParseV2(char *Pointer, v2 *Vector)
{
    for (u32 ComponentIndex = 0;
         ComponentIndex < 2;
         ++ComponentIndex)
    {
        Pointer = SkipWhitespace(++Pointer);
        char *ComponentString = Pointer;

        Pointer = SkipUntilWhitespace(Pointer);
        *Pointer = 0;

        Vector->Data[ComponentIndex] = atof(ComponentString);
    }

    return Pointer;
}

function char *
OBJParseV3(char *Pointer, v3 *Vector)
{
    for (u32 ComponentIndex = 0;
         ComponentIndex < 3;
         ++ComponentIndex)
    {
        Pointer = SkipWhitespace(++Pointer);
        char *ComponentString = Pointer;

        Pointer = SkipUntilWhitespace(Pointer);
        *Pointer = 0;

        Vector->Data[ComponentIndex] = atof(ComponentString);
    }

    return Pointer;
}

function u32
OBJGetIndexSetID(index_set_table *Table, u64 Position, u64 TextureCoordinate, u64 Normal)
{
    u32 ID = 0;
    b32 Found = false;

    // TODO(philip): Better hash function.
    u64 Hash = Position + TextureCoordinate + Normal;
    u64 Slot = Hash % INDEX_SET_TABLE_SLOT_COUNT;

    for (index_set *Set = Table->Slots[Slot];
         Set;
         Set = Set->Next)
    {
        if ((Set->Position == Position) && (Set->TextureCoordinate == TextureCoordinate) &&
            (Set->Normal == Normal))
        {
            ID = Set->ID;
            Found = true;

            break;
        }
    }

    if (!Found)
    {
        // TODO(philip): Change to using a memory arena.
        index_set *Set = (index_set *)Platform.AllocateMemory(sizeof(index_set));

        Set->ID = Table->Count++;
        Set->Position = Position;
        Set->TextureCoordinate = TextureCoordinate;
        Set->Normal = Normal;

        Set->Next = Table->Slots[Slot];
        Table->Slots[Slot] = Set;

        ID = Set->ID;
    }

    return ID;
}

function u64
GetVertexSize(vertex_attribute_flags Flags)
{
    u64 Size = 0;

    if (Flags & VertexAttributeFlags_Position)
    {
        Size += sizeof(v3);
    }

    if (Flags & VertexAttributeFlags_TextureCoordiante)
    {
        Size += sizeof(v2);
    }

    if (Flags & VertexAttributeFlags_Normal)
    {
        Size += sizeof(v3);
    }

    return Size;
}

// TODO(philip): Documentation.
// TODO(philip): Return success or failure.
function void
LoadOBJ(char *FilePath, mesh_asset *Asset)
{
    Assert(Asset);
    *Asset = { };

    char *DirectoryPath = ExtractDirectoryPath(FilePath);

    buffer FileData;
    if (Platform.ReadEntireFile(FilePath, &FileData))
    {
        u64 PositionCount = 0;
        u64 TextureCoordinateCount = 0;
        u64 NormalCount = 0;
        u64 TriangleCount = 0;
        u64 MaterialInstanceCount = 0;

        {
            char *Pointer = (char *)FileData.Data;
            char *End = (char *)FileData.Data + FileData.Size;

            while (Pointer != End)
            {
                switch (*Pointer)
                {
                    case 'v':
                    {
                        switch (*(++Pointer))
                        {
                            case 't':
                            {
                                ++TextureCoordinateCount;
                            } break;

                            case 'n':
                            {
                                ++NormalCount;
                            } break;

                            default:
                            {
                                ++PositionCount;
                            } break;
                        }
                    } break;

                    case 'g':
                    {
                        ++Asset->SubmeshCount;
                    } break;

                    case 'u':
                    {
                        // TODO(philip): Replace memcmp.
                        if (memcmp(Pointer, "usemtl", 6 * sizeof(char)) == 0)
                        {
                            ++MaterialInstanceCount;
                        }
                    } break;

                    case 'f':
                    {
                        ++TriangleCount;
                    } break;
                }

                Pointer = SkipLine(Pointer);
                ++Pointer;
            }
        }

        v3 *Positions = 0;
        v2 *TextureCoordinates = 0;
        v3 *Normals = 0;

        if (PositionCount)
        {
            Positions = (v3 *)Platform.AllocateMemory(PositionCount * sizeof(v3));
            Asset->VertexAttributeFlags |= VertexAttributeFlags_Position;
        }

        if (TextureCoordinateCount)
        {
            TextureCoordinates = (v2 *)Platform.AllocateMemory(TextureCoordinateCount * sizeof(v2));
            Asset->VertexAttributeFlags |= VertexAttributeFlags_TextureCoordiante;
        }

        if (NormalCount)
        {
            Normals = (v3 *)Platform.AllocateMemory(NormalCount * sizeof(v3));
            Asset->VertexAttributeFlags |= VertexAttributeFlags_Normal;
        }

        u64 PositionIndex = 0;
        u64 TextureCoordinateIndex = 0;
        u64 NormalIndex = 0;

        index_set_table IndexSetTable = { };

        Asset->Indices = (u32 *)Platform.AllocateMemory(TriangleCount * 3 * sizeof(u32));

        Asset->Submeshes = (submesh *)Platform.AllocateMemory(Asset->SubmeshCount * sizeof(submesh));
        s64 SubmeshIndex = -1;

        char **MaterialNames = (char **)Platform.AllocateMemory(MaterialInstanceCount * sizeof(char *));
        s64 CurrentMaterialIndex = -1;

        char *Pointer = (char *)FileData.Data;
        char *End = (char *)FileData.Data + FileData.Size;

        while (Pointer != End)
        {
            switch (*Pointer)
            {
                case 'v':
                {
                    switch (*(++Pointer))
                    {
                        case 't':
                        {
                            v2 *TextureCoordinate = TextureCoordinates + TextureCoordinateIndex++;
                            Pointer = OBJParseV2(Pointer, TextureCoordinate);
                        } break;

                        case 'n':
                        {
                            v3 *Normal = Normals + NormalIndex++;
                            Pointer = OBJParseV3(Pointer, Normal);
                        } break;

                        default:
                        {
                            v3 *Position = Positions + PositionIndex++;
                            Pointer = OBJParseV3(Pointer, Position);
                        } break;
                    }
                } break;

                case 'g':
                {
                    Pointer = SkipLine(Pointer);

                    submesh *Submesh = Asset->Submeshes + ++SubmeshIndex;
                    Submesh->IndexOffset = Asset->IndexCount;
                    Submesh->MaterialIndex = CurrentMaterialIndex;
                } break;

                case 'u':
                {
                    char *PropertyName = Pointer;

                    Pointer = SkipUntilWhitespace(++Pointer);
                    *Pointer = 0;

                    // TODO(philip): Replace strcmp.
                    if (strcmp(PropertyName, "usemtl") == 0)
                    {
                        Pointer = SkipWhitespace(++Pointer);
                        char *MaterialName = Pointer;

                        Pointer = SkipUntilWhitespace(++Pointer);
                        *Pointer = 0;

                        for (u64 Index = 0;
                             Index < MaterialInstanceCount;
                             ++Index)
                        {
                            if (MaterialNames[Index])
                            {
                                if (strcmp(MaterialNames[Index], MaterialName) == 0)
                                {
                                    CurrentMaterialIndex = Index;
                                    break;
                                }
                            }
                            else
                            {
                                u64 MaterialNameLength = strlen(MaterialName);
                                MaterialNames[Index] = (char *)Platform.AllocateMemory((MaterialNameLength + 1) * sizeof(char));
                                strcpy(MaterialNames[Index], MaterialName);

                                CurrentMaterialIndex = Index;
                                ++Asset->MaterialCount;

                                break;
                            }
                        }

                        submesh *Submesh = Asset->Submeshes + SubmeshIndex;
                        if (Submesh->IndexCount == 0)
                        {
                            Submesh->MaterialIndex = CurrentMaterialIndex;
                        }
                    }
                    else
                    {
                        Pointer = SkipLine(Pointer);
                    }
                } break;

                case 'f':
                {
                    for (u32 SetIndex = 0;
                         SetIndex < 3;
                         ++SetIndex)
                    {
                        char *PositionString = 0;
                        char *TextureCoordinateString = 0;
                        char *NormalString = 0;

                        Pointer = SkipWhitespace(++Pointer);

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_Position)
                        {
                            PositionString = Pointer;
                        }

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_TextureCoordiante)
                        {
                            Pointer = SkipUntil(Pointer, '/');
                            *Pointer = 0;

                            TextureCoordinateString = ++Pointer;
                        }

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_Normal)
                        {
                            Pointer = SkipUntil(Pointer, '/');
                            *Pointer = 0;

                            NormalString = ++Pointer;
                        }

                        Pointer = SkipUntilWhitespace(Pointer);
                        *Pointer = 0;

                        u64 Position = 0;
                        u64 TextureCoordinate = 0;
                        u64 Normal = 0;

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_Position)
                        {
                            Position = atoi(PositionString) - 1;
                        }

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_TextureCoordiante)
                        {
                            TextureCoordinate = atoi(TextureCoordinateString) - 1;
                        }

                        if (Asset->VertexAttributeFlags & VertexAttributeFlags_Normal)
                        {
                            Normal = atoi(NormalString) - 1;
                        }

                        u64 ID = OBJGetIndexSetID(&IndexSetTable, Position, TextureCoordinate, Normal);
                        Asset->Indices[Asset->IndexCount++] = ID;
                    }

                    submesh *Submesh = Asset->Submeshes + SubmeshIndex;
                    Submesh->IndexCount += 3;
                } break;

                default:
                {
                    Pointer = SkipLine(Pointer);
                } break;
            }

            ++Pointer;
        }

        u64 VertexSize = GetVertexSize(Asset->VertexAttributeFlags);
        Asset->VertexData.Size = (IndexSetTable.Count * VertexSize);
        Asset->VertexData.Data = (u8 *)Platform.AllocateMemory(Asset->VertexData.Size);

        for (u64 Slot = 0;
             Slot < INDEX_SET_TABLE_SLOT_COUNT;
             ++Slot)
        {
            index_set *Set = IndexSetTable.Slots[Slot];
            while (Set)
            {
                u8 *Pointer = ((u8 *)Asset->VertexData.Data + (Set->ID * VertexSize));

                if (Asset->VertexAttributeFlags & VertexAttributeFlags_Position)
                {
                    memcpy(Pointer, (Positions + Set->Position), sizeof(v3));
                    Pointer += sizeof(v3);
                }

                if (Asset->VertexAttributeFlags & VertexAttributeFlags_TextureCoordiante)
                {
                    memcpy(Pointer, (TextureCoordinates + Set->TextureCoordinate), sizeof(v2));
                    Pointer += sizeof(v2);
                }

                if (Asset->VertexAttributeFlags & VertexAttributeFlags_Normal)
                {
                    memcpy(Pointer, (Normals + Set->Normal), sizeof(v3));
                    Pointer += sizeof(v3);
                }

                // TODO(philip): Change to using a memory arena.
                index_set *Next = Set->Next;
                Platform.FreeMemory(Set);
                Set = Next;
            }
        }

        Asset->Materials = (material_asset *)Platform.AllocateMemory(Asset->MaterialCount * sizeof(material_asset));

        for (u64 Index = 0;
             Index < MaterialInstanceCount;
             ++Index)
        {
            material_asset *Material = Asset->Materials + Index;
            Material->DiffuseMap = ConcatenatePaths(DirectoryPath, "head.tga");

            Platform.FreeMemory(MaterialNames[Index]);
        }

        Platform.FreeMemory(MaterialNames);
        Platform.FreeMemory(Normals);
        Platform.FreeMemory(TextureCoordinates);
        Platform.FreeMemory(Positions);

        Platform.FreeFileMemory(&FileData);
    }

    Platform.FreeMemory(DirectoryPath);
}

function void
FreeMeshAsset(mesh_asset *Asset)
{
    Assert(Asset);

    for (u64 Index = 0;
         Index < Asset->MaterialCount;
         ++Index)
    {
        material_asset *Material = Asset->Materials + Index;
        Platform.FreeMemory(Material->DiffuseMap);
    }

    Platform.FreeMemory(Asset->VertexData.Data);
    Platform.FreeMemory(Asset->Indices);
    Platform.FreeMemory(Asset->Submeshes);
    Platform.FreeMemory(Asset->Materials);

    *Asset = { };
}

//
// NOTE(philip): TGA
//

// TODO(philip): Documentation.
// TODO(philip): Return success or failure.
function void
LoadTGA(char *Path, texture_asset *Asset)
{
    Assert(Asset);

    // TODO(philip): Redundant?
    *Asset = { };

    buffer FileData;
    if (Platform.ReadEntireFile(Path, &FileData))
    {
        u8 *Pointer = (u8 *)FileData.Data;

        tga_header *Header = (tga_header *)Pointer;
        Pointer += sizeof(tga_header);

        // NOTE(philip): Make sure we don't get something we do not support.
        Assert(Header->IDLength == 0);
        Assert(Header->ColorMapType == 0);
        Assert((Header->ImageType == 2) || (Header->ImageType == 10));
        Assert(Header->ColorMapFirstEntryIndex == 0);
        Assert(Header->ColorMapEntryCount == 0);
        Assert(Header->BitsPerColorMapEntry == 0);
        Assert(Header->OriginX == 0);
        Assert(Header->OriginY == 0);
        Assert(Header->Width != 0);
        Assert(Header->Height != 0);
        Assert((Header->BitsPerPixel == 24) || (Header->BitsPerPixel == 32));

        u32 BytesPerPixel = (Header->BitsPerPixel / 8);
        switch (BytesPerPixel)
        {
            case 3:
            {
                Asset->Format = TextureFormat_BGR;
            } break;

            case 4:
            {
                Asset->Format = TextureFormat_BGRA;
            } break;
        }

        Asset->Width = Header->Width;
        Asset->Height = Header->Height;

        u64 PixelCount = (Asset->Width * Asset->Height);
        u64 Size = (PixelCount * BytesPerPixel);

        Asset->Data = (u8 *)Platform.AllocateMemory(Size);

        switch (Header->ImageType)
        {
            case 2:
            {
                // NOTE(philip): In this image type, the pixel values are raw and uncompressed.
                // TODO(philip): Replace memcpy.
                memcpy(Asset->Data, Pointer, Size);
            } break;

            case 10:
            {
                // NOTE(philip): In this image type, the pixel values are raw and compressed using Run-Length
                // encoding.

                u64 PixelIndex = 0;
                while (PixelIndex < PixelCount)
                {
                    // NOTE(philip): In Run-Length encoding, each packet has a header. We use it to extract the
                    // type of the packet and the number of pixels it consists of.
                    u8 PacketHeader = *Pointer;
                    ++Pointer;

                    u8 PacketType = (PacketHeader >> 7);
                    Assert(PacketType == 0 || PacketType == 1);

                    // NOTE(philip): A pixel count of 0 is actually 1.
                    u8 PacketPixelCount = (PacketHeader & 0x7F) + 1;
                    Assert(PacketPixelCount > 0 && PacketPixelCount <= 128);

                    switch (PacketType)
                    {
                        case 0:
                        {
                            // NOTE(philip): In this packet type, there is a sequece of uniquely colored pixels.
                            u32 DataOffset = (PixelIndex * BytesPerPixel);
                            u32 PacketSize = (PacketPixelCount * BytesPerPixel);

                            // TODO(philip): Replace memcpy.
                            memcpy(Asset->Data + DataOffset, Pointer, PacketSize);

                            PixelIndex += PacketPixelCount;
                            Pointer += PacketSize;
                        } break;

                        case 1:
                        {
                            // NOTE(philip): In this packet type, there is a sequence of identically colored pixels.
                            u32 PacketSize = BytesPerPixel;

                            for (u32 Index = 0;
                                 Index < PacketPixelCount;
                                 ++Index)
                            {
                                u32 DataOffset = (PixelIndex * BytesPerPixel);

                                // TODO(philip): Replace memcpy.
                                memcpy(Asset->Data + DataOffset, Pointer, BytesPerPixel);

                                ++PixelIndex;
                            }

                            Pointer += PacketSize;
                        } break;
                    }
                }
            } break;
        }

        Platform.FreeFileMemory(&FileData);
    }
}

function void
FreeTextureAsset(texture_asset *Asset)
{
    Assert(Asset);

    Platform.FreeMemory(Asset->Data);

    *Asset = { };
}
