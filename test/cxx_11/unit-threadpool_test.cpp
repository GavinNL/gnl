#include <gnl/threadpool.h>

#include<catch2/catch.hpp>
#include <string>
#include <gnl/threadpool.h>

int longTime(int s)
{
    std::cout << "longTime() start" << std::endl;
    std::this_thread::sleep_for( std::chrono::seconds(s) );
    std::cout << "longTime() end" << std::endl;

    return s+2;
}

TEST_CASE( "Testing thread pool class" )
{

    gnl::thread_pool T(2);

    {
        std::cout << "Scope Opened" << std::endl;
        auto f = T.push( longTime, 5);

        //std::cout << "Returned: "<< f.get() << std::endl;
    }
    std::cout << "Scope closed" << std::endl;
    // coming soon
}
