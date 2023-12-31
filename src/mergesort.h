/* ----------------------------------------------------------------------
    This is the

    ██╗     ██╗ ██████╗  ██████╗  ██████╗ ██╗  ██╗████████╗███████╗
    ██║     ██║██╔════╝ ██╔════╝ ██╔════╝ ██║  ██║╚══██╔══╝██╔════╝
    ██║     ██║██║  ███╗██║  ███╗██║  ███╗███████║   ██║   ███████╗
    ██║     ██║██║   ██║██║   ██║██║   ██║██╔══██║   ██║   ╚════██║
    ███████╗██║╚██████╔╝╚██████╔╝╚██████╔╝██║  ██║   ██║   ███████║
    ╚══════╝╚═╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝®

    DEM simulation engine, released by
    DCS Computing Gmbh, Linz, Austria
    http://www.dcs-computing.com, office@dcs-computing.com

    LIGGGHTS® is part of CFDEM®project:
    http://www.liggghts.com | http://www.cfdem.com

    Core developer and main author:
    Christoph Kloss, christoph.kloss@dcs-computing.com

    LIGGGHTS® is open-source, distributed under the terms of the GNU Public
    License, version 2 or later. It is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. You should have
    received a copy of the GNU General Public License along with LIGGGHTS®.
    If not, see http://www.gnu.org/licenses . See also top-level README
    and LICENSE files.

    LIGGGHTS® and CFDEM® are registered trade marks of DCS Computing GmbH,
    the producer of the LIGGGHTS® software and the CFDEM®coupling software
    See http://www.cfdem.com/terms-trademark-policy for details.

-------------------------------------------------------------------------
    Contributing author and copyright for this file:
    This file is from LAMMPS
    LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
    http://lammps.sandia.gov, Sandia National Laboratories
    Steve Plimpton, sjplimp@sandia.gov

    Copyright (2003) Sandia Corporation.  Under the terms of Contract
    DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
    certain rights in this software.  This software is distributed under
    the GNU General Public License.
------------------------------------------------------------------------- */

#ifndef LMP_MERGESORT
#define LMP_MERGESORT

#include <string.h>

// custom hybrid upward merge sort implementation with support to pass
// an opaque pointer to the comparison function, e.g. for access to
// class members. this avoids having to use global variables.
// for improved performance, we employ an in-place insertion sort on
// chunks of up to 64 elements and switch to merge sort from then on.

// part 1. insertion sort for pre-sorting of small chunks

static void insertion_sort(int *index, int num, void *ptr,
                           int (*comp)(int, int, void*))
{
  if (num < 2) return;
  for (int i=1; i < num; ++i) {
    int tmp = index[i];
    for (int j=i-1; j >= 0; --j) {
      if ((*comp)(index[j],tmp,ptr) > 0) {
        index[j+1] = index[j];
      } else {
        index[j+1] = tmp;
        break;
      }
      if (j == 0) index[0] = tmp;
    }
  }
}

// part 2. merge two sublists

static void do_merge(int *idx, int *buf, int llo, int lhi, int rlo, int rhi,
                     void *ptr, int (*comp)(int, int, void *))
{
  int i = llo;
  int l = llo;
  int r = rlo;
  while ((l < lhi) && (r < rhi)) {
    if ((*comp)(buf[l],buf[r],ptr) < 0)
      idx[i++] = buf[l++];
    else idx[i++] = buf[r++];
  }

  while (l < lhi) idx[i++] = buf[l++];
  while (r < rhi) idx[i++] = buf[r++];
}

// part 3: loop over sublists doubling in size with each iteration.
//         pre-sort sublists with insertion sort for better performance.

static void merge_sort(int *index, int num, void *ptr,
                       int (*comp)(int, int, void *))
{
  if (num < 2) return;

  int chunk,i,j;

  // do insertion sort on chunks of up to 64 elements

  chunk = 64;
  for (i=0; i < num; i += chunk) {
    j = (i+chunk > num) ? num-i : chunk;
    insertion_sort(index+i,j,ptr,comp);
  }

  // already done?

  if (chunk >= num) return;

  // continue with merge sort on the pre-sorted chunks.
  // we need an extra buffer for temporary storage and two
  // pointers to operate on, so we can swap the pointers
  // rather than copying to the hold buffer in each pass

  int *buf = new int[num];
  int *dest = index;
  int *hold = buf;

  while (chunk < num) {
    int m;

    // swap hold and destination buffer

    int *tmp = dest; dest = hold; hold = tmp;

    // merge from hold array to destiation array

    for (i=0; i < num-1; i += 2*chunk) {
      j = i + 2*chunk;
      if (j > num) j=num;
      m = i+chunk;
      if (m > num) m=num;
      do_merge(dest,hold,i,m,m,j,ptr,comp);
    }
    chunk *= 2;
  }

  // if the final sorted data is in buf, copy back to index

  if (dest == buf) memcpy(index,buf,sizeof(int)*num);

  delete[] buf;
}

#endif
