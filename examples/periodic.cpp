#include <iostream>
#include <gnl/gnl_periodic.h>

using namespace std;
using namespace gnl;

int process(int x )
{
    static int i=0;
    int j = i++;
    std::cout << "Process started: " << j << std::endl;
    std::this_thread::sleep_for( std::chrono::seconds( rand() % 2 + 3) );
    std::cout << "Process ended: " << j  << std::endl;
    return 0;
}

struct Add
{
    void inc(int y) {
        x+=y;
    }

    int x = 0;
};

int main( )
{
    //if(1)
    //{
    //    {
    //        std::cout << "*****Testing Spawner mode:" << std::endl;
    //        std::cout << "  " << std::endl;
    //
    //        std::cout << "***** Spawning a new thread every second for 50 spawns" << std::endl;
    //        auto T = Periodic::Start_Spawner_Delayed<int>( std::bind( &process , 3 ) , 1000000, 250 );
    //
    //        std::cout << "*****Sleep for 10 seconds... Only 10 threads will be spawned" << std::endl;
    //        std::this_thread::sleep_for( std::chrono::seconds(10) );
    //
    //        std::cout << "*****Setting spawn interval to 100ms" << std::endl;
    //        T->SetInterval( std::chrono::milliseconds(100) );
    //
    //        std::cout << "*****Sleeping for another 10 seconds" << std::endl;
    //        std::this_thread::sleep_for( std::chrono::seconds(10) );
    //
    //        std::cout << "*****Exiting scope. Attempting to Destruct the Periodic." << std::endl;
    //        std::cout << "*****No more threads will be spawned. Wait till current threads finish" << std::endl;
    //    }
    //    std::cout << "*****Scope exited. All threads finished ** " << std::endl;
    //
    //}
    //
    //{
    //    Add add;
    //
    //    std::cout << "Calling the add::inc function once every second with the parameter argument = 1." << std::endl;
    //
    //    auto T2 = Periodic::Start<void>( std::bind( &Add::inc, &add , 1 ) , 1000000, 10 );
    //
    //    std::cout << "Sleeping for 5 seconds, then going to stop" << std::endl;
    //
    //    std::this_thread::sleep_for( std::chrono::seconds(5) );
    //
    //    std::cout << " Calling Stop() on Periodic" << std::endl;
    //    T2->Stop();
    //
    //    std::cout << "Value of Add::x = " << add.x << std::endl;
    //
    //}


    //std::cout << "Currently running threads: " << T->Running_Threads() << std::endl;
    return 0;
}
