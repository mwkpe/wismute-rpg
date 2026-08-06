#ifndef WIS_CORE_ANIMATION_H
#define WIS_CORE_ANIMATION_H


#include <cstdint>
#include "core/timer.h"


namespace wis {


class Animation
{
public:
  void init(std::uint32_t first_frame, std::uint32_t last_frame, std::uint32_t duration_ms);
  void start();
  void stop();
  void reset();
  void update(std::uint64_t elapsed_ns);

  [[nodiscard]] std::uint32_t is_active() const { return is_active_; }
  [[nodiscard]] std::uint32_t current_frame() const { return current_frame_; }
  [[nodiscard]] std::uint32_t duration() const;

private:
  std::uint32_t first_frame_ = 0u;
  std::uint32_t last_frame_ = 0u;
  std::uint32_t current_frame_ = 0u;
  Nano_timer timer_;
  bool is_active_ = false;
};


}  // namespace wis


#endif  // WIS_CORE_ANIMATION_H
