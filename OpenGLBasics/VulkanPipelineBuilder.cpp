#include "glPCH.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanRasterizationPresets.h"
#include "VulkanMultisamplingPresets.h"


void VkPipelineBuilder::SetInputAssembly(VkPrimitiveTopology* _topology)
{
    pipelineSetup->AddInputAssemblyInfo(*_topology);
}

void VkPipelineBuilder::LoadShaderModule(VkShaderStageConfigs& shaderConfig, VkShaderModule& shaderModule)
{
    if (!VkShaderUtil::LoadShaderModule(shaderConfig, shaderModule))
    {
        std::string shaderStageType = "";

        auto shaderStage = shaderConfig.shaderFlag;

        switch (shaderStage)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            shaderStageType = "vertex";
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            shaderStageType = "fragment";
            break;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            shaderStageType = "geometry";
            break;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            shaderStageType = "compute";
            break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            shaderStageType = "tessellation control";
            break;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            shaderStageType = "tessellation evaluation";
            break;
        default:
            shaderStageType = "default vertex";
            break;
        }

        printf("Unable to load %s shader stage of module!", shaderStageType.c_str());
    }
    else
        pipelineSetup->AddShaderStageInfo(shaderModule, (VkShaderStageFlagBits)shaderConfig.shaderFlag);
}

void VkPipelineBuilder::GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipelines)
{
    // TODO: insert return statement here
    VkPipelineSetup* pipelineSetup = new VkPipelineSetup();
    pipelineSetup->AddInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkShaderModule triangleFrag, triangleVert;
    VkShaderStageConfigs triangleVertConfig = {};
    triangleVertConfig.shaderFile = "trianglevert.spv";
    triangleVertConfig.shaderFlag = VK_SHADER_STAGE_VERTEX_BIT;
    if (!VkShaderUtil::LoadShaderModule(triangleVertConfig, triangleVert))
        printf("Error! Unable to load vertex shader module!");

    VkShaderStageConfigs triangleFragConfig = {};
    triangleFragConfig.shaderFile = "trianglefrag.spv";
    triangleFragConfig.shaderFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
    if (!VkShaderUtil::LoadShaderModule(triangleFragConfig, triangleFrag))
        printf("Error! Unable to load fragment shader module!");

    pipelineSetup->AddShaderStageInfo(triangleVert, VK_SHADER_STAGE_VERTEX_BIT);
    pipelineSetup->AddShaderStageInfo(triangleFrag, VK_SHADER_STAGE_FRAGMENT_BIT);

    pipelineSetup->AddVertexInputInfo();
    pipelineSetup->AddColorBlendAttachmentInfo();
    pipelineSetup->AddColorBlendStateInfo();
    pipelineSetup->AddMultiSamplingInfo(VkMultisamplingPresets::DefaultSingleSamplingSetup());
    pipelineSetup->AddPipelineLayout();
    pipelineSetup->AddDepthStencilInfo();
    pipelineSetup->AddRasterizationInfo(VkRasterizationPresets::Default2DImageRasterization());

    
    VkViewport viewport = {};
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.width = (float)windowExtent.width;
    viewport.height = (float)windowExtent.height;
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    VkRect2D scissor = {};
    scissor.extent = windowExtent;
    scissor.offset = { 0,0 };
    pipelineSetup->AddViewportInfo(&viewport, &scissor, 1, 1);


    auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
    pipelineSetup->BuildPipelines(vkSettings->device, vkSettings->renderPass, pipelines);
    delete pipelineSetup;
    
}
