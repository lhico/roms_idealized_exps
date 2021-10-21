# Experiment 05:
This case consists of vertically stratified,horizontally homogenous fields with a numerical grid set by the user, wind parallel (stress momentum flux) to the coast and tides.

1. Set up the directory
2. Set up build_roms.sh
3. Change upwelling.h and set up boundary conditions
4. Change roms_upwelling.in  and set up boundary conditions
5. Set up analytical fields

Changing the wind to an idealized field parallel to the coast will require us
to work with the .h files in the ``$PROJECT_PATH/functionals` directory (see below). Any change in
the .h files requires a recompilation of the model.

## 1. Set up directory (we are renaming upwelling.h)
We will recompile the model in an entire new directory:

```
# create a directory
  mkdir experiment02
  cd experiment02

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
  cp ${ROMS_HOME}/User/Functionals/* $PROJECT_PATH/functionals

  # renaming files
  mv roms_upwelling.in simple_tides.in
  mv upwelling.h roms_simpletides.h

```


## 2. Set up build_roms.sh

The switch `$ROMS_APPLICATION` is used to define values in analytical fields.
These fields are defined in scripts and you'll need to edit them and replace some
values

```
(...)

# defining ROMS header file
export   ROMS_APPLICATION=simple_tides

(...)
export MY_ROOT_DIR=${HOME}/src_code    #src_code contains ROMS source code
export MY_PROJECT_DIR=${PWD}
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


## 3. Set up simple_tides.h
  We are using the same grid from the upwelling with a non-analytical grid. The difference now is we will set the values of the analytical stress momentum flux. Notice that a description of the different switches are found in the roms manual. Here we are using the following boundary condition switches: `Rad` and `RadNud`; The forum and the manual advise the use `Rad` with `#define RADIATION_2D` flag, while the 'RadNud' switch requires information either from input fields or from files with analytical conditions, which are supplied by the `#define ANA_FSOBC` and  `#define ANA_M2OBC`.

```
#define MASKING
/* #define ANA_GRID */

#define TIDES_ASTRO
#define POT_TIDES
#define SSH_TIDES
#define UV_TIDES
#define RAMP_TIDES
#define ADD_FSOBC
#define ADD_M2OBC
#define ANA_FSOBC
#define ANA_M2OBC 

#define RADIATION_2D
```

## 4. Change wind_parallel.in
   I'm assuming you already created a roms grid and copied into the `input`. Here,  we are setting some physics in the boundary conditions (The energy should not accumulate at open boundary conditions, but we all know this is not always easy). Look for the documentation on boundary condition in the roms manual, roms forum and also the code itself for more informations. I set up a case that should work in our example (no tides, only winds!). Since we are nudging the free-surface (FS), we also need to set the nudging/relaxation time scales. You should look in the forum and manual to figure out how does the nudging time scales work.
```
  (...)
  VARNAME = varinfo.dat
   (...)
   Lm == 22            ! Number of I-direction INTERIOR RHO-points
   Mm == 45            ! Number of J-direction INTERIOR RHO-points
   N  == 20            ! Number of vertical levels

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

   LBC(isFsur) ==   Cha  Cha  Cha  Cha      ! free-surface
   LBC(isUbar) ==   Fla     Fla     Fla     Fla         ! 2D U-momentum
   LBC(isVbar) ==   Fla     Fla     Fla     Fla         ! 2D V-momentum
   LBC(isUvel) ==   Rad     Rad     Rad     Rad         ! 3D U-momentum
   LBC(isVvel) ==   Rad     Rad     Rad     Rad         ! 3D V-momentum
   LBC(isMtke) ==   Rad     Rad     Rad     Rad         ! mixing TKE

   LBC(isTvar) ==   Rad     Rad     Rad     Rad \       ! temperature
                    Rad     Rad     Rad     Rad         ! salinity

  ! Adjoint-based algorithms can have different lateral boundary
  ! conditions keywords.

  ad_LBC(isFsur) ==   Clo     Clo     Clo     Clo         ! free-surface
  ad_LBC(isUbar) ==   Clo     Clo     Clo     Clo         ! 2D U-momentum
  ad_LBC(isVbar) ==   Clo     Clo     Clo     Clo         ! 2D V-momentum
  ad_LBC(isUvel) ==   Clo     Clo     Clo     Clo         ! 3D U-momentum
  ad_LBC(isVvel) ==   Clo     Clo     Clo     Clo         ! 3D V-momentum
  ad_LBC(isMtke) ==   Clo     Clo     Clo     Clo         ! mixing TKE

  ad_LBC(isTvar) ==   Clo     Clo     Clo     Clo \       ! temperature
                      Clo     Clo     Clo     Clo         ! salinity

   (...)

   ! Nudging/relaxation time scales, inverse scales will be computed
   ! internally, [1:Ngrids].
          TNUDG == 2*0.0d0                    ! days
          ZNUDG == 360.0d0                    ! days
         M2NUDG == 0.0d0                      ! days
         M3NUDG == 0.0d0                      ! days
   ! Factor between passive (outflow) and active (inflow) open boundary
   ! conditions, [1:Ngrids]. If OBCFAC > 1, nudging on inflow is stronger
   ! than on outflow (recommended).
         OBCFAC == 180.0d0                      ! nondimensional


   (...)
   GRDNAME == input/roms_grid00.nc
```

## 5. Set up analytical fields
### 5.1 Fortran switches
I will not get into these details as simple_tides.h indicates what analytical fields require adjustments and compiling the model will tell what changes are required.


## 6. Changing horizontal viscosity values
The real ocean is not organized into bins. One of our model limitiations is the ability to simulate sub grid scale processes. For this reason we need to adjust the viscosity valeus in the .in file. The .in keys you'll use depend on the c++configuration file .h. In this case we are using the `UV_VIS2` in winds_parallel.h, which means you'll need to change the `VISC2` in the .in file. A suggestion of how to scale this parameter is [here (Radiation Stresses)](https://www.myroms.org/wiki/Horizontal_Mixing). 

# Execute 
```mpiexec -np 4 ./romsM roms_upwelling.in```