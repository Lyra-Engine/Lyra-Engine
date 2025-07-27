#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_RESOURCE_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_RESOURCE_H

#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Render/RPI/FrameGraphEnums.h>

namespace lyra::rpi
{
    using FrameGraphResource = std::uint32_t;

    struct FrameGraphResourceModel
    {
        FrameGraphResourceType type = FrameGraphResourceType::TRANSIENT;

        virtual ~FrameGraphResourceModel()    = default;
        virtual void create(void* allocator)  = 0;
        virtual void destroy(void* allocator) = 0;
    };

    template <typename T>
    struct FrameGraphResourceEntry : public FrameGraphResourceModel
    {
        typename T::Descriptor desc;
        T                      value;

        virtual T create(void* allocator)
        {
            if (type == FrameGraphResourceType::TRANSIENT)
                T::create(allocator);
        }

        virtual void destroy(void* allocator)
        {
            if (type == FrameGraphResourceType::TRANSIENT)
                T::destroy(allocator, value);
        }
    };

    struct FrameGraphResourceNode
    {
        FrameGraphResourceModel* entry     = nullptr;
        uint                     rsid      = 0;
        uint                     refcnt    = 0;
        uint                     last_pass = 0;
        Vector<uint>             consumers = {};
        Vector<uint>             producers = {};
    };

    struct FrameGraphResources
    {
    public:
        friend struct FrameGraph;

        void put(FrameGraphResource rsid, FrameGraphResourceModel* resource)
        {
            data[rsid] = resource;
        }

        template <typename T>
        const T& get(FrameGraphResource rsid) const
        {
            auto it = data.find(rsid);
            if (it == data.end())
                return nullptr;

            auto resource = dynamic_cast<FrameGraphResourceEntry<T>*>(it->second);
            return resource->value;
        }

    private:
        HashMap<FrameGraphResource, FrameGraphResourceModel*> data = {};
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_RESOURCE_H
