#include <iostream>
#include <gnl/gnl_periodic.h>

using namespace std;
using namespace gnl;

void process(int x )
{
    std::cout << "Function Called: " << x << std::endl;
    //return 0;
}

int main( )
{

    gnl::Periodic P;

    // Call process(3), 15 times, once every second.
    P.interval( 1.0 ).count(15).start( process, 3);

    // sleep for 10 seconds, so only 10 calls of process(3) will be called.
    std::this_thread::sleep_for( std::chrono::seconds(10 ) );

    return 0;
}
