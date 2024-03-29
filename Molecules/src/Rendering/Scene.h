#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include "Camera.h"
#include "RenderObjectList.h"
#include "MeshSet.h"
#include "Structs.h"
#include "../Simulation/Simulation.h"
#include "Shaders.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

class Scene
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSetBase>, std::vector<RenderObjectList>>;

public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources, Simulation* simulation, Evergreen::Viewport* viewport);
	void Update(const Evergreen::Timer& timer);
	void Render();

	// Event handlers
	void OnChar(Evergreen::CharEvent& e);
	void OnKeyPressed(Evergreen::KeyPressedEvent& e);
	void OnKeyReleased(Evergreen::KeyReleasedEvent& e);
	void OnMouseEntered(Evergreen::MouseMoveEvent& e);
	void OnMouseExited(Evergreen::MouseMoveEvent& e);
	void OnMouseMoved(Evergreen::MouseMoveEvent& e);
	void OnMouseScrolledVertical(Evergreen::MouseScrolledEvent& e);
	void OnMouseScrolledHorizontal(Evergreen::MouseScrolledEvent& e);
	void OnMouseButtonPressed(Evergreen::MouseButtonPressedEvent& e);
	void OnMouseButtonReleased(Evergreen::MouseButtonReleasedEvent& e);
	void OnClick(Evergreen::MouseButtonReleasedEvent& e);
	void OnDoubleClick(Evergreen::MouseButtonDoubleClickEvent& e);

	Camera* GetCamera() const noexcept { return m_camera.get(); }

	MaterialsArray* GetMaterials() noexcept { return m_materials.get(); }
	inline void UpdateMaterials()
	{
		m_materialsBuffer->UpdateData(m_materials.get());
	}

private:
	void CreateMainPipelineConfig();
	void CreateBoxPipelineConfig();
	void CreateMaterials();
	void LoadDefaultMaterials();

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Simulation* m_simulation;
	Evergreen::Viewport* m_viewport;

	std::vector<PipelineConfigAndObjectList> m_configsAndObjectLists;

	// Pass Constants that will be updated/bound only once per pass
	// NOTE: the ConstantBuffer is a shared_ptr so that it can be shared with EVERY PipelineConfig
	PassConstants m_passConstants;
	std::vector<std::shared_ptr<ConstantBuffer>> m_vsPerPassConstantsBuffers;
	std::vector<std::shared_ptr<ConstantBuffer>> m_psPerPassConstantsBuffers;

	std::unique_ptr<Camera> m_camera;

	// Materials
	std::shared_ptr<ConstantBuffer> m_materialsBuffer;
	std::unique_ptr<MaterialsArray> m_materials;
};