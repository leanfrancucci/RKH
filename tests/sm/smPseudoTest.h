/**
 *  \file       smPseudoTest.h
 *  \ingroup    Test
 *
 *  \brief      State machine to facilitate the test of production code.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2016.03.17  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */

#ifndef __SMPSEUDOTEST_H__
#define __SPSEUDOMTEST_H__

/* ----------------------------- Include files ----------------------------- */

#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */

/* Signals */
enum
{
    A, B, C, D, E, F, G, H, I, TERMINATE,
    SMTEST_NUM_EVENTS
};

/* Declare HSM */
RKH_SMA_DCLR(smPseudoTest);

/* Declare states and pseudostates */
RKH_DCLR_COMP_STATE smPT_s1;
RKH_DCLR_BASIC_STATE smPT_waiting, smPT_s0, smPT_s11, 
                     smPT_s12;
RKH_DCLR_SHIST_STATE smPT_h;

/* ------------------------------- Data types ------------------------------ */

typedef struct SmPseudoTest SmPseudoTest;
struct SmPseudoTest      /* SMA derived from RKH_SMA_T structure */
{
    RKH_SMA_T sma;  /* base structure */
    rui8_t foo;     /* private member */
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
