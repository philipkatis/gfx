function void
LoadMaterialAsset(char *DirectoryPath, char *FileName, u64 *AssetCount, material_asset **Assets)
{
    char *FilePath = ConcatenatePaths(DirectoryPath, FileName);

    buffer FileData;
    if (OS_ReadEntireFile(FilePath, &FileData))
    {
        char *Pointer = (char *)FileData.Data;
        char *End = (char *)FileData.Data + FileData.Size;

        while (Pointer != End)
        {
            if (memcmp(Pointer, "newmtl", (6 * sizeof(char))) == 0)
            {
                ++(*AssetCount);
            }

            Pointer = SkipLine(Pointer);
            ++Pointer;
        }

        *Assets = (material_asset *)OS_AllocateMemory(*AssetCount * sizeof(material_asset));
        s64 Index = -1;

        Pointer = (char *)FileData.Data;
        while (Pointer != End)
        {
            switch (*Pointer)
            {
                case 'n':
                {
                    char *Property = Pointer;

                    Pointer = SkipUntilWhitespace(++Pointer);
                    *Pointer = 0;

                    if (StringCompare(Property, "newmtl"))
                    {
                        Pointer = SkipWhitespace(++Pointer);
                        char *Name = Pointer;

                        Pointer = SkipUntilWhitespace(++Pointer);
                        *Pointer = 0;

                        material_asset *Asset = *Assets + ++Index;

                        u64 NameLength = strlen(Name);
                        Asset->Name = (char *)OS_AllocateMemory((NameLength + 1) * sizeof(char));
                        strcpy(Asset->Name, Name);

                        Asset->Properties = MaterialProperty_None;
                    }
                    else
                    {
                        Pointer = SkipLine(++Pointer);
                    }
                } break;

                case 'K':
                {
                    ++Pointer;

                    switch (*Pointer)
                    {
                        case 'a':
                        {
                            Pointer = SkipWhitespace(++Pointer);
                            char *ValueString = Pointer;

                            Pointer = SkipUntilWhitespace(++Pointer);
                            *Pointer = 0;

                            material_asset *Asset = *Assets + Index;
                            Asset->Ambient = atof(ValueString);
                        } break;

                        case 'd':
                        {
                            Pointer = SkipWhitespace(++Pointer);
                            char *ValueString = Pointer;

                            Pointer = SkipUntilWhitespace(++Pointer);
                            *Pointer = 0;

                            material_asset *Asset = *Assets + Index;
                            Asset->Diffuse = atof(ValueString);
                        } break;

                        case 's':
                        {
                            Pointer = SkipWhitespace(++Pointer);
                            char *ValueString = Pointer;

                            Pointer = SkipUntilWhitespace(++Pointer);
                            *Pointer = 0;

                            material_asset *Asset = *Assets + Index;
                            Asset->Specular = atof(ValueString);
                        } break;
                    }

                    Pointer = SkipLine(++Pointer);
                } break;

                case 'm':
                {
                    char *Property = Pointer;

                    Pointer = SkipUntilWhitespace(++Pointer);
                    *Pointer = 0;

                    if (StringCompare(Property, "map_Kd"))
                    {
                        Pointer = SkipWhitespace(++Pointer);
                        char *FileName = Pointer;

                        Pointer = SkipUntilWhitespace(++Pointer);
                        *Pointer = 0;

                        material_asset *Asset = *Assets + Index;
                        Asset->DiffuseMap = ConcatenatePaths(DirectoryPath, FileName);
                    }
                    else
                    {
                        Pointer = SkipLine(++Pointer);
                    }
                };

                default:
                {
                    Pointer = SkipLine(Pointer);
                } break;
            }

            ++Pointer;
        }

        OS_FreeFileMemory(FileData);
    }

    OS_FreeMemory(FilePath);
}

function void
FreeMaterialAsset(material_asset Asset)
{
    OS_FreeMemory(Asset.Name);
    OS_FreeMemory(Asset.DiffuseMap);
}

function u64
GetVertexSize(vertex_attributes Attributes)
{
    u64 Size = 0;

    if (Attributes & VertexAttribute_HasPositions)
    {
        Size += sizeof(v3);
    }

    if (Attributes & VertexAttribute_HasTextureCoordinates)
    {
        Size += sizeof(v2);
    }

    if (Attributes & VertexAttribute_HasNormals)
    {
        Size += sizeof(v3);
    }

    return Size;
}

function mesh_asset
GenerateUVSphere(u64 LatitudeCount, u64 LongitudeCount)
{
    mesh_asset Asset = { };

    Asset.VertexDataSize = (((LatitudeCount - 1) * LongitudeCount) + 2) * sizeof(v3);
    Asset.VertexData = (u8 *)OS_AllocateMemory(Asset.VertexDataSize);
    v3 *VertexPointer = (v3 *)Asset.VertexData;

    *VertexPointer = V3(0.0f, 1.0f, 0.0f);
    ++VertexPointer;

    for (u64 LatitudeIndex = 0;
         LatitudeIndex < (LatitudeCount - 1);
         ++LatitudeIndex)
    {
        f32 LatitudeAngle = (M_PI * ((f32)(LatitudeIndex + 1) / (f32)LatitudeCount));
        f32 LatitudeAngleSin = sinf(LatitudeAngle);
        f32 LatitudeAngleCos = cosf(LatitudeAngle);

        for (u64 LongitudeIndex = 0;
             LongitudeIndex < LongitudeCount;
             ++LongitudeIndex)
        {
            f32 LongitudeAngle = (2.0f * M_PI * ((f32)LongitudeIndex / (f32)LongitudeCount));
            f32 LongitudeAngleSin = sinf(LongitudeAngle);
            f32 LongitudeAngleCos = cosf(LongitudeAngle);

            *VertexPointer = V3(LatitudeAngleSin * LongitudeAngleCos,
                                LatitudeAngleCos,
                                LatitudeAngleSin * LongitudeAngleSin);
            ++VertexPointer;
        }
    }

    *VertexPointer = V3(0.0f, -1.0f, 0.0f);

    Asset.IndexCount = LatitudeCount * LongitudeCount * 6;
    Asset.IndexData = (u32 *)OS_AllocateMemory(Asset.IndexCount * sizeof(u32));
    u32 *IndexPointer = Asset.IndexData;

    for (u32 Index = 0;
         Index < LongitudeCount;
         ++Index)
    {
        *IndexPointer = 0;
        ++IndexPointer;

        *IndexPointer = (((Index + 1) % LongitudeCount) + 1);
        ++IndexPointer;

        *IndexPointer = (Index + 1);
        ++IndexPointer;
    }

    for (u32 LatitudeIndex = 0;
         LatitudeIndex < (LatitudeCount - 2);
         ++LatitudeIndex)
    {
        u32 IndexOffset1 = ((LatitudeIndex * LongitudeCount) + 1);
        u32 IndexOffset2 = (((LatitudeIndex + 1) * LongitudeCount) + 1);

        for (u32 LongitudeIndex = 0;
             LongitudeIndex < LongitudeCount;
             ++LongitudeIndex)
        {
            *IndexPointer = (IndexOffset1 + LongitudeIndex);
            ++IndexPointer;

            *IndexPointer = (IndexOffset1 + ((LongitudeIndex + 1) % LongitudeCount));
            ++IndexPointer;

            *IndexPointer = (IndexOffset2 + ((LongitudeIndex + 1) % LongitudeCount));
            ++IndexPointer;

            *IndexPointer = (IndexOffset2 + ((LongitudeIndex + 1) % LongitudeCount));
            ++IndexPointer;

            *IndexPointer = (IndexOffset2 + LongitudeIndex);
            ++IndexPointer;

            *IndexPointer = (IndexOffset1 + LongitudeIndex);
            ++IndexPointer;
        }
    }

    for (u32 Index = 0;
         Index < LongitudeCount;
         ++Index)
    {
        *IndexPointer = ((LatitudeCount - 1) * LongitudeCount) + 1;
        ++IndexPointer;

        *IndexPointer = Index + LongitudeCount * (LatitudeCount - 2) + 1;
        ++IndexPointer;

        *IndexPointer = (Index + 1) % LongitudeCount + LongitudeCount * (LatitudeCount - 2) + 1;
        ++IndexPointer;
    }

    Asset.VertexAttributes |= VertexAttribute_HasPositions;

    return Asset;
}

function char *
ParseVector(char *Pointer, v2 *Vector)
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
ParseVector(char *Pointer, v3 *Vector)
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
GetIndexSetID(index_set_table *Table, u64 Position, u64 TextureCoordinate, u64 Normal)
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
        index_set *Set = (index_set *)OS_AllocateMemory(sizeof(index_set));

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

function char *
ParseIndexSet(char *Pointer, index_set_table *Table, u32 *ID)
{
    Pointer = SkipWhitespace(++Pointer);

    char *PositionString = Pointer;

    Pointer = SkipUntil(Pointer, '/');
    *Pointer = 0;

    char *TextureCoordinateString = ++Pointer;

    Pointer = SkipUntil(Pointer, '/');
    *Pointer = 0;

    char *NormalString = ++Pointer;

    Pointer = SkipUntilWhitespace(Pointer);
    *Pointer = 0;

    u64 Position = atoi(PositionString) - 1;
    u64 TextureCoordinate = atoi(TextureCoordinateString) - 1;
    u64 Normal = atoi(NormalString) - 1;

    *ID = GetIndexSetID(Table, Position, TextureCoordinate, Normal);

    return Pointer;
}

function mesh_asset
LoadMeshAsset(char *FilePath)
{
    mesh_asset Asset = { };

    buffer FileData;
    if (OS_ReadEntireFile(FilePath, &FileData))
    {
        u64 PositionCount = 0;
        u64 TextureCoordinateCount = 0;
        u64 NormalCount = 0;
        u64 TriangleCount = 0;

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
                    ++Asset.SubmeshCount;
                } break;

                case 'f':
                {
                    ++TriangleCount;
                } break;
            }

            Pointer = SkipLine(Pointer);
            ++Pointer;
        }

        if (PositionCount)
        {
            Asset.VertexAttributes |= VertexAttribute_HasPositions;
        }

        if (TextureCoordinateCount)
        {
            Asset.VertexAttributes |= VertexAttribute_HasTextureCoordinates;
        }

        if (NormalCount)
        {
            Asset.VertexAttributes |= VertexAttribute_HasNormals;
        }

        Asset.IndexData = (u32 *)OS_AllocateMemory(TriangleCount * 3 * sizeof(u32));
        Asset.Submeshes = (submesh *)OS_AllocateMemory(Asset.SubmeshCount * sizeof(submesh));

        v3 *Positions = (v3 *)OS_AllocateMemory(PositionCount * sizeof(v3));
        v2 *TextureCoordinates = (v2 *)OS_AllocateMemory(TextureCoordinateCount * sizeof(v2));
        v3 *Normals = (v3 *)OS_AllocateMemory(NormalCount * sizeof(v3));

        u64 PositionIndex = 0;
        u64 TextureCoordinateIndex = 0;
        u64 NormalIndex = 0;
        s64 SubmeshIndex = -1;

        index_set_table IndexSetTable = { };

        Pointer = (char *)FileData.Data;
        while (Pointer != End)
        {
            switch (*Pointer)
            {
                case 'm':
                {
                    char *Property = Pointer;

                    Pointer = SkipUntilWhitespace(++Pointer);
                    *Pointer = 0;

                    if (StringCompare(Property, "mtllib"))
                    {
                        Pointer = SkipWhitespace(++Pointer);
                        char *FileName = Pointer;

                        Pointer = SkipUntilWhitespace(++Pointer);
                        *Pointer = 0;

                        char *DirectoryPath = ExtractDirectoryPath(FilePath);
                        LoadMaterialAsset(DirectoryPath, FileName, &Asset.MaterialAssetCount, &Asset.MaterialAssets);
                        OS_FreeMemory(DirectoryPath);
                    }
                    else
                    {
                        Pointer = SkipLine(++Pointer);
                    }
                } break;

                case 'v':
                {
                    switch (*(++Pointer))
                    {
                        case 't':
                        {
                            v2 *TextureCoordinate = TextureCoordinates + TextureCoordinateIndex++;
                            Pointer = ParseVector(Pointer, TextureCoordinate);
                        } break;

                        case 'n':
                        {
                            v3 *Normal = Normals + NormalIndex++;
                            Pointer = ParseVector(Pointer, Normal);
                        } break;

                        default:
                        {
                            v3 *Position = Positions + PositionIndex++;
                            Pointer = ParseVector(Pointer, Position);
                        } break;
                    }
                } break;

                case 'g':
                {
                    Pointer = SkipLine(Pointer);

                    submesh *Submesh = Asset.Submeshes + ++SubmeshIndex;
                    Submesh->IndexDataOffset = Asset.IndexCount;
                    Submesh->MaterialIndex = -1;
                } break;

                case 'u':
                {
                    char *Property = Pointer;

                    Pointer = SkipUntilWhitespace(++Pointer);
                    *Pointer = 0;

                    if (StringCompare(Property, "usemtl"))
                    {
                        Pointer = SkipWhitespace(++Pointer);
                        char *Name = Pointer;

                        Pointer = SkipUntilWhitespace(++Pointer);
                        *Pointer = 0;

                        s64 MaterialAssetIndex = -1;

                        for (u64 Index = 0;
                             Index < Asset.MaterialAssetCount;
                             ++Index)
                        {
                            material_asset *MaterialAsset = Asset.MaterialAssets + Index;

                            if (StringCompare(MaterialAsset->Name, Name))
                            {
                                MaterialAssetIndex = Index;
                                break;
                            }
                        }

                        submesh *Submesh = Asset.Submeshes + SubmeshIndex;
                        Submesh->MaterialIndex = MaterialAssetIndex;
                    }
                    else
                    {
                        Pointer = SkipLine(++Pointer);
                    }
                } break;

                case 'f':
                {
                    for (u32 SetIndex = 0;
                         SetIndex < 3;
                         ++SetIndex)
                    {
                        u32 *ID = (Asset.IndexData + Asset.IndexCount++);
                        Pointer = ParseIndexSet(Pointer, &IndexSetTable, ID);
                    }

                    submesh *Submesh = Asset.Submeshes + SubmeshIndex;
                    Submesh->IndexCount += 3;
                } break;

                default:
                {
                    Pointer = SkipLine(Pointer);
                } break;
            }

            ++Pointer;
        }

        u64 VertexSize = GetVertexSize(Asset.VertexAttributes);
        Asset.VertexDataSize = (IndexSetTable.Count * VertexSize);
        Asset.VertexData = (u8 *)OS_AllocateMemory(Asset.VertexDataSize);

        for (u64 Slot = 0;
             Slot < INDEX_SET_TABLE_SLOT_COUNT;
             ++Slot)
        {
            index_set *Set = IndexSetTable.Slots[Slot];

            while (Set)
            {
                u8 *VertexPointer = (Asset.VertexData + (Set->ID * VertexSize));

                memcpy(VertexPointer, (Positions + Set->Position), sizeof(v3));
                VertexPointer += sizeof(v3);

                memcpy(VertexPointer, (TextureCoordinates + Set->TextureCoordinate), sizeof(v2));
                VertexPointer += sizeof(v2);

                memcpy(VertexPointer, (Normals + Set->Normal), sizeof(v3));
                VertexPointer += sizeof(v3);

                index_set *Next = Set->Next;
                OS_FreeMemory(Set);
                Set = Next;
            }
        }

        OS_FreeMemory(Positions);
        OS_FreeMemory(TextureCoordinates);
        OS_FreeMemory(Normals);

        OS_FreeFileMemory(FileData);
    }

    return Asset;
}

function void
FreeMeshAsset(mesh_asset Asset)
{
    for (u64 Index = 0;
         Index < Asset.MaterialAssetCount;
         ++Index)
    {
        FreeMaterialAsset(Asset.MaterialAssets[Index]);
    }

    OS_FreeMemory(Asset.VertexData);
    OS_FreeMemory(Asset.IndexData);
    OS_FreeMemory(Asset.Submeshes);
    OS_FreeMemory(Asset.MaterialAssets);
}

function texture_asset
LoadTextureAsset(char *FilePath)
{
    texture_asset Asset = { };

    buffer FileData;
    if (OS_ReadEntireFile(FilePath, &FileData))
    {
        u8 *Pointer = FileData.Data;

        tga_header *Header = (tga_header *)Pointer;
        Pointer += sizeof(tga_header);

        u32 BytesPerPixel = (Header->BitsPerPixel / 8);
        switch (BytesPerPixel)
        {
            case 3:
            {
                Asset.Format = TextureFormat_BGR;
            } break;

            case 4:
            {
                Asset.Format = TextureFormat_BGRA;
            } break;
        }

        Asset.Width = Header->Width;
        Asset.Height = Header->Height;

        u64 PixelCount = (Asset.Width * Asset.Height);
        u64 Size = (PixelCount * BytesPerPixel);

        Asset.Data = (u8 *)OS_AllocateMemory(Size);

        switch (Header->ImageType)
        {
            case 2:
            {
                // NOTE(philip): In this image type, the pixel values are raw and uncompressed.
                // TODO(philip): Replace memcpy.
                memcpy(Asset.Data, Pointer, Size);
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

                    // NOTE(philip): A pixel count of 0 is actually 1.
                    u8 PacketPixelCount = (PacketHeader & 0x7F) + 1;

                    switch (PacketType)
                    {
                        case 0:
                        {
                            // NOTE(philip): In this packet type, there is a sequece of uniquely colored pixels.
                            u32 DataOffset = (PixelIndex * BytesPerPixel);
                            u32 PacketSize = (PacketPixelCount * BytesPerPixel);

                            // TODO(philip): Replace memcpy.
                            memcpy(Asset.Data + DataOffset, Pointer, PacketSize);

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
                                memcpy(Asset.Data + DataOffset, Pointer, BytesPerPixel);

                                ++PixelIndex;
                            }

                            Pointer += PacketSize;
                        } break;
                    }
                }
            } break;
        }

        OS_FreeFileMemory(FileData);
    }

    return Asset;
}

function void
FreeTextureAsset(texture_asset Asset)
{
    OS_FreeMemory(Asset.Data);
}
