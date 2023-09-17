#ifndef MATH_H
#define MATH_H

#include <features.h>

#define NAN	(0.0f/0.0f)
#define INFINITY (__builtin_inff ())

#define isnan(x) ((x) == NAN ? 1 : 0)

#define M_PI	3.1415926535897931

#define isinf(x) (false)

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
#endif
// vim: set ft=c:
