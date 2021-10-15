# ROMS: A simple tutorial

These experiments were created to introduce the Regional Ocean Model System (ROMS) to new users. They present an increasing complexity, which is built on top of the former case. 

We use [this ROMS implementation](https://www.notion.so/How-to-use-the-Docker-ROMS-stack-to-run-the-upwelling-test-case-on-your-personal-computer-cf9d8220c7c643cd9baf88fefa5c99e0) by Danilo Silva and also our [pyroms_tools](https://github.com/CoastalHydrodynamicsLab/pyroms_tools), which was built on  top of [pyroms](https://github.com/ESMG/pyroms) and [xesmf](https://xesmf.readthedocs.io/en/latest/#). 

ROMS files are:

* build_roms.sh
* roms_upwelling.in (roms config. file, name may change according to application)
* upwelling.h (header file, name may change according to application)
* varinfo.dat (sets names, units and other variable param. used in roms)
* input files created with external tools


The instructions for each experiment are located at the README.md at each directory. They are:

* experiment01 - simple case: upwelling test (analitic stratification)
* experiment02 - introducing a numerical grid: upwelling test with a grid (created using pyroms_tools)
* experiment03 - understanding analytic files: changing the idealized wind fields
* experiment04 - introduction to boundary conditions: idealized stratified fields, 'wind' parallel to the coast.
* experiment05 - including tide forcing: changing boundary conditions and dealing with horizontal viscosity parameterization


## 1. Usage
This repository provides a directory strucutre to organize the experiments. We avoid providing too many files, since ROMS files are version dependent and don't necessarily present backwards compatibility.


In order to download the file structure, use:
```
git clone https://github.com/lhico/roms_idealized_exps
```

Each experiment has its own README.md with instructions to build and run the experiment.

