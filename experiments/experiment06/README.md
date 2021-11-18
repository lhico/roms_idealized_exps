# Experiment 06:
This case consists of realistic initial & boundary conditions with wind parallel to the coast. We will include external netcdf files at the appropriate locations.

## Summary

1. Set up the directory
2. Set up build_roms.sh
3. Change upwelling.h and set up boundary conditions
4. Change roms_upwelling.in  and set up boundary conditions
5. Set up analytical fields

Changing the wind to an idealized field parallel to the coast will require us
to work with the .h files in the ``$PROJECT_PATH/functionals` directory (see below). Any change in
the .h files requires a recompilation of the model.

## 1. Set up directory (we are renaming upwelling.h)

```
# create a directory
mkdir experiment06
cd experiment06

PROJECT_PATH=/path/to/project               #the directory we just created
ROMS_HOME=/path/to/roms/source/code/ROMS    #this is the ROMS directory inside the roms we downloaded with the svn
cd ${PROJECT_PATH}

# the fortran files that configure analytical fields will  be copied here
mkdir functionals  

# we are creating an input dir to keep things organized
mkdir input

# set directories
cp ${ROMS_HOME}/Bin/build_roms.sh          $PROJECT_PATH
cp ${ROMS_HOME}/External/roms_upwelling.in $PROJECT_PATH
cp ${ROMS_HOME}/External/varinfo.dat       $PROJECT_PATH
cp ${ROMS_HOME}/Include/upwelling.h        $PROJECT_PATH
cp -r ${ROMS_HOME}/Functionals/* $PROJECT_PATH/functionals

# renaming files
mv roms_upwelling.in roms_realistic_ic.in
mv upwelling.h realistic_ic.h
mv varinfo.dat varinfo.dat.bk

```


## 2. Set up build_roms.sh

IMPORTANT: The switch `$ROMS_APPLICATION` is used to define values in analytical fields. It is NOT simply the name of your application. Also, your c++ definition file (the .ht file) will be identified by this switch.
You will need to edit the analytical files (`$PROJECT_PATH/functionals` directory) to your application. These fields are defined in scripts and you'll need to edit them and replace some
values

```
(...)

# defining ROMS header file
export   ROMS_APPLICATION=REALISTIC_IC

(...)
export MY_ROOT_DIR=${HOME}/src_code    #src_code contains ROMS source code
export MY_PROJECT_DIR=${PWD}
export MY_ROMS_SRC=${MY_ROOT_DIR}

(...)
export       MY_ROMS_SRC=${MY_ROOT_DIR}
(...)
export          USE_MPI=on               # distributed-memory parallelism
export          USE_MPIF90=on            # compile with mpif90 script
# export        which_MPI=mpich          # compile with MPICH library
# export        which_MPI=mpich2         # compile with MPICH2 library
# export        which_MPI=mvapich2       # compile with MVAPICH2 library
export        which_MPI=openmpi        # compile with OpenMPI library
# export        USE_OpenM =on            # shared-memory parallelism

export              FORT=gfortran
# export            FORT=ifort
# export            FORT=pgi

export         USE_NETCDF4=on              # compile with NetCDF-4 library
# export         USE_DEBUG=on              # use Fortran debugging flags
# export         USE_LARGE=on              # activate 64-bit compilation
# export          USE_HDF5=on              # compile with HDF5 library
# export   USE_PARALLEL_IO=on              # Parallel I/O with NetCDF-4/HDF5
(...)
export MY_ANALYTICAL_DIR=${MY_PROJECT_DIR}/functionals
(...)
```


## 3. Set up realistic_ic.h
  We will replace all cppdef keys in the .h. Simply replace the information below in realistic_ic.h

  Notice that LMD_MIXING has also been added. Look at the .h file - by choosing this option the #ANA_VMIX key is automatically ignored.

```
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
#define MASKING
#define TS_FIXED

#define SALINITY
#define SOLVE3D
#define AVERAGES
#define DIAGNOSTICS_TS
#define DIAGNOSTICS_UV

#undef ANA_GRID
#undef ANA_INITIAL
#define ANA_SMFLUX
#define ANA_STFLUX
#define ANA_SSFLUX
#define ANA_BTFLUX
#define ANA_BSFLUX

#define ANA_FSOBC
#define ANA_M2OBC
#define RADIATION_2D
#undef GLS_MIXING

# define LMD_MIXING
# ifdef LMD_MIXING
# define LMD_RIMIX
# define LMD_CONVEC
# define LMD_SKPP
# undef LMD_BKPP
# define LMD_NONLOCAL
# define LMD_SHAPIRO
# undef LMD_DDMIX
# endif


# define ATM_PRESS 
/* # define CCSM_FLUXES */
/* #define ANA_INITIAL */

# define BULK_FLUXES
# define QCORRECTION  /*used to correct heat fluxes */
# define ANA_BSFLUX
# define ANA_BTFLUX
# define ANA_STFLUX
# define ANA_SSFLUX
# define ANA_RAIN


#if defined GLS_MIXING || defined MY25_MIXING || defined LMD_MIXING
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



```

## 4. Change roms_realistic_ic.in
   I'm assuming you already created a roms grid/initical condition/boundary/surface forcing files and copied into the `input`. Here,  we are setting some physics in the boundary conditions (The energy should not accumulate at open boundary conditions, but we all know this is not always easy). Look for the documentation on boundary condition in the roms manual, roms forum and also the code itself for more informations. I set up a case that should work in our example (no tides, only winds!).

   **ATTENTION**: I haven't configured the horizontal values of interior rho-points. Figure it out by checking the gridfile netcdf.


  You can find the Lm and Mm values by checking the dimensions `xi_rho` and `eta_rho` and subtract 2 from each. You can use the command `ncdump -h roms_grid00.nc` to get the dimensions.
  The vertical stretching parameters are set while building the grid.

```
! Application title.

  TITLE = experiment: Wind parallel to the coast

! C-preprocessing Flag.

    MyAppCPP = REALISTIC_IC  ! this is just a name and doesn't



  (...)
  VARNAME = varinfo.dat
   (...)
   Lm == 22            ! Number of I-direction INTERIOR RHO-points
   Mm == 45            ! Number of J-direction INTERIOR RHO-points
   N == 20             ! Number of vertical levels

  (...)

  ! Domain decomposition parameters for serial, distributed-memory or
  ! shared-memory configurations used to determine tile horizontal range
  ! indices (Istr,Iend) and (Jstr,Jend), [1:Ngrids].

    NtileI == 2                               ! I-direction partition
    NtileJ == 2                               ! J-direction partition

   (...)

  THETA_S == 7.0d0                      ! surface stretching parameter
  THETA_B == 3.0d0                      ! bottom  stretching parameter
  TCLINE == 250.0d0                     ! critical depth (m)

   (...)
   # boundary conditions: check the .in for more details

   LBC(isFsur) ==   Clo     Che        Che        Che         ! free-surface
   LBC(isUbar) ==   Clo     Fla        Fla        Fla         ! 2D U-momentum
   LBC(isVbar) ==   Clo     Fla        Fla        Fla         ! 2D V-momentum
   LBC(isUvel) ==   Clo     RadNud     RadNud     RadNud         ! 3D U-momentum
   LBC(isVvel) ==   Clo     RadNud     RadNud     RadNud         ! 3D V-momentum
   LBC(isMtke) ==   Clo     Clo        Clo        Clo         ! mixing TKE

   LBC(isTvar) ==   Clo     RadNud     RadNud     RadNud \       ! temperature
                    Clo     RadNud     RadNud     RadNud         ! salinity




   (...)

     GRDNAME == input/roms_grid00.nc
     ININAME == input/pbs_202109_glorys_pbs_202109_glorys_ic.nc

   (...)

    BRYNAME == input/pbs_202109_glorys_bdry_2019-08-01T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-02T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-03T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-04T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-05T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-06T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-07T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-08T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-09T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-10T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-11T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-12T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-13T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-14T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-15T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-16T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-17T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-18T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-19T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-20T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-21T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-22T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-23T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-24T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-25T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-26T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-27T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-28T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-29T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-30T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-08-31T12:00:00.nc |
               input/pbs_202109_glorys_bdry_2019-09-01T12:00:00.nc

(...)

NFFILES == 1                           ! number of unique forcing files

FRCNAME == input/era5_2019-08-01T00:00:00.nc |
           input/era5_2019-08-02T00:00:00.nc |
           input/era5_2019-08-03T00:00:00.nc |
           input/era5_2019-08-04T00:00:00.nc |
           input/era5_2019-08-05T00:00:00.nc |
           input/era5_2019-08-06T00:00:00.nc |
           input/era5_2019-08-07T00:00:00.nc |
           input/era5_2019-08-08T00:00:00.nc |
           input/era5_2019-08-09T00:00:00.nc |
           input/era5_2019-08-10T00:00:00.nc |
           input/era5_2019-08-11T00:00:00.nc |
           input/era5_2019-08-12T00:00:00.nc |
           input/era5_2019-08-13T00:00:00.nc |
           input/era5_2019-08-14T00:00:00.nc |
           input/era5_2019-08-15T00:00:00.nc |
           input/era5_2019-08-16T00:00:00.nc |
           input/era5_2019-08-17T00:00:00.nc |
           input/era5_2019-08-18T00:00:00.nc |
           input/era5_2019-08-19T00:00:00.nc |
           input/era5_2019-08-20T00:00:00.nc |
           input/era5_2019-08-21T00:00:00.nc |
           input/era5_2019-08-22T00:00:00.nc |
           input/era5_2019-08-23T00:00:00.nc |
           input/era5_2019-08-24T00:00:00.nc |
           input/era5_2019-08-25T00:00:00.nc |
           input/era5_2019-08-26T00:00:00.nc |
           input/era5_2019-08-27T00:00:00.nc |
           input/era5_2019-08-28T00:00:00.nc |
           input/era5_2019-08-29T00:00:00.nc |
           input/era5_2019-08-30T00:00:00.nc |
           input/era5_2019-08-31T00:00:00.nc

```

**ATTENTION**
Download varinfo.dat FROM the repository (experiment06) and compare it with varinfo.dat.bk . I adjusted some dimensions names in varinfo.dat to cope with dimensions names of the boundary files; Something you can use is `vimdiff`

```
vimdiff varinfo.dat varinfo.dat.bk
```


## 5. Set up analytical fields

Try to compile the model:

```
./build_roms.sh  ! use sudo if necessary
```

An error related to analytical files should not be thrown. If it is, some analytical files need to be configured, as you know.

### 5.1 Fortran switches
 The command `export   ROMS_APPLICATION=WINDS_PARALLEL` sets a switch used in the fortran scripts in `$PROJECT_PATH/functionals`. By changing this switch, you need to modify these functions. The following code snippet from `ana_smflux.h` exemplifies how the switch work (we added the switch at an appropriate location):

```
!-----------------------------------------------------------------------
!  Set kinematic surface momentum flux (wind stress) component in the
!  XI-direction (m2/s2) at horizontal U-points.
!-----------------------------------------------------------------------
!
(...)
#elif defined MY_APPLICATION
      DO j=JstrT,JendT
        DO i=IstrP,IendT
          sustr(i,j)=???
        END DO
      END DO
#endif
```

Let's say `export $ROMS_APPLICATION=MY_APPLICATION` is defined in the build script. If it is defined, the `#if  defined MY_APPLICATION` condition will select the loop that will be compiled and ignore the other one (since we have a `???` value, the compilation will fail, as a value needs to be set).

[HERE](https://www.myroms.org/forum/viewtopic.php?t=4938) you can find some information on the kinematic surface momentum flux.

You'll also need to change `ana_smflux.h` and other analytical scripts according to your needs. You can figure out what analytical scripts must be changed by looking at the active analytical functions in `realistic_ic.h.h` or you can compile the model, which should return an error and indicating the error. 
The examples are in the following directory: `${ROMS_HOME}/ROMS/Functionals`

You'll need to change analytical files that uses UPWELLING. Check for it by typing (in the functioinals directory):

```
grep -r UPWELLING *h
```

An easier way to solve this issue and run this test case is replacing any occurrence of 'UPWELLING' for your application's name. Try this:

``` 
cd experiments03/functionals

sed -i 's/UPWELLING/WINDS_PARALLEL/g' *
```

# Execute 
```mpiexec -np 4 ./romsM roms_realistic_ic.in```
