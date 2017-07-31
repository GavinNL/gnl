/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#ifndef GNL_INTERPOLATION_H
#define GNL_INTERPOLATION_H

#include <cassert>
#include <vector>
#include <cmath>
#include <stdexcept>

#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{

template<typename T, typename FloatingType=float>
/**
 * @brief The CubicHermiteInterpolant struct
 *
 * Given start control point P0 with derivative M0
 * and end control point P1 with derivative M1
 *
 * Calculate the interpolant on the interval 0..1
 */
struct cubic_hermite_interpolant
{

    cubic_hermite_interpolant() {}

    cubic_hermite_interpolant(const T & P0, const T & M0 , const T & P1 , const T & M1) : p0(P0),
                                                                       p1(P1),
                                                                       m0( M0 ),
                                                                       m1( M1 )
    {

    }

    T get(FloatingType t) const
    {
        const FloatingType omt  = 1-t;
        const FloatingType omt2 = omt*omt;
        const FloatingType t2   = t*t;

        #if 1
            return omt2*( p0 * (1+2*t) + m0*t )  + t2 * ( (3-2*t)*p1 - m1 * omt  );
        #else
            const float f0  =  (1+2*t)*omt2;
            const float f1  =  t*omt2;
            const float f2  =  t2*(3-2*t);
            const float f3  = -t2*omt;


            return p0*f0 + m0*f1 + p1*f2 + f3*m1;
        #endif

    }

    T operator()(FloatingType t) const
    {
        return get(t);
    }

    T p0;
    T m0;

    T m1;
    T p1;

};

template<typename _T, typename FloatingType=float>
/**
 * @brief The CubicSpline struct
 * Given a vector of times and a vector of points
 *
 * The cubic spline class will interpolate values inbetween
 *
 * This class does not store a copy of the vectors, it uses
 * the vectors as a reference.
 */
struct linear_spline
{
    linear_spline(std::vector<FloatingType> const & time, std::vector<_T> const & x) : t(time.data()), y(x.data()), size( std::min(time.size(),x.size()))
    {
    }

    linear_spline( FloatingType const * time, _T const * x, std::size_t length) : t(time), y(x), size( length )
    {
    }

    _T  operator()(FloatingType time)
    {
        return get(time);
    }

    _T get(FloatingType time)
    {
        if( size < 3)
            return _T(0);

        const auto t0 = t[i];//nodes[i].first;//times[i];
        const auto t1 = t[i+1];  //nodes[i+1].first;//times[i+1];
        //const auto t0 = nodes[i].first;//times[i];
        //const auto t1 = nodes[i+1].first;//times[i+1];

        if( time > t1 )
        {
            __UpdateInterpolant(i+1);
            return (*this)(time);
        }
        else if( time < t0 )
        {
            __UpdateInterpolant(i-1);
            return (*this)(time);
        }


        const auto & p0 = y[i];
        const auto & p1 = y[i+1];

        time = (time-t0) * invT;

        return p0 * ( static_cast<FloatingType>(1.0) - time) + p1*time;
    }

    void __UpdateInterpolant(int j)
    {
        assert( j < (int)size - 1 );
        i = j;
        invT   = 1.0f / ( t[i+1] - t[i] );
    }

    FloatingType invT = 1.0f;
    int          i    = 0;

    FloatingType const * t;
    _T           const * y;
    std::size_t       size;
};


template<typename _T, typename FloatingType=float>
/**
 * @brief The linear_spline2 struct
 *
 * Newer implementation testing this out.
 */
struct linear_spline2
{
    linear_spline2(std::vector<FloatingType> const & time, std::vector<_T> const & x) : linear_spline2( time.data(), x.data(), std::min(time.size(),x.size()))
    {
    }

    linear_spline2( FloatingType const * time, _T const * x, std::size_t length) :
        t0(time), t1(time+1), y0(x),y1(x+1), last(t0+length)
    {
    }

    _T  operator()(FloatingType time)
    {
        return get(time);
    }

    _T at(FloatingType time)
    {
        if( time>*last)
            throw std::runtime_error("Parameter out of range");

        return get(time);
    }

    _T get(FloatingType time)
    {
        auto old = t0;
        while(time > *t1)
        {
            ++t0;++t1;
            ++y0;++y1;
        }
        while(time < *t0)
        {
            --t0;--t1;
            --y0;--y1;
        }

        if(!(old-t0)) invT = static_cast<FloatingType>(1.0) / ( *t1 - *t0 );

        time = (time-*t0) * invT;

        return std::fma(time, *y1, fma(-time, *y0, *y0));

    }

    FloatingType invT = 1.0f;

    FloatingType const * t0;
    FloatingType const * t1;
    _T           const * y0;
    _T           const * y1;

    _T const * last;

};

template<typename _T>
/**
 * @brief The CubicSpline struct
 * Given a vector of times and a vector of points
 *
 * The cubic spline class will interpolate values inbetween
 *
 * This class does not store a copy of the vectors, it uses
 * the vectors as a reference.
 */
struct cubic_spline
{
    cubic_spline() {}

    cubic_spline(const std::vector<float> & pT, const std::vector<_T> & p) : points(p), times(pT)
    {
       // assert( pT.size() > 3);

        //for(auto P : p) points.push_back(P);
        //for(auto T : t) times.push_back(T);
        i = 0;
        __UpdateInterpolant(i);
    }


    _T  operator()(float s)
    {
        if( s > t1 )
        {
            __UpdateInterpolant(i+1);
            return (*this)(s);
        }
        else if( s < t0 )
        {
            __UpdateInterpolant(i-1);
            return (*this)(s);
        }

        return interp( (s-t0) * invT );
    }

    _T __der(int j)
    {
        if( j == 0)
        {
            return (points[j+1]-points[j])/(times[j+1]-times[j]);
        }
        else if( j == times.size()-1)
        {
            return (points[j]-points[j-1]) / (times[j]-times[j-1]);
        }
        return (points[j+1]-points[j-1])/(times[j+1]-times[j-1]);
    }

    void __UpdateInterpolant(int j)
    {
        assert( j < times.size() - 1);

        i = j;

        interp.m0 = __der(i);
        interp.m1 = __der(i+1);

        interp.p0 = points[i];
        interp.p1 = points[i+1];

        t0        = times[i];
      //  t1        = times[i+1];

        invT         = 1.0f / (times[i+1] - times[i]);
    }

    float t0;
    float t1;
    float t    = 0.0f;
    float invT = 1.0f;
    int   i    = 0;

    std::vector<_T>           points;
    std::vector<float>        times;


    cubic_hermite_interpolant<_T>    interp;
};



/**
 * @brief The Bezier struct
 *
 * Bezier curve class. Given a reference to a vector of nodes
 * It will interpolate the values in between.
 *
 * This class is simply a wrapper around a vector of points.
 *
 */
template<typename T, typename FloatingType=float>
struct bezier
{
    bezier( const std::vector<T> & p ) : points(p)
    {
    }

    T operator()(FloatingType x) const
    {
        return B(0, (int)points.size()-1, x);
    }

    T get(float x) const
    {
        return B(0, (int)points.size()-1, x);
    }

    template<typename _t>
    T B(int i, int n, _t x) const
    {
        if(i-n)
            return ( static_cast<FloatingType>(1)-x)*B(i,n-1,x) + x*B(i+1, n,x);
        else
            return points[i];

    }

    const std::vector<T> & points;
};


}
#endif // GNL_INTERPOLATION_H
