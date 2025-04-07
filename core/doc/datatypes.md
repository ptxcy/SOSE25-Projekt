[back](index.md)

# Datatypes

alongside the usual c/c++ datatypes the n64 conventions can (and should) be used.


## Unsigned

| Type | Syntax | Correlation |
|-|-|-|
|single bit|unsigned|bool (both valid)|
|byte|u8|unsigned byte|
|16-bit data|u16|unsigned short|
|32-bit data|u32|unsigned int|
|64-bit data|u64|unsigned long|


## Signed

| Type | Syntax | Correlation |
|-|-|-|
|signed byte|s8|byte|
|signed short|s16|short|
|signed integer|s32|int|
|signed long|s64|long|


## Floating Point

| Type | Syntax | Correlation |
|-|-|-|
|single precision|f32|float|
|double precision|f64|double|


## Vector

| Type | Syntax | Correlation |
|-|-|-|
|2D single precision vector|vec2|Vector2f|
|3D single precision vector|vec3|Vector3f|
|4D single precision vector|vec4|Vector4f|
|hamilton quaternion|quat|Quaternion|
|4x4 matrix|mat4|Matrix4x4f|


## Complex Data

| Type | Syntax | Correlation |
|-|-|-|
|character string|string|std::string|
