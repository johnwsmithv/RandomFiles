#include "Sum.hpp"
#include <immintrin.h>
#include <algorithm> // std::clamp
#include <cmath> // std::log10
#include <vector>
#include <iostream>

#include <xsimd/xsimd.hpp>

float hsum256_ps(__m256 v) {
    __m128 vlow  = _mm256_castps256_ps128(v);          // low 128
    __m128 vhigh = _mm256_extractf128_ps(v, 1);       // high 128
    __m128 sum128 = _mm_add_ps(vlow, vhigh);          // add low + high
    sum128 = _mm_hadd_ps(sum128, sum128);             // horizontal add
    sum128 = _mm_hadd_ps(sum128, sum128);
    return _mm_cvtss_f32(sum128);                     // extract final sum
}

double hsum256_pd(__m256d v) {
    // Extract the high 128-bit lane
    __m128d vhigh = _mm256_extractf128_pd(v, 1); // upper 2 doubles
    // Extract the low 128-bit lane
    __m128d vlow  = _mm256_castpd256_pd128(v);   // lower 2 doubles
    // Add the two 128-bit halves
    __m128d sum128 = _mm_add_pd(vlow, vhigh);
    // Horizontal add the two doubles in sum128
    sum128 = _mm_hadd_pd(sum128, sum128);
    // Extract final scalar
    return _mm_cvtsd_f64(sum128);
}

double _sum_avx2(float* data, size_t dataSize) {
    __m256d vsum = _mm256_setzero_pd();
    size_t i = 0;
    for(; i + 8 <= dataSize; i += 8) {
        __m256 v = _mm256_loadu_ps(&data[i]);
        __m128 vlow  = _mm256_castps256_ps128(v);
        __m128 vhigh = _mm256_extractf128_ps(v, 1);
        __m256d sum0 = _mm256_cvtps_pd(vlow);   // convert first 4 to double
        __m256d sum1 = _mm256_cvtps_pd(vhigh);  // convert next 4 to double
        vsum = _mm256_add_pd(vsum, sum0);
        vsum = _mm256_add_pd(vsum, sum1);
    }

    double sum = hsum256_pd(vsum);

    for(; i < dataSize; i++) {
        sum += data[i];
    }

    return sum;
} 

/**
 * @brief Computes the sum of "data" using OpenMP and
 * AVX2 vector instructions
 * 
 * @param data The buffer which we are summing over
 * @param n The size of the buffer
 * @return double 
 */
double _sum_avx2_omp(float* data, size_t n) {
    double total = 0.0;

    #pragma omp parallel for reduction(+:total)
    for (size_t i = 0; i < n; i += 8) {
        __m256 v = _mm256_loadu_ps(&data[i]);

        __m128 vlow  = _mm256_castps256_ps128(v);
        __m128 vhigh = _mm256_extractf128_ps(v, 1);

        __m256d sum0 = _mm256_cvtps_pd(vlow);   // convert first 4 floats to double
        __m256d sum1 = _mm256_cvtps_pd(vhigh);  // convert next 4 floats to double

        // horizontal sum of this chunk
        __m128d hi0 = _mm256_extractf128_pd(sum0, 1);
        __m128d lo0 = _mm256_castpd256_pd128(sum0);
        
        __m128d hi1 = _mm256_extractf128_pd(sum1, 1);
        __m128d lo1 = _mm256_castpd256_pd128(sum1);
        
        auto sum2 = _mm_add_pd(lo0, hi0);
        double thread_sum = _mm_cvtsd_f64(_mm_hadd_pd(sum2, sum2));
        auto sum3 = _mm_add_pd(lo1, hi1);
        thread_sum += _mm_cvtsd_f64(_mm_hadd_pd(sum3, sum3));

        // accumulate to total (OpenMP handles reduction)
        total += thread_sum;
    }

    // handle tail elements
    size_t remainder = n % 8;
    for (size_t i = n - remainder; i < n; ++i)
        total += data[i];

    return total;
}

float* _remap(float *data, int dmin, int mmult, const int rows, const int cols) {
    double mean = 0.0;
    const size_t size = rows * cols;
    #pragma omp parallel for reduction(+:mean)
    for(size_t i = 0; i < size; i++) {
        mean += std::abs(data[i]);
    }
    mean /= size;

    const float C_L = 0.8f * mean;
    const float C_H = mmult * C_L;
    (void)C_H;
    const float slope = (255 - dmin) / std::log10(C_L);
    const float constant = dmin - (slope * std::log10(C_L));
    std::cout << "The slope is " << slope << " and the constant is " << constant << "\n";

    float *remappedData = new float[size];
    const float EPS = 1e-5f;

    #pragma omp parallel for collapse(2)
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            const auto remappedVal = slope * std::log10(std::max(std::abs(data[i * cols + j]), EPS)) + constant;
            if(remappedVal > 255) {
                remappedData[i * cols + j] = 255;
            } else if(remappedVal < 0) {
                remappedData[i * cols + j] = 0;
            } else {
                remappedData[i * cols + j] = remappedVal;
            }
        }
    }

    return remappedData;
}

float* remap_avx2_scalar_log10(const float* data, int dmin, int mmult, const int rows, const int cols) 
{
    double mean = 0.0;
    const size_t size = rows * cols;
    #pragma omp parallel for reduction(+:mean)
    for(size_t i = 0; i < size; i++) {
        mean += std::abs(data[i]);
    }
    mean /= size;

    const float C_L = 0.8f * mean;
    const float C_H = mmult * C_L;
    (void)C_H;
    const float slope = (255 - dmin) / std::log10(C_L);
    const float constant = dmin - (slope * std::log10(C_L));

    float *remappedData = new float[size];
    const float EPS = 1e-5f;

    size_t simdWidth = 8; // AVX2 floats
    size_t total = rows * cols;

    #pragma omp parallel for
    for (size_t i = 0; i < total; i += simdWidth) {
        __m256 v = _mm256_loadu_ps(data + i);

        // vectorized abs and max(EPS)
        __m256 vabs  = _mm256_max_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.f), v),
                                     _mm256_set1_ps(EPS));

        // extract elements to scalar for log10
        std::vector<float> temp(simdWidth, 0.f);
        _mm256_storeu_ps(temp.data(), vabs);  // store the processed values to temp array

        for (size_t j = 0; j < simdWidth; j++) {
            temp[j] = slope * std::log10(temp[j]) + constant;
        }

        // vectorize clamp
        __m256 vremap = _mm256_loadu_ps(temp.data());
        __m256 vclamp = _mm256_min_ps(_mm256_max_ps(vremap, _mm256_set1_ps(0.f)),
                                      _mm256_set1_ps(255.f));

        _mm256_storeu_ps(remappedData + i, vclamp);
    }

    // tail loop for remaining elements
    for (size_t i = (total / simdWidth) * simdWidth; i < total; i++) {
        float val = slope * std::log10(std::max(std::abs(data[i]), EPS)) + constant;
        remappedData[i] = std::clamp(val, 0.f, 255.f);
    }

    return remappedData;
}

namespace xs = xsimd;

float* remap_avx2_xsimd(const float* data, int dmin, int mmult, const int rows, const int cols) {
    double mean = 0.0;
    const size_t size = rows * cols;
    #pragma omp parallel for reduction(+:mean)
    for(size_t i = 0; i < size; i++) {
        mean += std::abs(data[i]);
    }
    mean /= size;

    const float C_L = 0.8f * mean;
    const float C_H = mmult * C_L;
    (void)C_H;
    const float slope = (255 - dmin) / std::log10(C_L);
    const float constant = dmin - (slope * std::log10(C_L));

    float *remappedData = new float[size];
    const float EPS = 1e-5f;

    size_t simdWidth = 8; // AVX2 floats
    size_t total = rows * cols;

    #pragma omp parallel for
    for(size_t i = 0; i < total; i += simdWidth) {
        auto v = xs::load_unaligned(&data[i]);
        v = (slope * xs::log10(xs::max(xs::abs(v), xs::batch(EPS))) + constant);
        v = xs::min(xs::max(v, xs::batch(0.f)), xs::batch(255.f));
        v.store_unaligned(&remappedData[i]);
    }

    return remappedData;
}