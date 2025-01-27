/**
 * @file model.h
 * @brief Contains model definitions
 * @author Carl Johan Gribel 2016, cjgribel@gmail.com
*/

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "Drawcall.h"
#include "OBJLoader.h"
#include "Texture.h"
#include "buffers.h"

using namespace linalg;

/**
 * @brief Abstract class. Defines the Render method and contains mesh data needed for a model.
*/
class Model
{
protected:
	// Pointers to the current device and device context
	ID3D11Device* const			m_dxdevice; //!< Graphics device, use for creating resources.
	ID3D11DeviceContext* const	m_dxdevice_context; //!< Graphics context, use for binding resources and draw commands.

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* m_vertex_buffer = nullptr; //!< Pointer to gpu side vertex buffer
	ID3D11Buffer* m_index_buffer = nullptr; //!< Pointer to gpu side index buffer
	ID3D11Buffer* m_material_buffer = nullptr;

	std::vector<Material> m_materials;



	virtual void UpdateMaterialBuffer(linalg::vec4f ambient, linalg::vec4f diffuse, linalg::vec4f specular, int isSkybox) const = 0;
	virtual void InitMaterialBuffer() = 0;
public:

	/**
	 * @brief Sets the protected member variables to the input params.
	 * @param dxdevice ID3D11Device to be used in the model.
	 * @param dxdevice_context ID3D11DeviceContext to be used in the model.
	*/
	Model(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context) 
		:	m_dxdevice(dxdevice), m_dxdevice_context(dxdevice_context) { }

	/**
	 * @brief Abstract render method: must be implemented by derived classes
	*/
	virtual void Render() const = 0;

	void compute_TB(Vertex& v0, Vertex& v1, Vertex& v2) const
	{
		vec3f tangent, binormal;

		vec3f D = v1.Position - v0.Position;
		vec3f E = v2.Position - v0.Position;

		vec2f F = v1.TexCoord - v0.TexCoord;
		vec2f G = v2.TexCoord - v0.TexCoord;

		float det = 1 / (F.x * G.y - F.y * G.x);

		tangent = normalize((D * G.y + E * -F.y) * det);
		binormal = normalize((D * -G.x + E * F.x) * det);

		v0.Tangent = v1.Tangent = v2.Tangent = tangent;
		v0.Binormal = v1.Binormal = v2.Binormal = binormal;
	}


	/**
	 * @brief Destructor.
	 * @details Releases the vertex and index buffers of the Model.
	*/
	virtual ~Model()
	{ 
		SAFE_RELEASE(m_vertex_buffer);
		SAFE_RELEASE(m_index_buffer);
		SAFE_RELEASE(m_material_buffer);
	}
};

#endif