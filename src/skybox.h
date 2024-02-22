#pragma once

#include "Model.h"
#include <string>
class Skybox : public Model
{
	unsigned m_number_of_indices = 0;

public:
	Texture cube_texture;

	/**
	 * @brief Create a model of a quad.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	Skybox(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

	/**
	 * @brief Render the model.
	*/
	virtual void Render() const;

	virtual void UpdateMaterialBuffer(linalg::vec4f ambient, linalg::vec4f diffuse, linalg::vec4f specular, int isSkybox) const;
	virtual void InitMaterialBuffer();
	void Swap(unsigned& a, unsigned& b);

	/**
	 *@brief Destructor.
	*/
	~Skybox();
};