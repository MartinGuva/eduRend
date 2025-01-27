/**
 * @file scene.h
 * @brief Contains scene related classes
*/

#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "buffers.h"

/**
 * @brief Abstract class defining scene rendering and updating.
*/
class Scene
{
public:
	/**
	 * @brief Setup for member variables, no initialization is done here.
	 * @note These params are saved in the scene so they must be valid for as long as the scene is.
	 * @param[in] dxdevice ID3D11Device that will be used in the scene.
	 * @param[in] dxdevice_context ID3D11DeviceContext that will be used in the scene.
	 * @param[in] window_width Window hight for the scene.
	 * @param[in] window_height Window width for the scene.
	*/
	Scene(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, int window_width, int window_height);

	/**
	 * @brief Initialize all scene data.
	*/
	virtual void Init() = 0;

	/**
	 * @brief Relese all scene data created in Init()
	*/
	virtual void Release() = 0;

	/**
	 * @brief Update any relevant scene data.
	 * @param[in] delta_time Seconds since the last call.
	 * @param[in] input_handler Reference to the current InputHandler.
	*/
	virtual void Update(float delta_time, const InputHandler& input_handler) = 0;
	
	/**
	 * @brief Render the scene.
	*/
	virtual void Render() = 0;
	

	/**
	 * @brief Method called whenever the Window has changed size.
	 * @param[in] window_width New window width.
	 * @param[in] window_height New window height.
	*/
	virtual void OnWindowResized(int window_width,	int window_height);

protected:
	ID3D11Device*			m_dxdevice; //!< Graphics device, use for creating resources.
	ID3D11DeviceContext*	m_dxdevice_context; //!< Graphics context, use for binding resources and draw commands.
	int						m_window_width; //!< Current width of the window.
	int						m_window_height; //!< Current height of the window.
};

/**
 * @brief Test scene used in the project.
*/
class OurTestScene : public Scene
{
	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* m_transformation_buffer = nullptr;
	ID3D11Buffer* lightCam_buffer = nullptr;

	ID3D11SamplerState* sampler = nullptr;

	ID3D11SamplerState* skyboxSampler = nullptr;
	// + other CBuffers

	//
	// Scene content
	//
	Camera* m_camera;

	Model* m_quad;
	Model* m_sponza;
	Model* m_trojan;
	Model* m_woodDoll;

	Model* m_boxModel;
	Model* m_boxModel2;
	Model* m_boxModel3;
	Model* m_boxModel4;

	Model* skyboxModel;


	mat4f m_sponza_transform;
	mat4f m_quad_transform;
	mat4f m_trojan_transform;
	mat4f m_woodDoll_transform;

	vec4f light_Transform;

	mat4f m_boxModel_transform;
	mat4f m_boxModel2_transform;
	mat4f m_boxModel3_transform;
	mat4f m_boxModel4_transform;

	mat4f skyboxTransform;

	mat4f m_view_matrix;
	mat4f m_projection_matrix;

	// Misc
	float m_angle = 0;			// A per-frame updated rotation angle (radians)...
	float m_angular_velocity = fPI / 2;	// ...and its velocity (radians/sec)
	float m_camera_velocity = 5.0f;	// Camera movement velocity in units/s
	float m_fps_cooldown = 0;

	float lightSpeed = 10;
	float lightPosZ = -100;

	void InitTransformationBuffer();
	void InitLightCamBuffer();

	void UpdateLightCamBuffer(vec4f lightPos, vec4f camPos);
	void UpdateTransformationBuffer(mat4f model_to_world_matrix, mat4f world_to_view_matrix, mat4f projection_matrix);

public:
	/**
	 * @brief Constructor
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	 * @param window_width Current window width.
	 * @param window_height Current window height.
	*/
	OurTestScene(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context,	int window_width, int window_height);

	/**
	 * @brief Initializes all resources held by the scene.
	*/
	void Init() override;

	D3D11_SAMPLER_DESC UpdateFilter(D3D11_FILTER filter);

	/**
	 * @brief Updates all ojects in the scene
	 * @param dt Time in seconds since last iteration
	 * @param input_handler Current InputHandler
	*/
	void Update(float dt, const InputHandler& input_handler) override;

	/**
	 * @brief Renders all objects in the scene
	*/
	void Render() override;

	/**
	 * @brief Releases all resources created by the scene.
	*/
	void Release() override;

	/**
	 * @brief Updates all scene data that relates to Window size
	 * @param window_width New width
	 * @param window_height New height
	*/
	void OnWindowResized(int window_width, int window_height) override;
};

#endif