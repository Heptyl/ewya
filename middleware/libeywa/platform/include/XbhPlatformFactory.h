#ifndef XBH_PLATFORM_FACTORY_H
#define XBH_PLATFORM_FACTORY_H

#include "XbhType.h"
#include "XbhMutex.h"

#ifdef AML_311D2
#include "XbhAmlogic_311d2.h"

#if (TYPE_BOARD == XMA311D2B2)
#include "XbhXMA311D2_B2.h"
#endif

#if (TYPE_BOARD == XMA311D2X)
#include "XbhXMA311D2_X.h"
#endif

#if (TYPE_BOARD == XMA311D2B)
#include "XbhXMA311D2_B.h"
#endif

#if (TYPE_BOARD == XMA311D2Y)
#include "XbhXMA311D2_Y.h"
#endif

#if (TYPE_BOARD == XMA311D2T2)
#include "XbhXMA311D2_T2.h"
#endif

#endif

#ifdef MTK_9679
#include "XbhMtk_9679.h"

#if (TYPE_BOARD == XMM9679A)
#include "XbhXMM9679_A.h"
#elif (TYPE_BOARD == XMM9679B)
#include "XbhXMM9679_B.h"
#elif (TYPE_BOARD == XMM9679C)
#include "XbhXMM9679_C.h"
#elif (TYPE_BOARD == XMM9679U)
#include "XbhXMM9679_U.h"
#elif (TYPE_BOARD == XMM9679D)
#include "XbhXMM9679_D.h"
#elif (TYPE_BOARD == XMDM9679A)
#include "XbhXMDM9679_A.h"
#elif (TYPE_BOARD == XMM9679E)
#include "XbhXMM9679_E.h"
#elif (TYPE_BOARD == XMM9679G)
#include "XbhXMM9679_G.h"
#endif

#endif

#ifdef RK_3576
#include "XbhRk_3576.h"

#if (TYPE_BOARD == XMR3576A)
#include "XbhXMR3576_A.h"
#endif

#if (TYPE_BOARD == XMR3576B)
#include "XbhXMR3576_B.h"
#endif

#if (TYPE_BOARD == XMR3588U)
#include "XbhXMR3588_U.h"
#endif

#if (TYPE_BOARD == XMR3576D)
#include "XbhXMR3576_D.h"
#endif

#if (TYPE_BOARD == XMR3576E)
#include "XbhXMR3576_E.h"
#endif

#if (TYPE_BOARD == XMR3576U)
#include "XbhXMR3576_U.h"
#endif

#endif

#ifdef MTK_8195
#include "XbhMtk_8195.h"
#if (TYPE_BOARD == XMM8195U)
#include "XbhXMM8195_U.h"
#elif (TYPE_BOARD == XMM8195G)
#include "XbhXMM8195_G.h"
#elif (TYPE_BOARD == XMM8195U_V2)
#include "XbhXMM8195_U_V2.h"
#endif
#endif

#ifdef HISI_610
#include "XbhHi_610.h"

#if (TYPE_BOARD == XMH610A)
#include "XbhXMH610_A.h"
#elif (TYPE_BOARD == XMH610B)
#include "XbhXMH610_B.h"
#endif
#endif

enum PLATFORM_TYPE
{
    AMLOGIC_311D2,
    MEDIATEK_9679,
    ROCKCHIP_3576,
    MEDIATEK_8195,
    HISILICON_610,
};

class XbhPlatformFactory
{
public:
    XBH_VOID* CreatePlatform(PLATFORM_TYPE type);
private:
#ifdef AML_311D2
    XbhAmlogic_311d2* pXbhAmlogic_311d2;
#endif
#ifdef MTK_9679
    XbhMtk_9679* pXbhMtk_9679;
#endif
#ifdef RK_3576
    XbhRk_3576* pXbhRk_3576;
#endif
#ifdef MTK_8195
    XbhMtk_8195* pXbhMtk_8195;
#endif
#ifdef HISI_610
    XbhHi_610* pXbhHi_610;
#endif
};


#endif //XBH_PLATFORM_FACTORY_H

