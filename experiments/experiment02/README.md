# Experiment 02:
This case consists of non-stratified, homogenous fields with a numerical grid set by the user.
The upwelling.h example is the basis to  this project.

1. Set up the directory
2. Set up build_roms.sh
3. Change upwelling.h
4. Change roms_upwelling.in


Including a non-analyitical grid requires us to update change upwelling.h. This file controls the physics and parameterization of ROMS.


## 1. Set up  the directory

```
  PROJECT_PATH=/path/to/project
  #ROMS_HOME=/path/to/roms
  ROMS_HOME = /home/lhico/src_code/ROMS
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


```

## 2. Set up build_roms.sh


```
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


```


##  3. Set up upwelling.h
  We are not using an analytical grid, so we are removing ANA_GRID from upwelling.h
  and recompiling the model and adding the land mask switch. This .h file defines
  the physics included in your compiled model.

```
  /* define ANA_GRID */
  define MASKING
```

## 4. Change roms_upwelling.in

You must create a gridfile (here: `sbb_grid_roms.nc`), which will be read by `roms_upwelling.in`. The grid parameters may be found in the tools you used to create the grid. In the example, the grid was saved into the `input` directory.

```
   (...)
   VARNAME = varinfo.dat
   (...)
   Lm == 250            ! Number of I-direction INTERIOR RHO-points
   Mm == 160            ! Number of J-direction INTERIOR RHO-points
   N == 27              ! Number of vertical levels

   (...)

  THETA_S == 7.0d0                      ! surface stretching parameter
  THETA_B == 3.0d0                      ! bottom  stretching parameter
  TCLINE == 250.0d0                      ! critical depth (m)

   (...)

   GRDNAME == input/sbb_grid_roms.nc

   (...)
```
