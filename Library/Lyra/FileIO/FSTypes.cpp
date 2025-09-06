#include <fstream>

#include <Lyra/Common/Enums.h>
#include <Lyra/Common/Assert.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/FileIO/FSTypes.h>

using namespace lyra;

using FileLoaderPlugin = Plugin<FileLoaderAPI>;
using FilePackerPlugin = Plugin<FilePackerAPI>;

static constexpr uint NUM_LOADER_BACKENDS = magic_enum::enum_count<FSLoader>();
static constexpr uint NUM_PACKER_BACKENDS = magic_enum::enum_count<FSPacker>();

static Own<FileLoaderPlugin> loader_plugins[NUM_LOADER_BACKENDS];
static Own<FilePackerPlugin> packer_plugins[NUM_PACKER_BACKENDS];

static FileLoaderAPI* create_file_loader_api(FSLoader loader)
{
    uint index = static_cast<uint>(loader);
    if (loader_plugins[index])
        return loader_plugins[index]->get_api();

    switch (loader) {
        case FSLoader::NATIVE:
            loader_plugins[index] = std::make_unique<FileLoaderPlugin>("lyra-nativefs");
            break;
        case FSLoader::PHYSFS:
            loader_plugins[index] = std::make_unique<FileLoaderPlugin>("lyra-physfs");
            break;
    }
    return loader_plugins[index]->get_api();
}

static FilePackerAPI* create_file_packer_api(FSPacker packer)
{
    uint index = static_cast<uint>(packer);
    if (packer_plugins[index])
        return packer_plugins[index]->get_api();

    switch (packer) {
        case FSPacker::PAK:
            packer_plugins[index] = std::make_unique<FilePackerPlugin>("lyra-pakbuilder");
            break;
        case FSPacker::ZIP:
            packer_plugins[index] = std::make_unique<FilePackerPlugin>("lyra-zipbuilder");
            break;
    }
    return packer_plugins[index]->get_api();
}

#pragma region FileLoader
FileLoader::FileLoader(FSLoader loader)
{
    api = create_file_loader_api(loader);
}

FileLoader::FileLoader(FileLoaderAPI* api)
{
    this->api = api;
}

FileLoader::~FileLoader()
{
    this->api = nullptr;
}

bool FileLoader::exists(VFSPath vpath) const
{
    return api->exists_file(vpath);
}

size_t FileLoader::size(VFSPath vpath) const
{
    return api->sizeof_file(vpath);
}

FileHandle FileLoader::open(VFSPath vpath) const
{
    FileHandle file;
    assert(api->open_file(file, vpath));
    return file;
}

void FileLoader::close(FileHandle file) const
{
    api->close_file(file);
}

Vector<uint8_t> FileLoader::read(VFSPath vpath) const
{
    Vector<uint8_t> data(size(vpath), 0);
    assert(api->read_whole_file(vpath, data.data()));
    return data;
}

size_t FileLoader::read(FileHandle file, void* buffer, size_t size) const
{
    size_t read;
    assert(api->read_file(file, buffer, size, read));
    return read;
}

void FileLoader::seek(FileHandle file, int64_t offset) const
{
    assert(api->seek_file(file, offset));
}

MountHandle FileLoader::mount(VFSPath vpath, FSPath path, uint priority) const
{
    MountHandle mount;
    assert(api->mount(mount, vpath, path, priority));
    return mount;
}

void FileLoader::unmount(MountHandle mount) const
{
    api->unmount(mount);
}
#pragma endregion FileLoader

#pragma region FilePacker
FilePacker::FilePacker(FSPacker packer, FSPath path)
{
    api = create_file_packer_api(packer);
    assert(api->open(archive, path));
}

FilePacker::~FilePacker()
{
    api->close(archive);
}

void FilePacker::write(VFSPath vpath, void* buffer, size_t size) const
{
    api->write(archive, vpath, buffer, size);
}

void FilePacker::write(VFSPath vpath, const Path& path) const
{
    // 1024B = 1kb
    // x1024 = 1Mb
    // x128  = 128Mb
    constexpr uint chunk = 1024 * 1024 * 128;

    // sanity check
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.good()) {
        engine::logger()->error("File {} does not exist!", path.string());
        return;
    }

    // get file size
    ifs.seekg(0, std::ios::end);
    std::size_t file_size = static_cast<std::size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    // small file → one-shot read
    if (file_size < chunk) {
        Vector<char> buffer(file_size);
        ifs.read(buffer.data(), buffer.size());
        write(vpath, buffer.data(), buffer.size());
        return;
    }

    // buffered writes
    Vector<char> buffer(chunk);
    while (ifs) {
        ifs.read(buffer.data(), buffer.size());
        std::streamsize read_bytes = ifs.gcount();
        if (read_bytes > 0)
            write(vpath, buffer.data(), static_cast<std::size_t>(read_bytes));
    }
}
#pragma endregion FilePacker
