# P3DPGE
3D engine based on javidx9's Pixel Game Engine

Dependencies:
[Boost Libraries 1.74.0](https://www.boost.org/users/history/version_1_74_0.html)

---
# Exporting a Model From Blender and Loading it Into The Engine
1. In the export menu for Blender choose Wavefront (.obj).
2. Then on the right side open the Geometry tab and make sure that everything is unchecked except for "Triangulate Faces"
3. Export it and place it into the objects folder 
4. In the code, create a new object of type Complex in this fashion: `Complex("name_of_obj_file.obj", id, position)`
