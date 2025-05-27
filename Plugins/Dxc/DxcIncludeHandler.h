// reference: https://simoncoenen.com/blog/programming/graphics/DxcCompiling

#ifndef LYRA_SHADER_DXC_INCLUDE_HANDLER_H
#define LYRA_SHADER_DXC_INCLUDE_HANDLER_H

// library headers
#include <Common/Container.h>
#include <Common/String.h>
#include <Common/Path.h>

// dxc headers
#ifdef LYRA_WINDOWS
#include <d3d12shader.h>
#endif
#include <dxc/dxcapi.h>

// local plugin headers
#include "WinComPtr.h"

using namespace lyra;

class CustomIncludeHandler : public IDxcIncludeHandler
{
public:
    explicit CustomIncludeHandler(IDxcUtils* pUtils) : pUtils(pUtils) {}

    HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
    {
        ComPtr<IDxcBlobEncoding> pEncoding;

        String path = normalize_path(to_string(pFilename));

        if (included_files.find(path) != included_files.end()) {
            // Return empty string blob if this file has been included before
            static const char nullStr[] = " ";
            pUtils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, &pEncoding);
            *ppIncludeSource = pEncoding.Detach();
            return S_OK;
        }

        HRESULT hr = pUtils->LoadFile(pFilename, nullptr, &pEncoding);
        if (SUCCEEDED(hr)) {
            included_files.insert(path);
            *ppIncludeSource = pEncoding.Detach();
        }
        return hr;
    }

    // clang-format off
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) override { return E_NOINTERFACE; }
    ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
    ULONG STDMETHODCALLTYPE Release(void) override { return 0; }
    // clang-format on

    HashSet<String> included_files;
    IDxcUtils*      pUtils;
};

#endif // LYRA_SHADER_DXC_INCLUDE_HANDLER_H
