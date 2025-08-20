#pragma once

#include "DirectXCapture.h"
#include "GstService.h"

using namespace CaptureStreamingLIb::Gst;
using namespace CaptureStreamingLIb::Capture;

namespace CaptureStreamingLIb
{
	class CaptureStreamingNative
	{
	public:
		CaptureStreamingNative();
		~CaptureStreamingNative();

		HMONITOR GetMonitorByIndex(int monitorIndex);
		void StartCaptureStreaming(int monitorIndex);
		void StartCaptureStreaming(HMONITOR monitorHandle);
		void StartCaptureStreaming(HWND hWnd);
		void StartCaptureEncDecStreaming(HMONITOR monitorHandle);
		void StartCaptureEncDecStreaming(HWND hWnd);
		void StartCaptureUdpStreaming(HMONITOR monitorHandle, std::string& ipAddress, int port);
		void StartCaptureUdpStreaming(HWND hWnd, std::string& ipAddress, int port);
		void StopCaptureStreaming();

	private:
		void InitCapture();
		void InitGst();

		void OnFrameArrivedNV12(
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
			winrt::Windows::Foundation::IInspectable const& args);

		void OnFrameArrivedI420(
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
			winrt::Windows::Foundation::IInspectable const& args);

		void OnFrameArrivedBmpDump(
			winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
			winrt::Windows::Foundation::IInspectable const& args);

	private:
		DirectXCapture* m_pCapture;
		GstService* m_pGstService;
		winrt::Windows::Graphics::SizeInt32 outputFrameSize;
	};
}


