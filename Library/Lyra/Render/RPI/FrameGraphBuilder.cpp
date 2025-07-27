#include <Lyra/Render/RPI/FrameGraphBuilder.h>

using namespace lyra;
using namespace lyra::rpi;

FrameGraphPass& FrameGraphBuilder::add_pass(StringView name)
{
    auto psid = static_cast<uint>(graph->passes.size());
    auto pass = new FrameGraphPass(name);
    auto node = FrameGraphPassNode{pass, psid};
    graph->passes.push_back(node);
    this->pass = psid;
    return *graph->passes.back().entry;
}

FrameGraphResource FrameGraphBuilder::read(FrameGraphResource resource, FrameGraphReadOp op)
{
    assert(pass && "must call FrameGraphBuilder::add_pass(...) prior to FrameGraphBuilder::read(...)");

    // add resource to pass
    auto  read_resource = FrameGraphReadResource{resource, op};
    auto& pass_node     = graph->passes.at(pass);
    pass_node.reads.push_back(read_resource);

    // add pass to resource
    auto& resource_node = graph->resources.at(resource);
    resource_node.consumers.push_back(pass);
    resource_node.last_pass = pass;

    return resource;
}

FrameGraphResource FrameGraphBuilder::write(FrameGraphResource resource, FrameGraphWriteOp op)
{
    assert(pass && "must call FrameGraphBuilder::add_pass(...) prior to FrameGraphBuilder::write(...)");

    // add resource to pass
    auto  write_resource = FrameGraphWriteResource{resource, op};
    auto& pass_node      = graph->passes.at(pass);
    pass_node.writes.push_back(write_resource);

    // add pass to resource
    auto& resource_node = graph->resources.at(resource);
    resource_node.producers.push_back(pass);
    resource_node.last_pass = pass;

    return resource;
}

FrameGraphResource FrameGraphBuilder::render(FrameGraphResource resource)
{
    assert(pass && "must call FrameGraphBuilder::add_pass(...) prior to FrameGraphBuilder::render(...)");
    return write(resource, FrameGraphWriteOp::RENDER);
}

FrameGraphResource FrameGraphBuilder::sample(FrameGraphResource resource)
{
    assert(pass && "must call FrameGraphBuilder::add_pass(...) prior to FrameGraphBuilder::sample(...)");
    return read(resource, FrameGraphReadOp::SAMPLE);
}

Own<FrameGraph> FrameGraphBuilder::FrameGraphBuilder::build() &&
{
    graph->compile();
    return std::move(graph);
}
