/*
** git $Id$
** svn $Id: upwelling.h 1001 2020-01-10 22:41:16Z arango $
*******************************************************************************
** Copyright (c) 2002-2020 The ROMS/TOMS Group                               **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**
** Options for Upwelling Test.
**
** Application flag:   SIMPLE_TIDES
** Input script:       roms_simpletides.in
*/


#define UV_ADV
#define UV_COR
#define UV_LDRAG
#define UV_VIS2
#undef  MIX_GEO_UV
#define MIX_S_UV
#define SPLINES_VDIFF
#define SPLINES_VVISC
#define DJ_GRADPS
#define TS_DIF2
#undef  TS_DIF4
#undef  MIX_GEO_TS
#define MIX_S_TS

#define SALINITY
#define SOLVE3D
#define AVERAGES
#define DIAGNOSTICS_TS
#define DIAGNOSTICS_UV

#define TIDES_ASTRO
#define POT_TIDES
#define SSH_TIDES
#define UV_TIDES
#define RAMP_TIDES
#define ADD_FSOBC
#define ADD_M2OBC
#define ANA_FSOBC
#define ANA_M2OBC

#define MASKING
/* #define ANA_GRID */
#define ANA_INITIAL
#define ANA_SMFLUX
#define ANA_STFLUX
#define ANA_SSFLUX
#define ANA_BTFLUX
#define ANA_BSFLUX

#if defined GLS_MIXING || defined MY25_MIXING
# define KANTHA_CLAYSON
# define N2S2_HORAVG
# define RI_SPLINES
#else
# define ANA_VMIX
#endif

#if defined BIO_FENNEL  || defined ECOSIM || \
    defined NPZD_POWELL || defined NEMURO
# define ANA_BIOLOGY
# define ANA_SPFLUX
# define ANA_BPFLUX
# define ANA_SRFLUX
#endif

#if defined NEMURO
# define HOLLING_GRAZING
# undef  IVLEV_EXPLICIT
#endif

#ifdef BIO_FENNEL
# define CARBON
# define DENITRIFICATION
# define BIO_SEDIMENT
# define DIAGNOSTICS_BIO
#endif

#ifdef PERFECT_RESTART
# undef  AVERAGES
# undef  DIAGNOSTICS_BIO
# undef  DIAGNOSTICS_TS
# undef  DIAGNOSTICS_UV
# define OUT_DOUBLE
#endif