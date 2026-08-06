#include "random_generator.h"


#include <algorithm>
#include <functional>
#include <limits>


void gdl::Random_int32_generator::init()
{
  init(std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::max());
}


void gdl::Random_int32_generator::init(std::uint32_t seed)
{
  init(std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::max(), seed);
}


void gdl::Random_int32_generator::init(std::int32_t a, std::int32_t b)
{
  std::random_device rd;
  std::array<std::uint32_t, std::mt19937::state_size> seed_data = {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq{std::begin(seed_data), std::end(seed_data)};
  engine_.seed(seq);
  distribution_ = std::uniform_int_distribution<std::int32_t>{a, b};
}


void gdl::Random_int32_generator::init(std::int32_t a, std::int32_t b, std::uint32_t seed)
{
  engine_.seed(seed);
  engine_.discard(std::mt19937::state_size);
  distribution_ = std::uniform_int_distribution<std::int32_t>{a, b};
}


void gdl::Random_uint32_generator::init()
{
  init(std::numeric_limits<std::uint32_t>::min(), std::numeric_limits<std::uint32_t>::max());
}


void gdl::Random_uint32_generator::init(std::uint32_t seed)
{
  init(std::numeric_limits<std::uint32_t>::min(), std::numeric_limits<std::uint32_t>::max(), seed);
}


void gdl::Random_uint32_generator::init(std::uint32_t a, std::uint32_t b)
{
  std::random_device rd;
  std::array<std::uint32_t, std::mt19937::state_size> seed_data = {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq{std::begin(seed_data), std::end(seed_data)};
  engine_.seed(seq);
  distribution_ = std::uniform_int_distribution<std::uint32_t>{a, b};
}


void gdl::Random_uint32_generator::init(std::uint32_t a, std::uint32_t b, std::uint32_t seed)
{
  engine_.seed(seed);
  engine_.discard(std::mt19937::state_size);
  distribution_ = std::uniform_int_distribution<std::uint32_t>{a, b};
}


void gdl::Random_float_generator::init(float a, float b)
{
  std::random_device rd;
  std::array<std::uint32_t, std::mt19937::state_size> seed_data = {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq{std::begin(seed_data), std::end(seed_data)};
  engine_.seed(seq);
  distribution_ = std::uniform_real_distribution<float>{a, b};
}


void gdl::Random_float_generator::init(float a, float b, std::uint32_t seed)
{
  engine_.seed(seed);
  engine_.discard(std::mt19937::state_size);
  distribution_ = std::uniform_real_distribution<float>{a, b};
}
