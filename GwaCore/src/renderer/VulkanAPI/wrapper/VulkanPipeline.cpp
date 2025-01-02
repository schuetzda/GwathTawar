#include "VulkanPipeline.h"
#include "tools/FileReader.h"
#include <stdexcept>
#include <array>
#include <cassert>
namespace gwa {
	VulkanPipeline::VulkanPipeline(VkDevice logicalDevice, uint32_t stride, const std::vector<uint32_t>& attributeDescriptionOffsets,
		VkRenderPass renderPass, const VkExtent2D& swapchainExtent, const VkPushConstantRange& pushConstantRange, 
		VkDescriptorSetLayout descriptorSetLayout): logicalDevice_(logicalDevice)
	{
		std::vector<char> vertexShaderCode = readBinaryFile("src/shaders/vert.spv");
		std::vector<char> fragmentShaderCode = readBinaryFile("src/shaders/frag.spv");

		VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
		VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

		// SHADER STAGE CREATIION
		//Vertex Stage creation information
		VkPipelineShaderStageCreateInfo vertexCreateInfo = {};
		vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexCreateInfo.module = vertexShaderModule;
		vertexCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentCreateInfo = {};
		fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentCreateInfo.module = fragmentShaderModule; 
		fragmentCreateInfo.pName = "main";

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{ vertexCreateInfo, fragmentCreateInfo };

		// How the data for a single vertex is as a whole
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;								// Can bind multiple streams of data, this defines which one

		bindingDescription.stride = stride;					// Size of a single vertex object
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	// VK_VERTEX_INPUT_RATE_VERTEX: Move on to the next vertex
		// VK_VERTEX_INPUT_RATE_INSTANCE: Move on to a vertex for the next instance

		// How the data for an attribute is defined within a vertex
		const uint32_t attributeDescriptionsSize = 2;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(attributeDescriptionsSize);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Format the data will take. Also defines size of format
		attributeDescriptions[0].offset = attributeDescriptionOffsets[0];

		// Colour Attribute
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = attributeDescriptionOffsets[1];


		// VERTEX INPUT (TODO: Put in vertex descriptions when resources are created) Vertex -> Primitives
		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;		//List of Vertex Binding Description (data spacing, stride info,...)
		vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptionsSize;
		vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();	//data format

		// INPUT ASSEMBLYconst 
		// What kind of topology will be drawn?
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;	//Ability to restart a strip topology

		// VIEWPORT & SCISSOR
		VkViewport viewport = {};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = (float) swapchainExtent.width;
		viewport.height = (float) swapchainExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		// Create a scissor info struct
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };			// Offset to use region from
		scissor.extent = swapchainExtent;	// Extent to describe region to use, starting at offset

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

		// RASTERIZER Primitves -> Fragments
		//Configuration for the fixed-function rasterization. In here is where we enable or disable backface culling, and set line width or wireframe drawing.
		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = VK_FALSE;				// Change if fragments beyond near/ far planes are clipped or clamped. Remember to enable in device feature
		rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;		// Whether to discard data and skip rasterizer. Never create fragments, only suitable for pipeline without framebuffer output.
		rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;		// How to handle filling points between vertices
		rasterizerCreateInfo.lineWidth = 1.f;							// How thick lines should be when drawn
		rasterizerCreateInfo.cullMode = VK_CULL_MODE_NONE;			// Which face of a triangle to cull
		rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;		// Winding to determine which side is front
		rasterizerCreateInfo.depthBiasEnable = VK_FALSE;				// Whether to add depth to fragments (good for stopping shadow acne)

		// MULTISAMPLING (using MSAA)
		VkPipelineMultisampleStateCreateInfo multiSamplingCreateInfo = {};
		multiSamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSamplingCreateInfo.sampleShadingEnable = VK_FALSE;					// Enable multisample shading
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
		// Summarized (1 * newAlpha) = (0 * oldAlpha) = newAlpha

		VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
		colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingCreateInfo.logicOpEnable = VK_FALSE;					// Alternative calculation is to use logical operations
		colorBlendingCreateInfo.attachmentCount = 1;
		colorBlendingCreateInfo.pAttachments = &colorState;

		// PIPELINE LAYOUT
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		// Create Pipeline Layout
		VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_);
		assert(result == VK_SUCCESS);

		// DEPTH STENCIL TESTING
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable = VK_TRUE;		//Enalbing checking depth to determine fragment write
		depthStencilCreateInfo.depthWriteEnable = VK_TRUE;		// Enable writing to depth buffer (to replace old values)
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; // Comparison operation that allows an overwrite (is in front otherwise)
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;	// Does the depth value exist between two bounds
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;		// Whether to enable the stencil test


		// GRAPHICS PIPELINE CREATION
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages.data();
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
		vkDestroyShaderModule(logicalDevice, fragmentShaderModule, nullptr);
		vkDestroyShaderModule(logicalDevice, vertexShaderModule, nullptr);

	}
	void VulkanPipeline::cleanup()
	{
		vkDestroyPipeline(logicalDevice_, pipeline_, nullptr);
		vkDestroyPipelineLayout(logicalDevice_, pipelineLayout_, nullptr);
	}
	VkShaderModule VulkanPipeline::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(logicalDevice_, &shaderModuleCreateInfo, nullptr, &shaderModule);
		assert(result == VK_SUCCESS);

		return shaderModule;
	}
}
