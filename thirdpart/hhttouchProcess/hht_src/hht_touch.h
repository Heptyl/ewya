/// @file hht_touch.h
/// @author Ophelia (liujigui@honghe-tech.com)
/// @brief
/// @version 0.1
/// @date 2023-03-15
///
/// Copyright (c) 2023 HiteVision. All Rights Reserved.
///

#ifndef HHT_TOUCH_H
#define HHT_TOUCH_H

#include <linux/ioctl.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

#define HHT_TOUCH_DEVICE_NAME "hht-touch"

#define HHT_TOUCH_MASK_ZONE_COUNT       20
#define HHT_TOUCH_COORDINATE_RANGE      0x7fff
#define HHT_TOUCH_MAX_TOUCH_POINT_COUNT 64

enum HHTTouchChannel
{
  HHT_CHANNEL_SYSTEM   = 1 << 0,
  HHT_CHANNEL_LOOPBACK = 1 << 1,
  HHT_CHANNEL_ALL      = HHT_CHANNEL_SYSTEM | HHT_CHANNEL_LOOPBACK
};

typedef enum HHTTouchChannel hht_touch_channel_t;

enum HHTToolType
{
  HHT_TOOL_PEN_NIB = 1 << 0,
  HHT_TOOL_PEN_CAP = 1 << 1,
  HHT_TOOL_FINGER  = 1 << 2,
  HHT_TOOL_ERASER  = 1 << 3
};

typedef enum HHTToolType hht_tool_t;

typedef enum HHTTouchCMD
{
  HHT_TOUCH_SET_CHANNEL,
  HHT_TOUCH_SET_TOOL_EVENT,
  HHT_TOUCH_SET_PEN_SIZE,
  HHT_TOUCH_SET_MASK_ZONE,
  HHT_TOUCH_SET_TRANSFORM,
  HHT_TOUCH_SET_PALM_REJECTION,
  HHT_TOUCH_GET_FIRMWARE_INFO,
  HHT_TOUCH_GET_DRIVER_INFO,
  HHT_TOUCH_GET_DEVICE_DIMENSION,
  HHT_TOUCH_SET_ERASER_CODE,
  HHT_TOUCH_QUERY_SUPPORT_PRES,
  HHT_TOUCH_GET_PRES_LEVEL,
  HHT_TOUCH_GET_PRES_SENSITIVITY,
  HHT_TOUCH_SET_PRES_SENSITIVITY
} hht_touch_cmd_t;

typedef struct HHTTouchPenPropertys
{
  __s32 nib_size;
  __s32 cap_size;
} hht_touch_pen_propertys_t;

struct HHTTouchMisProperty
{
  __s32 channel;
  __s32 value;
};

struct HHTTouchMaskZone
{
  __s32 id;
  __s32 enabled;
  __s32 left;
  __s32 right;
  __s32 top;
  __s32 bottom;
};

struct HHTTouchTransform
{
  __s32 channel;
  __s32 x_scale;
  __s32 x_offset;
  __s32 y_scale;
  __s32 y_offset;
};

struct HHTTouchVersions
{
  __s32 major;
  __s32 minor;
  __s32 revision;
};

struct HHTTouchDate
{
  __s32 year;
  __s32 month;
  __s32 day;
};

struct HHTTouchSoftwareInfo
{
  struct HHTTouchVersions version;
  struct HHTTouchDate build_date;
};

struct HHTTouchDeviceDimension
{
  __s32 inch;
  __s32 w;
  __s32 h;
};

#define HHT_TOUCH_IOC_TYPE 'H'
#define HTOUCHIOCSETCHANNEL \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_CHANNEL, struct HHTTouchMisProperty)
#define HTOUCHIOCSETTOOL \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_TOOL_EVENT, struct HHTTouchMisProperty)
#define HTOUCHIOCSETERASERCODE \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_ERASER_CODE, __s32)
#define HTOUCHIOCSETPENSIZE \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_PEN_SIZE, struct HHTTouchPenPropertys)
#define HTOUCHIOCSETMASKZONE \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_MASK_ZONE, struct HHTTouchMaskZone)
#define HTOUCHIOCSETTRANSFORM \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_TRANSFORM, struct HHTTouchTransform)
#define HTOUCHIOCSETPALMREJECTION                        \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_PALM_REJECTION, \
       struct HHTTouchMisProperty)
#define HTOUCHIOCGETFIRMWAREINFO                        \
  _IOR(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_GET_FIRMWARE_INFO, \
       struct HHTTouchSoftwareInfo)
#define HTOUCHIOCGETDRIVERINFO                        \
  _IOR(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_GET_DRIVER_INFO, \
       struct HHTTouchSoftwareInfo)
#define HTOUCHIOCGETDEVICEDIMENSION                        \
  _IOR(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_GET_DEVICE_DIMENSION, \
       struct HHTTouchDeviceDimension)
#define HTOUCHIOCSETPRESSURESENSITIVITY \
  _IOW(HHT_TOUCH_IOC_TYPE, HHT_TOUCH_SET_PRES_SENSITIVITY, __s32)

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif // !HHT_TOUCH_H