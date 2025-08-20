#include "pch.h"
#include "framework.h"

#include <chrono>
#include <string>
#include <fstream>

#include "CaptureStreamingNative.h"

using Microsoft::WRL::ComPtr;

namespace CaptureStreamingLIb
{
	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(dwData);
		monitors->push_back(hMonitor);
		return TRUE;
	}

	HMONITOR CaptureStreamingNative::GetMonitorByIndex(int monitorIndex)
	{
		int currentMonitor = 0;
		std::vector<HMONITOR> monitors;
		EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors));

		return monitorIndex >= 0 ? monitors[monitorIndex] : nullptr;
	}

	CaptureStreamingNative::CaptureStreamingNative()
	{
		InitCapture();
		InitGst();
		outputFrameSize = { 1920, 1080 };
	}

	void CaptureStreamingNative::InitCapture()
	{
		ComPtr<ID3D11Device> d3dDevice = CreateD3DDevice();
		auto device = GetWinRTDirect3DDeviceFromD3D11Device(d3dDevice.Get());
		m_pCapture = new DirectXCapture(device);
	}

	void CaptureStreamingNative::InitGst()
	{
		m_pGstService = new GstService();
		m_pGstService->Initialize();
	}

	void CaptureStreamingNative::StartCaptureStreaming(int monitorIndex)
	{
		HMONITOR monitor = GetMonitorByIndex(monitorIndex);
		auto item = CreateCaptureItemForMonitor(monitor);

		m_pGstService->StartStream(outputFrameSize);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureStreaming(HMONITOR hMonitor)
	{
		auto item = CreateCaptureItemForMonitor(hMonitor);

		m_pGstService->StartStream(outputFrameSize);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureStreaming(HWND hWnd)
	{
		auto item = CreateCaptureItemForWindow(hWnd);

		m_pGstService->StartStream(outputFrameSize);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureEncDecStreaming(HMONITOR hMonitor)
	{
		auto item = CreateCaptureItemForMonitor(hMonitor);

		m_pGstService->StartUdpStream(outputFrameSize);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureEncDecStreaming(HWND hWnd)
	{
		auto item = CreateCaptureItemForWindow(hWnd);

		m_pGstService->StartUdpStream(outputFrameSize);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureUdpStreaming(HMONITOR hMonitor, std::string& ipAddress, int port)
	{
		auto item = CreateCaptureItemForMonitor(hMonitor);

		m_pGstService->StartUdpStream(outputFrameSize, ipAddress, port);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StartCaptureUdpStreaming(HWND hWnd, std::string& ipAddress, int port)
	{
		auto item = CreateCaptureItemForWindow(hWnd);

		m_pGstService->StartUdpStream(outputFrameSize, ipAddress, port);
#ifdef USE_NV12_COMPUTE_SHADER
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedNV12 });
#else
		m_pCapture->StartCapture(item, { this, &CaptureStreamingNative::OnFrameArrivedI420 });
#endif
	}

	void CaptureStreamingNative::StopCaptureStreaming()
	{
		m_pCapture->StopCapture();
		m_pGstService->StopStream();
	}

	struct Constants {
		uint32_t FrameSize[2];
		uint32_t OutputSize[2];
	};

	void SaveBitmap(const std::wstring& filePath, uint8_t* data, UINT width, UINT height, UINT pitch)
	{
		BITMAPFILEHEADER fileHeader = {};
		BITMAPINFOHEADER infoHeader = {};

		// BMP 헤더 설정
		fileHeader.bfType = 0x4D42; // BM
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfSize = fileHeader.bfOffBits + height * pitch;

		infoHeader.biSize = sizeof(BITMAPINFOHEADER);
		infoHeader.biWidth = width;
		infoHeader.biHeight = -static_cast<int>(height); // 상하 반전
		infoHeader.biPlanes = 1;
		infoHeader.biBitCount = 32; // 32비트 픽셀
		infoHeader.biCompression = BI_RGB;
		infoHeader.biSizeImage = height * pitch;

		std::ofstream file(filePath, std::ios::binary);
		if (!file)
		{
			OutputDebugString(L"Failed to open file for bitmap dump.\n");
			return;
		}

		file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
		file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

		for (UINT row = 0; row < height; ++row)
		{
			file.write(reinterpret_cast<const char*>(data + row * pitch), width * 4);
		}

		file.close();
		OutputDebugString(L"Bitmap dump saved.\n");
	}

	void CaptureStreamingNative::OnFrameArrivedNV12(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args)
	{
		auto frame = sender.TryGetNextFrame();
		if (!frame) {
			OutputDebugString(L"TryGetNextFrame failed.\n");
			return;
		}

		auto frameContentSize = frame.ContentSize();

		if (frameContentSize.Width != m_pCapture->m_lastSize.Width ||
			frameContentSize.Height != m_pCapture->m_lastSize.Height)
		{
			m_pCapture->m_lastSize = frameContentSize;

			m_pCapture->m_framePool.Recreate(
				m_pCapture->m_device,
				winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
				2,
				m_pCapture->m_lastSize);

			return;
		}

		auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
		if (!frameSurface) {
			OutputDebugString(L"Failed to get frame surface!\n");
			return;
		}

		D3D11_TEXTURE2D_DESC fdesc = {};
		frameSurface->GetDesc(&fdesc);

		ComPtr<ID3D11DeviceContext> d3dContext;
		auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_pCapture->m_device);
		d3dDevice->GetImmediateContext(d3dContext.GetAddressOf());

		// Y 평면 텍스처
		D3D11_TEXTURE2D_DESC descY = {};
		descY.Width = static_cast<uint32_t>(outputFrameSize.Width);
		descY.Height = static_cast<uint32_t>(outputFrameSize.Height);
		descY.MipLevels = 1;
		descY.ArraySize = 1;
		descY.Format = DXGI_FORMAT_R8_UINT; // Y 평면 포맷
		descY.SampleDesc.Count = 1;
		descY.Usage = D3D11_USAGE_DEFAULT;
		descY.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		ComPtr<ID3D11Texture2D> textureY;
		d3dDevice->CreateTexture2D(&descY, nullptr, &textureY);

		// UV 평면 텍스처
		D3D11_TEXTURE2D_DESC descUV = descY;
		descUV.Width = static_cast<uint32_t>(outputFrameSize.Width / 2);
		descUV.Height = static_cast<uint32_t>(outputFrameSize.Height / 2);
		descUV.Format = DXGI_FORMAT_R8G8_UINT;

		ComPtr<ID3D11Texture2D> textureUV;
		d3dDevice->CreateTexture2D(&descUV, nullptr, &textureUV);

		// 상수 버퍼 생성
		// 프레임 크기 데이터 설정
		Constants constants = {
			{static_cast<uint32_t>(frameContentSize.Width), static_cast<uint32_t>(frameContentSize.Height)},
			{ outputFrameSize.Width, outputFrameSize.Height}
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 동적 업데이트 가능
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA cbInitData = { &constants };

		ComPtr<ID3D11Buffer> constantBuffer;
		d3dDevice->CreateBuffer(&bufferDesc, &cbInitData, &constantBuffer);

		// SamplerState 설정
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ComPtr<ID3D11SamplerState> samplerState;
		d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);

		// SRV 생성
		ComPtr<ID3D11ShaderResourceView> inputSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		d3dDevice->CreateShaderResourceView(frameSurface.get(), &srvDesc, &inputSRV);

		// Y 평면 UAV
		ComPtr<ID3D11UnorderedAccessView> uavY;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDescY = {};
		uavDescY.Format = DXGI_FORMAT_R8_UINT;
		uavDescY.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		d3dDevice->CreateUnorderedAccessView(textureY.Get(), &uavDescY, &uavY);

		// UV 평면 UAV
		uavDescY.Format = DXGI_FORMAT_R8G8_UINT;;
		ComPtr<ID3D11UnorderedAccessView> uavUV;
		d3dDevice->CreateUnorderedAccessView(textureUV.Get(), &uavDescY, &uavUV);

		ID3D11UnorderedAccessView* uavs[2] = { uavY.Get(), uavUV.Get() };

		d3dContext->CSSetShader(m_pCapture->m_computeShader, nullptr, 0);
		d3dContext->CSSetShaderResources(0, 1, inputSRV.GetAddressOf());
		d3dContext->CSSetSamplers(1, 1, samplerState.GetAddressOf()); // SamplerState
		d3dContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

		d3dContext->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		d3dContext->Dispatch((outputFrameSize.Width + 15) / 16, (outputFrameSize.Height + 15) / 16, 1);

		// 결과 읽기
		D3D11_TEXTURE2D_DESC readbackDescY = descY;
		readbackDescY.Usage = D3D11_USAGE_STAGING;
		readbackDescY.BindFlags = 0;
		readbackDescY.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ComPtr<ID3D11Texture2D> readbackTextureY;
		d3dDevice->CreateTexture2D(&readbackDescY, nullptr, &readbackTextureY);
		d3dContext->CopyResource(readbackTextureY.Get(), textureY.Get());

		// UV 평면 읽기 설정
		D3D11_TEXTURE2D_DESC readbackDescUV = descUV; // UV 평면의 크기와 포맷
		readbackDescUV.Usage = D3D11_USAGE_STAGING;
		readbackDescUV.BindFlags = 0;
		readbackDescUV.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ComPtr<ID3D11Texture2D> readbackTextureUV;
		d3dDevice->CreateTexture2D(&readbackDescUV, nullptr, &readbackTextureUV);
		d3dContext->CopyResource(readbackTextureUV.Get(), textureUV.Get());

		// 결과 매핑 및 확인
		D3D11_MAPPED_SUBRESOURCE mappedY;
		d3dContext->Map(readbackTextureY.Get(), 0, D3D11_MAP_READ, 0, &mappedY);
		uint8_t* outputY = static_cast<uint8_t*>(mappedY.pData);

		D3D11_MAPPED_SUBRESOURCE mappedUV;
		d3dContext->Map(readbackTextureUV.Get(), 0, D3D11_MAP_READ, 0, &mappedUV);
		uint8_t* outputUV = static_cast<uint8_t*>(mappedUV.pData);

		size_t yPlaneSize = outputFrameSize.Width * outputFrameSize.Height;
		size_t uvPlaneSize = (outputFrameSize.Width / 2) * (outputFrameSize.Height / 2) * 2;

		GstBuffer* inGstBuffer = gst_buffer_new_allocate(nullptr, yPlaneSize + uvPlaneSize, nullptr);
		GstMapInfo mapInfo;

		if (gst_buffer_map(inGstBuffer, &mapInfo, GST_MAP_WRITE))
		{
			uint8_t* dst = mapInfo.data;

			// Y 평면 복사
			for (UINT row = 0; row < descY.Height; ++row) {
				memcpy(dst + row * descY.Width, outputY + row * mappedY.RowPitch, descY.Width);
			}

			// UV 평면 복사
			for (UINT row = 0; row < descUV.Height; ++row) {
				memcpy(dst + yPlaneSize + row * descUV.Width * 2,
					outputUV + row * mappedUV.RowPitch, descUV.Width * 2);
			}

			gst_buffer_unmap(inGstBuffer, &mapInfo);
		}

		d3dContext->Unmap(readbackTextureY.Get(), 0);
		d3dContext->Unmap(readbackTextureUV.Get(), 0);

		if (inGstBuffer)
		{
			// 버퍼 푸시
			GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(m_pGstService->m_pSource), inGstBuffer);
		}

		ID3D11ShaderResourceView* nullSRV = nullptr;
		ID3D11UnorderedAccessView* nullUAV = nullptr;
		ID3D11SamplerState* nullSampler = nullptr;
		d3dContext->CSSetShaderResources(0, 1, &nullSRV);
		d3dContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		d3dContext->CSSetSamplers(1, 1, &nullSampler);

		d3dContext->CSSetShader(nullptr, nullptr, 0);
	}

	void CaptureStreamingNative::OnFrameArrivedI420(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args)
	{
		auto frame = sender.TryGetNextFrame();
		if (!frame) {
			OutputDebugString(L"TryGetNextFrame failed.\n");
			return;
		}

		auto frameContentSize = frame.ContentSize();

		if (frameContentSize.Width != m_pCapture->m_lastSize.Width ||
			frameContentSize.Height != m_pCapture->m_lastSize.Height)
		{
			m_pCapture->m_lastSize = frameContentSize;

			m_pCapture->m_framePool.Recreate(
				m_pCapture->m_device,
				winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
				2,
				m_pCapture->m_lastSize);

			return;
		}

		auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
		if (!frameSurface) {
			OutputDebugString(L"Failed to get frame surface!\n");
			return;
		}

		D3D11_TEXTURE2D_DESC fdesc = {};
		frameSurface->GetDesc(&fdesc);

		ComPtr<ID3D11DeviceContext> d3dContext;
		auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_pCapture->m_device);
		d3dDevice->GetImmediateContext(d3dContext.GetAddressOf());

		// Y 평면 텍스처
		D3D11_TEXTURE2D_DESC descY = {};
		descY.Width = static_cast<uint32_t>(outputFrameSize.Width);
		descY.Height = static_cast<uint32_t>(outputFrameSize.Height);
		descY.MipLevels = 1;
		descY.ArraySize = 1;
		descY.Format = DXGI_FORMAT_R8_UINT; // Y 평면 포맷
		descY.SampleDesc.Count = 1;
		descY.Usage = D3D11_USAGE_DEFAULT;
		descY.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		ComPtr<ID3D11Texture2D> textureY;
		d3dDevice->CreateTexture2D(&descY, nullptr, &textureY);

		// U, V 평면 디스크립션
		D3D11_TEXTURE2D_DESC descUV = descY;
		descUV.Width = static_cast<uint32_t>(outputFrameSize.Width / 2);
		descUV.Height = static_cast<uint32_t>(outputFrameSize.Height / 2);

		// U 평면 텍스처
		ComPtr<ID3D11Texture2D> textureU;
		d3dDevice->CreateTexture2D(&descUV, nullptr, &textureU);

		// V 평면 텍스쳐
		ComPtr<ID3D11Texture2D> textureV;
		d3dDevice->CreateTexture2D(&descUV, nullptr, &textureV);

		// 상수 버퍼 생성
		// 프레임 크기 데이터 설정
		Constants constants = {
			{static_cast<uint32_t>(frameContentSize.Width), static_cast<uint32_t>(frameContentSize.Height)},
			{ outputFrameSize.Width, outputFrameSize.Height} 
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 동적 업데이트 가능
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA cbInitData = { &constants };

		ComPtr<ID3D11Buffer> constantBuffer;
		d3dDevice->CreateBuffer(&bufferDesc, &cbInitData, &constantBuffer);

		// SamplerState 설정
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ComPtr<ID3D11SamplerState> samplerState;
		d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);

		// SRV 생성
		ComPtr<ID3D11ShaderResourceView> inputSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		d3dDevice->CreateShaderResourceView(frameSurface.get(), &srvDesc, &inputSRV);

		// Y, U, V 평면 UAV
		ComPtr<ID3D11UnorderedAccessView> uavY, uavU, uavV;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R8_UINT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		d3dDevice->CreateUnorderedAccessView(textureY.Get(), &uavDesc, &uavY);
		d3dDevice->CreateUnorderedAccessView(textureU.Get(), &uavDesc, &uavU);
		d3dDevice->CreateUnorderedAccessView(textureV.Get(), &uavDesc, &uavV);
		ID3D11UnorderedAccessView* uavs[3] = { uavY.Get(), uavU.Get(), uavV.Get() };

		d3dContext->CSSetShader(m_pCapture->m_computeShader, nullptr, 0);
		d3dContext->CSSetShaderResources(0, 1, inputSRV.GetAddressOf());
		d3dContext->CSSetSamplers(1, 1, samplerState.GetAddressOf()); // SamplerState
		d3dContext->CSSetUnorderedAccessViews(0, 3, uavs, nullptr);

		d3dContext->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		d3dContext->Dispatch((outputFrameSize.Width + 15) / 16, (outputFrameSize.Height + 15) / 16, 1);
		
		// 결과 읽기
		D3D11_TEXTURE2D_DESC readbackDesc = descY;
		readbackDesc.Usage = D3D11_USAGE_STAGING;
		readbackDesc.BindFlags = 0;
		readbackDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ComPtr<ID3D11Texture2D> readbackTextureY, readbackTextureU, readbackTextureV;;
		d3dDevice->CreateTexture2D(&readbackDesc, nullptr, &readbackTextureY);
		d3dContext->CopyResource(readbackTextureY.Get(), textureY.Get());

		// UV 평면 읽기 설정
		readbackDesc.Width /= 2;
		readbackDesc.Height /= 2;
		d3dDevice->CreateTexture2D(&readbackDesc, nullptr, &readbackTextureU);
		d3dDevice->CreateTexture2D(&readbackDesc, nullptr, &readbackTextureV);

		d3dContext->CopyResource(readbackTextureU.Get(), textureU.Get());
		d3dContext->CopyResource(readbackTextureV.Get(), textureV.Get());

		// 결과 매핑 및 확인
		D3D11_MAPPED_SUBRESOURCE mappedY, mappedU, mappedV;
		d3dContext->Map(readbackTextureY.Get(), 0, D3D11_MAP_READ, 0, &mappedY);
		d3dContext->Map(readbackTextureU.Get(), 0, D3D11_MAP_READ, 0, &mappedU);
		d3dContext->Map(readbackTextureV.Get(), 0, D3D11_MAP_READ, 0, &mappedV);

		uint8_t* outputY = static_cast<uint8_t*>(mappedY.pData);
		uint8_t* outputU = static_cast<uint8_t*>(mappedU.pData);
		uint8_t* outputV = static_cast<uint8_t*>(mappedV.pData);

		size_t yPlaneSize = outputFrameSize.Width * outputFrameSize.Height;
		size_t uvPlaneSize = (outputFrameSize.Width / 2) * (outputFrameSize.Height / 2);

		GstBuffer* inGstBuffer = gst_buffer_new_allocate(nullptr, yPlaneSize + (uvPlaneSize * 2), nullptr);
		GstMapInfo mapInfo;

		if (gst_buffer_map(inGstBuffer, &mapInfo, GST_MAP_WRITE))
		{
			uint8_t* dst = mapInfo.data;

			for (UINT row = 0; row < descY.Height; ++row) {
				memcpy(dst + row * descY.Width, outputY + row * mappedY.RowPitch, descY.Width);
			}

			for (UINT row = 0; row < descUV.Height; ++row) {
				memcpy(dst + yPlaneSize + row * descUV.Width,
					outputU + row * mappedU.RowPitch, descUV.Width);
			}

			for (UINT row = 0; row < descUV.Height; ++row) {
				memcpy(dst + yPlaneSize + uvPlaneSize + row * descUV.Width,
					outputV + row * mappedV.RowPitch, descUV.Width);
			}

			gst_buffer_unmap(inGstBuffer, &mapInfo);
		}

		d3dContext->Unmap(readbackTextureY.Get(), 0);
		d3dContext->Unmap(readbackTextureU.Get(), 0);
		d3dContext->Unmap(readbackTextureV.Get(), 0);

		if (inGstBuffer != nullptr && m_pGstService->m_state == StreamingState::Streaming)
		{
			// 버퍼 푸시
			GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(m_pGstService->m_pSource), inGstBuffer);
		}


		// ++++++ 이미지 덤프 ++++++++++

		//D3D11_TEXTURE2D_DESC stagingDesc = {};
		//frameSurface->GetDesc(&stagingDesc);
		//stagingDesc.Usage = D3D11_USAGE_STAGING;
		//stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		//stagingDesc.BindFlags = 0;
		//stagingDesc.MiscFlags = 0;

		//ComPtr<ID3D11Texture2D> stagingTexture;
		//HRESULT hr = d3dDevice->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
		//if (FAILED(hr)) {
		//	OutputDebugString(L"Failed to create staging texture.\n");
		//	return;
		//}

		//// 텍스처 복사
		//d3dContext->CopyResource(stagingTexture.Get(), frameSurface.get());

		//// 매핑 및 데이터 읽기
		//D3D11_MAPPED_SUBRESOURCE mappedResource;
		//hr = d3dContext->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		//if (FAILED(hr)) {
		//	OutputDebugString(L"Failed to map staging texture.\n");
		//	return;
		//}

		//// Bitmap 저장
		//SaveBitmap(L"frame_dump.bmp",
		//	static_cast<uint8_t*>(mappedResource.pData),
		//	frameContentSize.Width,
		//	frameContentSize.Height,
		//	mappedResource.RowPitch);

		//d3dContext->Unmap(stagingTexture.Get(), 0);

		// ------- 이미지 덤프 --------

		ID3D11ShaderResourceView* nullSRV = nullptr;
		ID3D11UnorderedAccessView* nullUAV = nullptr;
		ID3D11SamplerState* nullSampler = nullptr;
		d3dContext->CSSetShaderResources(0, 1, &nullSRV);
		d3dContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		d3dContext->CSSetSamplers(1, 1, &nullSampler);

		d3dContext->CSSetShader(nullptr, nullptr, 0);
	}

	std::chrono::steady_clock::time_point lastSaveTime;

	void CaptureStreamingNative::OnFrameArrivedBmpDump(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args)
	{
		lastSaveTime = std::chrono::steady_clock::now();

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSaveTime);

		lastSaveTime = now;

		auto frame = sender.TryGetNextFrame();
		if (!frame) {
			OutputDebugString(L"TryGetNextFrame failed.\n");
			return;
		}

		auto frameContentSize = frame.ContentSize();
		auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
		if (!frameSurface) {
			OutputDebugString(L"Failed to get frame surface!\n");
			return;
		}

		D3D11_TEXTURE2D_DESC desc;
		frameSurface->GetDesc(&desc);
		OutputDebugString((L"Frame Surface Format: " + std::to_wstring(desc.Format) + L"\n").c_str());

		auto test = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		ComPtr<ID3D11DeviceContext> d3dContext;
		auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_pCapture->m_device);
		d3dDevice->GetImmediateContext(d3dContext.GetAddressOf());

		// 상수 버퍼 생성
		// 프레임 크기 데이터 설정
		Constants constants = { {static_cast<uint32_t>(frameContentSize.Width), static_cast<uint32_t>(frameContentSize.Height)} };
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 동적 업데이트 가능
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA cbInitData = { &constants };

		ComPtr<ID3D11Buffer> constantBuffer;
		d3dDevice->CreateBuffer(&bufferDesc, &cbInitData, &constantBuffer);

		// Dump 평면 텍스처
		D3D11_TEXTURE2D_DESC descDump = {};
		descDump.Width = static_cast<uint32_t>(frameContentSize.Width);
		descDump.Height = static_cast<uint32_t>(frameContentSize.Height);
		descDump.MipLevels = 1;
		descDump.ArraySize = 1;
		descDump.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // Y 평면 포맷
		descDump.SampleDesc.Count = 1;
		descDump.Usage = D3D11_USAGE_DEFAULT;
		descDump.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		ComPtr<ID3D11Texture2D> textureDump;
		d3dDevice->CreateTexture2D(&descDump, nullptr, &textureDump);

		// SRV 생성
		ComPtr<ID3D11ShaderResourceView> inputSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		HRESULT hrSrv = d3dDevice->CreateShaderResourceView(frameSurface.get(), &srvDesc, &inputSRV);

		// Dump 평면 UAV
		ComPtr<ID3D11UnorderedAccessView> uavDump;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		HRESULT hrUav = d3dDevice->CreateUnorderedAccessView(textureDump.Get(), &uavDesc, &uavDump);

		d3dContext->CSSetShader(m_pCapture->m_computeShader, nullptr, 0);
		d3dContext->CSSetShaderResources(0, 1, inputSRV.GetAddressOf());

		ID3D11UnorderedAccessView* uavs[1] = { uavDump.Get() };
		d3dContext->CSSetUnorderedAccessViews(0, 1, uavDump.GetAddressOf(), nullptr);

		d3dContext->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		if (inputSRV == nullptr) {
			OutputDebugString(L"Shader Resource View not bound!\n");
		}
		if (uavDump == nullptr) {
			OutputDebugString(L"Unordered Access View not bound!\n");
		}
		if (constantBuffer == nullptr) {
			OutputDebugString(L"constantBuffer not bound!\n");
		}

		d3dContext->Dispatch((frameContentSize.Width + 15) / 16, (frameContentSize.Height + 15) / 16, 1);
		d3dContext->Flush();

		D3D11_TEXTURE2D_DESC stagingDesc = {};
		frameSurface->GetDesc(&stagingDesc);
		stagingDesc.Width = static_cast<uint32_t>(frameContentSize.Width);
		stagingDesc.Height = static_cast<uint32_t>(frameContentSize.Height);
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.BindFlags = 0;
		stagingDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> stagingTexture;
		HRESULT hr = d3dDevice->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
		if (FAILED(hr)) {
			OutputDebugString(L"Failed to create staging texture.\n");
			return;
		}

		// 텍스처 복사
		d3dContext->CopyResource(stagingTexture.Get(), textureDump.Get());

		// 매핑 및 데이터 읽기
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = d3dContext->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		if (FAILED(hr)) {
			OutputDebugString(L"Failed to map staging texture.\n");
			return;
		}

		// Bitmap 저장
		SaveBitmap(L"frame_dump.bmp",
			static_cast<uint8_t*>(mappedResource.pData),
			frameContentSize.Width,
			frameContentSize.Height,
			mappedResource.RowPitch);

		d3dContext->Unmap(stagingTexture.Get(), 0);
	}

	CaptureStreamingNative::~CaptureStreamingNative()
	{
		if (m_pCapture != nullptr)
		{
			delete m_pCapture;
		}
		m_pCapture = nullptr;

		if (m_pGstService != nullptr)
		{
			delete m_pGstService;
		}
		m_pGstService = nullptr;
	}
}

