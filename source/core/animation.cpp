#include "animation.h"


#include <algorithm>
#include <cmath>


void wis::Animation::init(std::uint32_t first_frame, std::uint32_t last_frame,
    std::uint32_t duration_ms)
{
  first_frame_ = first_frame;
  last_frame_ = last_frame;
  current_frame_ = first_frame_;

  timer_.set_delay(static_cast<std::uint64_t>(duration_ms) * 1'000'000u);
}


void wis::Animation::start()
{
  if (!is_active_) {
    reset();
    is_active_ = true;
  }
}


void wis::Animation::stop()
{
  is_active_ = false;
}


void wis::Animation::reset()
{
  stop();
  current_frame_ = first_frame_;
  timer_.reset();
}


void wis::Animation::update(std::uint64_t elapsed_ns)
{
  if (!is_active_ || current_frame_ == last_frame_) {
    return;
  }

  timer_.update(elapsed_ns);

  if (timer_.is_elapsed()) {
    current_frame_++;

    if (current_frame_ < last_frame_) {
      timer_.restart();
    }
    else {
      is_active_ = false;
    }
  }
}


[[nodiscard]] std::uint32_t wis::Animation::duration() const
{
  return (last_frame_ - first_frame_ + 1u) * static_cast<std::uint32_t>(timer_.delay() / 1'000'000u);
}
