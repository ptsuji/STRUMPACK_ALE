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
#ifndef STRUMPACK_CONFIG_H
#define STRUMPACK_CONFIG_H

#include <stdbool.h>

#if (PARALLEL == 1)
#define STRUMPACK_USE_MPI
#endif
 /* #undef STRUMPACK_USE_OPENMP */

 /* #undef STRUMPACK_USE_CUDA */
 /* #undef STRUMPACK_USE_HIP */
 /* #undef STRUMPACK_HIP_PLATFORM_HCC */
 /* #undef STRUMPACK_HIP_PLATFORM_NVCC */
 /* #undef STRUMPACK_USE_SYCL */

 /* #undef STRUMPACK_USE_BLAS64 */
#if defined(STRUMPACK_USE_BLAS64)
#define strumpack_blas_int long long int
#else
#define strumpack_blas_int int
#endif

/* #undef STRUMPACK_USE_METIS */
#if (PARALLEL == 1)
#define STRUMPACK_USE_PARMETIS
#endif
/* #undef STRUMPACK_USE_SCOTCH */
/* #undef STRUMPACK_USE_PTSCOTCH */
/* #undef STRUMPACK_USE_PAPI */
/* #undef STRUMPACK_USE_COMBBLAS */
/* #undef STRUMPACK_USE_BPACK */
/* #undef STRUMPACK_USE_ZFP */
/* #undef STRUMPACK_USE_SLATE_SCALAPACK */
/* #undef STRUMPACK_USE_GETOPT */
/* #undef STRUMPACK_USE_MAGMA */

/* #undef STRUMPACK_USE_MATLAB */

/* #undef STRUMPACK_COUNT_FLOPS */
/* #undef STRUMPACK_TASK_TIMERS */
/* #undef STRUMPACK_MESSAGE_COUNTER */

/* #undef STRUMPACK_USE_OPENMP_TASKLOOP */
/* #undef STRUMPACK_USE_OPENMP_TASK_DEPEND */

#define STRUMPACK_PBLAS_BLOCKSIZE 32

#define STRUMPACK_VERSION_MAJOR 7
#define STRUMPACK_VERSION_MINOR 1
#define STRUMPACK_VERSION_PATCH 3

/**
 * Return major.minor.patch STRUMPACK version.
 * TODO get the git commit ID?
 *
 * \param major major version number
 * \param minor minor version number
 * \param patch patch version number
 */
inline void get_version(int* major, int* minor, int* patch) {
	*major = STRUMPACK_VERSION_MAJOR;
	*minor = STRUMPACK_VERSION_MINOR;
	*patch = STRUMPACK_VERSION_PATCH;
}

inline bool have_parmetis() {
#if defined(STRUMPACK_USE_PARMETIS)
	return true;
#else
	return false;
#endif
}

inline bool have_scotch() {
#if defined(STRUMPACK_USE_SCOTCH)
	return true;
#else
	return false;
#endif
}

inline bool have_pt_scotch() {
#if defined(STRUMPACK_USE_PTSCOTCH)
	return true;
#else
	return false;
#endif
}

inline bool have_papi() {
#if defined(STRUMPACK_USE_PAPI)
	return true;
#else
	return false;
#endif
}

inline bool have_combblas() {
#if defined(STRUMPACK_USE_COMBBLAS)
	return true;
#else
	return false;
#endif
}

inline bool have_butterflypack() {
#if defined(STRUMPACK_USE_BPACK)
	return true;
#else
	return false;
#endif
}

inline bool have_zfp() {
#if defined(STRUMPACK_USE_ZFP)
	return true;
#else
	return false;
#endif
}

inline bool have_slate() {
#if defined(STRUMPACK_USE_SLATE_SCALAPACK)
	return true;
#else
	return false;
#endif
}

inline bool have_getopt() {
#if defined(STRUMPACK_USE_GETOPT)
	return true;
#else
	return false;
#endif
}

inline bool have_magma() {
#if defined(STRUMPACK_USE_MAGMA)
	return true;
#else
	return false;
#endif
}

inline bool have_kblas() {
#if defined(STRUMPACK_USE_KBLAS)
	return true;
#else
	return false;
#endif
}

inline bool have_matlab() {
#if defined(STRUMPACK_USE_MATLAB)
	return true;
#else
	return false;
#endif
}

#endif // STRUMPACK_CONFIG_H

