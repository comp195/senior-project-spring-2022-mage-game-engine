#pragma once

#include "../pipeline-resources/pipeline.hpp"
#include "../pipeline-resources/device.hpp"
#include "../camera-resources/camera.hpp"
#include "object.hpp"
#include <vector>
#include <memory>

namespace mage {

	class TransportPass {
	private:	
  		VkPipelineLayout pipeline_layout;
  		DeviceHandling &device;
	public:
		TransportPass(DeviceHandling &device_pass, VkRenderPass render_pass);
		~TransportPass();
		std::unique_ptr<GraphicsPipeline> pipeline;
		void create_pipeline(VkRenderPass render_pass);
		void render_game_objects(VkCommandBuffer command_buffer, std::vector<GameObject> &game_objects, const CameraHandling &camera);
	};

}
