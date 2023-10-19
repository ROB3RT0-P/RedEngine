#include "red_engine.h"
#include "device_resources.h"
#include "view.h"

using namespace DirectX;

namespace DirectX
{

	View::View(DeviceResources* deviceResources) :
		m_deviceResources(deviceResources),
		m_constantBuffer(nullptr),
		m_worldMatrix{},
		m_viewMatrix{},
		m_projectionMatrix{}
	{
	}

	View::~View()
	{
	}

	void View::Initialise()
	{
		ASSERT(m_deviceResources != nullptr, "Device resources doesn't exist.\n");
		ID3D11DeviceContext* const deviceContext = m_deviceResources->GetD3DDeviceContext();

		ID3D11Device* device = nullptr;
		deviceContext->GetDevice(&device);
		ASSERT(device != nullptr, "Unable to get D3D device.\n");

		CD3D11_BUFFER_DESC bufferDesc(sizeof(ConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
		ASSERT(m_constantBuffer != nullptr, "Unable to create constant buffer.\n");

		// Initialize the world matrix
		XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());

		// Initialize the view matrix



		// Initialize the projection matrix
		auto size = m_deviceResources->GetOutputSize();
		XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, float(size.right) / float(size.bottom), 0.01f, 100.0f);
		XMStoreFloat4x4(&m_projectionMatrix, projection);




	}

	void View::Refresh()
	{
		ASSERT(m_deviceResources != nullptr, "Device resources doesn't exist.\n");
		ID3D11DeviceContext* const deviceContext = m_deviceResources->GetD3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = S_OK;

		// Set the per-frame constants
		ConstantBuffer sceneParameters = {};

		// Shaders compiled with default row-major matrices
		sceneParameters.viewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix));
		sceneParameters.projectionMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix));

		ASSERT(m_constantBuffer != nullptr, "Constant buffer doesn't exist. Has View::Initialise() been called?\n");

		hr = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		ASSERT_HANDLE(hr);
		memcpy(mapped.pData, &sceneParameters, sizeof(ConstantBuffer));
		deviceContext->Unmap(m_constantBuffer, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	}

	void View::Shutdown()
	{
		m_constantBuffer->Release();
	}

} // namespace DX