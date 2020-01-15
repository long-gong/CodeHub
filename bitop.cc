#include "Timer.hpp"
#include <bitset>
#include <cstdint>
#include <random>
#include <vector>
#include <stdarg.h>
#include <stdio.h>
#include <cassert>

constexpr unsigned _MAX_DIM_ = 2;

template <typename T> std::vector<T> genUniformData(unsigned dim, ...) {
  static_assert(std::is_scalar<T>::value);

  assert(dim > 0 && dim <= _MAX_DIM_);
  va_list args;
  va_start(args, dim);

  std::vector<T> dims(dim, 0);
  size_t tot_n = 1;
  for (unsigned i = 0; i < dim; ++i) {
    int tmp = va_arg(args, int);
    dims[i] = tmp;
    tot_n *= tmp;
  }
  va_end(args);

#ifndef DISABLE_VERBOSE
  printf("Generating %d-D data\n", dim);
#endif

  std::vector<T> data;
  data.reserve(tot_n);
  std::mt19937_64 gen(
      (unsigned)std::chrono::system_clock::now().time_since_epoch().count());
  if constexpr (std::is_integral<T>::value) {
    std::uniform_int_distribution<T> dist;
    for (size_t i = 0; i < tot_n; ++i)
      data.emplace_back(dist(gen));
  } else {
    std::uniform_real_distribution<T> dist(-1, 1);
    for (size_t i = 0; i < tot_n; ++i)
      data.emplace_back(dist(gen));
  }

#ifndef DISABLE_VERBOSE
  printf("Done\n");
#endif
  assert(tot_n == data.size());
  return data;
}

std::vector<uint64_t> gen64_bitset(const std::vector<float> &x) {
  std::bitset<64> bits;
  std::vector<uint64_t> res(x.size() / 64);
  unsigned gid = 0;
  for (unsigned k = 0; k < res.size(); ++k) {
    for (unsigned i = 0; i < 64; ++i) {
      if (x[gid] > 0)
        bits[i] = true;
      else
        bits[i] = false;
      ++gid;
    }
    res[k] = bits.to_ullong();
  }

  return res;
}

constexpr uint64_t C = (1lu << 63lu);
std::vector<uint64_t> gen64(const std::vector<float> &x) {
  uint64_t bits;
  std::vector<uint64_t> res(x.size() / 64);
  unsigned gid = 0;
  for (unsigned k = 0; k < res.size(); ++k) {
    bits = 0;
    for (unsigned i = 0; i < 64; ++i) {
      if (x[gid] > 0) {
        bits |= C;
      }
      if (i < 63) bits >>= 1u;
      ++gid;
    }
    res[k] = bits;
  }

  return res;
}

int main() {
  int n = 64 * (1e4);

  auto x = genUniformData<float>(1, n);

  HighResolutionTimer timer;
  timer.restart();
  auto y1 = gen64_bitset(x);
  auto e1 = timer.elapsed();

  timer.restart();
  auto y2 = gen64(x);
  auto e2 = timer.elapsed();

  printf("bitset: %.2f\nraw: %.2f\n", e1, e2);

  FILE *fp = fopen("1.txt", "w");
  for (unsigned i = 0; i < y1.size(); ++i)
    fprintf(fp, "%lu\n", y1[i]);
  fclose(fp);

  fp = fopen("2.txt", "w");
  for (unsigned i = 0; i < y2.size(); ++i)
    fprintf(fp, "%lu\n", y2[i]);
  fclose(fp);
}