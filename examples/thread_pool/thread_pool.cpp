#include <iostream>
#include <thread>

#include <gnl/gnl_threadpool.h>


void task(int count)
{
    std::this_thread::sleep_for( std::chrono::seconds( 1 + rand()%3 ));
}


int get_x(int time_to_wait)
{
    std::this_thread::sleep_for( std::chrono::seconds( time_to_wait ));
    std::cout << "finished" << std::endl;
    return time_to_wait;
}

int main(int argc, char ** argv)
{
    try
    {
        gnl::thread_pool P;

        std::cout << "Pushing tasks" << std::endl;

        for(int i=0;i<10;i++)
            P.push( task , i);



        std::cout << "Tasks pushed" << std::endl;

        P.create_workers(4);

        std::this_thread::sleep_for( std::chrono::seconds( 20 ));

        auto f1 = P.push( get_x, 2);
        auto f2 = P.push( get_x, 4);

        std::cout << f1.get() * f2.get() << std::endl;

    }
    catch( std::exception & e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
