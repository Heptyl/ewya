#include "XbhShell.h"
#include <linux/input.h>
#include <linux/uinput.h>

#define UNUSED(x) (void)x
int uinp_fd;
static int g_fdEvent;
#define XBH_KEYPAD_NAME "xbh_keypad"

void open_xbh_kepad_input();
int uinput_keypad_init() 
{
    struct uinput_user_dev uinp; // uInput device structure
    int i;
    // Open the input device
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinp_fd == 0) {
        LOGD("Unable to open /dev/uinput\n");
        return -1;
    }

    // Intialize the uInput device to NULL
    memset(&uinp, 0x00, sizeof(uinp));
    strncpy(uinp.name, XBH_KEYPAD_NAME, sizeof(uinp.name)-1);
    uinp.id.vendor = 0x365f;
    uinp.id.product = 0x4002;
    uinp.id.bustype = BUS_VIRTUAL;

    // Keyboard value of area
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);

    for (i = 1; i < 0xff; i++) {
        ioctl(uinp_fd, UI_SET_KEYBIT, i);
    }
    ioctl(uinp_fd, UI_SET_EVBIT, EV_SYN);

    // Create input device into input sub-system
    if (write(uinp_fd, &uinp, sizeof(uinp)) != sizeof(uinp)) {
        LOGD("First write returned fail.\n");
        return -1;
    }
    //  创建
    if (ioctl(uinp_fd, UI_DEV_CREATE)) {
        LOGD("ioctl UI_DEV_CREATE returned fail.\n");
        return -1;
    }

    return 1;
}

// 打开XBH KEYPAD输入设备
void open_xbh_keypad_input()
{
    int fd = -1; // 设备文件描述符
    char name[256] = "Unknown"; // 设备名称
    char eventname[256]; // 设备路径
    int i = 0; // 循环计数器
     // 遍历/dev/input/eventX设备
    for (i = 0; i < 20; i++)
    {
        memset(eventname, 0, sizeof(eventname)); // 清空eventname数组
        sprintf(eventname, "/dev/input/event%d", i); // 构建设备路径
        LOGD("eventname is %s\n", eventname); // 打印设备路径
         // 打开设备文件
        if ((fd = open(eventname, O_RDWR)) < 0) 
        {
            LOGD("evdev open error !!!! "); // 打开失败，输出错误信息
            continue; // 继续下一个设备
        }
        memset(name, 0, sizeof(name)); // 清空name数组
        // 获取设备名称
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) 
        {
            LOGD("evdev ioctl error !!!! "); // 获取失败，输出错误信息
            continue; // 继续下一个设备
        }
        LOGD("The device on %s its name is %s\n", eventname, name); // 打印设备名称
        // 判断设备名称是否为"xbh_keypad"
        if (0 == strcmp(name, XBH_KEYPAD_NAME))
        {
            g_fdEvent = fd;
            return; // 找到对应设备后跳出循环
        }
        close(fd); // 关闭设备文件
    }
}

void send_keypad_keycode(int keyCode, int down) 
{
    struct input_event event; // Input device structure
    if(g_fdEvent <= 0)
    {
        LOGD("g_fdEvent = %d ", g_fdEvent);
        open_xbh_keypad_input();
        if(g_fdEvent <= 0)
        {
            LOGD("g_fdEvent = %d again ", g_fdEvent);
            return;
        }
    }
    //  发送按键事件
    memset(&event, 0x00, sizeof(event));
    event.type = EV_KEY;                        // 事件类型
    event.code = keyCode;                       // 键值
    event.value = down;                         // 抬起或者按下
    write(g_fdEvent, &event, sizeof(event));

    //发送sync 事件
    memset(&event, 0x00, sizeof(event));
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(g_fdEvent, &event, sizeof(event));
}
