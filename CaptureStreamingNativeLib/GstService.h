#pragma once

#include <future>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "CaptureStreamingDefines.h"

using namespace winrt::Windows::Graphics;

namespace CaptureStreamingLIb
{
	namespace Gst
	{
		class GstService
		{
		public:
			GstService();
			~GstService();

			bool Initialize();
			void StartStream();
			void StartStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize);
			void StartUdpStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize);
			void StartUdpStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize, std::string ipAddress, int port);
			void StopStream();

		public:
			GstBus* m_pBus;
			GstMessage* m_pMsg;
			GstElement* m_pSource;
			GstElement* m_pPipeline;
			winrt::Windows::Graphics::SizeInt32 frameSize = { 0, 0 };
			StreamingState m_state = StreamingState::Unknown;

		private:
		private:
			std::string status;
			std::future<void> asyncTask;
		};
	}
}
