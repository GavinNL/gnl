#include <iostream>
#include <thread>

#include <gnl/gnl_threadpool.h>


void task(int count)
{
    std::this_thread::sleep_for( std::chrono::seconds( 1 + rand()%3 ));
}

int main(int argc, char ** argv)
{
    try
    {
        gnl::thread_pool P(8);

        std::cout << "Pushing tasks" << std::endl;

        for(int i=0;i<20;i++)
            P.push( task , i);

        std::cout << "Tasks pushed" << std::endl;


        std::this_thread::sleep_for( std::chrono::seconds( 20 ));

    }
    catch( std::exception & e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
