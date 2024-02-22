
#include "Scene.h"
#include "QuadModel.h"
#include "BoxModel.h"
#include "OBJModel.h"
#include "Skybox.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	InitLightCamBuffer();

	D3D11_SAMPLER_DESC samplerdesc = UpdateFilter(D3D11_FILTER_MIN_MAG_MIP_POINT);
	m_dxdevice->CreateSamplerState(&samplerdesc, &sampler);
	m_dxdevice_context->PSSetSamplers(0, 1, &sampler);



	D3D11_SAMPLER_DESC samplerdescSkybox =
	{
	D3D11_FILTER_MIN_MAG_MIP_LINEAR, // Filter
	D3D11_TEXTURE_ADDRESS_CLAMP, // AddressU
	D3D11_TEXTURE_ADDRESS_CLAMP, // AddressV
	D3D11_TEXTURE_ADDRESS_CLAMP, // AddressW
	0.0f, // MipLODBias
	16, // MaxAnisotropy
	D3D11_COMPARISON_NEVER, // ComapirsonFunc
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // BorderColor
	-FLT_MAX, // MinLOD
	FLT_MAX, // MaxLOD
	};
	m_dxdevice->CreateSamplerState(&samplerdescSkybox, &skyboxSampler);
	m_dxdevice_context->PSSetSamplers(1, 1, &skyboxSampler);
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	m_camera->MoveTo({ 0, 0, 5 });

	// Create objects
	m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);
	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context);
	m_trojan = new OBJModel("assets/Trojan/Trojan.obj", m_dxdevice, m_dxdevice_context);
	m_woodDoll = new OBJModel("assets/wooddoll/wooddoll.obj", m_dxdevice, m_dxdevice_context);
	m_boxModel = new BoxModel(m_dxdevice, m_dxdevice_context);
	m_boxModel2 = new BoxModel(m_dxdevice, m_dxdevice_context);
	m_boxModel3 = new BoxModel(m_dxdevice, m_dxdevice_context);
	m_boxModel4 = new BoxModel(m_dxdevice, m_dxdevice_context);
	skyboxModel = new Skybox(m_dxdevice, m_dxdevice_context);




}

D3D11_SAMPLER_DESC OurTestScene::UpdateFilter(D3D11_FILTER filter)
{
	if (sampler)
	{
		SAFE_RELEASE(sampler)
		sampler = nullptr;
	}

	D3D11_SAMPLER_DESC samplerdesc =
	{
	filter, // Filter
	D3D11_TEXTURE_ADDRESS_WRAP, // AddressU
	D3D11_TEXTURE_ADDRESS_WRAP, // AddressV
	D3D11_TEXTURE_ADDRESS_WRAP, // AddressW
	0.0f, // MipLODBias
	16, // MaxAnisotropy
	D3D11_COMPARISON_NEVER, // ComapirsonFunc
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // BorderColor
	-FLT_MAX, // MinLOD
	FLT_MAX, // MaxLOD
	};
	

	return samplerdesc;

}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
	{

	if (input_handler.IsKeyPressed(Keys::P))
	{
		D3D11_SAMPLER_DESC samplerdesc = UpdateFilter(D3D11_FILTER_MIN_MAG_MIP_POINT);
		m_dxdevice->CreateSamplerState(&samplerdesc, &sampler);
		m_dxdevice_context->PSSetSamplers(0, 1, &sampler);
	}
	else if (input_handler.IsKeyPressed(Keys::L))
	{
		D3D11_SAMPLER_DESC samplerdesc = UpdateFilter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
		m_dxdevice->CreateSamplerState(&samplerdesc, &sampler);
		m_dxdevice_context->PSSetSamplers(0, 1, &sampler);
	}
	else if (input_handler.IsKeyPressed(Keys::O))
	{
		D3D11_SAMPLER_DESC samplerdesc = UpdateFilter(D3D11_FILTER_ANISOTROPIC);
		m_dxdevice->CreateSamplerState(&samplerdesc, &sampler);
		m_dxdevice_context->PSSetSamplers(0, 1, &sampler);
	}

	m_camera->Rotate(input_handler.GetMouseDeltaX(), input_handler.GetMouseDeltaY());
	// Basic camera control
	if (input_handler.IsKeyPressed(Keys::Up) || input_handler.IsKeyPressed(Keys::W))
		m_camera->Move({ 0.0f, 0.0f, -m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Down) || input_handler.IsKeyPressed(Keys::S))
		m_camera->Move({ 0.0f, 0.0f, m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Right) || input_handler.IsKeyPressed(Keys::D))
		m_camera->Move({ m_camera_velocity * dt, 0.0f, 0.0f });
	if (input_handler.IsKeyPressed(Keys::Left) || input_handler.IsKeyPressed(Keys::A))
		m_camera->Move({ -m_camera_velocity * dt, 0.0f, 0.0f });


	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 
	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, -5) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	m_trojan_transform = mat4f::translation(-5, -2, -30);

	m_woodDoll_transform = mat4f::translation(-5, -2, 10) *
		mat4f::scaling(4);

	m_boxModel_transform = mat4f::translation(0, -3, -15) *
		mat4f::rotation(0.0f, 0.0f, 0.0f) *
		mat4f::scaling(2, 2, 2);

	

	mat4f box2Tra = mat4f::translation(0, 0, 20);
	mat4f box2Rot = mat4f::rotation(0.0f, -m_angle, 0.0f);
	//mat4f box2Rot = mat4f::rotation(0.0f, 0.0f, 0.0f);
	mat4f box2Sca = mat4f::scaling(1, 1, 1);;

	mat4f box3Tra = mat4f::translation(-2, 0, 0);
	mat4f box3Rot = mat4f::rotation(m_angle, 0.0f, 0.0f);
	//mat4f box3Rot = mat4f::rotation(0.0f, 0.0f, 0.0f);
	mat4f box3Sca = mat4f::scaling(0.75, 0.75, 0.75);

	mat4f box4Tra = mat4f::translation(0, 2, 0);
	mat4f box4Rot = mat4f::rotation(0.0f, 0.0f, m_angle);
	//mat4f box4Rot = mat4f::rotation(0.0f, 0.0f, 0.0f);
	mat4f box4Sca = mat4f::scaling(0.5, 0.5, 0.5);

	m_boxModel2_transform = box2Tra * box2Rot;

	m_boxModel3_transform = (box2Tra * box2Rot * box2Sca) * (box3Tra * box3Rot * box3Sca);

	m_boxModel4_transform = (box2Tra * box2Rot * box2Sca) * (box3Tra * box3Rot * box3Sca) * (box4Tra * box4Rot * box4Sca);




	skyboxTransform = mat4f::translation(-100, -100, 100) *
		mat4f::rotation(0.0f, 0.0f, 0.0f) *
		mat4f::scaling(200, 200, 200);

	// Increment the rotation angle.
	m_angle += m_angular_velocity * dt;
	lightPosZ += lightSpeed * dt;
	lightPosZ = 0;

	if (lightPosZ > 100)
	{
		lightPosZ = -100;
	}

	light_Transform = vec4f(0, 0, lightPosZ, 0);

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);


	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
	
	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();

	// Load matrices + the Quad's transformation to the device and render it


	m_dxdevice_context->PSSetConstantBuffers(0, 1, &lightCam_buffer);

	UpdateLightCamBuffer(light_Transform, vec4f(m_camera->m_position, 1));

	UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	m_quad->Render();

	UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	m_sponza->Render();

	UpdateTransformationBuffer(m_trojan_transform, m_view_matrix, m_projection_matrix);
	m_trojan->Render();

	UpdateTransformationBuffer(m_woodDoll_transform, m_view_matrix, m_projection_matrix);
	m_woodDoll->Render();

	UpdateTransformationBuffer(m_boxModel_transform, m_view_matrix, m_projection_matrix);
	m_boxModel->Render();

	UpdateTransformationBuffer(m_boxModel2_transform, m_view_matrix, m_projection_matrix);
	m_boxModel2->Render();

	UpdateTransformationBuffer(m_boxModel3_transform, m_view_matrix, m_projection_matrix);
	m_boxModel3->Render();

	UpdateTransformationBuffer(m_boxModel4_transform, m_view_matrix, m_projection_matrix);
	m_boxModel4->Render();

	UpdateTransformationBuffer(skyboxTransform, m_view_matrix, m_projection_matrix);
	skyboxModel->Render();

}

void OurTestScene::Release()
{
	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_boxModel);
	SAFE_DELETE(m_boxModel2);
	SAFE_DELETE(m_boxModel3);
	SAFE_DELETE(m_boxModel4);
	SAFE_DELETE(m_camera);
	SAFE_DELETE(m_trojan);
	SAFE_DELETE(skyboxModel);
	SAFE_DELETE(m_woodDoll);

	SAFE_RELEASE(m_transformation_buffer);
	SAFE_RELEASE(lightCam_buffer);

	SAFE_RELEASE(sampler);
	// + release other CBuffers
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}

void OurTestScene::InitLightCamBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC lightCamBufferDesc = { 0 };
	lightCamBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightCamBufferDesc.ByteWidth = sizeof(LightCamBuffer);
	lightCamBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCamBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCamBufferDesc.MiscFlags = 0;
	lightCamBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCamBufferDesc, nullptr, &lightCam_buffer));
}


void OurTestScene::UpdateLightCamBuffer(vec4f lightPos, vec4f camPos)
{
	// Map the resource buffer, obtain a pointer and then write our vectors to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(lightCam_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCamBuffer* lightCamBufferData = (LightCamBuffer*)resource.pData;
	lightCamBufferData->lightPos = lightPos;
	lightCamBufferData->camPos = camPos;
	m_dxdevice_context->Unmap(lightCam_buffer, 0);
}

