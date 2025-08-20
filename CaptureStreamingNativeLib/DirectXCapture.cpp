#include "pch.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>

#include <windows.system.h>
#include <dispatcherqueue.h>

#include "DirectXCapture.h"

namespace CaptureStreamingLIb
{
	namespace Capture
	{
		DirectXCapture::DirectXCapture(winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device)
		{
			m_state = CaptureState::Initializing;
			m_device = device;
			auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);

			if (d3dDevice == nullptr)
			{
				OutputDebugString(L"DirectXCapture() - GetDXGIInterfaceFromObject<ID3D11Device>(m_device) fail!\n");
				return;
			}

			
#ifdef USE_NV12_COMPUTE_SHADER
			std::wstring filePath = L"ComputeShaderNV12.cso";
#else
			std::wstring filePath = L"ComputeShaderI420.cso";
#endif

			std::ifstream file(filePath, std::ios::binary);
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open hlsl cso file.");
			}

			std::vector<char> sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();

			HRESULT hr = d3dDevice->CreateComputeShader(sourceCode.data(), sourceCode.size(), nullptr, &m_computeShader);

			if (FAILED(hr))
			{
				std::cerr << "CreateComputeShader failed! HRESULT: " << std::hex << hr << std::dec << std::endl;
			}
			else
			{
				std::cout << "CreateComputeShader successfully!" << std::endl;
			}

			//winrt::Windows::System::DispatcherQueueController controller{ nullptr };
			//DispatcherQueueOptions options{
			//	sizeof(DispatcherQueueOptions),
			//	DQTYPE_THREAD_CURRENT,
			//	DQTAT_COM_NONE
			//};
			//CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(controller)));
			//m_dispatcherQueue = controller.DispatcherQueue();

			m_state = CaptureState::Ready;
		}

		// Start sending capture frames
		void DirectXCapture::StartCapture(winrt::Windows::Graphics::Capture::GraphicsCaptureItem const& item,
			winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool,
			winrt::Windows::Foundation::IInspectable> const& handler)
		{
			if (m_state == CaptureState::Ready)
			{
				m_item = item;
				m_lastSize = m_item.Size();

				m_item = item;

				// Create framepool, define pixel format (DXGI_FORMAT_B8G8R8A8_UNORM), and frame size. 
				m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
					m_device,
					winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
					2,
					m_lastSize);
				m_session = m_framePool.CreateCaptureSession(m_item);
				//m_session.IsBorderRequired(false);
				m_frameArrived = m_framePool.FrameArrived(winrt::auto_revoke, handler);

				m_session.StartCapture();

				m_state = CaptureState::Capturing;

				//m_dispatcherQueue.TryEnqueue([this, handler]()
				//	{
				//		StartCaptureInternal(handler);
				//	}
				//);
			}
		}

		void DirectXCapture::StartCaptureInternal(
			winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool,
			winrt::Windows::Foundation::IInspectable> const& handler)
		{
			m_lastSize = m_item.Size();

			// Create framepool, define pixel format (DXGI_FORMAT_B8G8R8A8_UNORM), and frame size. 
			m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
				m_device,
				winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
				2,
				m_lastSize);
			m_session = m_framePool.CreateCaptureSession(m_item);
			//m_session.IsBorderRequired(false);
			m_frameArrived = m_framePool.FrameArrived(winrt::auto_revoke, handler);

			m_session.StartCapture();

			m_state = CaptureState::Capturing;
		}


		void DirectXCapture::StopCapture()
		{
			m_frameArrived.revoke();

			if (m_framePool != nullptr)
			{
				m_framePool.Close();
			}

			if (m_session != nullptr)
			{
				m_session.Close();
			}

			m_framePool = nullptr;
			m_session = nullptr;
			m_item = nullptr;

			m_state = CaptureState::Ready;
		}

		void DirectXCapture::CheckClosed()
		{
			if (m_closed.load() == true)
			{
				throw winrt::hresult_error(RO_E_CLOSED);
			}
		}

		void DirectXCapture::OnFrameArrived(
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
			winrt::Windows::Foundation::IInspectable const& args)
		{
			auto newSize = false;
			auto frame = sender.TryGetNextFrame();
			auto frameContentSize = frame.ContentSize();
		}

		DirectXCapture::~DirectXCapture()
		{
			StopCapture();
		}
	}
}