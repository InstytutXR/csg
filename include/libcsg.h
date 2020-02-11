// Public API for CSG library
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <string>

#include "trimesh.h"

namespace CSG
{
enum CSGOperation
{
   kBadValue = 0,
   kIntersection = 1,
   kUnion = 2,
   kDifference = 3
};

enum IParent
{
   kClay,
   kKnife,
   kBoth,
   kNew
};

class IPointRef
{
public:
   IParent parent;
   uint32_t idx;  // index into parent face's mesh (if applicable)
};


// Structure to contain points generated by triangle-triangle intersections
class IPoint
{
 public:
   IPointRef ref;

   // Indicies of triangles that created this point
   uint32_t cidx;
   uint32_t kidx;
};

class IFace
{
public:
   IPointRef v[3];
   uint32_t  orig; // original face
};

enum TriTriIntersectionType
{
   kPointPoint,
   kPointEdge,
   kEdgeEdge
};


class TriangleIntersection
{
 public:
#ifndef SWIG
   friend std::ostream& operator<<(std::ostream& os, const TriangleIntersection& ix)
   {
      if (!ix.intersect)
      {
         os << "no intersection";
      }
      else
      {
         os << "intersection. ";
         if (ix.coplanar)
            os << "coplanar. ";
         os << "p0: [" << ix.p[0][0] << ", " << ix.p[0][1] << ", " << ix.p[0][2] << "]  "
            << "p1: [" << ix.p[1][0] << ", " << ix.p[1][1] << ", " << ix.p[1][2] << "]  ";
         os << "alpha:" << ix.alpha << "  beta:" << ix.beta;
      }
      return os;
   }
#endif

   bool intersect;        // is there an intersection at all?
   bool coplanar;         // are the triangles coplanar
   Eigen::Vector3d p[2];  // the two points defining the line of intersection
   double alpha;          // clay points
   double beta;           // knife points
};


class CSGEngine
{
 public:
   CSGEngine(const TriMesh& clay, const TriMesh& knife);

   void construct(CSGOperation operation, bool cap, TriMesh& output_A, TriMesh& output_B);

 private:
   // Member functions
   const Eigen::Vector3d& ipointPos(const IPointRef& ref) const;

   std::vector<IPoint> convertIntersectionToIpoints(const TriangleIntersection& ix,
                                                    uint32_t clay_face_idx,
                                                    uint32_t knife_face_idx);
   std::vector<IFace> retriangulate(const TriMesh& mesh, IParent which_mesh, uint32_t fidx,
                                    const std::vector<uint32_t>& new_vert_indices) const;

   std::vector<char> classifyCutFaces(const std::vector<IFace>& in_faces, IParent which);


   // Data members
   const TriMesh& m_clay;
   const TriMesh& m_knife;

   // Storage for points "created" by intersections of triangles.  The points may
   // be existing points on one of the two CSG surfaces, or (most likely) it will
   // be new.  Both result meshes will share these vertices
   std::vector<IPoint> m_newPoints;

   // The positions of the genuinely new points created by triangle intersections
   std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> m_newPointPositions;
};


}  //namespace CSG
