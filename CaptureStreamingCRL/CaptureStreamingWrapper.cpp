#include "pch.h"

#include "CaptureStreamingWrapper.h"
#include <msclr/marshal_cppstd.h>

namespace CaptureStreamingCRL
{
	CaptureStreamingWrapper::CaptureStreamingWrapper()
	{
		m_pCaptureStreaming = new CaptureStreamingNative();
	}

	void CaptureStreamingWrapper::StartCaptureStreaming(int monitorIndex)
	{
		m_pCaptureStreaming->StartCaptureStreaming(monitorIndex);
	}

	void CaptureStreamingWrapper::StartCaptureStreaming(System::IntPtr itemHandle)
	{
		void* pHandle = itemHandle.ToPointer();

		if (IsWindow(static_cast<HWND>(pHandle)))
		{
			HWND hWnd = static_cast<HWND>(pHandle);
			m_pCaptureStreaming->StartCaptureStreaming(hWnd);
		}
		else
		{
			MONITORINFO monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (GetMonitorInfo(static_cast<HMONITOR>(pHandle), &monitorInfo))
			{
				HMONITOR hMonitor = static_cast<HMONITOR>(pHandle);
				m_pCaptureStreaming->StartCaptureStreaming(hMonitor);
			}
			else
			{
				System::Console::WriteLine("Unknown handle type.");
			}
		}
	}
	void CaptureStreamingWrapper::StartCaptureEncDecStreaming(int monitorIndex)
	{
		HMONITOR hMonitor = m_pCaptureStreaming->GetMonitorByIndex(monitorIndex);
		m_pCaptureStreaming->StartCaptureEncDecStreaming(hMonitor);
	}

	void CaptureStreamingWrapper::StartCaptureEncDecStreaming(System::IntPtr itemHandle)
	{
		void* pHandle = itemHandle.ToPointer();

		if (IsWindow(static_cast<HWND>(pHandle)))
		{
			HWND hWnd = static_cast<HWND>(pHandle);
			m_pCaptureStreaming->StartCaptureEncDecStreaming(hWnd);
		}
		else
		{
			MONITORINFO monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (GetMonitorInfo(static_cast<HMONITOR>(pHandle), &monitorInfo))
			{
				HMONITOR hMonitor = static_cast<HMONITOR>(pHandle);
				m_pCaptureStreaming->StartCaptureEncDecStreaming(hMonitor);
			}
			else
			{
				System::Console::WriteLine("Unknown handle type.");
			}
		}
	}

	void CaptureStreamingWrapper::StartCaptureUdpStreaming(int monitorIndex, System::String^ ipAddress, int port)
	{
		HMONITOR hMonitor = m_pCaptureStreaming->GetMonitorByIndex(monitorIndex);

		msclr::interop::marshal_context context;
		std::string nativeIpAddress = context.marshal_as<std::string>(ipAddress);

		m_pCaptureStreaming->StartCaptureUdpStreaming(hMonitor, nativeIpAddress, port);
	}

	void CaptureStreamingWrapper::StartCaptureUdpStreaming(System::IntPtr itemHandle, System::String^ ipAddress, int port)
	{
		void* pHandle = itemHandle.ToPointer();

		msclr::interop::marshal_context context;
		std::string nativeIpAddress = context.marshal_as<std::string>(ipAddress);

		if (IsWindow(static_cast<HWND>(pHandle)))
		{
			HWND hWnd = static_cast<HWND>(pHandle);
			m_pCaptureStreaming->StartCaptureUdpStreaming(hWnd, nativeIpAddress, port);
		}
		else
		{
			MONITORINFO monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (GetMonitorInfo(static_cast<HMONITOR>(pHandle), &monitorInfo))
			{
				HMONITOR hMonitor = static_cast<HMONITOR>(pHandle);
				m_pCaptureStreaming->StartCaptureUdpStreaming(hMonitor, nativeIpAddress, port);
			}
			else
			{
				System::Console::WriteLine("Unknown handle type.");
			}
		}
	}

	void CaptureStreamingWrapper::StopCaptureStreaming()
	{
		m_pCaptureStreaming->StopCaptureStreaming();
	}

	CaptureStreamingWrapper::~CaptureStreamingWrapper()
	{
		delete m_pCaptureStreaming;
	}
}


