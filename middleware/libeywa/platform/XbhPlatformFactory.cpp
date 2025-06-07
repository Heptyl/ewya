#include "XbhPlatformFactory.h"


XBH_VOID* XbhPlatformFactory::CreatePlatform(PLATFORM_TYPE type)
{
    switch (type)
    {
    #ifdef AML_311D2
    case PLATFORM_TYPE::AMLOGIC_311D2:
        if(pXbhAmlogic_311d2 == XBH_NULL)
        {
            #if (TYPE_BOARD == XMA311D2B2)
            pXbhAmlogic_311d2 = XbhXMA311D2_B2::getInstance();
            #endif

            #if (TYPE_BOARD == XMA311D2X)
            pXbhAmlogic_311d2 = XbhXMA311D2_X::getInstance();
            #endif

            #if (TYPE_BOARD == XMA311D2B)
            pXbhAmlogic_311d2 = XbhXMA311D2_B::getInstance();
            #endif

            #if (TYPE_BOARD == XMA311D2Y)
            pXbhAmlogic_311d2 = XbhXMA311D2_Y::getInstance();
            #endif

            #if (TYPE_BOARD == XMA311D2T2)
            pXbhAmlogic_311d2 = XbhXMA311D2_T2::getInstance();
            #endif

        }
        return (XBH_VOID*)pXbhAmlogic_311d2;
        break;
    #endif
    #ifdef MTK_9679
    case PLATFORM_TYPE::MEDIATEK_9679:
        if(pXbhMtk_9679 == XBH_NULL)
        {
            #if (TYPE_BOARD == XMM9679A)
            pXbhMtk_9679 = XbhXMM9679_A::getInstance();
            #elif (TYPE_BOARD == XMM9679B)
            pXbhMtk_9679 = XbhXMM9679_B::getInstance();
            #elif (TYPE_BOARD == XMM9679C)
            pXbhMtk_9679 = XbhXMM9679_C::getInstance();
            #elif (TYPE_BOARD == XMM9679E)
            pXbhMtk_9679 = XbhXMM9679_E::getInstance();
            #elif (TYPE_BOARD == XMM9679U)
            pXbhMtk_9679 = XbhXMM9679_U::getInstance();
            #elif (TYPE_BOARD == XMM9679D)
            pXbhMtk_9679 = XbhXMM9679_D::getInstance();
            #elif (TYPE_BOARD == XMDM9679A)
            pXbhMtk_9679 = XbhXMDM9679_A::getInstance();
            #elif (TYPE_BOARD == XMM9679G)
            pXbhMtk_9679 = XbhXMM9679_G::getInstance();
            #endif
        }
        return (XBH_VOID*)pXbhMtk_9679;
        break;
    #endif
    #ifdef RK_3576
    case PLATFORM_TYPE::ROCKCHIP_3576:
        if(pXbhRk_3576 == XBH_NULL)
        {
            #if (TYPE_BOARD == XMR3576B)
            pXbhRk_3576 = XbhXMR3576_B::getInstance();
            #endif

            #if (TYPE_BOARD == XMR3588U)
            pXbhRk_3576 = XbhXMR3588_U::getInstance();
            #endif

            #if (TYPE_BOARD == XMR3576D)
            pXbhRk_3576 = XbhXMR3576_D::getInstance();
            #endif

            #if (TYPE_BOARD == XMR3576E)
            pXbhRk_3576 = XbhXMR3576_E::getInstance();
            #endif

            #if (TYPE_BOARD == XMR3576U)
            pXbhRk_3576 = XbhXMR3576_U::getInstance();
            #endif
        }
        return (XBH_VOID*)pXbhRk_3576;
        break;
    #endif
    #ifdef MTK_8195
    case PLATFORM_TYPE::MEDIATEK_8195:
        if(pXbhMtk_8195 == XBH_NULL)
        {
            #if (TYPE_BOARD == XMM8195U)
            pXbhMtk_8195 = XbhXMM8195_U::getInstance();
            #elif (TYPE_BOARD == XMM8195G)
            pXbhMtk_8195 = XbhXMM8195_G::getInstance();
            #elif (TYPE_BOARD == XMM8195U_V2)
            pXbhMtk_8195 = XbhXMM8195_U_V2::getInstance();
            #endif
        }
        return (XBH_VOID*)pXbhMtk_8195;
        break;
    #endif
    #ifdef HISI_610
    case PLATFORM_TYPE::HISILICON_610:
        if(pXbhHi_610 == XBH_NULL)
        {
            #if (TYPE_BOARD == XMH610A)
            pXbhHi_610 = XbhXMH610_A::getInstance();
            #endif
        }
        return (XBH_VOID*)pXbhHi_610;
        break;
    #endif
    default:
        return NULL;
        break;
    }
}

