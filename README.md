# Idealized runs for ROMS
Compile ROMS at a given directory using the build scripts. We need to set up the
paths. In this examples we are using a parallel implementation:

I'm assuming you are using [this ROMS implementation](https://www.notion.so/How-to-use-the-Docker-ROMS-stack-to-run-the-upwelling-test-case-on-your-personal-computer-cf9d8220c7c643cd9baf88fefa5c99e0) by Danilo Silva and also our [pyroms_tools](https://github.com/CoastalHydrodynamicsLab/pyroms_tools)


## 1. Set up build_roms.sh
To configure parallel runs, you'll need to change the build_roms.sh:

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

The run `./build_bash`

## 2. Set up roms_upwelling.in
The number of processors used in parallel  simulations must be equal the product of NtileI and NtileJ (in this  case we are using only 4).
```
  VARNAME = varinfo.dat
  (...)
  NtileI = 2
  NtileJ = 2
```

## 3. Execute the model

`mpiexec -np 4 ./romsM roms_upwelling.in  #np is the number of cores you'll be using`

*Please be considerate about your fellow colleagues and let them know when you are using many nodes at once.*

## 4. Most relevant files:
* build_roms.sh
* roms_upwelling.in (roms config. file, name may change according to application)
* upwelling.h (header file, name may change according to application)
* varinfo.dat (sets names, units and other variable param. used in roms)
* (...)


## 5. Experiments
* experiment01 - upwelling test
* experiment02 - upwelling test with a grid (created using pyroms_tools)
* experiment03 - homogeneous fields, 'wind' parallel to the coast (analytical experiment). The wind is actually the stress momentum flux.  
* experiment04 - idealized stratified fields, 'wind' parallel to the coast.
