#pragma once

#include <cstddef>

double _sum_avx2(float* __restrict__ data, size_t dataSize);
double _sum_avx2_omp(float* __restrict__ data, size_t dataSize);
double _sum_avx2_xsimd_omp(float* __restrict__ data, size_t dataSize);

float* _remap(float *data, int dmin, int mmult, const int rows, const int cols);
float* remap_avx2_scalar_log10(const float* data, int dmin, int mmult, const int rows, const int cols);
float* remap_avx2_xsimd(const float* data, int dmin, int mmult, const int rows, const int cols);