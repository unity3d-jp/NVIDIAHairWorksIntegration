#include "pch.h"
#include "HairWorkdIntegration.h"
#include "hwContext.h"


hwContext::hwContext()
    : m_sdk(nullptr)
{

}

hwContext::~hwContext()
{
    finalize();
}

bool hwContext::initialize(const char *path_to_dll)
{
    if (m_sdk != nullptr) {
        return false;
    }

    m_sdk = GFSDK_LoadHairSDK(path_to_dll, GFSDK_HAIRWORKS_VERSION);
    if (m_sdk == nullptr) {
        return false;
    }
}

bool hwContext::finalize()
{
    if (m_sdk != nullptr) {
        m_sdk->Release();
        m_sdk = nullptr;
    }
}

