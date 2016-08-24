#ifndef mathhelper_h__
#define mathhelper_h__


#define _TANH_RANGE   5.f
#define _TANH_CLAMP   1.f
#define _TANH_K0      21.f
#define _TANH_K1      210.f
#define _TANH_K2      1260.f
#define _TANH_K3      4725.f
#define _TANH_K4      10395.f

inline float
	fast_tanh(const float x)
{
	const     float s = x*x;
	register  float d;

	if      (x < -_TANH_RANGE)
		return -_TANH_CLAMP;
	else if (x > _TANH_RANGE)
		return _TANH_CLAMP;

	d =     (s*(s*(s + _TANH_K1) + _TANH_K3) + _TANH_K4);
	return  (x*(s*(_TANH_K0*s + _TANH_K2) + _TANH_K4)) / d;
}

#endif // mathhelper_h__