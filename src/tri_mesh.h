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

    Christoph Kloss (DCS Computing GmbH, Linz)
    Christoph Kloss (JKU Linz)
    Philippe Seil (JKU Linz)

    Copyright 2012-     DCS Computing GmbH, Linz
    Copyright 2009-2012 JKU Linz
------------------------------------------------------------------------- */

#ifndef LMP_TRI_MESH_H
#define LMP_TRI_MESH_H

#include "surface_mesh.h"
#include "atom.h"
#include "math_extra_liggghts.h"
#include "tri_line.h"
#include "region_neighbor_list.h"

#ifdef TRI_LINE_ACTIVE_FLAG
#include "math_extra_dist_lineTriangle.h"
#endif

#ifdef SUPERQUADRIC_ACTIVE_FLAG
#include "math_extra_liggghts_superquadric.h"
using namespace MathExtraLiggghtsNonspherical;
#endif

#include <fstream>

namespace LAMMPS_NS
{

typedef SurfaceMesh<3,5> SurfaceMeshBase;

class TriMesh : public SurfaceMeshBase
{
public:

    TriMesh(LAMMPS *lmp);
    virtual ~TriMesh();

    double resolveTriSphereContact(const int iPart, const int nTri, const double rSphere, const double *const cSphere,
                                   double *delta, int &barysign);
    double resolveTriSphereContactBary(const int iPart, const int nTri, const double rSphere,
                                       const double *const cSphere, double *delta, double *bary,
                                       int &barySign,bool skip_inactive=true);

    #ifdef SUPERQUADRIC_ACTIVE_FLAG

    double resolveTriSuperquadricContact(int nTri, double *normal, double *contactPoint, Superquadric particle);
    double resolveTriSuperquadricContact(int nTri, double *normal, double *contactPoint, Superquadric particle, double *bary);

    bool sphereTriangleIntersection(int nTri, double rSphere, double *cSphere);
    int superquadricTriangleIntersection(int nTri, double *point_of_lowest_potential, Superquadric particle);
    double pointToTriangleDistance(int iTri, double *Csphere, double *delta, bool treatActiveFlag, double *bary);

    #endif

    bool resolveTriSphereNeighbuild(int nTri, double rSphere, double *cSphere, double treshold);

    #ifdef TRI_LINE_ACTIVE_FLAG
    // Extra for Line Contact Calculation ********
    double resolveTriSegmentContact    (int iPart, int nTri, double *line, double *cLine, double length, double cylRadius,
                                        double *delta, double &segmentParameter,int &barysign);
    double resolveTriSegmentContactBary(int iPart, int nTri, double *line, double *cLine, double length, double cylRadius,
                                        double *delta, double  &segmentParameter, double *bary,int &barysign);
    bool resolveTriSegmentNeighbuild(int nTri, double *cLine, double length, double cylRadius, double treshold);
    // Extra for Line Contact Calculation ********
    #endif

    int generateRandomOwnedGhost(double *pos);
    int generateRandomSubbox(double *pos);
    void setWeightedWallFormulation(const bool flag)
    { weightedWallFormulation_ = flag; }
    bool getWeightedWallFormulation() const
    { return weightedWallFormulation_; }

    virtual int generateRandomOwnedGhostWithin(double *pos,double delta);

protected:

    double calcArea(const int n);
    bool isInElement(double *pos,int i);

private:

    inline double precision_trimesh()
    { return MultiNodeMesh<3>::precision(); }

    double calcDist(const double *const cs, const double *const closestPoint, double *const delta);
    double calcDistToPlane(const double *const p, const double *const pPlane, const double *const nPlane);

    double resolveCornerContactBary(const int iTri, const int iNode, const bool obtuse,
                                    const double *const p, double *const delta,
                                    double *const bary, const bool skip_inactive = true);
    double resolveEdgeContactBary(const int iTri, const int iEdge, const double *const p,
                                  double *const delta, double *const bary, const bool skip_inactive = true);
    double resolveFaceContactBary(const int iTri, const double *const p, const double *const node0ToSphereCenter, double *const delta);

    double calculateWeightedProperties(const double *const x, const double *const *const tri_nodes,
                                       const double *const tri_normal, const double *const bary,
                                       const double *const x_to_node0, const double radius, double *const delta);
    double weightedFaceContact(const double *const tri_normal, const double *const bary,
                               const double *const x_to_node0, const double radius,
                               double &delta, double *const normal);
    void weightedEdgeContact(const double *const x, const double *const *const tri_nodes,
                             const double radius, int *const contact, double *const delta,
                             double normal[3][3]);
    void weightedVertexContact(const double *const x, const double *const *const tri_nodes,
                               const double radius, bool *const contact, double *const delta,
                               double normal[3][3], double *const vertex_angle);

    bool weightedWallFormulation_;
};

// *************************************
#include "tri_mesh_I.h"
#ifdef SUPERQUADRIC_ACTIVE_FLAG
#include "tri_mesh_I_superquadric.h"
#endif

#ifdef TRI_LINE_ACTIVE_FLAG
#include "tri_mesh_I_line.h"
#endif
// *************************************

} /* LAMMPS_NS */
#endif /* TRIMESH_H_ */
