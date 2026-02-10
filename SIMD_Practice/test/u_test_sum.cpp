#include <gtest/gtest.h>
#include <random>
#include <xsimd/xsimd.hpp>

namespace xs = xsimd;

#include "Sum.hpp"
#include "Stopwatch.hpp"

TEST(Sum, Avx2Sum) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.f, 2.f);
    
    std::vector<float,  xs::aligned_allocator<float>> example(10000000, 0.f);
    #pragma omp parallel for
    for(auto& val : example) {
        val = dis(gen);
    }

    double goldSum = 0.f;
    Stopwatch watch;

    watch.start();
    #pragma omp parallel for reduction(+:goldSum)
    for (size_t i = 0; i < example.size(); i++) {
        goldSum += example[i];
    }
    watch.stop();
    std::cout << "The gold test took " << watch.elapsed() << "\n";

    watch.start();
    double testSum = _sum_avx2(example.data(), example.size());
    watch.stop();
    std::cout << "The test took " << watch.elapsed() << "\n";

    watch.start();
    double testSum2 = _sum_avx2_omp(example.data(), example.size());
    watch.stop();
    std::cout << "The test took " << watch.elapsed() << "\n";

    EXPECT_FLOAT_EQ(goldSum, testSum);
    EXPECT_FLOAT_EQ(goldSum, testSum2);
}

TEST(Sum, Avx2Remap) {
    const int rows = 10000;
    const int cols = 20000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.f, 2.f);
    
    std::vector<float> example(rows * cols, 0.f);
    #pragma omp parallel for
    for(size_t i = 0; i < rows * cols; i++) {
        example[i] = dis(gen);
    }

    constexpr int dmin = 60;
    constexpr int mmult = 40;

    Stopwatch watch;

    watch.start();
    float* nonVectorRemap = _remap(example.data(), dmin, mmult, rows, cols);
    watch.stop();
    std::cout << "The gold test took " << watch.elapsed() << "\n";

    watch.start();
    float* vectorRemap = remap_avx2_scalar_log10(example.data(), dmin, mmult, rows, cols);
    watch.stop();
    std::cout << "The vectorized test took " << watch.elapsed() << "\n";

    watch.start();
    float* xsimd_vectorRemap = remap_avx2_xsimd(example.data(), dmin, mmult, rows, cols);
    watch.stop();
    std::cout << "The XSIMD test took " << watch.elapsed() << "\n";

    // #pragma omp parallel for collapse(2)
    // for(size_t i = 0; i < rows; i++) {
    //     for(size_t j = 0; j < cols; j++) {
    //         const size_t idx = i * cols + j;

    //         EXPECT_FLOAT_EQ(nonVectorRemap[idx], vectorRemap[idx]);
    //         // EXPECT_FLOAT_EQ(nonVectorRemap[idx], xsimd_vectorRemap[idx]);
    //     }
    // }

    delete [] nonVectorRemap;
    delete [] vectorRemap;
    delete [] xsimd_vectorRemap;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}