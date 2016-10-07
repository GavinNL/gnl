#ifndef GNL_INTERPOLATION_H
#define GNL_INTERPOLATION_H


#include <vector>

namespace gnl
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
struct CubicHermiteInterpolant
{

    CubicHermiteInterpolant() {}

    CubicHermiteInterpolant(const T & P0, const T & M0 , const T & P1 , const T & M1) : p0(P0),
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
struct LinerSpline
{
    LinerSpline() {}


    _T  operator()(FloatingType t)
    {
        return get(t);
    }

    _T get(FloatingType t)
    {
        if( nodes.size() < 3)
            return _T(0);


        const auto t0 = nodes[i].first;//times[i];
        const auto t1 = nodes[i+1].first;//times[i+1];

        if( t > t1 )
        {
            __UpdateInterpolant(i+1);
            return (*this)(t);
        }
        else if( t < t0 )
        {
            __UpdateInterpolant(i-1);
            return (*this)(t);
        }


        const auto & p0 = nodes[i].second;
        const auto & p1 = nodes[i+1].second;

        t = (t-t0) * invT;

        return p0 * ( static_cast<FloatingType>(1.0) - t) + p1*t;
    }

    void Insert( FloatingType t, _T value )
    {
        nodes.push_back( std::pair<FloatingType,_T>(t,value) );
    }

    std::pair<FloatingType,_T> & operator[](std::size_t j)
    {
        return nodes[j];
    }

    void __UpdateInterpolant(int j)
    {
        assert( j < nodes.size() - 1 );
        i = j;
        invT   = 1.0f / ( nodes[i+1].first - nodes[i].first );
    }

    FloatingType invT = 1.0f;
    int          i    = 0;

    //std::vector<_T>             points;
    //std::vector<FloatingType>   times;

    std::vector< std::pair<FloatingType,_T> >   nodes;

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
struct CubicSpline
{
    CubicSpline() {}

    CubicSpline(const std::vector<float> & pT, const std::vector<_T> & p) : points(p), times(pT)
    {
        assert( pT.size() > 3);

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

    const std::vector<_T>          & points;
    const std::vector<float>       & times;

    CubicHermiteInterpolant<_T>    interp;
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
struct Bezier
{
    Bezier( const std::vector<T> & p ) : points(p)
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
