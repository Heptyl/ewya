#include "hht_touch_device.h"

#include <assert.h>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef __ANDROID__
#define LOG_NDEBUG 0
#define LOG_TAG    "hht_touch"
#include <cutils/log.h>
#define LOGD(...) ALOGD(__VA_ARGS__)
#define LOGI(...) ALOGI(__VA_ARGS__)
#define LOGW(...) ALOGW(__VA_ARGS__)
#else
#include <stdio.h>
#define LOGD(format, ...) \
  printf("%s(%d):" format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGI(format, ...) \
  printf("%s(%d):" format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGW(format, ...) \
  printf("%s(%d):" format, __FILE__, __LINE__, ##__VA_ARGS__)
#endif // __ANDROID__

namespace hht { namespace touch {
namespace {
std::once_flag initialized;
std::unique_ptr<Device> d = nullptr;
int fd                    = -1;
} // namespace

Device::Device() noexcept = default;

Device::~Device() { close(); }

Device* Device::instance() noexcept
{
  std::call_once(initialized,
                 [&] { d = std::unique_ptr<Device> {new Device {}}; });
  return d.get();
}

int Device::open() noexcept
{
  assert(d);
  if (opened()) return 0;

  fd = ::open("/dev/" HHT_TOUCH_DEVICE_NAME, O_RDWR);
  if (fd == -1) {
    LOGW("Open touch device failed: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

bool Device::opened() noexcept { return fd != -1; }

void Device::close() noexcept
{
  if (opened()) {
    ::close(fd);
    fd = -1;
  }
}

#define CHECK_NODE_OTHERWISE_RETURN(ret) \
  {                                      \
    if (! opened() && open()) {          \
      return ret;                        \
    }                                    \
  }

int Device::firmwareVersions(int& major, int& minor, int& revision) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchSoftwareInfo info;
  int ret = ioctl(fd, HTOUCHIOCGETFIRMWAREINFO, &info);
  if (! ret) {
    major    = info.version.major;
    minor    = info.version.minor;
    revision = info.version.revision;
  }

  return ret;
}

int Device::driverVersions(int& major, int& minor, int& revision) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchSoftwareInfo info;
  int ret = ioctl(fd, HTOUCHIOCGETDRIVERINFO, &info);
  if (! ret) {
    major    = info.version.major;
    minor    = info.version.minor;
    revision = info.version.revision;
  }

  return ret;
}

int Device::setChannel(Channel c, bool enable) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchMisProperty prop
      = {static_cast<__s32>(c), static_cast<__s32>(enable)};
  return ioctl(fd, HTOUCHIOCSETCHANNEL, &prop);
}

int Device::setToolEvent(Channel c, ToolType tools) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchMisProperty prop
      = {static_cast<__s32>(c), static_cast<__s32>(tools)};
  return ioctl(fd, HTOUCHIOCSETTOOL, &prop);
}

int Device::setEraserCode(int code) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  return ioctl(fd, HTOUCHIOCSETERASERCODE, &code);
}

int Device::setPenSize(size_t nib, size_t cap) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchPenPropertys prop
      = {static_cast<__s32>(nib), static_cast<__s32>(cap)};
  return ioctl(fd, HTOUCHIOCSETPENSIZE, &prop);
}

int Device::setMaskZone(int id, int left, int top, int right,
                        int bottom) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  if (id >= SUPPORTED_MASK_ZONE_NUM) {
    LOGW("Invalid zone id!!!\n");
    return -1;
  }

  if (right < left || bottom < top || right > HHT_TOUCH_COORDINATE_RANGE
      || bottom > HHT_TOUCH_COORDINATE_RANGE) {
    LOGW("Abnormal zone value!!!\n");
    return -1;
  }

  struct HHTTouchMaskZone prop
      = {static_cast<__s32>(id),   static_cast<__s32>(true),
         static_cast<__s32>(left), static_cast<__s32>(right),
         static_cast<__s32>(top),  static_cast<__s32>(bottom)};
  return ioctl(fd, HTOUCHIOCSETMASKZONE, &prop);
}

int Device::unsetMaskZone(int id) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  if (id >= SUPPORTED_MASK_ZONE_NUM) {
    LOGW("Invalid zone id!!!\n");
    return -1;
  }

  struct HHTTouchMaskZone prop
      = {static_cast<__s32>(id), static_cast<__s32>(false), 0, 0, 0, 0};
  return ioctl(fd, HTOUCHIOCSETMASKZONE, &prop);
}

int Device::setPalmRejection(Channel c, bool yes) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchMisProperty prop
      = {static_cast<__s32>(c), static_cast<__s32>(yes)};
  return ioctl(fd, HTOUCHIOCSETPALMREJECTION, &prop);
}

int Device::setTransform(Channel c, int32_t x_scale, int32_t x_offset,
                         int32_t y_scale, int32_t y_offset) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchTransform prop
      = {static_cast<__s32>(c), static_cast<__s32>(x_scale),
         static_cast<__s32>(x_offset), static_cast<__s32>(y_scale),
         static_cast<__s32>(y_offset)};
  return ioctl(fd, HTOUCHIOCSETTRANSFORM, &prop);
}

int Device::dimension(int& inch, int& w, int& h) noexcept
{
  CHECK_NODE_OTHERWISE_RETURN(-1);

  struct HHTTouchDeviceDimension dim;
  int ret = ioctl(fd, HTOUCHIOCGETDEVICEDIMENSION, &dim);
  if (! ret) {
    inch = dim.inch;
    w    = dim.w;
    h    = dim.h;
  }

  return ret;
}

}} // namespace hht::touch
