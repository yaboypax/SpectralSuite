/*
  ==============================================================================
    
    Based on Dozius' cplxf
    
   "// adjacent complex
	//
	// I wrote this class because std::complex didn't optimize very well in vc6 (when I checked the
	// asm code it had rubbish that wouldn't optimize away) "
    

  ==============================================================================
*/

#pragma once
#include <complex>

struct cplxd
{
	// element 0 is real, element 1 is data
	double data[2];

	cplxd() {}
	cplxd(const double& r, const double& i = 0.0f) { data[0] = r; data[1] = i; }
	cplxd operator () (const double& r, const double& i) { data[0] = r; data[1] = i; return *this; }
	cplxd(const std::complex<double>& src) { data[0] = src.real(); data[1] = src.imag(); }

	double real() const { return data[0]; }
	double imag() const { return data[1]; }

	double& real() { return data[0]; }
	double& imag() { return data[1]; }

	double& setReal(const double& r) { return data[0] = r; }
	double& setImag(const double& i) { return data[1] = i; }

	// auto-cast to std::complex
	operator std::complex<double>() { return std::complex<double>(data[0], data[1]); }

	cplxd operator*(const cplxd& b) const { return cplxd(data[0] * b.data[0] - data[1] * b.data[1], data[0] * b.data[1] + data[1] * b.data[0]); }
	cplxd operator+(const cplxd& b) const { return cplxd(data[0] + b.data[0], data[1] + b.data[1]); }
	cplxd operator-(const cplxd& b) const { return cplxd(data[0] - b.data[0], data[1] - b.data[1]); }
	cplxd operator*(const double& b) const { return cplxd(data[0] * b, data[1] * b); }
	cplxd operator+(const double& b) const { return cplxd(data[0] + b, data[1]); }
	cplxd operator-(const double& b) const { return cplxd(data[0] - b, data[1]); }
	cplxd operator-() const { return cplxd(-data[0], -data[1]); }
	bool operator==(const cplxd& b) const { return data[0] == b.data[0] && data[1] == b.data[1]; }
	bool operator!=(const cplxd& b) const { return data[0] != b.data[0] || data[1] != b.data[1]; }

	template <class T> cplxd& operator += (const T& b) { return *this = *this + b; }
	template <class T> cplxd& operator -= (const T& b) { return *this = *this - b; }
	template <class T> cplxd& operator *= (const T& b) { return *this = *this * b; }
};


inline cplxd operator-(const double& a, const cplxd& b) { return cplxd(a - b.data[0], a - b.data[1]); }
inline cplxd operator*(const double& a, const cplxd& b) { return b * a; }
inline cplxd operator+(const double& a, const cplxd& b) { return b + a; }


inline double norm(const cplxd& a) { return a.real() * a.real() + a.imag() * a.imag(); }
inline double angle(const cplxd& a) { return atan2f(a.imag(), a.real()); }
inline double abs(const cplxd& a) { return sqrtf(norm(a)); }

inline cplxd polar_to_cplxd(const double& mag, const double& ang) { return mag * cplxd(cosf(ang), sinf(ang)); }
inline cplxd conj(const cplxd& src) { return cplxd(src.data[0], -src.data[1]); }

// element wise multiply
inline cplxd el_mul(const cplxd& a, const cplxd& b) { return cplxd(a.real() * b.real(), a.imag() * b.imag()); }


#define cplxd_I cplxd(0.0f, 1.0f)

// can linear interpolate
inline cplxd lin_interp(const double& frac, const cplxd& out_min, const cplxd& out_max)
{
	return out_min + (out_max - out_min) * frac;
}

//-------------------------------------------------------------------------------------------------
struct cplxdPtrPair
	// pair of pointers to cplxd (use for defining an array range)
{
	cplxd* a, * b;

	cplxdPtrPair() {}

	cplxdPtrPair(cplxd* start, long len = 0)
	{
		a = start;
		b = a + len;
	}

	cplxdPtrPair(cplxd* start, long b0, long b1)
	{
		a = start + b0;
		b = start + b1;
	}

	bool equal() const { return a == b; }

	// these operators dereference "a"
	cplxd& operator * () { return *a; }
	cplxd& operator ()() { return *a; }

	// return number of elements in length (assume "b" points to one after last element in range)
	int size() { return (int)(b - a); }
};