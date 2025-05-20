#ifndef MATH_H
#define MATH_H

#include <features.h>

typedef float float_t;
typedef double double_t;

#define NAN	(0.0f/0.0f)

#define M_PI	3.1415926535897931

#define HUGE_VAL  (1e10000)
#define HUGE_VALF (1e10000f)
#define HUGE_VALL (1e10000l)

#define INFINITY HUGE_VALF

#define	FP_INFINITE 1
#define	FP_NAN  0
#define	FP_NORMAL   4
#define	FP_SUBNORMAL    3
#define	FP_ZERO 2   

int __fpclassifyf(float);
int __fpclassifyd(double);
int __fpclassifyl(long double);
int __signbitf(float);
int __signbitd(double);
double log(double x);
float logf(float x);
long double logl(long double x);
double exp(double x);
float expf(float x);
long double expl(long double x);
double atan(double x);
float atanf(float x);
long double atanl( long double x);
double acos(double x);
float acosf(float x);
long double acosl(long double x);
double log10(double x);
float log10f(float x);
long double log10l(long double x);
double cosh(double x);
float coshf(float x);
long double coshl(long double x);
double sin(double x);
float sinf(float x);
long double sinl(long double x);
double sinh(double x);
float sinhf(float x);
long double sinhl(long double x);
double cos(double x);
float cosf(float x);
long double cosl(long double x);
double asin(double x);
float asinf(float x);
long double asinl(long double x);
double atan2(double y, double x);
float atan2f(float y, float x);
long double atan2l(long double y, long double x);
double log1p(double x);
float log1pf(float x);
long double log1pl(long double x);
double hypot(double x, double y);
float hypotf(float x, float y);
long double hypotl(long double x, long double y);
double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);
double fmod(double x, double y);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);
double fabs(double x);
float fabsf(float x);
long double fabsl(long double x);
double copysign(double x, double y);
float copysignf(float x, float y);
long double copysignl(long double x, long double y);
double scalbln(double x, long exp);
float scalblnf(float x, long exp);
long double scalblnl(long double x, long exp);
double scalbn(double x, int exp);
float scalbnf(float x, int exp);
long double scalbnl(long double x, int exp);

#define fpclassify(x) ( \
        (sizeof(x) == sizeof(float)) ? __fpclassifyf(x) : \
        (sizeof(x) == sizeof(double)) ? __fpclassifyd(x) : \
         __fpclassifyl(x))
#define isnan(x) (fpclassify(x) == FP_NAN ? 1 : 0)
#define isinf(x) (fpclassify(x) == FP_INFINITE ? 1 : 0)
#define isnormal(x) (fpclassify(x) == FP_NORMAL ? 1 : 0)
#define signbit(x) ( \
         (sizeof(x) == sizeof(float)) ? __builtin_signbitf(x) : \
         (sizeof(x) == sizeof(double)) ? __builtin_signbit(x) : \
         __builtin_signbitl(x))



#endif
// vim: set ft=c:
