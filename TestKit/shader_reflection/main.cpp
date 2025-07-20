#include "helper.h"

void test_shader_vertex_attribute_reflection(CompileTarget target, CompileFlags flags)
{
    String code = R"""(
    struct VertexInput
    {
        float3 position : POSITION;
        float2 texcoord : TEXCOORD0;
        float3 color    : TEXCOORD1;
    };

    struct VertexOutput
    {
        float4 position : SV_POSITION;
        float3 color;
    };

    struct Camera
    {
        float4x4 proj;
        float4x4 view;
    };

    struct Hello
    {
        ConstantBuffer<Camera> cam;
        Texture2D<float4> tex;
        Texture2D<float4> tex2;
        SamplerState smp;
    };

    // [[vk::binding(10, 5)]]
    // ConstantBuffer<Camera> camera : register(b11, space6);

    ParameterBlock<Hello> haha;
    ParameterBlock<Hello> bibi;

    [shader("vertex")]
    VertexOutput vsmain(VertexInput input)
    {
        VertexOutput output;
        output.color = input.color;
        output.position = float4(input.position, 1.0);
        output.position = mul(output.position, haha.cam.view);
        output.position = mul(output.position, bibi.cam.proj);
        return output;
    }

    [shader("fragment")]
    float4 fsmain(VertexOutput input) : SV_TARGET
    {
        return haha.tex.Sample(haha.smp, input.color.xy);
    }
    )""";

    // initialize compiler
    auto compiler = execute([&]() {
        auto desc   = CompilerDescriptor{};
        desc.target = target;
        desc.flags  = flags;
        return Compiler::init(desc);
    });

    auto module = execute([&]() {
        auto desc   = CompileDescriptor{};
        desc.module = "test";
        desc.path   = "test.slang";
        desc.source = code.c_str();
        return compiler->compile(desc);
    });

    auto reflection = compiler->reflect({
        {*module, "vsmain"},
        {*module, "fsmain"},
    });

    auto attributes = reflection->get_vertex_attributes({
        {"position", offsetof(Vertex, position)},
        {"texcoord", offsetof(Vertex, uv)},
    });

    std::cout << "attributes.size() = " << attributes.size() << std::endl;
    for (auto& attrib : attributes) {
        std::cout << "attrib[" << attrib.shader_location << "].offset = " << attrib.offset << std::endl;
        std::cout << "attrib[" << attrib.shader_location << "].location = " << attrib.shader_location << std::endl;
        std::cout << "attrib[" << attrib.shader_location << "].semantics = " << attrib.shader_semantics << std::endl;
    }

    auto bindgroups = reflection->get_bind_group_layouts();
    std::cout << "attributes.size() = " << attributes.size() << std::endl;
    for (auto& bindgroup : bindgroups) {
        std::cout << "bindgroup: " << bindgroup.label << std::endl;
        for (auto& entry : bindgroup.entries) {
            std::cout << "- entries[" << entry.binding << "].type       = " << (int)entry.type << std::endl;
            std::cout << "- entries[" << entry.binding << "].binding    = " << entry.binding << std::endl;
            std::cout << "- entries[" << entry.binding << "].count      = " << entry.count << std::endl;
            std::cout << "- entries[" << entry.binding << "].visibility =";
            if (entry.visibility.contains(GPUShaderStage::VERTEX)) std::cout << " VERTEX";
            if (entry.visibility.contains(GPUShaderStage::FRAGMENT)) std::cout << " FRAGMENT";
            if (entry.visibility.contains(GPUShaderStage::COMPUTE)) std::cout << " COMPUTE";
            std::cout << std::endl;
        }
    }
}

TEST_CASE("slc::vulkan::shader_reflection" * doctest::description("shader vertex attributes reflection"))
{
    test_shader_vertex_attribute_reflection(
        CompileTarget::SPIRV,
        CompileFlag::DEBUG | CompileFlag::REFLECT);
}
