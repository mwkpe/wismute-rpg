#ifndef WIS_CORE_TIMER_H
#define WIS_CORE_TIMER_H


#include <cstdint>
#include <algorithm>


namespace wis {


class Nano_timer
{
public:
  Nano_timer() = default;
  explicit Nano_timer(std::uint64_t delay) : delay_{delay} {}

  void reset() { current_ = 0u; }
  void restart() {
    if (current_ <= delay_) {
      current_ = 0u;
    }
    else {
      current_ = current_ - delay_;
    }
  }

  void set_elapsed() { current_ = delay_; }
  void set_delay(std::uint64_t delay) { delay_ = delay; }

  void update(std::uint64_t elapsed) {
    if (current_ < delay_) {
      current_ += elapsed;
    }
  }

  [[nodiscard]] bool is_elapsed() const { return current_ >= delay_; }
  [[nodiscard]] std::uint64_t delay() const { return delay_; }
  [[nodiscard]] std::uint64_t current() const { return current_; }

private:
  std::uint64_t current_ = 0u;
  std::uint64_t delay_ = 0u;
};


}  // namespace wis


#endif  // WIS_CORE_TIMER_H
