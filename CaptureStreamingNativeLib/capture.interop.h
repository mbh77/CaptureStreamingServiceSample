#pragma once

#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.capture.h>

inline winrt::Windows::Graphics::Capture::GraphicsCaptureItem
CreateCaptureItemForMonitor(HMONITOR monitor)
{
    // GraphicsCaptureItem Activation Factory 가져오기
    auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
    auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();

    // GraphicsCaptureItem 생성
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };
    HRESULT hr = interop_factory->CreateForMonitor(
        monitor,
        winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(winrt::put_abi(item))
    );

    // HRESULT 체크 및 반환
    winrt::check_hresult(hr);
    return item;
}

inline winrt::Windows::Graphics::Capture::GraphicsCaptureItem
CreateCaptureItemForWindow(HWND hwnd)
{
    // GraphicsCaptureItem Activation Factory 가져오기
    auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
    auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();

    // GraphicsCaptureItem 생성
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };
    HRESULT hr = interop_factory->CreateForWindow(
        hwnd,
        winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(winrt::put_abi(item))
    );

    // HRESULT 체크 및 반환
    winrt::check_hresult(hr);
    return item;
}