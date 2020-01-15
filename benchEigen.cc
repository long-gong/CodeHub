#include "Timer.hpp"
#include <cassert>
#include <chrono>
#include <eigen3/Eigen/Dense>
#include <random>
#include <stdarg.h>
#include <stdio.h>
#include <type_traits>

using namespace Eigen;

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
    std::uniform_real_distribution<T> dist;
    for (size_t i = 0; i < tot_n; ++i)
      data.emplace_back(dist(gen));
  }

#ifndef DISABLE_VERBOSE
  printf("Done\n");
#endif
  assert(tot_n == data.size());
  return data;
}

template <typename T>
MatrixXd flatVecToEigenMat(const std::vector<T> &data, unsigned m, unsigned n) {
  MatrixXd mat(m, n);
  for (unsigned i = 0; i < m; ++i)
    for (unsigned j = 0; j < n; ++j)
      mat(i, j) = data[i * n + j];
  return mat;
}

template <typename T>
std::vector<std::vector<T>> unflat(const std::vector<T> &data, unsigned m,
                                   unsigned n) {
  std::vector<std::vector<T>> mat;
  mat.resize(m);
  for (unsigned i = 0; i < m; ++i) {
    mat[i].resize(n);
    for (unsigned j = 0; j < n; ++j)
      mat[i][j] = data[i * n + j];
  }

  return mat;
}

VectorXd eigenMutiply(const MatrixXd &M, const VectorXd &x) {
  auto y = M * x;
  return y;
}

VectorXd eigenManualMutiply(const MatrixXd &M, const VectorXd &x) {
  VectorXd y(M.rows());
  for (unsigned i = 0; i < M.rows(); ++i) {
    y(i) = 0;
    for (unsigned j = 0; j < M.cols(); ++j) {
      y(i) += M(i, j) * x(j);
    }
  }
  return y;
}

template <typename T>
std::vector<T> twoDimVecMutiply(const std::vector<std::vector<T>> &M,
                                const std::vector<T> &x) {
  std::vector<T> y(M.size());
  for (unsigned i = 0; i < M.size(); ++i) {
    y[i] = 0;
    for (unsigned j = 0; j < M[i].size(); ++j) {
      y[i] += M[i][j] * x[j];
    }
  }
  return y;
}

template <typename T>
std::vector<T> flatVecMutiply(const std::vector<T> &M,
                              const std::vector<T> &x) {

  unsigned n = x.size();
  unsigned m = M.size() / n;
  std::vector<T> y(m);
  for (unsigned i = 0; i < m; ++i) {
    y[i] = 0;
    for (unsigned j = 0; j < x.size(); ++j) {
      y[i] += M[i * n + j] * x[j];
    }
  }
  return y;
}

// int main() {
//   unsigned n = 1e4;
//   VectorXd x(n);
//   std::vector<double> y(n);

//   std::mt19937_64 gen(
//       (unsigned)std::chrono::system_clock::now().time_since_epoch().count());

//   std::uniform_real_distribution<double> dist(0.0f, 1.0f);
//   for (unsigned i = 0; i < n; ++i) {
//     auto tmp = dist(gen);
//     x(i) = tmp;
//     y[i] = tmp;
//   }

//   double sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

//   HighResolutionTimer timer;

//   timer.restart();
//   for (unsigned i = 0; i < n; ++i)
//     sum2 += x(i);
//   auto e2 = timer.elapsed();

//   auto data = x.data();
//   timer.restart();
//   for (unsigned i = 0; i < n; ++i)
//     sum0 += data[i];
//   auto e0 = timer.elapsed();

//   timer.restart();
//   for (unsigned i = 0; i < n; ++i)
//     sum1 += y[i];
//   auto e1 = timer.elapsed();

//   auto data2 = &y[0];
//   timer.restart();
//   for (unsigned i = 0; i < n; ++i)
//     sum3 += data2[i];
//   auto e3 = timer.elapsed();

//   printf("eigen: %.2f\neigen (raw): %.2f\nstd: %.2f\nstd (raw): %.2f\n\nsum: "
//          "%.6f, %.6f, %.6f, %.6f\n\n",
//          e2, e0, e1, e3, sum2, sum0, sum1, sum3);
// }
int main() {
  unsigned m = 1e3;
  unsigned n = 400;
  auto flatM = genUniformData<float>(2, m, n);

  printf("Convert to Eigen matrix ...\n");
  auto mat = flatVecToEigenMat<float>(flatM, m, n);

  printf("Convert to 2d vector ...\n");
  auto twoDimVec = unflat<float>(flatM, m, n);

  printf("Generate 1d vector ...\n");
  auto x = genUniformData<float>(1, n);

  VectorXd eigenV(n);
  for (unsigned i = 0; i < n; ++i)
    eigenV(i) = x[i];

  printf("Start benchmarking ...\n");
  printf("size(M) = [%ld, %ld], size(x) = %lu\n", mat.rows(), mat.cols(),
         x.size());
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

  printf("eigenMutiply: %.2f\neigenManualMutiply: %.2f\ntwoDimVecMutiply: "
         "%.2f\nflatVecMutiply: %.2f\n\n",
         e1, e2, e3, e4);

  FILE *fp = fopen("1.txt", "w");
  for (unsigned i = 0; i < y1.size(); ++i)
    fprintf(fp, "%.6f\n", y1[i]);
  fclose(fp);

  fp = fopen("2.txt", "w");
  for (unsigned i = 0; i < y2.size(); ++i)
    fprintf(fp, "%.6f\n", y2[i]);
  fclose(fp);

  fp = fopen("3.txt", "w");
  for (unsigned i = 0; i < y3.size(); ++i)
    fprintf(fp, "%.6f\n", y3[i]);
  fclose(fp);

  fp = fopen("4.txt", "w");
  for (unsigned i = 0; i < y4.size(); ++i)
    fprintf(fp, "%.6f\n", y4[i]);
  fclose(fp);

  return 0;
}