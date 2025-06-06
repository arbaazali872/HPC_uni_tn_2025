#ifndef MATMUL_H
#define MATMUL_H
#define _USE_MATH_DEFINES
#define _POSIX_C_SOURCE 200809L


#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h> // for rand_r, malloc, etc.
#include <string.h> // For using memcpy()
#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>   // for offsetof, HGT-GWO_hybrid.c
#include <errno.h>
#include <mpi.h>
