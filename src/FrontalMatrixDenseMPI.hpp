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
 * five (5) year renewals, the U.S. Government igs granted for itself
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
#ifndef FRONTAL_MATRIX_DENSE_MPI_HPP
#define FRONTAL_MATRIX_DENSE_MPI_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "CompressedSparseMatrix.hpp"
#include "DistributedMatrix.hpp"
#include "MatrixReordering.hpp"
#include "TaskTimer.hpp"
#include "MPI_wrapper.hpp"
#include "FrontalMatrixMPI.hpp"

namespace strumpack {

  template<typename scalar_t,typename integer_t> class FrontalMatrixHSSMPI;

  template<typename scalar_t,typename integer_t>
  class FrontalMatrixDenseMPI : public FrontalMatrixMPI<scalar_t,integer_t> {
    using DistM_t = DistributedMatrix<scalar_t>;
    using DistMW_t = DistributedMatrixWrapper<scalar_t>;
    using FDMPI_t = FrontalMatrixDenseMPI<scalar_t,integer_t>;
    using FD_t = FrontalMatrixDense<scalar_t,integer_t>;
    using F_t = FrontalMatrix<scalar_t,integer_t>;
    using ExtAdd = ExtendAdd<scalar_t,integer_t>;
    template<typename _scalar_t,typename _integer_t> friend class ExtendAdd;

  public:
    FrontalMatrixDenseMPI(CompressedSparseMatrix<scalar_t,integer_t>* _A,
                          integer_t _sep, integer_t _sep_begin,
                          integer_t _sep_end, integer_t _dim_upd,
                          integer_t* _upd, MPI_Comm _front_comm,
                          int _total_procs);
    FrontalMatrixDenseMPI(const FrontalMatrixDenseMPI&) = delete;
    FrontalMatrixDenseMPI& operator=(FrontalMatrixDenseMPI const&) = delete;
    ~FrontalMatrixDenseMPI();
    void release_work_memory();
    void build_front();
    void partial_factorization();

    void extend_add();

    void sample_CB(const SPOptions<scalar_t>& opts,
                   const DistM_t& R, DistM_t& Sr, DistM_t& Sc,
                   F_t* pa) const;

    void multifrontal_factorization(const SPOptions<scalar_t>& opts,
                                    int etree_level=0, int task_depth=0);
    void forward_multifrontal_solve(scalar_t* b_loc, DistM_t* b_dist,
                                    scalar_t* wmem, int etree_level=0,
                                    int task_depth=0);
    void backward_multifrontal_solve(scalar_t* y_loc, DistM_t* b_dist,
                                     scalar_t* wmem, int etree_level=0,
                                     int task_depth=0);

    void extract_CB_sub_matrix_2d(const std::vector<std::size_t>& I,
                                  const std::vector<std::size_t>& J,
                                  DistM_t& B) const;

    long long factor_nonzeros(int task_depth=0) const;
    std::string type() const { return "FrontalMatrixDenseMPI"; }

  private:
    DistM_t F11, F12, F21, F22;
    std::vector<int> piv;
  };

  template<typename scalar_t,typename integer_t>
  FrontalMatrixDenseMPI<scalar_t,integer_t>::FrontalMatrixDenseMPI
  (CompressedSparseMatrix<scalar_t,integer_t>* _A,
   integer_t _sep, integer_t _sep_begin, integer_t _sep_end,
   integer_t _dim_upd, integer_t* _upd,
   MPI_Comm _front_comm, int _total_procs)
    : FrontalMatrixMPI<scalar_t,integer_t>
    (_A, _sep, _sep_begin, _sep_end, _dim_upd, _upd,
     _front_comm, _total_procs) {}

  template<typename scalar_t,typename integer_t>
  FrontalMatrixDenseMPI<scalar_t,integer_t>::~FrontalMatrixDenseMPI() {
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::release_work_memory() {
    F22.clear(); // remove the update block
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::extend_add() {
    if (!this->lchild && !this->rchild) return;
    auto P = mpi_nprocs(this->front_comm);
    std::vector<std::vector<scalar_t>> sbuf(P);
    for (auto ch : {this->lchild, this->rchild}) {
      if (ch && mpi_rank(this->front_comm) == 0) {
        STRUMPACK_FLOPS(static_cast<long long int>(ch->dim_upd)*ch->dim_upd);
      }
      if (FDMPI_t* ch_mpi = dynamic_cast<FDMPI_t*>(ch)) {
        ExtAdd::extend_add_copy_to_buffers
          (ch_mpi->F22, F11, F12, F21, F22, sbuf, this,
           ch_mpi->upd_to_parent(this));
      } else if (FD_t* ch_seq = dynamic_cast<FD_t*>(ch)) {
        if (mpi_rank(this->front_comm) == this->child_master(ch))
          ExtAdd::extend_add_seq_copy_to_buffers
            (ch_seq->F22, F11, F12, F21, F22, sbuf, this, ch_seq);
      }
    }
    scalar_t *rbuf = nullptr, **pbuf = nullptr;
    all_to_all_v(sbuf, rbuf, pbuf, this->front_comm);
    for (auto ch : {this->lchild, this->rchild}) {
      if (FDMPI_t* ch_mpi = dynamic_cast<FDMPI_t*>(ch)) {
        ExtAdd::extend_add_copy_from_buffers
          (F11, F12, F21, F22, pbuf+this->child_master(ch),
           this, ch_mpi);
      } else if (FD_t* ch_seq = dynamic_cast<FD_t*>(ch)) {
        ExtAdd::extend_add_seq_copy_from_buffers
          (F11, F12, F21, F22, pbuf[this->child_master(ch_seq)],
           this, ch_seq);
      }
    }
    delete[] pbuf;
    delete[] rbuf;
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::build_front() {
    if (this->dim_sep) {
      F11 = DistM_t(this->ctxt, this->dim_sep, this->dim_sep);
      using ExtractFront = ExtractFront<scalar_t,integer_t>;
      ExtractFront::extract_F11(F11, this->A, this->sep_begin, this->dim_sep);
      if (this->dim_upd) {
        F12 = DistM_t(this->ctxt, this->dim_sep, this->dim_upd);
        ExtractFront::extract_F12
          (F12, this->A, this->sep_begin, this->sep_end,
           this->dim_upd, this->upd);
        F21 = DistM_t(this->ctxt, this->dim_upd, this->dim_sep);
        ExtractFront::extract_F21
          (F21, this->A, this->sep_end, this->sep_begin,
           this->dim_upd, this->upd);
      }
    }
    if (this->dim_upd) {
      F22 = DistM_t(this->ctxt, this->dim_upd, this->dim_upd);
      F22.zero();
    }
    extend_add();
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::partial_factorization() {
    if (this->dim_sep && F11.active()) {
#if defined(WRITE_ROOT)
      if (etree_level == 0) {
        if (!mpi_rank(this->front_comm))
          std::cout << "Writing root node to file..." << std::endl;
        F11.MPI_binary_write();
        if (!mpi_rank(this->front_comm))
          std::cout << "Done. Early abort." << std::endl;
        MPI_Finalize();
        exit(0);
      }
#endif
      piv = F11.LU();
      if (this->dim_upd) {
        F12.permute_rows_fwd(piv);
        trsm(Side::L, UpLo::L, Trans::N, Diag::U, scalar_t(1.), F11, F12);
        trsm(Side::R, UpLo::U, Trans::N, Diag::N, scalar_t(1.), F11, F21);
        gemm(Trans::N, Trans::N, scalar_t(-1.), F21, F12, scalar_t(1.), F22);
      }
    }
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::multifrontal_factorization
  (const SPOptions<scalar_t>& opts, int etree_level, int task_depth) {
    if (this->visit(this->lchild))
      this->lchild->multifrontal_factorization
        (opts, etree_level+1, task_depth);
    if (this->visit(this->rchild))
      this->rchild->multifrontal_factorization
        (opts, etree_level+1, task_depth);
    build_front();
    if (this->lchild) this->lchild->release_work_memory();
    if (this->rchild) this->rchild->release_work_memory();
    partial_factorization();
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::forward_multifrontal_solve
  (scalar_t* b_loc, DistM_t* b_dist, scalar_t* wmem,
   int etree_level, int task_depth) {
    if (this->visit(this->lchild))
      this->lchild->forward_multifrontal_solve
        (b_loc, b_dist, wmem, etree_level, task_depth);
    if (this->visit(this->rchild))
      this->rchild->forward_multifrontal_solve
        (b_loc, b_dist, wmem, etree_level, task_depth);
    DistMW_t Bupd(this->ctxt, this->dim_upd, 1, wmem+this->p_wmem);
    Bupd.zero();
    this->look_left(b_dist[this->sep], wmem);
    if (this->dim_sep) {
      TIMER_TIME(TaskType::SOLVE_LOWER, 0, t_s);
      b_dist[this->sep].permute_rows_fwd(piv);
      trsv(UpLo::L, Trans::N, Diag::U, F11, b_dist[this->sep]);
      if (this->dim_upd)
        gemv(Trans::N, scalar_t(-1.), F21, b_dist[this->sep],
             scalar_t(1.), Bupd);
      TIMER_STOP(t_s);
    }
  }

  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::backward_multifrontal_solve
  (scalar_t* y_loc, DistM_t* y_dist, scalar_t* wmem,
   int etree_level, int task_depth) {
    if (this->dim_sep) {
      TIMER_TIME(TaskType::SOLVE_UPPER, 0, t_s);
      if (this->dim_upd) {
        DistMW_t Yupd(this->ctxt, this->dim_upd, 1, wmem+this->p_wmem);
        gemv(Trans::N, scalar_t(-1.), F12, Yupd,
             scalar_t(1.), y_dist[this->sep]);
      }
      trsv(UpLo::U, Trans::N, Diag::N, F11, y_dist[this->sep]);
      TIMER_STOP(t_s);
    }
    this->look_right(y_dist[this->sep], wmem);
    if (this->visit(this->lchild))
      this->lchild->backward_multifrontal_solve
        (y_loc, y_dist, wmem, etree_level, task_depth);
    if (this->visit(this->rchild))
      this->rchild->backward_multifrontal_solve
        (y_loc, y_dist, wmem, etree_level, task_depth);
  }

  /**
   * Note that B should be defined on the same context as used in this
   * front. This simplifies communication.
   */
  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::extract_CB_sub_matrix_2d
  (const std::vector<std::size_t>& I,
   const std::vector<std::size_t>& J, DistM_t& B) const {
    if (this->front_comm == MPI_COMM_NULL) return;
    std::vector<std::size_t> lJ, oJ, lI, oI;
    this->find_upd_indices(J, lJ, oJ);
    this->find_upd_indices(I, lI, oI);
    auto P = mpi_nprocs(this->front_comm);
    std::vector<std::vector<scalar_t>> sbuf(P);
    ExtAdd::extract_copy_to_buffers(F22, lI, lJ, oI, oJ, B, sbuf);
    scalar_t *rbuf = nullptr, **pbuf = nullptr;
    all_to_all_v(sbuf, rbuf, pbuf, this->front_comm);
    ExtAdd::extract_copy_from_buffers(B, lI, lJ, oI, oJ, F22, pbuf);
    delete[] rbuf;
    delete[] pbuf;
  }

  /**
   *  Sr = F22 * R
   *  Sc = F22^* * R
   */
  template<typename scalar_t,typename integer_t> void
  FrontalMatrixDenseMPI<scalar_t,integer_t>::sample_CB
  (const SPOptions<scalar_t>& opts, const DistM_t& R, DistM_t& Sr, DistM_t& Sc,
   FrontalMatrix<scalar_t,integer_t>* pa) const {
    if (F11.active() || F22.active()) {
      auto b = R.cols();
      Sr = DistM_t(this->ctxt, this->dim_upd, b);
      Sc = DistM_t(this->ctxt, this->dim_upd, b);
      gemm(Trans::N, Trans::N, scalar_t(1.), F22, R, scalar_t(0.), Sr);
      gemm(Trans::C, Trans::N, scalar_t(1.), F22, R, scalar_t(0.), Sc);
    }
  }

  template<typename scalar_t,typename integer_t> long long
  FrontalMatrixDenseMPI<scalar_t,integer_t>::factor_nonzeros
  (int task_depth) const {
    return this->dense_factor_nonzeros(task_depth);
  }

} // end namespace strumpack

#endif //FRONTAL_MATRIX_DENSE_MPI_HPP
