#pragma once

#include "CaptureStreamingDefines.h"

namespace CaptureStreamingLIb
{
	namespace Capture
	{
		class DirectXCapture
		{
		public:
			DirectXCapture(winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device);
			~DirectXCapture();

			void StartCapture(winrt::Windows::Graphics::Capture::GraphicsCaptureItem const& item,
				winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool,
				winrt::Windows::Foundation::IInspectable> const& handler);
			void StopCapture();

		public:
			winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
			winrt::Windows::Graphics::SizeInt32 m_lastSize { 0, 0 };
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived;
			ID3D11ComputeShader* m_computeShader = nullptr;
			CaptureState m_state = CaptureState::Unknown;

		private:
			void StartCaptureInternal(
				winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool,
				winrt::Windows::Foundation::IInspectable> const& handler);
			void OnFrameArrived(
				winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
				winrt::Windows::Foundation::IInspectable const& args);

			void CheckClosed();

		private:
			winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
			winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
			winrt::Windows::System::DispatcherQueue m_dispatcherQueue{ nullptr };
			std::atomic<bool> m_closed = false;
		};
	}
}


