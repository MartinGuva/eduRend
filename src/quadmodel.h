/**
 * @file quadmodel.h
 * @brief Contains the QuadModel
 * @author Carl Johan Gribel 2016, cjgribel@gmail.com
*/

#pragma once
#include "model.h"

/**
 * @brief Model reprsenting a Quad.
*/
class QuadModel : public Model
{
	unsigned m_number_of_indices = 0;

public:
	/**
	 * @brief Create a model of a quad.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	QuadModel(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

	/**
	 * @brief Render the model.
	*/
	virtual void Render() const;
	virtual void UpdateMaterialBuffer(linalg::vec4f ambient, linalg::vec4f diffuse, linalg::vec4f specular, int isSkybox) const;
	virtual void InitMaterialBuffer();

	/**
	 *@brief Destructor. 
	*/
	~QuadModel();
};