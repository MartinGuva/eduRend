#include "QuadModel.h"

QuadModel::QuadModel(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	// Vertex and index arrays
	// Once their data is loaded to GPU buffers, they are not needed anymore
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	// Populate the vertex array with 4 Vertices
	Vertex v0, v1, v2, v3;
	v0.Position = { -0.5, -0.5f, 0.0f };
	v0.Normal = { 0, 0, 1 };
	v0.TexCoord = { 0, 0 };
	v1.Position = { 0.5, -0.5f, 0.0f };
	v1.Normal = { 0, 0, 1 };
	v1.TexCoord = { 1, 0 };
	v2.Position = { 0.5, 0.5f, 0.0f };
	v2.Normal = { 0, 0, 1 };
	v2.TexCoord = { 1, 1 };
	v3.Position = { -0.5, 0.5f, 0.0f };
	v3.Normal = { 0, 0, 1 };
	v3.TexCoord = { 0, 1 };
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	// Populate the index array with two triangles
	// Triangle #1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	// Triangle #2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc { 0 };
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = { 0 };
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData { 0 };
	indexData.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	m_number_of_indices = (unsigned int)indices.size();

	Material material = Material();

	material.AmbientColour = linalg::vec3f((0.0f, 0.0f, 0.4f));
	material.DiffuseColour = linalg::vec3f(0.0f, 0.0f, 0.6f);
	material.SpecularColour = linalg::vec3f(1.0f, 1.0f, 1.0f);

	HRESULT hr;
	hr = LoadTextureFromFile(
		dxdevice,
		"assets/textures/brick_diffuse.png",
		&material.DiffuseTexture);
	std::cout << "\t" << material.DiffuseTextureFilename
		<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;

	hr = LoadTextureFromFile(
		dxdevice,
		dxdevice_context,
		"assets/textures/brick_bump.png",
		&material.NormalTexture);
	std::cout << "\t" << material.DiffuseTextureFilename
		<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;

	m_materials.push_back(material);

	InitMaterialBuffer();

}


void QuadModel::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind our index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
	m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);
	m_dxdevice_context->PSSetShaderResources(0, 1, &m_materials[0].DiffuseTexture.TextureView);
	m_dxdevice_context->PSSetShaderResources(1, 1, &m_materials[0].NormalTexture.TextureView);
	// Make the drawcall
	

	for (auto& material : m_materials)
	{
		UpdateMaterialBuffer(linalg::vec4f(material.AmbientColour, 0), linalg::vec4f(material.DiffuseColour, 0), linalg::vec4f(material.SpecularColour, 20), 0);
	}
	

	m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);

	
}

QuadModel::~QuadModel()
{
	for (auto& material : m_materials)
	{
		SAFE_RELEASE(material.DiffuseTexture.TextureView);
		SAFE_RELEASE(material.NormalTexture.TextureView);

	}
}

void QuadModel::InitMaterialBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC materialBufferDesc = { 0 };
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBuffer);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBufferDesc, nullptr, &m_material_buffer));
}


void QuadModel::UpdateMaterialBuffer(linalg::vec4f ambient, linalg::vec4f diffuse, linalg::vec4f specular, int isSkybox) const
{
	// Map the resource buffer, obtain a pointer and then write our vectors to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* materialBufferData = (MaterialBuffer*)resource.pData;
	materialBufferData->ambientColor = ambient;
	materialBufferData->diffuseColor = diffuse;
	materialBufferData->specularColor = specular;
	materialBufferData->skybox = isSkybox;
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}