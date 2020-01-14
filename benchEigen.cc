#include <eigen3/Eigen/Dense>
#include <random>
#include <chrono>
#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <type_traits>

using namespace Eigen;

constexpr unsigned _MAX_DIM_ = 2;

template <typename T>
std::vector<T>  genUniformData(unsigned dim, ...) {
    static_assert(std::is_scalar_v<T>);
    
    assert (dim > 0 && dim <= _MAX_DIM_);
    va_list args;
    va_start(args, dim);

    std::vector<T> dims(dim, 0);
    size_t tot_n = 1;
    for (unsigned i = 0;i < dim;++ i)  {
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
    std::mt19937_64 gen((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
    if (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist;
        for (size_t i = 0;i < tot_n;++ i) data.emplace_back(dist(gen));
    } else {
        std::uniform_real_distribution<T> dist;
        for (size_t i = 0;i < tot_n;++ i) data.emplace_back(dist(gen));
    }
    return data;
}

template <typename T> 
MatrixXd flatVecToEigenMat(const std::vector<T>& data, unsigned m, unsigned n) {
    MatrixXd mat(m, n);
    for (unsigned i = 0;i < m;++ i)
    for (unsigned j = 0;j < n;++ j) 
    mat(i, j) = data[i * m + j];
    return mat;
}

template <typename T>
std::vector<std::vector<T>> unflat(const std::vector<T>& data, unsigned m, unsigned n) {
    std::vector<std::vector<T>> mat;
    mat.resize(m);
        for (unsigned i = 0;i < m;++ i) {
            mat[i].resize(n);
                for (unsigned j = 0;j < n;++ j) 
    mat[i][j] = data[i * m + j];
        }

        return mat;

}

VecorXd eigenMutiply(const MatrixXd& M, const VectorXd& x) {
    auto y = M * x;
    return y;
}

VectorXd eigenManualMutiply(const MatrixXd& M, const VectorXd& x) {
    VectorXd y(M.rows());
        for (unsigned i = 0;i < M.rows();++ i) {
        y(i) = 0;
        for (unsigned j = 0;j < M[i].cols();++ j) {
            y(i) += M(i,j) * x(j);
        }
    }
    return y;
}


template <typename T>
std::vector<T> twoDimVecMutiply(const std::vector<std::vector<T>>& M, const std::vector<T>& x) {
    std::vector<T> y(M.size());
    for (unsigned i = 0;i < M.size();++ i) {
        y[i] = 0;
        for (unsigned j = 0;j < M[i].size();++ j) {
            y[i] += M[i][j] * x[j];
        }
    }
    return y;
}

template <typename T>
std::vector<T> flatVecMutiply(const std::vector<T>& M, const std::vector<T>& x) {
    unsigned m = M.size() / x.size();
    std::vector<T> y(m);
    for (unsigned i = 0;i < m;++ i) {
        y[i] = 0;
        for (unsigned j = 0;j < x.size();++ j) {
            y[i] += M[I * m  + j] * x[j];
        }
    }
    return y;
}


int main() {
    unsigned m = 1e3;
    unsigned n = 400;
    auto flatM = genUniformData<float>(2, m, n);
    auto mat = flatVecToEigenMat<float>(flatM, m, n);
    auto twoDimVec = unflat<float>(flatM, m , n);

    auto x = genUniformData<float>(1, n);
    VectorXd eigenV(n);
    for (unsigned i = 0;i < n;++ i) eigenV(i) = x[i];

    HighResolutionTimer timer;
    timer.restart();
    auto y1 = eigenMutiply(mat, eigenV);
    auto e1 = timer.elapsed();

    timer.restart();
    auto y2 = eigenManualMutiply(mat, eigenV);
    auto e2 = timer.elapsed();

    timer.restart();
    auto y3 = twoDimVecMutiply(twoDimVec, x);
    auto e3 = timer.elapsed();

    timer.restart();
    auto y4 = flatVecMutiply(flatM, x);
    auto e4 = timer.elapsed();

    printf("eigenMutiply: %.2f\neigenManualMutiply: %.2f\ntwoDimVecMutiply: %.2f\nflatVecMutiply: %.2f\n\n", e1, e2, e3, e4);

    FILE *fp = fopen("1.txt", "w");
    for (unsigned i = 0;i < y1.size();++ i) fprintf(fp, "%.6f\n", y1[i]);
    fclose(fp);

    fp = fopen("2.txt", "w");
    for (unsigned i = 0;i < y2.size();++ i) fprintf(fp, "%.6f\n", y2[i]);
    fclose(fp);

    fp = fopen("3.txt", "w");
    for (unsigned i = 0;i < y3.size();++ i) fprintf(fp, "%.6f\n", y3[i]);
    fclose(fp);

    fp = fopen("4.txt", "w");
    for (unsigned i = 0;i < y4.size();++ i) fprintf(fp, "%.6f\n", y4[i]);
    fclose(fp);

    return 0;

}