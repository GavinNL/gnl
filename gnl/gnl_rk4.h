#ifndef GNL_RK4_H
#define GNL_RK4_H

#include <array>
#include <cmath>

// Runge-Kutta 4th order solver


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{

template<typename T, int N, typename Time_T>
class rk4
{
    public:
        using vec_type      = std::array<T,N>;
        using time_type     = Time_T;
        typedef void (*function_type)(vec_type & out, time_type t, vec_type const & X );

        vec_type k1;
        vec_type k2;
        vec_type k3;
        vec_type k4;

        vec_type kt;

        /**
         * @brief update
         * @param output
         * @param y - starting value array
         * @param t - the time value to start at
         * @param delta_t - the timestep
         * @param F - function which has the following signature
         *            F(vec_type & out, time_type t, vec_type const & x)
         *
         *            The function takes the input vector x, and returns the output
         *            vector, out, whose elements are the derivatives of x with respect to
         *            time.
         *
         * Perform one iteration of a RK4 timestep
         */
        void update(  vec_type & y , time_type t, time_type delta_t, function_type F)
        {
            F(k1,t,y);

            //for(int i=0;i<N;++i) kt[i] = y[i] + delta_t * static_cast<time_type>(0.5) * k1[i];
            for(int i=0;i<N;++i) kt[i] = std::fma(delta_t*0.5, k1[i] , y[i]);

            F(k2, t+delta_t * static_cast<time_type>(0.5), kt );

            //for(int i=0;i<N;++i) kt[i] = y[i] + delta_t * static_cast<time_type>(0.5 )*k2[i];
            for(int i=0;i<N;++i) kt[i] = std::fma( 0.5*delta_t, k2[i], y[i]);

            F(k3 , t+delta_t * static_cast<time_type>(0.5), kt );

            //for(int i=0;i<N;++i) kt[i] = y[i] + delta_t*k3[i];
            for(int i=0;i<N;++i) kt[i] = std::fma( delta_t, k3[i], y[i] );

            F(k4 , t+delta_t    , kt );

           // for(int i=0;i<N;++i) y[i] += (delta_t/static_cast<time_type>(6.0)) * ( k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);

            for(int i=0;i<N;++i) y[i] = std::fma( (delta_t/static_cast<time_type>(6.0)) , ( std::fma(2,k2[i],k1[i]) + std::fma( 2, k3[i] , k4[i]) ), y[i]);
        }
};


}

#endif
