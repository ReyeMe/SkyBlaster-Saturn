#ifndef __MODEL_LOADER_H__
#define __MODEL_LOADER_H__

/*
 * Face flags
 */
typedef enum
{
    Tmf_Face_None = 0,
    Tmf_Face_Double = 1,
    Tmf_Face_Meshed = 2
} TmfFaceFlags;

/*
 * Model file type
 */
typedef enum
{
    Tmf_Type_Static = 0
} TmfType;

/*
 * Texture data (image name, diffuse color)
 */
typedef struct
{
    char FileName[13];
    unsigned char color[3];
} TmfTexture;

/*
 * Quad data
 */
typedef struct
{
    POINT Normal;
    unsigned short Indexes[4];
    unsigned char Flags;
    unsigned char Texture;
    unsigned char Reserved[2];
} TmfFace;

/*
 * 3D model entry
 */
typedef struct
{
    unsigned short VerticesCount;
    unsigned short FaceCount;
} TmfModelHeader;

/*
 * Tank model file header
 */
typedef struct
{
    unsigned char Type;
    unsigned char TextureCount;
    unsigned char ModelCount;
    unsigned char Reserved[5];
} TmfHeader;

/** @brief Loaded mesh
 */
typedef struct
{
    jo_3d_mesh * Meshes;
    int MeshCount;
} SaturnMesh;

/** @brief Load Jo-Engine mesh (textures are loaded from the same folder model is in)
 * @param mesh Loaded mesh
 * @param file File name inside models folder
 * @param dir Model file folder
 * @return loaded array of meshes
 */
void TmfLoadMesh(SaturnMesh * mesh, const char *file, const char *dir);

/** @brief Load Jo-Engine mesh
 * @param mesh Loaded mesh
 * @param file File name inside models folder
 * @param dir Model file folder
 * @param texture_loader texture loader
 * @return loaded array of meshes
 */
void TmfLoadMeshWithCustomTextureLoader(SaturnMesh * mesh, const char *file, const char *dir, int (*texture_loader)(TmfTexture *, const char *, int));

/** @brief Draw mesh
 * @param mesh Mesh to draw
 */
void TmfDraw(const SaturnMesh * mesh);

/** @brief Dispose of mesh
 * @param mesh Mest to be disposed
 */
void TmfFree(const SaturnMesh * mesh);

#endif