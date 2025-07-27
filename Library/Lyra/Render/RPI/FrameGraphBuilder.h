#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUILDER_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUILDER_H

#include <Lyra/Common/Pointer.h>
#include <Lyra/Render/RPI/FrameGraph.h>
#include <Lyra/Render/RPI/FrameGraphEnums.h>
#include <Lyra/Render/RPI/FrameGraphResource.h>

namespace lyra::rpi
{
    struct FrameGraph;

    struct FrameGraphBuilder
    {
    public:
        explicit FrameGraphBuilder() { graph = std::make_unique<FrameGraph>(); }
        explicit FrameGraphBuilder(FrameGraphBuilder&&)      = delete;
        explicit FrameGraphBuilder(const FrameGraphBuilder&) = delete;
        virtual ~FrameGraphBuilder()                         = default;

        FrameGraphBuilder&  operator=(const FrameGraphBuilder&) = delete;
        FrameGraphBuilder&& operator=(FrameGraphBuilder&&)      = delete;

        template <typename T>
        [[nodiscard]] FrameGraphResource import(const T& entry)
        {
            uint index           = static_cast<uint>(graph->resources.size());
            auto resource        = FrameGraphResourceNode{};
            resource.rsid        = index;
            resource.entry       = new FrameGraphResourceEntry<T>{};
            resource.entry->type = FrameGraphResourceType::IMPORTED;

            // imported resources directly stores the resource entry
            *(reinterpret_cast<T*>(resource.entry)) = entry;

            // mark the resource to be created at the current pass
            auto& pass_node = graph->passes.at(pass);
            pass_node.creates.push_back(index);
            return index;
        }

        template <typename T>
        [[nodiscard]] FrameGraphResource create(const typename T::Descriptor& desc)
        {
            uint index           = static_cast<uint>(graph->resources.size());
            auto resource        = FrameGraphResourceNode{};
            resource.rsid        = index;
            resource.entry       = new FrameGraphResourceEntry<T>{};
            resource.entry->type = FrameGraphResourceType::TRANSIENT;

            // mark the resource to be created at the current pass
            auto& pass_node = graph->passes.at(pass);
            pass_node.creates.push_back(index);
            return index;
        }

        [[nodiscard]] FrameGraphPass& add_pass(StringView name);

        [[nodiscard]] FrameGraphResource read(FrameGraphResource resource, FrameGraphReadOp op = FrameGraphReadOp::READ);
        [[nodiscard]] FrameGraphResource write(FrameGraphResource resource, FrameGraphWriteOp op = FrameGraphWriteOp::WRITE);
        [[nodiscard]] FrameGraphResource render(FrameGraphResource resource);
        [[nodiscard]] FrameGraphResource sample(FrameGraphResource resource);

        [[nodiscard]] auto build() && -> Own<FrameGraph>;

        // avoid build from being called twice
        void build() & = delete;

    private:
        Own<FrameGraph> graph = nullptr;
        uint            pass  = 0u;
    }; // end of FrameGraphBuilder

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUILDER_H
