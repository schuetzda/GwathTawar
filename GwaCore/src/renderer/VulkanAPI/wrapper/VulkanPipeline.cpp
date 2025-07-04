#include "VulkanPipeline.h"
#include "io/FileReader.h"
#include <stdexcept>
#include <array>
#include <cassert>
#include <span>
namespace gwa::renderer {

	VulkanPipeline::VulkanPipeline(VkDevice logicalDevice, const renderer::PipelineConfig& config, VkRenderPass renderPass, const VkPushConstantRange& pushConstantRange, std::span<const VkDescriptorSetLayout> descriptorSetLayout)
	{
		//Shader Creation
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo{};
		std::vector<VkShaderModule> shaderModules{};
		const size_t shaderModulesSize = config.shaderModules.size();
		shaderStagesInfo.resize(shaderModulesSize);
		shaderModules.resize(shaderModulesSize);

		for (size_t i = 0; i < shaderModulesSize; i++)
		{
			std::vector<char> shaderCode = FileReader::readBinaryFile<char>(config.shaderModules[i].shaderPath);
			 shaderModules[i] = createShaderModule(logicalDevice, shaderCode);

			shaderStagesInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStagesInfo[i].stage = static_cast<VkShaderStageFlagBits>(config.shaderModules[i].stage);
			shaderStagesInfo[i].module = shaderModules[i];
			shaderStagesInfo[i].pName = "main";
		}

		//Vertex Binding
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector< VkVertexInputAttributeDescription> attributeDescriptions{};
		const size_t vertexInputsSize = config.vertexInputs.size();
		bindingDescriptions.resize(vertexInputsSize);
		attributeDescriptions.resize(vertexInputsSize);
		for (size_t i = 0; i < vertexInputsSize; i++)
		{
			bindingDescriptions[i].binding = config.vertexInputs[i].binding;
			bindingDescriptions[i].stride = config.vertexInputs[i].stride;
			bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			attributeDescriptions[i].binding = config.vertexInputs[i].binding;
			attributeDescriptions[i].location = config.vertexInputs[i].location;
			attributeDescriptions[i].format = static_cast<VkFormat>(config.vertexInputs[i].format);
			attributeDescriptions[i].offset = config.vertexInputs[i].offset;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();		//List of Vertex Binding Description (data spacing, stride info,...)
		vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();	//data format

		//Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = static_cast<VkPrimitiveTopology>(renderer::PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		inputAssembly.primitiveRestartEnable = VK_FALSE;	//Ability to restart a strip topology

		// VIEWPORT & SCISSOR
		VkViewport viewport = {};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = config.viewport.width;
		viewport.height = config.viewport.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		// Create a scissor info struct
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };			// Offset to use region from
		scissor.extent = VkExtent2D{ static_cast<uint32_t>(config.viewport.width), static_cast<uint32_t>(config.viewport.height) };	// Extent to describe region to use, starting at offset

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		// DYNAMIC STATES (don't bake those directly into the pipeline) Have to resize depth buffer, recreate swap chain and swap chain images
		std::vector<VkDynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);	//Dynamic Viewport : Can resize in command buffer with vkCmdSetViewport(commandbuffer,0,1,&viewport);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);	//Dynamic Scissor : Can resize in command buffer with vkCmdSetScissor(commandbuffer,0,1,&scissor);

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();

		//Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = config.rasterizerConfig.depthClampEnabled;				// Change if fragments beyond near/ far planes are clipped or clamped. Remember to enable in device feature
		rasterizerCreateInfo.rasterizerDiscardEnable = config.rasterizerConfig.rasterizerDiscard;		// Whether to discard data and skip rasterizer. Never create fragments, only suitable for pipeline without framebuffer output.
		rasterizerCreateInfo.polygonMode = static_cast<VkPolygonMode>(config.rasterizerConfig.polygonMode);		// How to handle filling points between vertices
		rasterizerCreateInfo.lineWidth = config.rasterizerConfig.lineWidth;							// How thick lines should be when drawn
		rasterizerCreateInfo.cullMode = static_cast<VkCullModeFlags>(config.rasterizerConfig.cullMode);			// Which face of a triangle to cull
		rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;		// Winding to determine which side is front
		rasterizerCreateInfo.depthBiasEnable = config.rasterizerConfig.depthBiasEnable;				// Whether to add depth to fragments (good for stopping shadow acne)

		// MULTISAMPLING (using MSAA)
		VkPipelineMultisampleStateCreateInfo multiSamplingCreateInfo = {};
		multiSamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSamplingCreateInfo.sampleShadingEnable = config.enableMSAA;					// Enable multisample shading
		multiSamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// Number of samples to use per fragment

		// BLENDING How to blend a new colour being written to a fragment, with a previous value
		VkPipelineColorBlendAttachmentState colorState = {};
		colorState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT // Colors to apply blending to
			| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorState.blendEnable = VK_TRUE;

		// Blending equations (srcColorBlendFactor * newColor) colorBlendOp (dstColorBlendFactor * oldColor)
		colorState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorState.colorBlendOp = VK_BLEND_OP_ADD;

		colorState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorState.alphaBlendOp = VK_BLEND_OP_ADD;

		std::vector<VkPipelineColorBlendAttachmentState> colorStates(config.colorAttachmentCount, colorState);
		// Summarized (1 * newAlpha) = (0 * oldAlpha) = newAlpha

		VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
		colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingCreateInfo.logicOpEnable = VK_FALSE;					// Alternative calculation is to use logical operations
		colorBlendingCreateInfo.attachmentCount = config.colorAttachmentCount;
		colorBlendingCreateInfo.pAttachments = colorStates.data();

		// PIPELINE LAYOUT
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		// Create Pipeline Layout
		VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_);
		assert(result == VK_SUCCESS);

		// DEPTH STENCIL TESTING
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable = config.enableDepthTesting;		//Enalbing checking depth to determine fragment write
		depthStencilCreateInfo.depthWriteEnable = VK_TRUE;		// Enable writing to depth buffer (to replace old values)
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; // Comparison operation that allows an overwrite (is in front otherwise)
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;	// Does the depth value exist between two bounds
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;		// Whether to enable the stencil test


		// GRAPHICS PIPELINE CREATION
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStagesInfo.size());
		pipelineCreateInfo.pStages = shaderStagesInfo.data();
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo; \
			pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multiSamplingCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout_;
		pipelineCreateInfo.renderPass = renderPass;
		pipelineCreateInfo.subpass = 0;									// Subpass of render pass to use with pipeline

		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;	// Existing pipeline to derive from
		pipelineCreateInfo.basePipelineIndex = -1;				//What is the base pipeline, all other pipelines are derivatives of it for optimization

		result = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline_);
		assert(result == VK_SUCCESS);
		// Destroy Shader Modules, no longer needed after Pipeline created
		for (VkShaderModule shaderModule : shaderModules)
		{
			vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
		}
	}
		void VulkanPipeline::cleanup(VkDevice logicalDevice)
	{
		vkDestroyPipeline(logicalDevice, pipeline_, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipelineLayout_, nullptr);
	}
	VkShaderModule VulkanPipeline::createShaderModule(VkDevice logicalDevice, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
		assert(result == VK_SUCCESS);

		return shaderModule;
	}
}
