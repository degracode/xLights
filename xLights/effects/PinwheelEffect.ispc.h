//
// C:\Projects\xLights\xLights\effects\PinwheelEffect.ispc.h
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#pragma once
#include <stdint.h>

#if !defined(__cplusplus)
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#else
typedef int bool;
#endif
#endif



#ifdef __cplusplus
namespace ispc { /* namespace */
#endif // __cplusplus
///////////////////////////////////////////////////////////////////////////
// Enumerator types with external visibility from ispc code
///////////////////////////////////////////////////////////////////////////

#ifndef __ISPC_ENUM_Pinwheel3DType__
#define __ISPC_ENUM_Pinwheel3DType__
enum Pinwheel3DType {
    PW_3D_NONE = 0,
    PW_3D = 1,
    PW_3D_Inverted = 2,
    PW_SWEEP = 3 
};
#endif


#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif

#ifndef __ISPC_STRUCT_xlColor__
#define __ISPC_STRUCT_xlColor__
struct xlColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};
#endif


///////////////////////////////////////////////////////////////////////////
// Functions exported from ispc code
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
extern "C" {
#endif // __cplusplus
    extern void pinwheelEffectKernel(struct xlColor * outputColumn, uint8_t * outputColumnIsSet, int32_t bufferHt, int32_t x, int32_t x1, int32_t xc_adj, int32_t yc_adj, int32_t max_radius, int32_t pinwheel_twist, int32_t pinwheel_rotation, int32_t pinwheel_arms, int32_t poffset, int32_t degrees_per_arm, float pos, enum Pinwheel3DType pw3dType, float tmax, bool bufferAllowAlpha, const struct xlColor * colorsAsColor);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus
