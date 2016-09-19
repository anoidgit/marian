#pragma once

#include <random>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdint.h>

#include "tensor.h"

namespace marian {

float xor128() {
    static uint64_t x = 123456789;
    static uint64_t y = 362436069;
    static uint64_t z = 521288629;
    static uint64_t w = 88675123;
    uint64_t t;

    t = (x ^ (x << 11)) % 1000;
    x = y; y = z; z = w;
    w = (w ^ (w >> 19) ^ t ^ (t >> 8)) % 1000;
    return 0.1 * ((w % 1000) / 1000.f) - 0.05;
}

// Use a constant seed for deterministic behaviour.
std::default_random_engine engine(42);

void zeros(Tensor t) {
  t.set(0.f);
}

void ones(Tensor t) {
  t.set(1.0f);
}

template <class Distribution>
void distribution(Tensor t, float a, float b) {
  //std::random_device device;
  //std::default_random_engine engine(device());
  Distribution dist(a, b);
  auto gen = std::bind(dist, engine);

  std::vector<float> vals(t.size());
  std::generate(begin(vals), end(vals), gen);

  t << vals;
}

std::function<void(Tensor)> normal(float mean = 0.0, float std = 0.05) {
  return [mean, std](Tensor t) {
    distribution<std::normal_distribution<float>>(t, mean, std);
  }; 
}

std::function<void(Tensor)> uniform(float a = -0.05, float b = 0.05) {
  return [a, b](Tensor t) {
    distribution<std::uniform_real_distribution<float>>(t, a, b);
  };  
}

void glorot_uniform(Tensor t) {
  float b = sqrtf( 6.0f / (t.shape()[0] + t.shape()[1]) );
  distribution<std::uniform_real_distribution<float>>(t, -b, b);
}

void xorshift(Tensor t) {
  std::vector<float> vals(t.size());
  for(auto&& v : vals)
    v = xor128();
  t << vals;
}

void glorot_normal(Tensor t) {
  float b = sqrtf( 2.0f / (t.shape()[0] + t.shape()[1]) );
  distribution<std::uniform_real_distribution<float>>(t, -b, b);
}

std::function<void(Tensor)> from_vector(const std::vector<float>& v) {
  return [v](Tensor t) {
    t << v;
  };
}
  
} // namespace marian
