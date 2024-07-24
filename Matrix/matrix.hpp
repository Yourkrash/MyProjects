#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 private:
  std::vector<std::vector<T>> matrix_;

 public:
  Matrix() : matrix_(std::vector<std::vector<T>>(N, std::vector<T>(M, T()))){};
  Matrix(std::vector<std::vector<T>> matrix) : matrix_(matrix) {}
  Matrix(const Matrix& matrix) { matrix_ = matrix.matrix_; }
  std::vector<std::vector<T>>& Data() { return matrix_; }
  const std::vector<std::vector<T>>& Data() const { return matrix_; }
  Matrix(T element)
      : matrix_(std::vector<std::vector<T>>(N, std::vector<T>(M, element))) {}
  T& operator()(size_t itr, size_t jtr) { return matrix_[itr][jtr]; }
  const T& operator()(size_t itr, size_t jtr) const {
    return matrix_[itr][jtr];
  }
  Matrix& operator+=(const Matrix<N, M, T>& matrix) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < M; ++jtr) {
        matrix_[itr][jtr] += matrix(itr, jtr);
      }
    }
    return *this;
  }
  Matrix& operator-=(const Matrix<N, M, T>& matrix) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < M; ++jtr) {
        matrix_[itr][jtr] -= matrix(itr, jtr);
      }
    }
    return *this;
  }
  Matrix& operator*=(const T& multiply) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < M; ++jtr) {
        matrix_[itr][jtr] *= multiply;
      }
    }
    return *this;
  }
  Matrix<M, N, T> Transposed() {
    Matrix<M, N, T> transpos;
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < M; ++jtr) {
        transpos(jtr, itr) = matrix_[itr][jtr];
      }
    }
    return transpos;
  }
  bool operator==(const Matrix<N, M, T>& matrix) {
    return matrix_ == matrix.matrix_;
  }
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator+(const Matrix<N, M, T>& matrix1,
                          const Matrix<N, M, T>& matrix2) {
  Matrix<N, M, T> sum = matrix1;
  sum += matrix2;
  return sum;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator-(const Matrix<N, M, T>& matrix1,
                          const Matrix<N, M, T>& matrix2) {
  Matrix<N, M, T> sum = matrix1;
  sum -= matrix2;
  return sum;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator*(const Matrix<N, M, T>& matrix1, const T& multiply) {
  Matrix<N, M, T> sum = matrix1;
  sum *= multiply;
  return sum;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator*(const T& multiply, const Matrix<N, M, T>& matrix1) {
  Matrix<N, M, T> sum = matrix1;
  sum *= multiply;
  return sum;
}

template <size_t N, size_t M, size_t K, typename T>
Matrix<N, K, T> operator*(const Matrix<N, M, T>& matrix1,
                          const Matrix<M, K, T>& matrix2) {
  Matrix<N, K, T> multiply;
  for (size_t itr = 0; itr < N; ++itr) {
    for (size_t jtr = 0; jtr < K; ++jtr) {
      multiply(itr, jtr) = matrix1(itr, 0) * matrix2(0, jtr);
      for (size_t k = 1; k < M; ++k) {
        multiply(itr, jtr) += matrix1(itr, k) * matrix2(k, jtr);
      }
    }
  }
  return multiply;
}

template <size_t N, typename T>
class Matrix<N, N, T> {
 private:
  std::vector<std::vector<T>> matrix_;

 public:
  Matrix() : matrix_(std::vector<std::vector<T>>(N, std::vector<T>(N, T()))){};
  Matrix(std::vector<std::vector<T>> matrix) : matrix_(matrix) {}
  Matrix(const Matrix& matrix) { matrix_ = matrix.matrix_; }
  std::vector<std::vector<T>>& Data() { return matrix_; }
  const std::vector<std::vector<T>>& Data() const { return matrix_; }
  Matrix(T element)
      : matrix_(std::vector<std::vector<T>>(N, std::vector<T>(N, element))) {}
  T& operator()(size_t itr, size_t jtr) { return matrix_[itr][jtr]; }
  const T& operator()(size_t itr, size_t jtr) const {
    return matrix_[itr][jtr];
  }
  Matrix operator+=(const Matrix<N, N, T>& matrix) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < N; ++jtr) {
        matrix_[itr][jtr] += matrix(itr, jtr);
      }
    }
    return *this;
  }
  Matrix operator-=(const Matrix<N, N, T>& matrix) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < N; ++jtr) {
        matrix_[itr][jtr] -= matrix(itr, jtr);
      }
    }
    return *this;
  }
  Matrix operator*=(const T& multiply) {
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < N; ++jtr) {
        matrix_[itr][jtr] *= multiply;
      }
    }
    return *this;
  }
  Matrix<N, N, T> Transposed() {
    Matrix<N, N, T> transpos;
    for (size_t itr = 0; itr < N; ++itr) {
      for (size_t jtr = 0; jtr < N; ++jtr) {
        transpos(jtr, itr) = matrix_[itr][jtr];
      }
    }
    return transpos;
  }
  bool operator==(const Matrix<N, N, T>& matrix) {
    return matrix_ == matrix.matrix_;
  }
  T Trace() {
    T trace = matrix_[0][0];
    for (size_t itr = 1; itr < N; ++itr) {
      trace += matrix_[itr][itr];
    }
    return trace;
  }
};