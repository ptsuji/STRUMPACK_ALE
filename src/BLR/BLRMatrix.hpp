/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The
 * Regents of the University of California, through Lawrence Berkeley
 * National Laboratory (subject to receipt of any required approvals
 * from the U.S. Dept. of Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE. This software is owned by the U.S. Department of Energy. As
 * such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * Developers: Pieter Ghysels, Francois-Henry Rouet, Xiaoye S. Li.
 *             (Lawrence Berkeley National Lab, Computational Research
 *             Division).
 *
 */
/*! \file BLRMatrix.hpp
 * \brief Contains the BLRMatrix class.
 */
#ifndef BLR_MATRIX_HPP
#define BLR_MATRIX_HPP

#include <cassert>
#include <memory>
#include <functional>
#include <algorithm>

#include "BLROptions.hpp"
#include "BLRTileBLAS.hpp" // TODO remove

namespace strumpack {
  namespace BLR {

    // forward declarations
    template<typename scalar> class BLRTile;


    template<typename T>
    using extract_t = std::function<void(const std::vector<std::size_t>&,
                                         const std::vector<std::size_t>&,
                                         DenseMatrix<T>&)>;
    using adm_t = DenseMatrix<bool>;

    template<typename scalar_t> class BLRMatrix {
      using DenseM_t = DenseMatrix<scalar_t>;
      using DenseMW_t = DenseMatrixWrapper<scalar_t>;
      using Opts_t = BLROptions<scalar_t>;
      // using elem_t = std::function<scalar_t(std::size_t,std::size_t)>;

    public:
      BLRMatrix() = default;

      BLRMatrix(DenseM_t& A,
                const std::vector<std::size_t>& rowtiles,
                const std::vector<std::size_t>& coltiles,
                const Opts_t& opts);

      // implement and test, separate from factorization
      // BLRMatrix(std::size_t n, const std::vector<std::size_t>& tiles,
      //           const adm_t& admissible, const elem_t& Aelem,
      //           std::vector<int>& piv, const Opts_t& opts);

      BLRMatrix(DenseM_t& A, const std::vector<std::size_t>& tiles,
                const adm_t& admissible, std::vector<int>& piv,
                const Opts_t& opts);

      //BLRMatrix(DenseM_t &A11, BLRMatrix<scalar_t>& B11, const Opts_t& opts);

      std::size_t rows() const { return m_; }
      std::size_t cols() const { return n_; }

      std::size_t memory() const;
      std::size_t nonzeros() const;
      std::size_t maximum_rank() const;

      DenseM_t dense() const;
      void dense(DenseM_t& A) const;

      void draw(std::ostream& of, std::size_t roff, std::size_t coff) const;

      void print(const std::string& name) const;

      void clear();

      scalar_t operator()(std::size_t i, std::size_t j) const;
      DenseM_t extract(const std::vector<std::size_t>& I,
                       const std::vector<std::size_t>& J) const;

      std::size_t rowblocks() const { return nbrows_; }
      std::size_t colblocks() const { return nbcols_; }
      std::size_t tilerows(std::size_t i) const { return roff_[i+1] - roff_[i]; }
      std::size_t tilecols(std::size_t j) const { return coff_[j+1] - coff_[j]; }
      std::size_t tileroff(std::size_t i) const { return roff_[i]; }
      std::size_t tilecoff(std::size_t j) const { return coff_[j]; }

      BLRTile<scalar_t>& tile(std::size_t i, std::size_t j);
      const BLRTile<scalar_t>& tile(std::size_t i, std::size_t j) const;
      std::unique_ptr<BLRTile<scalar_t>>& block(std::size_t i, std::size_t j);
      DenseMW_t tile(DenseM_t& A, std::size_t i, std::size_t j) const;

      static void construct_and_partial_factor
      (DenseMatrix<scalar_t>& A11, DenseMatrix<scalar_t>& A12,
       DenseMatrix<scalar_t>& A21, DenseMatrix<scalar_t>& A22,
       BLRMatrix<scalar_t>& B11, std::vector<int>& piv,
       BLRMatrix<scalar_t>& B12, BLRMatrix<scalar_t>& B21,
       const std::vector<std::size_t>& tiles1,
       const std::vector<std::size_t>& tiles2,
       const adm_t& admissible, const BLROptions<scalar_t>& opts);

      void LUAR_B11
      (std::size_t i, std::size_t j,
       std::size_t kmax, DenseMatrix<scalar_t>&A11, const BLROptions<scalar_t>& opts, int* B);

      void LUAR_B12
      (std::size_t i, std::size_t j,
       std::size_t kmax, BLRMatrix<scalar_t>& B11, DenseMatrix<scalar_t>&A12, const BLROptions<scalar_t>& opts, int* B);

      void LUAR_B21
      (std::size_t i, std::size_t j,
       std::size_t kmax, BLRMatrix<scalar_t>& B11, DenseMatrix<scalar_t>&A21, const BLROptions<scalar_t>& opts, int* B);

      static void construct_and_partial_factor
      (std::size_t n1, std::size_t n2,
       const extract_t<scalar_t>& A11, const extract_t<scalar_t>& A12,
       const extract_t<scalar_t>& A21, const extract_t<scalar_t>& A22,
       BLRMatrix<scalar_t>& B11, std::vector<int>& piv,
       BLRMatrix<scalar_t>& B12, BLRMatrix<scalar_t>& B21,
       BLRMatrix<scalar_t>& B22,
       const std::vector<std::size_t>& tiles1,
       const std::vector<std::size_t>& tiles2,
       const adm_t& admissible, const BLROptions<scalar_t>& opts);

      static void trsmLNU_gemm
      (const BLRMatrix<scalar_t>& F1, const BLRMatrix<scalar_t>& F2,
       DenseMatrix<scalar_t>& B1, DenseMatrix<scalar_t>& B2, int task_depth);

      static void gemm_trsmUNN
      (const BLRMatrix<scalar_t>& F1, const BLRMatrix<scalar_t>& F2,
       DenseMatrix<scalar_t>& B1, DenseMatrix<scalar_t>& B2, int task_depth);

    private:
      std::size_t m_ = 0, n_ = 0, nbrows_ = 0, nbcols_ = 0;
      std::vector<std::size_t> roff_, coff_;
      std::vector<std::unique_ptr<BLRTile<scalar_t>>> blocks_;

      BLRMatrix(std::size_t m, const std::vector<std::size_t>& rowtiles,
                std::size_t n, const std::vector<std::size_t>& coltiles);

      BLRMatrix(std::size_t m, std::size_t n);

      void create_dense_tile(std::size_t i, std::size_t j, DenseM_t& A);
      void create_dense_tile
      (std::size_t i, std::size_t j, const extract_t<scalar_t>& Aelem);
      void create_dense_tile_left_looking
      (std::size_t i, std::size_t j, const extract_t<scalar_t>& Aelem);
      void create_dense_tile_left_looking
      (std::size_t i, std::size_t j, std::size_t k,
       const extract_t<scalar_t>& Aelem, const BLRMatrix<scalar_t>& B21,
       const BLRMatrix<scalar_t>& B12);
      void create_LR_tile
      (std::size_t i, std::size_t j, DenseM_t& A, const Opts_t& opts);
      void create_LR_tile_left_looking
      (std::size_t i, std::size_t j, const extract_t<scalar_t>& Aelem,
       const Opts_t& opts);
      void create_LR_tile_left_looking
      (std::size_t i, std::size_t j, std::size_t k,
       const extract_t<scalar_t>& Aelem, const BLRMatrix<scalar_t>& B21,
       const BLRMatrix<scalar_t>& B12, const Opts_t& opts);

      template<typename T> friend void draw
      (const BLRMatrix<T>& H, const std::string& name);
    };

    template<typename scalar_t> void
    LUAR_B22(std::size_t i, std::size_t j, std::size_t kmax, BLRMatrix<scalar_t>& B12, 
             BLRMatrix<scalar_t>& B21, DenseMatrix<scalar_t>&A22, const BLROptions<scalar_t>& opts, int* B);

    template<typename scalar_t> void
    trsm(Side s, UpLo ul, Trans ta, Diag d, scalar_t alpha,
         const BLRMatrix<scalar_t>& a, DenseMatrix<scalar_t>& b,
         int task_depth);

    template<typename scalar_t> void
    trsv(UpLo ul, Trans ta, Diag d, const BLRMatrix<scalar_t>& a,
         DenseMatrix<scalar_t>& b, int task_depth);

    template<typename scalar_t> void
    gemv(Trans ta, scalar_t alpha, const BLRMatrix<scalar_t>& a,
         const DenseMatrix<scalar_t>& x, scalar_t beta,
         DenseMatrix<scalar_t>& y, int task_depth);

    template<typename scalar_t> void
    gemm(Trans ta, Trans tb, scalar_t alpha, const BLRMatrix<scalar_t>& a,
         const BLRMatrix<scalar_t>& b, scalar_t beta,
         DenseMatrix<scalar_t>& c, int task_depth);

    template<typename scalar_t> void
    gemm(Trans ta, Trans tb, scalar_t alpha, const BLRMatrix<scalar_t>& A,
         const DenseMatrix<scalar_t>& B, scalar_t beta,
         DenseMatrix<scalar_t>& C, int task_depth);

    template<typename scalar_t>
    void draw(const BLRMatrix<scalar_t>& B, const std::string& name) {
      std::ofstream of("plot" + name + ".gnuplot");
      of << "set terminal pdf enhanced color size 5,4" << std::endl;
      of << "set output '" << name << ".pdf'" << std::endl;
      B.draw(of, 0, 0);
      of << "set xrange [0:" << B.cols() << "]" << std::endl;
      of << "set yrange [" << B.rows() << ":0]" << std::endl;
      of << "plot x lt -1 notitle" << std::endl;
      of.close();
    }

  } // end namespace BLR
} // end namespace strumpack

#endif // BLR_MATRIX_HPP
