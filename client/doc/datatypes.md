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
|full system register length|__system_word|void*|


## BitwiseWords

structure to store boolean arrays next to each other in a block that benefits the cpu

usage
```c++
u32 number_of_booleans = 16;
BitwiseWords arr = BitwiseWords(number_of_booleans);  // create array
u8 i = arr[4] ? 1 : 0;  // access boolean in array
arr.set(4);  // set boolean in array to true
arr.unset(4);  // set boolean in array to false
arr.reset();  // set all bits to false
```


## InPlaceArray

array that allows to overwrite and extract the most optimal memory position \
!never use this in a self-allocating data structure!

usage
```c++
u32 maximum_number_of_entities = 16;
InPlaceArray<T> arr = InPlaceArray<T>(maximum_number_of_entities);  // create array
T* ex = arr.next_free();  // get next free memory address
T ex = arr.mem[4];  // extract value from memory
arr.active_range = 4;  // resize the active range
arr.overwrites.push(4);  // sign an index as free and overwritable
```


## ThreadSignal

structure to guarantee thread safety and to streamline signaling

usage \
thread 1
```c++
ThreadSignal signal;
// do preparing things
signal.proceed();
// do terminating things
signal.exit();
```

thread 2
```c++
while (signal.running)
{
    signal.wait();
    // do depending things
}
```


## Rect

defines a rectangle that can mathematically interact with other geometry

usage
```c++
Rect rect = {
    .position = vec2(200,200),  // lower left position
    .extent = vec2(600,600)  // upper right position
};
bool its = rect.intersect(vec2(400,400))  // tests intersection with given point
```
