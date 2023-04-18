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
 */
#ifndef STRUMPACK_CONFIG_HPP
#define STRUMPACK_CONFIG_HPP

#if (PARALLEL == 1)
#define STRUMPACK_USE_MPI
#endif
// #define STRUMPACK_USE_OPENMP

// #define STRUMPACK_USE_CUDA
// #define STRUMPACK_USE_HIP
// #define STRUMPACK_HIP_PLATFORM_HCC
// #define STRUMPACK_HIP_PLATFORM_NVCC
// #define STRUMPACK_USE_SYCL

// #define STRUMPACK_USE_BLAS64
// #if defined(STRUMPACK_USE_BLAS64)
// #define strumpack_blas_int long long int
// #else
#define strumpack_blas_int int
// #endif

// #cmakedefine STRUMPACK_USE_METIS
#if (PARALLEL == 1)
#define STRUMPACK_USE_PARMETIS
#endif
// #cmakedefine STRUMPACK_USE_SCOTCH
// #cmakedefine STRUMPACK_USE_PTSCOTCH
// #cmakedefine STRUMPACK_USE_PAPI
// #cmakedefine STRUMPACK_USE_COMBBLAS
// #cmakedefine STRUMPACK_USE_BPACK
// #cmakedefine STRUMPACK_USE_ZFP
// #cmakedefine STRUMPACK_USE_SLATE_SCALAPACK
// #cmakedefine STRUMPACK_USE_GETOPT
// #cmakedefine STRUMPACK_USE_MAGMA
// #cmakedefine STRUMPACK_USE_MATLAB

#define STRUMPACK_COUNT_FLOPS
#define STRUMPACK_TASK_TIMERS
#define STRUMPACK_MESSAGE_COUNTER

// #cmakedefine STRUMPACK_USE_OPENMP_TASKLOOP
// #cmakedefine STRUMPACK_USE_OPENMP_TASK_DEPEND

#define STRUMPACK_PBLAS_BLOCKSIZE 32

#define STRUMPACK_VERSION_MAJOR 7
#define STRUMPACK_VERSION_MINOR 0
#define STRUMPACK_VERSION_PATCH 1

#endif // STRUMPACK_CONFIG_HPP

