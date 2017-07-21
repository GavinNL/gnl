#include <iostream>
#include <assert.h>
#include <thread>

#include <cstdlib>
#include <array>

#include <gnl/gnl_rk4.h>

#include <iomanip>
#define G  -9.81
#define V0 1500.0
#define DT 0.01
#define TOTAL_TIME 1000

int main(int argc, char ** argv)
{
    using Solver = gnl::rk4< double , 3, double >;
    Solver R;

    Solver::vec_type y;
    y[0] = 0.0;
    y[1] = V0;

    auto U = []( Solver::vec_type       & dU,  // output change vector
                 Solver::time_type         t,  // time value
                 Solver::vec_type const &  U)  // input vector
    {
        // Let U[1] be the velocity of the projectile
        // Let U[0] be the position of the projectile

        dU[0] = U[1]; // // The change in position is equal to the velocity
        dU[1] = G;//  U[1]*0.5;  // the change in velocity is equal to the gravitational
    };


    // Actual solution
    auto Y = [](Solver::time_type t)
    {
        return V0*t + 0.5*G*t*t;
    };


    #define FFORMAT(width,decimals, float_val) std::fixed << std::setw(width) << std::setprecision(decimals) << (float_val)

    Solver::time_type t = 0;


    while(t < TOTAL_TIME)
    {
        R.update(y, t, DT, U);
        t += DT;
    }
    std::cout << "RK4: " <<  FFORMAT(12,6,y[0]) << "   Actual: " << FFORMAT(12,6,Y(t)) << "       error: " << FFORMAT(16,18, y[0]-Y(t) ) << std::endl;


    return 0;
}


