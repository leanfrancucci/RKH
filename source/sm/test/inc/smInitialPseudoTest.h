/**
 *  \file       smInitialPseudoTest.h
 *  \ingroup    Test
 *
 *  \brief      State machine to facilitate the test of production code.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.02.16  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SMINITIALPSEUDOTEST_H__
#define __SMINITIALPSEUDOTEST_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
enum
{
    A, B, C, D, E, F, TERMINATE,
    SMIPT_NUM_EVENTS
};

/* ................................. Events ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(smInitialPseudoTest);

/* ............................ Declares vertex ............................ */
RKH_DCLR_BASIC_STATE smIPT_s0, smIPT_s11, smIPT_s12, smIPT_s211, smIPT_s31,
                     smIPT_s32, smIPT_s331, smIPT_s411, smIPT_s52, smIPT_s511,
                     smIPT_s61, smIPT_s62;
RKH_DCLR_COMP_STATE smIPT_s1, smIPT_s2, smIPT_s21, smIPT_s3, smIPT_s33,
                    smIPT_s4, smIPT_s41, smIPT_s5, smIPT_s51, smIPT_s6;
RKH_DCLR_SHIST_STATE smIPT_s1Hist, smIPT_s2Hist, smIPT_s4Hist;
RKH_DCLR_DHIST_STATE smIPT_s3Hist;
RKH_DCLR_CHOICE_STATE smIPT_choice1;
RKH_DCLR_COND_STATE smIPT_junction1;

/* ------------------------------- Data types ------------------------------ */
typedef struct SmInitialPseudoTest SmInitialPseudoTest;
struct SmInitialPseudoTest      /* SMA derived from RKH_SMA_T structure */
{
    RKH_SMA_T sma;
    rui8_t foo;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */