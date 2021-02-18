# Experiment 03
This case consists of non-stratified, homogenous fields with wind parallel to
the coast (in this case we will the stress momentum flux).
The upwelling.h example is the basis to  this project.

1. Set up the directory
2. Set up build_roms.sh
3. Change upwelling.h
4. Change roms_upwelling.in
5. Set up analytical fields

Changing the wind to an idealized field parallel to the coast will require us
to work with the .h files in the ``$PROJECT_PATH/functionals` directory (see below). Any change in
the .h files requires a recompilation of the model.

## 1. Set up directory (we are renaming upwelling.h)

```
  PROJECT_PATH=/path/to/project
  ROMS_HOME=/path/to/roms
  cd ${ROMS_HOME}

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
  mv roms_upwelling.in winds_parallel.in
  mv upwelling.h roms_windsparallel.h

```


## 2. Set up build_roms.sh

The switch `$ROMS_APPLICATION` is used to define values in analytical fields.
These fields are defined in scripts and you'll need to edit them and replace some
values

```
(...)

# defining ROMS header file
export   ROMS_APPLICATION=WINDS_PARALLEL

(...)
export MY_ROOT_DIR=${HOME}/src_code    #src_code contains ROMS source code
export     MY_PROJECT_DIR=${PWD}

(...)
export       MY_ROMS_SRC=${MY_ROOT_DIR}
(...)
export          USE_MPI=on               # distributed-memory parallelism
export          USE_MPIF90=on            # compile with mpif90 script
# export        which_MPI=mpich          # compile with MPICH library
# export        which_MPI=mpich2         # compile with MPICH2 library
# export        which_MPI=mvapich2       # compile with MVAPICH2 library
# export        which_MPI=openmpi        # compile with OpenMPI library
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


## 3. Set up winds_parallel.h
  We are using the same grid from the upwelling with a non-analytical grid. The difference now is we will set the values of the analytical stress momentum flux. Notice that a description of the different switches are found in the roms manual.


## 4. Change wind_parallel.in
   I'm assuming you already created a roms grid and copied into the `input`. Here,  we are setting some physics in the boundary conditions.
```
  (...)
  VARNAME = varinfo.dat
   (...)
   Lm == 250            ! Number of I-direction INTERIOR RHO-points
   Mm == 160            ! Number of J-direction INTERIOR RHO-points
   N  == 27            ! Number of vertical levels

   (...)

  THETA_S == 7.0d0                      ! surface stretching parameter
  THETA_B == 3.0d0                      ! bottom  stretching parameter
  TCLINE == 250.0d0                     ! critical depth (m)

   (...)
   # boundary conditions: check the .in for more details

   LBC(isFsur) ==   Rad     Clo     Rad     Rad         ! free-surface
   LBC(isUbar) ==   Rad     Clo     Rad     Rad         ! 2D U-momentum
   LBC(isVbar) ==   Rad     Clo     Rad     Rad         ! 2D V-momentum
   LBC(isUvel) ==   Rad     Clo     Rad     Rad         ! 3D U-momentum
   LBC(isVvel) ==   Rad     Clo     Rad     Rad         ! 3D V-momentum
   LBC(isMtke) ==   Rad     Clo     Rad     Rad         ! mixing TKE

   LBC(isTvar) ==   Clo     Clo     Clo     Clo \       ! temperature
                    Clo     Clo     Clo     Clo         ! salinity

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
   GRDNAME == input/sbb_grid_roms.nc
```

## 5. Set up analytical fields
### 5.1 Fortran switches
 The command `export   ROMS_APPLICATION=WINDS_PARALLEL` sets a switch that is used in the fortran scripts in `$PROJECT_PATH/functionals`. The following code snippet from `ana_smflux.h` exemplifies how the switch work:

```
!-----------------------------------------------------------------------
!  Set kinematic surface momentum flux (wind stress) component in the
!  XI-direction (m2/s2) at horizontal U-points.
!-----------------------------------------------------------------------
!
#if defined MY_APPLICATION
      DO j=JstrT,JendT
        DO i=IstrP,IendT
          sustr(i,j)=???
        END DO
      END DO
#else
      DO j=JstrT,JendT
        DO i=IstrP,IendT
          sustr(i,j)=0.0005_r8
        END DO
      END DO
#endif
```

Let's say `export $ROMS_APPLICATION=MY_APPLICATION` is defined in the build script. If it is defined, the `#if  defined MY_APPLICATION` condition will select the loop that will be compiled and ignore the other one (since we have a `???` value, the compilation will fail, as a value needs to be set).

In our case, you'll need to change `ana_smflux.h` and other analytical scripts according to your needs. You can figure out what analytical scripts must be changed by looking at the active analytical functions in `winds_upwelling.h` or you can try to compile the model and it should return you an error and indicate what is wrong. For example:

```
analytical.f90:1072:14:

       ana_vmix.h: no values provided for Akv.
              1
Error: 'ana_vmix' at (1) is not a variable
analytical.f90:1091:14:

       ana_vmix.h: no values provided for Akt.
              1
Error: 'ana_vmix' at (1) is not a variable
ROMS/Functionals/Module.mk:15: recipe for target '/home/lhico/projects/experiments/experiment03/Build_roms/analytical.o' failed
make: *** [/home/lhico/projects/experiments/experiment03/Build_roms/analytical.o] Error 1
```

Practical examples are present in the following directory: `${ROMS_HOME}/ROMS/Functionals`
