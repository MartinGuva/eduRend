/**
 * @file buffers.h
 * @brief Contains constant buffers
*/

#pragma once

#include "vec/mat.h"

/**
 * @brief Contains transformation matrices.
*/
struct TransformationBuffer
{
	linalg::mat4f ModelToWorldMatrix; //!< Matrix for converting from object space to world space.
	linalg::mat4f WorldToViewMatrix; //!< Matrix for converting from world space to view space.
	linalg::mat4f ProjectionMatrix; //!< Matrix for converting from view space to clip cpace.
};

struct LightCamBuffer
{
	linalg::vec4f lightPos;
	linalg::vec4f camPos;
};

struct MaterialBuffer
{
	linalg::vec4f ambientColor;// w does not contain anything
	linalg::vec4f diffuseColor; // w does not contain anything
	linalg::vec4f specularColor; // w contains shinyness
	alignas(16) int skybox;
};