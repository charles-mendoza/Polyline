# Polyline
This project is a direct implementation of drawing polylines by tessellation in C++ using DirectX inspired by a [paulhoux's](https://github.com/paulhoux) [approach](https://github.com/paulhoux/Cinder-Samples/tree/master/GeometryShader). His project used OpenGL for rendering, I simply took idea from his approach and used DirectX so I could quickly and efficiently draw ESP bone lines on my game cheats. As an added bonus, I made a bettter implementation of drawing circles instead of those usual crappy functions that call trigonometric functions in a loop.

![Preview](https://user-images.githubusercontent.com/28957075/67417628-cb6c9c80-f5fb-11e9-86b3-75200c7a647f.png)

## Approach

Read more [here](https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader).

Construction of the line starts with a segment:

![1](https://user-images.githubusercontent.com/28957075/106991709-c4967880-67b1-11eb-8fba-00dc64698d06.png)

P0 and P1 are Vec2f's. The line is defined by `Vec2f line = p1 - p0`
and the normal (n) is defined by `Vec2f normal = Vec2f( -line.y, line.x).normalized()`.

You could then draw the segment by multiplying the normal by the thickness and add or subtract it from the end points:
```
Vec2f a = p0 - thickness * normal;
Vec2f b = p0 + thickness * normal;
Vec2f c = p1 - thickness * normal;
Vec2f d = p1 + thickness * normal;
```

But when two segments are joined, we need to calculate the cross-section of the joint to form the miter:

![2](https://user-images.githubusercontent.com/28957075/106991713-c6603c00-67b1-11eb-8821-8d95d8bb3323.png)

First, I calculate the tangent (t) as follows:
`Vec2f tangent = ( (p2-p1).normalized() + (p1-p0).normalized() ).normalized()`

Next, I find the miter line (m), which is the normal of the tangent:
`Vec2f miter = Vec2f( -tangent.y, tangent.x )`

The correct length (d) of the miter can then be found by projecting it on one of the normals (shown in red) using the dotproduct. This gives us a value that is the inverse of the desired length, so:
`float length = thickness / miter.dot( normal )`

That's all the information we need. Now we can construct the triangle mesh:

![3](https://user-images.githubusercontent.com/28957075/106991720-c829ff80-67b1-11eb-9b15-479d6e93ceae.png)
