#include "bgfxutils.h"

std::shared_ptr<FileReader> reader = std::make_shared<FileReader>();

bx::FileReaderI* getFileReader(){
    return reader.get();
}

bx::AllocatorI* getDefaultAllocator(){
    static bx::DefaultAllocator allocator;
    return &allocator;
}

void Group::reset()
{
    m_vbh.idx = bgfx::kInvalidHandle;
    m_ibh.idx = bgfx::kInvalidHandle;
    m_numVertices = 0;
    m_vertices = NULL;
    m_numIndices = 0;
    m_indices = NULL;
    m_prims.clear();
}

void Mesh::load(bx::ReaderSeekerI* _reader, bool _ramcopy)
{
    constexpr uint32_t kChunkVertexBuffer           = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
    constexpr uint32_t kChunkVertexBufferCompressed = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
    constexpr uint32_t kChunkIndexBuffer            = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
    constexpr uint32_t kChunkIndexBufferCompressed  = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
    constexpr uint32_t kChunkPrimitive              = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

    using namespace bx;
    using namespace bgfx;

    Group group;

    bx::AllocatorI* allocator = &s_allocator;

    uint32_t chunk;
    bx::Error err;
    while (4 == bx::read(_reader, chunk, &err)
           &&  err.isOk() )
    {
        switch (chunk)
        {
            case kChunkVertexBuffer:
            {
                read(_reader, group.m_sphere);
                read(_reader, group.m_aabb);
                read(_reader, group.m_obb);

                read(_reader, m_layout);

                uint16_t stride = m_layout.getStride();

                read(_reader, group.m_numVertices);
                const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices*stride);
                read(_reader, mem->data, mem->size);

                if (_ramcopy)
                {
                    group.m_vertices = (uint8_t*)BX_ALLOC(allocator, group.m_numVertices*stride);
                    bx::memCopy(group.m_vertices, mem->data, mem->size);
                }

                group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
            }
                break;

            case kChunkVertexBufferCompressed:
            {
                read(_reader, group.m_sphere);
                read(_reader, group.m_aabb);
                read(_reader, group.m_obb);

                read(_reader, m_layout);

                uint16_t stride = m_layout.getStride();

                read(_reader, group.m_numVertices);

                const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices*stride);

                uint32_t compressedSize;
                bx::read(_reader, compressedSize);

                void* compressedVertices = BX_ALLOC(allocator, compressedSize);
                bx::read(_reader, compressedVertices, compressedSize);

                meshopt_decodeVertexBuffer(mem->data, group.m_numVertices, stride, (uint8_t*)compressedVertices, compressedSize);

                BX_FREE(allocator, compressedVertices);

                if (_ramcopy)
                {
                    group.m_vertices = (uint8_t*)BX_ALLOC(allocator, group.m_numVertices*stride);
                    bx::memCopy(group.m_vertices, mem->data, mem->size);
                }

                group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
            }
                break;

            case kChunkIndexBuffer:
            {
                read(_reader, group.m_numIndices);
                const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices*2);
                read(_reader, mem->data, mem->size);

                if (_ramcopy)
                {
                    group.m_indices = (uint16_t*)BX_ALLOC(allocator, group.m_numIndices*2);
                    bx::memCopy(group.m_indices, mem->data, mem->size);
                }

                group.m_ibh = bgfx::createIndexBuffer(mem);
            }
                break;

            case kChunkIndexBufferCompressed:
            {
                bx::read(_reader, group.m_numIndices);

                const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices*2);

                uint32_t compressedSize;
                bx::read(_reader, compressedSize);

                void* compressedIndices = BX_ALLOC(allocator, compressedSize);

                bx::read(_reader, compressedIndices, compressedSize);

                meshopt_decodeIndexBuffer(mem->data, group.m_numIndices, 2, (uint8_t*)compressedIndices, compressedSize);

                BX_FREE(allocator, compressedIndices);

                if (_ramcopy)
                {
                    group.m_indices = (uint16_t*)BX_ALLOC(allocator, group.m_numIndices*2);
                    bx::memCopy(group.m_indices, mem->data, mem->size);
                }

                group.m_ibh = bgfx::createIndexBuffer(mem);
            }
                break;

            case kChunkPrimitive:
            {
                uint16_t len;
                read(_reader, len);

                std::string material;
                material.resize(len);
                read(_reader, const_cast<char*>(material.c_str() ), len);

                uint16_t num;
                read(_reader, num);

                for (uint32_t ii = 0; ii < num; ++ii)
                {
                    read(_reader, len);

                    std::string name;
                    name.resize(len);
                    read(_reader, const_cast<char*>(name.c_str() ), len);

                    Primitive prim;
                    read(_reader, prim.m_startIndex);
                    read(_reader, prim.m_numIndices);
                    read(_reader, prim.m_startVertex);
                    read(_reader, prim.m_numVertices);
                    read(_reader, prim.m_sphere);
                    read(_reader, prim.m_aabb);
                    read(_reader, prim.m_obb);

                    group.m_prims.push_back(prim);
                }

                m_groups.push_back(group);
                group.reset();
            }
                break;

            default:
                DBG("%08x at %d", chunk, bx::skip(_reader, 0) );
                break;
        }
    }
}

void Mesh::unload()
{
    bx::AllocatorI* allocator = getDefaultAllocator();

    for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd; ++it)
    {
        const Group& group = *it;
        bgfx::destroy(group.m_vbh);

        if (bgfx::isValid(group.m_ibh) )
        {
            bgfx::destroy(group.m_ibh);
        }

        if (NULL != group.m_vertices)
        {
            BX_FREE(allocator, group.m_vertices);
        }

        if (NULL != group.m_indices)
        {
            BX_FREE(allocator, group.m_indices);
        }
    }
    m_groups.clear();
}

void Mesh::submit(bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const
{
    if (BGFX_STATE_MASK == _state)
    {
        _state = 0
                 | BGFX_STATE_WRITE_RGB
                 | BGFX_STATE_WRITE_A
                 | BGFX_STATE_WRITE_Z
                 | BGFX_STATE_DEPTH_TEST_LESS
                 | BGFX_STATE_CULL_CCW
                 | BGFX_STATE_MSAA
                ;
    }

    bgfx::setTransform(_mtx);
    bgfx::setState(_state);

    for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd; ++it)
    {
        const Group& group = *it;

        bgfx::setIndexBuffer(group.m_ibh);
        bgfx::setVertexBuffer(0, group.m_vbh);
        bgfx::submit(_id, _program, 0, (it == itEnd-1) ? (BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS | BGFX_DISCARD_STATE) : BGFX_DISCARD_NONE);
    }
}

void Mesh::submit(const MeshState*const* _state, uint8_t _numPasses, const float* _mtx, uint16_t _numMatrices) const
{
    uint32_t cached = bgfx::setTransform(_mtx, _numMatrices);

    for (uint32_t pass = 0; pass < _numPasses; ++pass)
    {
        bgfx::setTransform(cached, _numMatrices);

        const MeshState& state = *_state[pass];
        bgfx::setState(state.m_state);

        for (uint8_t tex = 0; tex < state.m_numTextures; ++tex)
        {
            const MeshState::Texture& texture = state.m_textures[tex];
            bgfx::setTexture(
                    texture.m_stage
                    , texture.m_sampler
                    , texture.m_texture
                    , texture.m_flags
            );
        }

        for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd; ++it)
        {
            const Group& group = *it;

            bgfx::setIndexBuffer(group.m_ibh);
            bgfx::setVertexBuffer(0, group.m_vbh);
            bgfx::submit(
                    state.m_viewId
                    , state.m_program
                    , 0
                    , (it == itEnd - 1) ? (BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS | BGFX_DISCARD_STATE) : BGFX_DISCARD_NONE
            );
        }
    }
}

Mesh* meshLoad(bx::ReaderSeekerI* _reader, bool _ramcopy)
{
    Mesh* mesh = new Mesh;
    mesh->load(_reader, _ramcopy);
    return mesh;
}

Mesh* meshLoad(const char* _filePath, bool _ramcopy)
{
    bx::FileReaderI* reader = getFileReader();
    if (bx::open(reader, _filePath) )
    {
        Mesh* mesh = meshLoad(reader, _ramcopy);
        bx::close(reader);
        return mesh;
    }

    return NULL;
}

void meshUnload(Mesh* _mesh)
{
    _mesh->unload();
    delete _mesh;
}

MeshState* meshStateCreate()
{
    MeshState* state = (MeshState*)BX_ALLOC(getDefaultAllocator(), sizeof(MeshState) );
    return state;
}

void meshStateDestroy(MeshState* _meshState)
{

    BX_FREE(getDefaultAllocator(), _meshState);
}

void meshSubmit(const Mesh* _mesh, bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state)
{
    _mesh->submit(_id, _program, _mtx, _state);
}

void meshSubmit(const Mesh* _mesh, const MeshState*const* _state, uint8_t _numPasses, const float* _mtx, uint16_t _numMatrices)
{
    _mesh->submit(_state, _numPasses, _mtx, _numMatrices);
}

const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
    if (bx::open(_reader, _filePath) )
    {
        uint32_t size = (uint32_t)bx::getSize(_reader);
        const bgfx::Memory* mem = bgfx::alloc(size+1);
        bx::read(_reader, mem->data, size);
        bx::close(_reader);
        mem->data[mem->size-1] = '\0';
        return mem;
    }

    DBG("Failed to load %s.", _filePath);
    return NULL;
}

bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name)
{
    char filePath[512];

    const char* shaderPath = "???";

    switch (bgfx::getRendererType() )
    {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
        case bgfx::RendererType::Agc:
        case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
        case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
        case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
        case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
        case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
        case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
        case bgfx::RendererType::WebGPU:     shaderPath = "shaders/spirv/"; break;

        case bgfx::RendererType::Count:
            BX_ASSERT(false, "You should not be here!");
            break;
    }

    bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
    bx::strCat(filePath, BX_COUNTOF(filePath), _name);
    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, filePath) );
    bgfx::setName(handle, _name);

    return handle;
}

bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
{
    bgfx::ShaderHandle vsh = loadShader(_reader, _vsName);
    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
    if (NULL != _fsName)
    {
        fsh = loadShader(_reader, _fsName);
    }

    return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}

bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName)
{
    return loadProgram(getFileReader(), _vsName, _fsName);
}

