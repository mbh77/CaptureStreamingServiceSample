#pragma once

#include "../CaptureStreamingNativeLib/CaptureStreamingNative.h"

using namespace System;

using namespace CaptureStreamingLIb;
using namespace CaptureStreamingLIb::Gst;
using namespace CaptureStreamingLIb::Capture;

namespace CaptureStreamingCRL {
	public ref class CaptureStreamingWrapper : IDisposable
	{
	private:
		CaptureStreamingNative* m_pCaptureStreaming;
	public:
		CaptureStreamingWrapper();
		~CaptureStreamingWrapper();

		void StartCaptureStreaming(int monitorIndex);
		void StartCaptureStreaming(System::IntPtr itemHandle);
		void StartCaptureEncDecStreaming(int monitorIndex);
		void StartCaptureEncDecStreaming(System::IntPtr itemHandle);
		void StartCaptureUdpStreaming(int monitorIndex, System::String^ ipAddress, int port);
		void StartCaptureUdpStreaming(System::IntPtr itemHandle, System::String^ ipAddress, int port);
		void StopCaptureStreaming();
	};
}
