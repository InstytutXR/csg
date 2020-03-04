# libCSG - A constructive "solid" geometry library #

This is a clean re-implementation of work I did years ago at a visual effects company.  This library implements the three core CSG operations (intersection, union, and difference) on triangulated meshes.  Unlike typical CSG implementations, the meshes to
be operated on do not need to be solid (*i.e.* watertight) or even manifold.

## Dependencies ##
Requires [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page#Download). 

In my previous implementation, I used a propietary retriangulation algorithm to break up
the cut triangles. However, for this public implementation I am using
[Jonathan Shewchuk](https://people.eecs.berkeley.edu/~jrs/)'s excellent
[Triangle](http://www.cs.cmu.edu/~quake/triangle.html) library. This code is free for
personal and research use but not for commercial use, and does not fall under the
license that the rest of this code uses.

If you wish to create the Python bindings or the Blender integration,
[SWIG](http://www.swig.org/) also must be installed.

## To Build ##
In the top-level ```csg``` directory:
```
mkdir build
cd build
cmake ..
make
```

## Explanation of the Algorithm ##
Essentially, the CSG algorithm finds all intersections of the triangles in mesh "A" and
mesh "B", retriangulates the intersected faces to preserve these new edges, and then
seperates the cut mesh (what we called the "clay") into two new meshes.

In more detail:
1. Create AABB trees of the two meshes (the "clay" and the "knife").
2. Using the AABB trees, do a broadphase intersection step to find pairs of
bounding boxes that may contain intersecting triangles
3. Determine exact triangle-triangle intersections, yeilding potentially new
points that describe new edges.
4. Retriangulate the cut triangles with any added points and edges from step 3.
5. Categorize the cut triangles into two new surfaces, based on whether they
are above or below the face that cut them. We then flood-fill the membership among
the uncut triangles.
6. Generate the output meshes by combining the four mesh fragment results (clay above the knife, clay below the knife, knife above the clay, knife below the clay) in
various ways, depending on the operation we desire.