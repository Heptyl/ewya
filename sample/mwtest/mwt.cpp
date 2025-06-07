#include <stdio.h>
#include <string.h>
#include <utils/String8.h>
#include <hwbinder/Parcel.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmw.h>

using namespace android;
using vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmw;
using android::sp;
using android::hardware::Parcel;
using android::hardware::hidl_string;

int main(int argc, char **argv)
{
    int i = 1;
    int paramInt[10] = {0};
    char paramStr[10][64];

    sp<IXbhHwmw> service = IXbhHwmw::getService();
    if( service == nullptr )
    {
        printf("Can't find IXbhHwmw service...");
        return -1;
    }

    //Confirm args number
    if (argc > 10 || argc < 2)
    {
        printf("prarm count is wrong: %d\n", argc);
        printf("Please input command with follow format:\n");
        printf("getOutputStatus(MCU_PC14)\n");
        printf("CMD: mwt 57345 196622\n");
        printf("CMD: mwt 0xe001 0x3000e\n");
        printf("setOutputStatus(MCU_PC14)\n");
        printf("CMD: mwt 57344 196622 1\n");
        printf("CMD: mwt 0xe000 0x3000e 1\n");
        printf("setSn(123456789)\n");
        printf("CMD: mwt 0x900e s123456789\n");
        return -1;
    }

    android::hardware::Parcel request;
    android::hardware::Parcel reply;

    memset(paramStr, 0, sizeof(paramStr));
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '0' && argv[i][1] == 'x')
        {
            paramInt[i] = strtol(argv[i], NULL, 16);
            request.writeInt32(paramInt[i]);
        }
        else if (argv[i][0] >= '0' && argv[i][0] <= '9')
        {
            paramInt[i] = strtol(argv[i], NULL, 10);
            request.writeInt32(paramInt[i]);
        }
        else if (argv[i][0] == 's')
        {
            memcpy(paramStr[i], &argv[i][1], strlen(&argv[i][1]));
            String16 ss(paramStr[i]);
            request.writeString16(ss);
        }
        else
        {
            printf("Unknown paramInt: %s\n", argv[i]);
            return -1;
        }
    }

    //debug
    #if 1
    printf("--------- Input ----------\n");
    printf("paramInt(hex): ");
    for (i = 1; i < argc; i++)
    {
        if (strlen(paramStr[i]) != 0)
        {
            printf("%s, ", paramStr[i]);
        }
        else
        {
            printf("0x%x, ", paramInt[i]);
        }
    }
    printf("\n");
    printf("paramInt(int): ");
    for (i = 1; i < argc; i++)
    {
        if (strlen(paramStr[i]) != 0)
        {
            printf("%s, ", paramStr[i]);
        }
        else
        {
            printf("%d, ", paramInt[i]);
        }
    }
    printf("\n");
    #endif

    hardware::hidl_string inputStr((char*)request.data(), request.dataSize());
    String8 retReply;
    auto cb = [&](hidl_string strReply){
        retReply = String8(strReply.c_str(), strReply.size());
    };
    service->hwInvoke(inputStr, cb);

    reply.setDataSize(retReply.size());
    reply.setDataPosition(0);
    reply.write(retReply.c_str(), retReply.size());
    reply.setDataPosition(0);

    printf("--------- Output ---------\n");
    for (i = 0; i < (int)retReply.size() / 4; i++)
    {
        if (i == 0)
        {
            printf("Eywa ret: %d\n", reply.readInt32());
        }
        else if (i == 1)
        {
            int retVal = reply.readInt32();
            printf("Eywa return: 0x%x(%d), ", retVal, retVal);
        }
        else
        {
            int retVal = reply.readInt32();
            printf("0x%x(%d), ", retVal, retVal);
        }
    }
    printf("\n");

    return 0;
}

