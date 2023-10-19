#pragma once

namespace DirectXX
{

	class DeviceResources;

	class View
	{
	public:
		struct ConstantBuffer
		{
			DirectX::XMMATRIX viewMatrix;
			DirectX::XMMATRIX projectionMatrix;
		};
		static_assert((sizeof(ConstantBuffer) % 16) == 0, "Constant buffer must always be 16-byte aligned");

		struct WorldConstantBuffer
		{
			DirectX::XMMATRIX worldMatrix;
		};
		static_assert((sizeof(ConstantBuffer) % 16) == 0, "Constant buffer must always be 16-byte aligned");

		View(DeviceResources* const deviceResources);
		~View();

		void							Initialise();
		void							Refresh();
		void							Shutdown();

		void							SetViewMatrix(const DirectX::XMFLOAT4X4& viewMatrix)
		{
			m_viewMatrix = viewMatrix;
		}

	private:
		DeviceResources* m_deviceResources;
		ID3D11Buffer* m_constantBuffer;

		DirectX::XMFLOAT4X4				m_worldMatrix;
		DirectX::XMFLOAT4X4				m_viewMatrix;
		DirectX::XMFLOAT4X4				m_projectionMatrix;
	};

} // namespace DX