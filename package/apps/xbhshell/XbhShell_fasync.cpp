#include "XbhShell.h"
#include <linux/input.h>
#include <linux/uinput.h>
#include <cutils/properties.h>

#define UNUSED(x) (void)x
int mFd;
char mValue[5];
char str[64];
/*
mValue[0]: msgType, 0 is inputEvent 
mValue[1]: keycode, from 0x00 to 0xff
mValue[2]: isPressed, 0x00: release, 0x01: press
mValue[3]: devType, 1 is ir_keypad, 2 is adc_keypad, 3 is gpio_keypad
mValue[4]: 0
*/
static void sig_handler(int num)
{
    //LOGD("sig_handler() num = %d ",num);
    UNUSED(num);
    while(true)
    {
        //set msg type = 0
        mValue[0] = 0;
        int ret = read(mFd, &mValue, 5);
        //LOGD("XBH_sig_handler read count = %d ",ret);
        if(ret <= 0)
        {
            break;
        }
        //LOGD("sig_handler mValue[0] = %d mValue[1] = %d mValue[2] = %d mValue[3] = %d mValue[4] = %d \n", mValue[0], mValue[1], mValue[2], mValue[3], mValue[3]);
        if(mValue[0] == 0) //msgType = 0
        {
            if( mValue[2] == 1) //down
            {
                if(mValue[3] == 1) //is ir
                {
                    send_ir_keycode(mValue[1], 1); //keycode
                }
                else
                {
                    send_keypad_keycode(mValue[1], 1); //keycode
                }
            }
            else if (mValue[2] == 0) //up
            {
                if(mValue[3] == 1)
                {
                    send_ir_keycode(mValue[1], 0);
                }
                else
                {
                    send_keypad_keycode(mValue[1], 0);
                }
            }
        }
    }
}

void fasync_init()
{
    signal(SIGIO, sig_handler);
    mFd = open("/dev/xbh_input_fasync", O_RDONLY);
    if(mFd >= 0)
    {
        LOGD("XbhFasyncTask() mFd = %d ",mFd);
        fcntl(mFd, F_SETOWN, getpid());
        fcntl(mFd, F_SETFL, fcntl(mFd, F_GETFL) | FASYNC);
    }
    else
    {
        LOGD("Error %d: Failed to open file\n", errno);
    }
}