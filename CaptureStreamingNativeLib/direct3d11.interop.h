#pragma once
#include <windows.graphics.directx.direct3d11.interop.h> 
#include <winrt/windows.graphics.directx.direct3d11.h>
#include <wrl/client.h>

inline Microsoft::WRL::ComPtr<IInspectable> GetInspectableDirect3DDeviceFromD3D11Device(ID3D11Device* d3d11Device) {
    Microsoft::WRL::ComPtr<IInspectable> inspectableDevice;

    if (!d3d11Device) {
        return nullptr; // d3d11Device�� nullptr�̸� �۾� ����
    }

    // IDXGIDevice�� ��ȯ
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    HRESULT hr = d3d11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (FAILED(hr)) {
        return nullptr; // ��ȯ ���� �� nullptr ��ȯ
    }

    // IDirect3DDevice ����
    hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.Get(), &inspectableDevice);
    if (FAILED(hr)) {
        return nullptr; // ���� ���� �� nullptr ��ȯ
    }

    return inspectableDevice; // ���� �� IInspectable ��ȯ
}

inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice GetWinRTDirect3DDeviceFromD3D11Device(ID3D11Device* d3d11Device) {
    winrt::com_ptr<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice> direct3DDevice;

    if (!d3d11Device) {
        return nullptr; // d3d11Device�� nullptr�̸� �۾� ����
    }

    // IDXGIDevice�� ��ȯ
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    HRESULT hr = d3d11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (FAILED(hr)) {
        return nullptr; // ��ȯ ���� �� nullptr ��ȯ
    }

    // WinRT IDirect3DDevice ����
    winrt::com_ptr<::IInspectable> inspectableDevice;
    hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.Get(), inspectableDevice.put());
    if (FAILED(hr)) {
        return nullptr; // ���� ���� �� nullptr ��ȯ
    }

    return inspectableDevice.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(); // ���� �� IDirect3DDevice ��ȯ
}

struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1"))
    IDirect3DDxgiInterfaceAccess : ::IUnknown
{
    virtual HRESULT __stdcall GetInterface(GUID const& id, void** object) = 0;
};

template <typename T>
auto GetDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object)
{
    auto access = object.as<IDirect3DDxgiInterfaceAccess>();
    winrt::com_ptr<T> result;
    if (access != nullptr)
    {
        winrt::check_hresult(access->GetInterface(winrt::guid_of<T>(), result.put_void()));
    }
    else
    {
        result = nullptr;
    }
    return result;
}