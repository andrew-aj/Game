#ifndef VULKAN_BGFXUTILS_H
#define VULKAN_BGFXUTILS_H

#include <bx/pixelformat.h>
#include <bx/string.h>
#include <bgfx/bgfx.h>
#include <bimg/bimg.h>
#include <vector>
#include <string>

#include <bgfxExtra/bounds.h>
#include <bx/error.h>
#include <bx/readerwriter.h>
#include <meshoptimizer/src/meshoptimizer.h>

#include <bx/debug.h>
#include <bx/file.h>

#include <memory>

#define DBG_STRINGIZE(_x) DBG_STRINGIZE_(_x)
#define DBG_STRINGIZE_(_x) #_x
#define DBG_FILE_LINE_LITERAL "" __FILE__ "(" DBG_STRINGIZE(__LINE__) "): "
#define DBG(_format, ...) bx::debugPrintf(DBG_FILE_LINE_LITERAL "" _format "\n", ##__VA_ARGS__)

class FileReader : public bx::FileReader
{
    typedef bx::FileReader super;

public:
    virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override
    {
        //std::string filePath(s_currentDir);
        //filePath.append(_filePath);
        return super::open(_filePath.getCPtr(), _err);
    }
};

extern std::shared_ptr<FileReader> reader;

bx::FileReaderI* getFileReader();

bx::AllocatorI* getDefaultAllocator();

struct Primitive
{
    uint32_t m_startIndex;
    uint32_t m_numIndices;
    uint32_t m_startVertex;
    uint32_t m_numVertices;

    Sphere m_sphere;
    Aabb m_aabb;
    Obb m_obb;
};

typedef std::vector<Primitive> PrimitiveArray;

struct Group
{
    Group(){
        reset();
    }
    void reset();

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    uint16_t m_numVertices;
    uint8_t* m_vertices;
    uint32_t m_numIndices;
    uint16_t* m_indices;
    Sphere m_sphere;
    Aabb m_aabb;
    Obb m_obb;
    PrimitiveArray m_prims;
};
typedef std::vector<Group> GroupArray;

struct MeshState
{
    struct Texture
    {
        uint32_t            m_flags;
        bgfx::UniformHandle m_sampler;
        bgfx::TextureHandle m_texture;
        uint8_t             m_stage;
    };

    Texture             m_textures[4];
    uint64_t            m_state;
    bgfx::ProgramHandle m_program;
    uint8_t             m_numTextures;
    bgfx::ViewId        m_viewId;
};

struct Mesh
{
    void load(bx::ReaderSeekerI* _reader, bool _ramcopy);
    void unload();
    void submit(bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const;
    void submit(const MeshState*const* _state, uint8_t _numPasses, const float* _mtx, uint16_t _numMatrices) const;

    bgfx::VertexLayout m_layout;
    GroupArray m_groups;
};

static bx::DefaultAllocator s_allocator;

Mesh* meshLoad(bx::ReaderSeekerI* _reader, bool _ramcopy);

Mesh* meshLoad(const char* _filePath, bool _ramcopy = false);

Mesh* meshLoad(const char* _filePath, bool _ramcopy);

void meshUnload(Mesh* _mesh);

MeshState* meshStateCreate();

void meshStateDestroy(MeshState* _meshState);

void meshSubmit(const Mesh* _mesh, bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state);

void meshSubmit(const Mesh* _mesh, const MeshState*const* _state, uint8_t _numPasses, const float* _mtx, uint16_t _numMatrices);

const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath);

bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name);

bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName);

bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName);

#endif //VULKAN_BGFXUTILS_H
