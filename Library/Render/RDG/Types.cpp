#include <Render/RDG/Types.h>

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::rdg;

#pragma region RDG
RDG::~RDG()
{
    // clear all buffers
    for (auto& buffer : buffers)
        if (!buffer.imported)
            buffer.buffer.destroy();

    // clear all textures
    for (auto& texture : textures)
        if (!texture.imported)
            texture.texture.destroy();

    // clear all passes
    for (auto& pass : passes)
        delete pass;

    textures.clear();
    buffers.clear();
    passes.clear();
}

void RDG::execute()
{
    TransitionState initial_state{};
    initial_state.sync   = GPUBarrierSync::NONE;
    initial_state.layout = GPUBarrierLayout::UNDEFINED;
    initial_state.access = GPUBarrierAccess::NO_ACCESS;

    // initialize all texture states
    texture_states.resize(textures.size(), initial_state);

    auto& device = RHI::get_current_device();

    // allocate command buffer
    auto descriptor = GPUCommandBufferDescriptor{};
    auto cmdbuffer  = device.create_command_buffer(descriptor);

    // iterate through all passes
    for (auto pass_id : sequence) {
        auto& pass = passes.at(pass_id);

        // state transition for all input textures
        for (auto& texture : pass->textures)
            transition(cmdbuffer, texture);

        // state transition for all attachments
        for (auto& attachment : pass->attachments)
            transition(cmdbuffer, attachment);
    }
}

void RDG::transition(const GPUCommandBuffer& cmdbuffer, const RDGTextureView& texture)
{
    auto& prev_state = texture_states.at(texture.texture.value);
    auto  curr_state = TransitionState{};

    switch (texture.action) {
        case RDGTextureAction::PRESENT:
            curr_state = present_src_state();
            break;
        case RDGTextureAction::READ:
        case RDGTextureAction::WRITE:
        case RDGTextureAction::SAMPLE:
            curr_state = shader_resource_state(texture.sync);
            break;
        default:
            assert(!!!"Unsupported RDGTextureAction!");
    }

    if (prev_state.layout != curr_state.layout) {
        auto  barrier = GPUTextureBarrier{};
        auto& tex     = get_texture(texture.texture);

        barrier.texture    = tex;
        barrier.src_sync   = prev_state.sync;
        barrier.src_access = prev_state.access;
        barrier.src_layout = prev_state.layout;
        barrier.dst_sync   = prev_state.sync;
        barrier.dst_access = curr_state.access;
        barrier.dst_layout = prev_state.layout;

        barrier.subresources.base_mip_level   = 0;
        barrier.subresources.base_array_layer = 0;
        barrier.subresources.mip_level_count  = tex.mip_level_count;
        barrier.subresources.array_layers     = tex.array_layers;
        cmdbuffer.resource_barrier(barrier);
    }

    texture_states.at(texture.texture.value) = curr_state;
}

void RDG::transition(const GPUCommandBuffer& cmdbuffer, const RDGAttachment& texture)
{
    auto& prev_state = texture_states.at(texture.texture.value);
    auto  curr_state = TransitionState{};

    switch (texture.type) {
        case RDGAttachmentType::COLOR:
        case RDGAttachmentType::COLOR_RESOLVE:
            curr_state = color_attachment_state();
            break;
        case RDGAttachmentType::DEPTH:
        case RDGAttachmentType::STENCIL:
        case RDGAttachmentType::DEPTH_STENCIL:
            curr_state = depth_stencil_attachment_state();
            break;
        default:
            assert(!!!"Unsupported RDGTextureAction!");
    }

    if (prev_state.layout != curr_state.layout) {
        auto  barrier = GPUTextureBarrier{};
        auto& tex     = get_texture(texture.texture);

        barrier.texture    = tex;
        barrier.src_sync   = prev_state.sync;
        barrier.src_access = prev_state.access;
        barrier.src_layout = prev_state.layout;
        barrier.dst_sync   = prev_state.sync;
        barrier.dst_access = curr_state.access;
        barrier.dst_layout = prev_state.layout;

        barrier.subresources.base_mip_level   = 0;
        barrier.subresources.base_array_layer = 0;
        barrier.subresources.mip_level_count  = tex.mip_level_count;
        barrier.subresources.array_layers     = tex.array_layers;
        cmdbuffer.resource_barrier(barrier);
    }

    texture_states.at(texture.texture.value) = curr_state;
}

const GPUBuffer& RDG::get_buffer(RDGBufferHandle handle)
{
    auto& view = buffers.at(handle.value);
    if (!view.buffer.handle.valid()) {
        auto& device = RHI::get_current_device();
        view.buffer  = device.create_buffer(view.descriptor);
    }
    return view.buffer;
}

const GPUTexture& RDG::get_texture(RDGTextureHandle handle)
{
    auto& view = textures.at(handle.value);
    if (!view.texture.handle.valid()) {
        auto& device = RHI::get_current_device();
        view.texture = device.create_texture(view.descriptor);
    }
    return view.texture;
}

#pragma endregion RDG

#pragma region RDGPass
RDGPass::RDGPass(RDG* graph, const String& label) : graph(graph), label(label)
{
    // do nothing
}

void RDGPass::read(RDGBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
    RDGBufferView view{};
    view.size   = size;
    view.offset = offset;
    view.buffer = buffer;
    view.action = RDGBufferAction::READ;
    buffers.push_back(view);
}

void RDGPass::write(RDGBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
    RDGBufferView view{};
    view.size   = size;
    view.offset = offset;
    view.buffer = buffer;
    view.action = RDGBufferAction::WRITE;
    buffers.push_back(view);
}

void RDGPass::read(RDGTextureHandle texture, GPUBarrierSync sync)
{
    RDGTextureView view{};
    view.sync    = sync;
    view.texture = texture;
    view.action  = RDGTextureAction::READ;
    textures.push_back(view);
}

void RDGPass::write(RDGTextureHandle texture, GPUBarrierSync sync)
{
    RDGTextureView view{};
    view.sync    = sync;
    view.texture = texture;
    view.action  = RDGTextureAction::WRITE;
    textures.push_back(view);
}

void RDGPass::sample(RDGTextureHandle texture, GPUBarrierSync sync)
{
    RDGTextureView view{};
    view.sync    = sync;
    view.texture = texture;
    view.action  = RDGTextureAction::SAMPLE;
    textures.push_back(view);
}

void RDGPass::present(RDGTextureHandle texture)
{
    preserve(texture);

    RDGTextureView view{};
    view.sync    = GPUBarrierSync::NONE;
    view.texture = texture;
    view.action  = RDGTextureAction::PRESENT;
    textures.push_back(view);
}

void RDGPass::preserve(RDGBufferHandle texture)
{
    auto& view     = graph->buffers.at(texture.value);
    view.preserved = true;
}

void RDGPass::preserve(RDGTextureHandle texture)
{
    auto& view     = graph->textures.at(texture.value);
    view.preserved = true;
}
#pragma endregion RDGPass

#pragma region RDGComputePass
RDGComputePass::RDGComputePass(RDG* graph, const String& label) : RDGPass(graph, label)
{
    // do nothing
}
#pragma endregion RDGComputePass

#pragma region RDGRenderPass
RDGRenderPass::RDGRenderPass(RDG* graph, const String& label) : RDGPass(graph, label)
{
    // do nothing
}

void RDGRenderPass::use_color_attachment(RDGTextureHandle texture, GPUStoreOp store)
{
    RDGAttachment attachment{};
    attachment.type     = RDGAttachmentType::COLOR;
    attachment.texture  = texture;
    attachment.load_op  = GPULoadOp::LOAD;
    attachment.store_op = store;

    attachments.push_back(attachment);
}

void RDGRenderPass::use_depth_attachment(RDGTextureHandle texture, GPUStoreOp store)
{
    RDGAttachment attachment{};
    attachment.type     = RDGAttachmentType::DEPTH;
    attachment.texture  = texture;
    attachment.load_op  = GPULoadOp::LOAD;
    attachment.store_op = store;

    attachments.push_back(attachment);
}

void RDGRenderPass::use_stencil_attachment(RDGTextureHandle texture, GPUStoreOp store)
{
    RDGAttachment attachment{};
    attachment.type     = RDGAttachmentType::STENCIL;
    attachment.texture  = texture;
    attachment.load_op  = GPULoadOp::LOAD;
    attachment.store_op = store;

    attachments.push_back(attachment);
}

void RDGRenderPass::use_depth_stencil_attachment(RDGTextureHandle texture, GPUStoreOp store)
{
    RDGAttachment attachment{};
    attachment.type     = RDGAttachmentType::DEPTH_STENCIL;
    attachment.texture  = texture;
    attachment.load_op  = GPULoadOp::LOAD;
    attachment.store_op = store;

    attachments.push_back(attachment);
}

void RDGRenderPass::clear_color(RDGTextureHandle texture, const GPUColor& color)
{
    for (auto& attachment : attachments) {
        if (attachment.texture == texture) {
            attachment.clear_color = color;
            return;
        }
    }
    assert(!!!"Failed to find attachment from input RDGTextureHandle!");
}

void RDGRenderPass::clear_depth(RDGTextureHandle texture, float depth)
{
    for (auto& attachment : attachments) {
        if (attachment.texture == texture) {
            attachment.clear_depth = depth;
            return;
        }
    }
    assert(!!!"Failed to find attachment from input RDGTextureHandle!");
}

void RDGRenderPass::clear_stencil(RDGTextureHandle texture, uint8_t stencil)
{
    for (auto& attachment : attachments) {
        if (attachment.texture == texture) {
            attachment.clear_stencil = stencil;
            return;
        }
    }
    assert(!!!"Failed to find attachment from input RDGTextureHandle!");
}
#pragma endregion RDGRenderPass

#pragma region RDGBuilder
RDGBuilder::RDGBuilder()
{
    rdg = std::make_unique<RDG>();
}

Own<RDG> RDGBuilder::compile()
{
    prune();

    // keep only pass with non-zero refcnts
    rdg->sequence.clear();
    for (uint32_t i = 0; i < rdg_pass_counter; i++)
        if (pass_refcnts.at(i) > 0)
            rdg->sequence.push_back(i);

    return std::move(rdg);
}

RDGRenderPass& RDGBuilder::create_render_pass(const String& label)
{
    auto pass = new RDGRenderPass(rdg.get(), label);
    rdg->passes.push_back(pass);
    pass->pid = rdg_pass_counter++;
    return *pass;
}

RDGComputePass& RDGBuilder::create_compute_pass(const String& label)
{
    auto pass = new RDGComputePass(rdg.get(), label);
    rdg->passes.push_back(pass);
    pass->pid = rdg_pass_counter++;
    return *pass;
}

RDGBufferHandle RDGBuilder::create_buffer(const String& label, const GPUBuffer& buffer)
{
    RDGBufferResource resource{};
    resource.rid       = rdg_buffer_counter++;
    resource.label     = label;
    resource.buffer    = buffer;
    resource.imported  = true;
    resource.preserved = false;

    rdg->buffers.push_back(resource);
    return RDGBufferHandle(resource.rid);
}

RDGBufferHandle RDGBuilder::create_buffer(const String& label, const GPUBufferDescriptor& desc)
{
    RDGBufferResource resource{};
    resource.rid        = rdg_buffer_counter++;
    resource.label      = label;
    resource.descriptor = desc;
    resource.imported   = false;
    resource.preserved  = false;

    rdg->buffers.push_back(resource);
    return RDGBufferHandle(resource.rid);
}

RDGBufferHandle RDGBuilder::create_buffer(const String& label, GPUSize64 size, GPUBufferUsageFlags usages)
{
    RDGBufferResource resource{};
    resource.rid              = rdg_buffer_counter++;
    resource.label            = label;
    resource.descriptor.label = resource.label.c_str();
    resource.descriptor.size  = size;
    resource.descriptor.usage = usages;
    resource.imported         = false;
    resource.preserved        = false;

    rdg->buffers.push_back(resource);
    return RDGBufferHandle(resource.rid);
}

RDGTextureHandle RDGBuilder::create_texture(const String& label, const GPUTexture& texture)
{
    RDGTextureResource resource{};
    resource.rid       = rdg_texture_counter++;
    resource.label     = label;
    resource.texture   = texture;
    resource.imported  = true;
    resource.preserved = false;

    rdg->textures.push_back(resource);
    return RDGTextureHandle(resource.rid);
}

RDGTextureHandle RDGBuilder::create_texture(const String& label, const GPUTextureDescriptor& desc)
{
    RDGTextureResource resource{};
    resource.rid              = rdg_texture_counter++;
    resource.label            = label;
    resource.descriptor.label = resource.label.c_str();
    resource.descriptor       = desc;
    resource.imported         = false;
    resource.preserved        = false;

    rdg->textures.push_back(resource);
    return RDGTextureHandle(resource.rid);
}

RDGTextureHandle RDGBuilder::create_texture(const String& label, GPUTextureFormat format, GPUExtent2D extent)
{
    RDGTextureResource resource{};
    resource.rid                     = rdg_texture_counter++;
    resource.label                   = label;
    resource.descriptor.label        = resource.label.c_str();
    resource.descriptor.format       = format;
    resource.descriptor.size.width   = extent.width;
    resource.descriptor.size.height  = extent.height;
    resource.descriptor.size.depth   = 1;
    resource.descriptor.array_layers = 1;
    resource.imported                = false;
    resource.preserved               = false;

    rdg->textures.push_back(resource);
    return RDGTextureHandle(resource.rid);
}

void RDGBuilder::enable_async_compute()
{
    assert(!"RDG::enable_async_compute() is not supported yet!");
    async_compute = true;
}

void RDGBuilder::prune()
{
    init_relations();
    init_refcnts();
    init_unused();

    // pop a resource and decrement ref count of its producer,
    // if producer.refCount == 0, decrement ref counts of resources that it reads,
    // add them	to the stack when their refCount == 0

    auto update_pass = [&](RDGPass* pass) {
        // decrement pass.refCnt,
        // and decrement all resources it reads if the pass is culled
        if (--pass_refcnts.at(pass->pid) <= 0)
            return;

        // decrement buffer resource refcnt
        for (auto& buffer : pass->buffers) {
            if (is_read_action(buffer.action))
                if (--buffer_refcnts.at(buffer.buffer.value) == 0)
                    unused_buffers.push_back(buffer.buffer.value);
        }

        // decrement texture resource refcnt
        for (auto& texture : pass->textures) {
            if (is_read_action(texture.action))
                if (--texture_refcnts.at(texture.texture.value) == 0)
                    unused_textures.push_back(texture.texture.value);
        }
    };

    // loop through all unused buffers and textures
    while (unused_buffers.size() + unused_textures.size()) {

        // check unused buffers
        if (!unused_buffers.empty()) {
            uint32_t rid = unused_buffers.back();
            unused_buffers.pop_back();

            // ignore preserved buffers
            auto& buffer = rdg->buffers.at(rid);
            if (!buffer.preserved) continue;

            for (auto& producer : buffer_producers.at(rid))
                update_pass(rdg->passes.at(producer));
        }

        // check unused textures
        if (!unused_textures.empty()) {
            uint32_t rid = unused_textures.back();
            unused_textures.pop_back();

            // ignore preserved textures
            auto& texture = rdg->textures.at(rid);
            if (!texture.preserved) continue;

            for (auto& producer : texture_producers.at(rid))
                update_pass(rdg->passes.at(producer));
        }
    }
}

void RDGBuilder::init_refcnts()
{
    pass_refcnts.resize(rdg_pass_counter, 0);
    buffer_refcnts.resize(rdg_buffer_counter, 0);
    texture_refcnts.resize(rdg_texture_counter, 0);

    // resource.refCount++ for every resource read
    // pass.refCount++ for every resource write
    for (auto& pass : rdg->passes) {

        // buffer inputs
        pass->iter_buffer_inputs([&](auto handle) {
            buffer_refcnts.at(handle.value)++;
        });

        // buffer outputs
        pass->iter_buffer_outputs([&](auto _) {
            pass_refcnts.at(pass->pid)++;
        });

        // texture inputs
        pass->iter_texture_inputs([&](auto handle) {
            texture_refcnts.at(handle.value)++;
        });

        // texture inputs
        pass->iter_texture_outputs([&](auto _) {
            pass_refcnts.at(pass->pid)++;
        });
    }
}

void RDGBuilder::init_relations()
{
    buffer_producers.resize(rdg_buffer_counter);
    texture_producers.resize(rdg_texture_counter);

    for (auto& pass : rdg->passes) {

        // buffer outputs
        pass->iter_buffer_outputs([&](auto handle) {
            buffer_producers.at(handle.value).push_back(pass->pid);
        });

        // texture outputs
        pass->iter_texture_outputs([&](auto handle) {
            texture_producers.at(handle.value).push_back(pass->pid);
        });
    }
}

void RDGBuilder::init_unused()
{
    for (auto& buffer : rdg->buffers)
        if (buffer_refcnts.at(buffer.rid) <= 0)
            unused_buffers.push_back(buffer.rid);

    for (auto& texture : rdg->textures)
        if (texture_refcnts.at(texture.rid) <= 0)
            unused_textures.push_back(texture.rid);
}
#pragma endregion RDGBuilder
