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
        gnl::thread_pool P(5);

        std::cout << "Pushing tasks" << std::endl;

        for(int i=0;i<30;i++)
            P.push( task , i);

        std::cout << "Tasks pushed" << std::endl;


        std::this_thread::sleep_for( std::chrono::seconds( 10 ));
        P.remove_thread();
        P.remove_thread();
        P.remove_thread();
        P.remove_thread();
        P.remove_thread();
    }
    catch( std::exception & e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
