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
  We are using the same grid from the upwelling with a non-analytical grid and realistic initial and boundary conditions. Notice that a description of the different switches are found in the roms manual.
  
  Here we are using the following boundary condition switches: `Rad` and `RadNud`; The forum and the manual advise the use `Rad` with `#define RADIATION_2D` flag, while the 'RadNud' switch requires information either from input fields or from files with analytical conditions, which are supplied by the `#define ANA_FSOBC` and  `#define ANA_M2OBC`.

  Notice that GLS_MIXING has also been added. Look at the .h file - by choosing this option the #ANA_VMIX key is automatically ignored.

```
#define MASKING
/* #define ANA_GRID */
#undef ANA_GRID
#undef ANA_INITIAL


#define ANA_FSOBC
#define ANA_M2OBC
#define RADIATION_2D

#define GLS_MIXING

```

## 4. Change roms_realistic_ic.in
   I'm assuming you already created a roms grid/initical condition/boundary file and copied into the `input`. Here,  we are setting some physics in the boundary conditions (The energy should not accumulate at open boundary conditions, but we all know this is not always easy). Look for the documentation on boundary condition in the roms manual, roms forum and also the code itself for more informations. I set up a case that should work in our example (no tides, only winds!).

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

   LBC(isFsur) ==   Clo     Rad     Rad     Rad         ! free-surface
   LBC(isUbar) ==   Clo     Rad     Rad     Rad         ! 2D U-momentum
   LBC(isVbar) ==   Clo     Rad     Rad     Rad         ! 2D V-momentum
   LBC(isUvel) ==   Clo     Rad     Rad     Rad         ! 3D U-momentum
   LBC(isVvel) ==   Clo     Rad     Rad     Rad         ! 3D V-momentum
   LBC(isMtke) ==   Clo     Clo     Clo     Clo         ! mixing TKE

   LBC(isTvar) ==   Clo     Rad     Rad     Rad \       ! temperature
                    Clo     Rad     Rad     Rad         ! salinity




   (...)

     GRDNAME == input/roms_grd00.nc
     ININAME == input/pbs_202109_glorys_pbs_202109_glorys_ic.nc

   (...)

    BRYNAME == input/pbs_202109_glorys_bdry_2019-08-01T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-02T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-03T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-04T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-05T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-06T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-07T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-08T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-09T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-10T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-11T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-12T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-13T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-14T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-15T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-16T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-17T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-18T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-19T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-20T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-21T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-22T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-23T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-24T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-25T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-26T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-27T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-28T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-29T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-30T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-08-31T12:00:00.nc |
            input/pbs_202109_smooth_bdry_2019-09-01T12:00:00.nc

```

You may need to adjust the dimensions names in varinfo.dat, depending on the input files you use.

## 5. Set up analytical fields
### 5.1 Fortran switches
I will not get into these details as winds_parallel.h indicates what analytical fields require adjustments.
You can take the analytical stratification from the upwelling case in `src_code/ROMS/Functionals/ana_initial.h`


## 5. Set up analytical fields

Try to compile the model:

```
./build_roms.sh  ! use sudo if necessary
```

An error related to analytical files should be thrown. This shows up because the analytical files need to be configured. Let's understand what is going on.

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

You'll also need to change `ana_smflux.h` and other analytical scripts according to your needs. You can figure out what analytical scripts must be changed by looking at the active analytical functions in `realistic_ic.h.h` or you can compile the model, which should return an error and indicating the error. For example (when we don't set #GLS_MIXING in the .h):

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

The examples are present in the following directory: `${ROMS_HOME}/ROMS/Functionals`

You'll need to change analytical files that uses UPWELLING. Check for it by typing (in the functioinals directory):

```
grep -r UPWELLING *h
```

# Execute 
```mpiexec -np 4 ./romsM roms_windsparallel.in```
