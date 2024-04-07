#include "glPCH.h"
#include "VulkanPipelineBuilder.h"


VkPipelineBuilder::VkPipelineBuilder()
{
    pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .pStages = VK_NULL_HANDLE,
        .pVertexInputState = VK_NULL_HANDLE,
        .pInputAssemblyState = &defaultInputAssemblyState,
        .pTessellationState = VK_NULL_HANDLE,
        .pViewportState = VK_NULL_HANDLE,
        .pRasterizationState = &defaultRasterizationState,
        .pMultisampleState = &defaultMultisamplingState,
        .pDepthStencilState = &defaultDepthStencilState,
        .pColorBlendState = &defaultColorBlendState,
        .renderPass = PVulkanPlatformInit::Get()->GetInfo()->renderPass
    };
}

void VkPipelineBuilder::SetInputAssembly(VkPipelineBuilderParams& params, VkPrimitiveTopology topology, bool primitiveRestart)
{
    params.inputAssemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .topology = topology,
        .primitiveRestartEnable = primitiveRestart
    };

    pipelineInfo.pInputAssemblyState = &params.inputAssemblyInfo;
}

void VkPipelineBuilder::LoadShaderModule(VkShaderStageConfigs shaderConfig, VkPipelineBuilderParams& params)
{
    VkShaderModule shader;
    if (!VkShaderUtil::LoadShaderModule(shaderConfig, shader))
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
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            // dictate what shader stage is being created on the graphics pipeline
            .stage = (VkShaderStageFlagBits)shaderConfig.shaderFlag,
            // add the shader module associated with this shader stage
            .module = shader,
            // the entry point of the shader
            .pName = "main"
        };

        params.shaderStages.push_back(shaderStageInfo);
        pipelineInfo.stageCount = (VkBool32)params.shaderStages.size();
        pipelineInfo.pStages = params.shaderStages.data();        
        params.shaders.push_back(shader);
    }
}

void VkPipelineBuilder::LoadDepthStencilState(VkPipelineBuilderParams& params, VkPipelineDepthStencilStateCreateInfo depthStencilInfo)
{
    params.depthStencilInfo = depthStencilInfo;
    pipelineInfo.pDepthStencilState = &params.depthStencilInfo;
}

void VkPipelineBuilder::LoadPipelineLayout(VkPipelineBuilderParams& params, VkPipelineLayout pipelineLayout)
{
    params.pipelineLayout = pipelineLayout;
    pipelineInfo.layout = pipelineLayout;
}

void VkPipelineBuilder::LoadColorBlendState(VkPipelineBuilderParams& params, VkPipelineColorBlendStateCreateInfo colorBlendInfo)
{
    params.colorBlendInfo = colorBlendInfo;
    pipelineInfo.pColorBlendState = &params.colorBlendInfo;
}

void VkPipelineBuilder::LoadMultispamplingState(VkPipelineBuilderParams& params, VkPipelineMultisampleStateCreateInfo multisampleInfo)
{
    params.multisamplingInfo = multisampleInfo;
    pipelineInfo.pMultisampleState = &params.multisamplingInfo;
}

void VkPipelineBuilder::LoadViewportInfo(VkPipelineBuilderParams& params, VkBool32 viewportCount, VkViewport* viewports, VkBool32 scissorCount, VkRect2D* scissors)
{
    params.viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .viewportCount = viewportCount,
        .pViewports = viewports,
        .scissorCount = scissorCount,
        .pScissors = scissors
    };
    for (VkBool32 viewportIndex = 0; viewportIndex < viewportCount; viewportIndex++)
        params.viewports.push_back(viewports[viewportIndex]);

    for (VkBool32 scissorIndex = 0; scissorIndex < scissorCount; scissorIndex++)
        params.scissors.push_back(scissors[scissorIndex]);
    
    
    pipelineInfo.pViewportState = &params.viewportInfo;
}

void VkPipelineBuilder::LoadViewportInfo(VkPipelineBuilderParams& params, VkExtent2D screenResolution)
{
    VkViewport viewport = {};
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.width = (float)screenResolution.width;
    viewport.height = (float)screenResolution.height;
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    params.viewports.push_back(viewport);

    VkRect2D scissor = {};
    scissor.extent = screenResolution;
    scissor.offset = { 0,0 };

    params.scissors.push_back(scissor);

    params.viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .viewportCount = (VkBool32)params.viewports.size(),
        .pViewports = params.viewports.data(),
        .scissorCount = (VkBool32)params.scissors.size(),
        .pScissors = params.scissors.data()
    };

    pipelineInfo.pViewportState = &params.viewportInfo;
}

//void VkPipelineBuilder::GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipelines, VkPipelineBuilderParams params)
//{
//    // TODO: insert return statement here
//    SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
//    VkShaderModule triangleFrag, triangleVert;
//    VkShaderStageConfigs triangleVertConfig = {};
//    triangleVertConfig.shaderFile = "trianglevert.spv";
//    triangleVertConfig.shaderFlag = VK_SHADER_STAGE_VERTEX_BIT;
//    if (!VkShaderUtil::LoadShaderModule(triangleVertConfig, triangleVert))
//        printf("Error! Unable to load vertex shader module!");
//
//    VkShaderStageConfigs triangleFragConfig = {};
//    triangleFragConfig.shaderFile = "trianglefrag.spv";
//    triangleFragConfig.shaderFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
//    if (!VkShaderUtil::LoadShaderModule(triangleFragConfig, triangleFrag))
//        printf("Error! Unable to load fragment shader module!");
//
//
//    pipelineSetup->AddShaderStageInfo(triangleVert, VK_SHADER_STAGE_VERTEX_BIT);
//    pipelineSetup->AddShaderStageInfo(triangleFrag, VK_SHADER_STAGE_FRAGMENT_BIT);
//
//    pipelineSetup->AddVertexInputInfo();
//    pipelineSetup->AddDepthStencilInfo();
//    pipelineSetup->AddColorBlendAttachmentInfo();
//    pipelineSetup->AddColorBlendStateInfo();
//    pipelineSetup->AddMultiSamplingInfo();
//    pipelineSetup->AddPipelineLayout();
//    pipelineSetup->AddRasterizationInfo();
//
//    
//    VkViewport viewport = {};
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;
//    viewport.width = (float)windowExtent.width;
//    viewport.height = (float)windowExtent.height;
//    viewport.x = 0.0f;
//    viewport.y = 0.0f;
//
//    VkRect2D scissor = {};
//    scissor.extent = windowExtent;
//    scissor.offset = { 0,0 };
//    pipelineSetup->AddViewportInfo(&viewport, &scissor, 1, 1);
//
//
//    auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
//    pipelineSetup->BuildPipelines(vkSettings->device, vkSettings->renderPass, pipelines);
//    
//}

void VkPipelineBuilder::CreateMeshShaderPipeline(VkPipeline* pipeline,  VkPipelineBuilderParams& params)
{
    auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
    auto result  = vkCreateGraphicsPipelines(vkSettings->device, VK_NULL_HANDLE, 1, &pipelineInfo, vkSettings->allocationCallback, pipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Unable to create graphics pipeline for mesh!");
    }
}

void VkPipelineBuilder::BuildVertexInputState(VkPipelineBuilderParams& params, VkPipelineVertexInputStateCreateInfo vertexInputInfo)
{
    params.vertexInputInfo = vertexInputInfo;
   
    pipelineInfo.pVertexInputState = &params.vertexInputInfo;
}