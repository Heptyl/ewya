/// @file device.h
/// @author Ophelia (liujigui@honghe-tech.com)
/// @brief
/// @version 0.1
/// @date 2021-03-05
///
/// Copyright (c) 2021 HiteVision. All Rights Reserved.
///
#ifndef HHT_TOUCH_DEVICE_H
#define HHT_TOUCH_DEVICE_H

#include "hht_touch.h"

#include <cstdint>
#include <tuple>
#include <type_traits>

namespace hht { namespace touch {
enum class Channel
{
  System = HHT_CHANNEL_SYSTEM,
  Output = HHT_CHANNEL_LOOPBACK,
  All    = HHT_CHANNEL_ALL
};

enum class ToolType
{
  PenNib = HHT_TOOL_PEN_NIB,
  PenCap = HHT_TOOL_PEN_CAP,
  Finger = HHT_TOOL_FINGER,
  Eraser = HHT_TOOL_ERASER,
  All = HHT_TOOL_PEN_NIB | HHT_TOOL_PEN_CAP | HHT_TOOL_FINGER | HHT_TOOL_ERASER
};

static inline Channel operator|(Channel left, Channel right) noexcept
{
  using underlying_t = typename std::underlying_type<Channel>::type;
  return static_cast<Channel>(static_cast<underlying_t>(left)
                              | static_cast<underlying_t>(right));
}

static inline ToolType operator|(ToolType left, ToolType right) noexcept
{
  using underlying_t = typename std::underlying_type<ToolType>::type;
  return static_cast<ToolType>(static_cast<underlying_t>(left)
                               | static_cast<underlying_t>(right));
}

class DeviceInterface
{
 public:
  /// @brief max mask zone number supported.
  ///
  static constexpr int SUPPORTED_MASK_ZONE_NUM = HHT_TOUCH_MASK_ZONE_COUNT;

  /// @brief Open device node, should call this funtion first.
  ///
  /// @return int 0 if successed, or -1 if failed.
  virtual int open() noexcept    = 0;
  virtual bool opened() noexcept = 0;

  /// @brief Close device node
  ///
  virtual void close() noexcept = 0;

  /// @brief Get device's firmware versions
  /// @param major
  /// @param minor
  /// @param revision
  /// @return
  virtual int firmwareVersions(int& major, int& minor, int& revision) noexcept
      = 0;

  /// @brief Get driver versions
  /// @param major
  /// @param minor
  /// @param revision
  /// @return
  virtual int driverVersions(int& major, int& minor, int& revision) noexcept
      = 0;

  /// @brief Set the Channel enable or not
  ///
  /// @param c
  /// @param enable
  /// @return int
  virtual int setChannel(Channel c, bool enable) noexcept = 0;

  /// @brief Set or unset the active tool events
  ///
  /// @param c
  /// @param toolbits
  /// @param set
  /// @return int
  virtual int setToolEvent(Channel c, ToolType tools) noexcept = 0;

  /// @brief Set eraser code for reporting to Linux input system
  /// @param code
  /// @return
  virtual int setEraserCode(int code) noexcept = 0;

  /// @brief Set nib and cap size for pen events
  ///
  /// @param nib
  /// @param cap
  /// @return int
  virtual int setPenSize(std::size_t nib, std::size_t cap) noexcept = 0;

  /// @brief Set 'Mask Zone', the scaling of coordination check
  /// COORDINATION_SCALING
  ///
  /// @param id
  /// @param left
  /// @param top
  /// @param right
  /// @param bottom
  /// @return int 0 if successed, or -1 if failed.
  virtual int setMaskZone(int id, int left, int top, int right,
                          int bottom) noexcept
      = 0;

  /// @brief Unset 'Mask Zone'
  ///
  /// @param id
  /// @return int
  virtual int unsetMaskZone(int id) noexcept = 0;

  /// @brief Enable palm-rejection or not.
  ///
  /// @param c
  /// @param yes
  /// @return int
  virtual int setPalmRejection(Channel c, bool yes) noexcept = 0;

  /// @brief Set transform matrix
  ///
  /// @param c
  /// @param x_scale
  /// @param x_offset
  /// @param y_scale
  /// @param y_offset
  /// @return int
  virtual int setTransform(Channel c, int32_t x_scale, int32_t x_offset,
                           int32_t y_scale, int32_t y_offset) noexcept
      = 0;

  /// @brief Get touch frame' physical dimension.
  /// @param inch 
  /// @param w 
  /// @param h 
  /// @return 
  virtual int dimension(int& inch, int& w, int& h) noexcept = 0;

 public:
  virtual ~DeviceInterface() = default;

 protected:
  DeviceInterface() = default;
};

class Device : public DeviceInterface
{
 public:
  ~Device();

  static Device* instance() noexcept;

  int open() noexcept override;
  bool opened() noexcept override;

  void close() noexcept override;

  int firmwareVersions(int& major, int& minor, int& revision) noexcept override;

  int driverVersions(int& major, int& minor, int& revision) noexcept override;

  int setChannel(Channel c, bool enable) noexcept override;

  int setToolEvent(Channel c, ToolType toolbits) noexcept override;

  int setEraserCode(int code) noexcept override;

  int setPenSize(std::size_t nib, std::size_t cap) noexcept override;

  int setMaskZone(int id, int left, int top, int right,
                  int bottom) noexcept override;

  int unsetMaskZone(int id) noexcept override;

  int setPalmRejection(Channel c, bool yes) noexcept override;

  int setTransform(Channel c, int32_t x_scale, int32_t x_offset,
                   int32_t y_scale, int32_t y_offset) noexcept;

  int dimension(int& inch, int& w, int& h) noexcept override;

 private:
  Device() noexcept;
};

}} // namespace hht::touch

#endif // !HHT_TOUCH_DEVICE_H