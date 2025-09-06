#pragma once

#ifndef LYRA_LIBRARY_AMS_TYPES_H
#define LYRA_LIBRARY_AMS_TYPES_H

#include <ctime>
#include <fstream>

#include <Lyra/Common/GUID.h>
#include <Lyra/Common/UUID.h>
#include <Lyra/Common/Path.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Container.h>
#include <Lyra/FileIO/FSEnums.h>
#include <Lyra/FileIO/FSUtils.h>
#include <Lyra/Assets/AMSAPI.h>

namespace lyra
{
    struct AssetManager
    {
    public:
        explicit AssetManager(const AMSDescriptor& descriptor)
            : descriptor(descriptor) {}

        // AssetType needs to define the following:
        // 1. static constexpr CString name
        // 2. static constexpr UUID uuid
        // 3. static constexpr List<CString> extensions
        // 4. static AssetImportAPI importer();
        // 5. static AssetLoaderAPI loader();
        template <typename AssetType>
        void register_asset()
        {
            loaders.emplace(AssetType::uuid, AssetType::loader());
            importers.emplace(AssetType::uuid, AssetType::importer());
            for (const auto& extension : AssetType::extensions)
                extensions.emplace(extension, AssetType::uuid);
        }

        // import the asset via asset path in actual file system,
        // returns a typed asset handle (with uuid)
        bool import_asset(const Path& path, UUID& uuid)
        {
            // sanity check if extensions has been registered
            auto ext = path.extension().string();
            auto it  = extensions.find(ext);
            if (it == extensions.end()) {
                engine::logger()->error("AssetManager cannot handle files with extension: {}", ext);
                return false;
            }

            // process asset with importer
            auto it2 = importers.find(it->second);
            assert(it2 != importers.end()); // shouldn't happen

            // asset GUID
            GUID guid = 0ull;

            // asset file paths
            Path source_path = descriptor.importer.asset_path / path;
            Path target_path = descriptor.importer.cache_path / path;
            Path import_file = get_metadata_path(source_path);

            // check if metadata exists, reuse uuid if possible
            if (std::filesystem::exists(import_file))
                load_guid(import_file, guid);

            // choose a random uuid if no uuid has ever been assigned
            if (guid != 0)
                guid = random_guid();

            // compose metadata
            JSON metadata;
            metadata["version"] = "1";
            metadata["time"]    = get_timestamp();
            metadata["guid"]    = guid;
            metadata["data"]    = it2->second.process(this, source_path.string().c_str(), target_path.string().c_str());

            // write metadata side by side with the imported asset
            save_json(import_file, metadata);
            return true;
        }

        // load the asset via asset path in virtual file system,
        // returns a typed asset handle (with uuid)
        template <typename AssetType>
        auto load_asset(VFSPath path) -> AssetHandle<AssetType>
        {
            // sanity check if given asset has been registered
            auto it = loaders.find(AssetType::uuid);
            if (it == loaders.end()) {
                engine::logger()->error("AssetType ({}) has not been registered!", AssetType::name);
                return nullptr;
            }

            auto metadata_file = get_metadata_path(Path(path));
            auto metadata_vfs  = metadata_file.string();

            FileLoader loader(descriptor.loader.metadata);

            // check if file exists
            if (loader.exists(metadata_vfs.c_str())) {
                engine::logger()->error("Failed to find metadata for {}", path);
                throw std::runtime_error("Failed to find asset metadata!");
            }

            // load asset guid
            auto data = loader.read(metadata_vfs.c_str());
            auto json = JSON::parse(data.begin(), data.end());
            auto guid = json["guid"].template get<GUID>();

            // issue asset load (if necessary)
            // TODO: move asset loading to worker threads
            auto it2 = assets.find(guid);
            if (it2 == assets.end()) {
                assets[guid] = it->second.load(descriptor.loader.assets, guid);
            }

            return AssetHandle<AssetType>{guid};
        }

        // // get the actual asset pointer, returns nullptr if asset is not ready
        // template <typename AssetType>
        // auto get_asset(AssetHandle<AssetType> handle) -> AssetType*
        // {
        //     // sanity check if given asset has been registered
        //     auto it = loaders.find(AssetType::uuid);
        //     if (it == loaders.end()) {
        //         engine::logger()->error("AssetType ({}) has not been registered!", AssetType::name);
        //         return nullptr;
        //     }
        // }

    private:
        auto get_metadata_path(Path path) const -> Path
        {
            path += ".import";
            return path;
        }

        auto get_timestamp() const -> time_t
        {
            time_t timestamp;
            time(&timestamp);
            return timestamp;
        }

        void load_guid(const Path& path, GUID& guid) const
        {
            std::ifstream f(path, std::ios::in);
            assert(f.good());
            JSON data = JSON::parse(f);
            if (data.contains("guid")) {
                guid = data["guid"].get<GUID>();
            }
            f.close();
        }

        void save_json(const Path& path, const JSON& data, int indent = 2) const
        {
            std::ofstream f(path, std::ios::out);
            assert(f.good());
            f << data.dump(indent);
            f.close();
        }

    private:
        // descriptor
        AMSDescriptor descriptor;

        // mapping from names to asset uuids
        HashMap<String, UUID> name2uuid;

        // mapping from extensions to asset type uuid
        HashMap<String, UUID> extensions;

        // mapping from asset type uuids to asset loaders
        HashMap<UUID, AssetLoaderAPI> loaders;

        // mapping from asset type uuids to asset importers
        // (only used in development time)
        HashMap<UUID, AssetImportAPI> importers;

        // loaded asset pointers
        HashMap<GUID, void*> assets;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_TYPES_H
