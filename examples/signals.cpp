#include <iostream>
#include <gnl/gnl_signals.h>
#include <assert.h>
#include <thread>
#include <cstdlib>


void function(int x, float y)
{
    std::cout << std::this_thread::get_id() << ": " << "Function: " << x << ", " << y << std::endl;
}

struct St
{
    void method(int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Member Function: " << x << ", " << y << std::endl;
    }

    static void static_method(int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Static Member Function: " << x << ", " << y << std::endl;
    }

};




void SignalsExample()
{
    std::cout << "================================" << std::endl;
    std::cout << " Signals Example" << std::endl;
    std::cout << "================================" << std::endl;
    gnl::Signal<void(int,float)> S1;

    // Save the slots, they are disconnected
    // when their destructor is called.
    auto s1 = S1.Connect(function);
    auto s2 = S1.Connect(St::static_method);

    auto s3 = S1.Connect(
                [&S1](int x, float y)
                    {
                        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << std::endl;
                    }
                );

    {
        St MyStruct;

        auto s4 = S1.Connect( std::bind(&St::method, &MyStruct, std::placeholders::_1, std::placeholders::_2) ) ;

        // slots will disconnect themselves once they go out of scope
        std::cout << "==== Will call 4 functions ====" << std::endl;
        S1(3 , 3.14159f);
    }



    std::cout << "====== Will call 3 functions: =====" << std::endl;
    S1(3,3.14159f);

}


void Signals2Example()
{
    std::cout << "================================" << std::endl;
    std::cout << " Dispatcher Example" << std::endl;
    std::cout << "================================" << std::endl;
    gnl::DispatcherSignal<void(int,float)> S1;

    // Save the slots, they are disconnected
    // when their destructor is called.
    auto s1 = S1.Connect(function);
    auto s2 = S1.Connect(St::static_method);

    auto s3 = S1.Connect(
                [ &S1](int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << ". Calling Signal again wont cause infinte loop for Signals2" << std::endl;
        S1(4,999.f);
    }
                );
    {
        St MyStruct;

        auto s4 = S1.Connect( std::bind(&St::method, &MyStruct, std::placeholders::_1, std::placeholders::_2) ) ;

        // slots will disconnect themselves once they go out of scope
        std::cout << "Three slots queued up" << std::endl;
        S1(3 , 3.14159f);
    }




    std::cout << "Dispatching the slots ." << std::endl;
    S1.Dispatch();


    std::cout << "Dispatching a second time will call the function queued up by the lamda function." << std::endl;
    S1.Dispatch();



}

void ThreadedSignalsExample()
{
    std::cout << "================================" << std::endl;
    std::cout << " Threaded Signals Example" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "The invoked slots are called in their own thread." << std::endl << std::endl;

    gnl::ThreadedSignal<void(int,float)> S1;

    // Save the slots, they are disconnected
    // when their destructor is called.
    auto s1 = S1.Connect(function);
    auto s2 = S1.Connect(St::static_method);

    auto s3 = S1.Connect(
                [&S1](int x, float y)
                    {
                        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << std::endl;
                    }
                );

    {
        St MyStruct;

        auto s4 = S1.Connect( std::bind(&St::method, &MyStruct, std::placeholders::_1, std::placeholders::_2) ) ;

        // slots will disconnect themselves once they go out of scope
        std::cout << "==== Will call 4 functions ====" << std::endl;
        S1(3 , 3.14159f);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }



    std::cout << "====== Will call 3 functions: =====" << std::endl;
    S1(3,3.14159f);
    std::this_thread::sleep_for(std::chrono::seconds(1));

}

void function2(int x , float y )
{

}

void MutexTests()
{
    std::cout << "================================" << std::endl;
    std::cout << " Mutex tests " << std::endl;
    std::cout << "================================" << std::endl;

    using SignalType = gnl::ThreadedSignal<void(int,float)>;
    using SlotType   = gnl::ThreadedSignal<void(int,float)>::Slot;

    SignalType Signal;
    auto s1 = std::make_shared<SlotType>();
    auto s2 = std::make_shared<SlotType>();
    auto s3 = std::make_shared<SlotType>();



    auto L1 = [&s1](int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(  2 ));
        if( s1->Disconnect() )
        {
            std::cout << "Slot 1 Disconnected" << std::endl;
        }
    };

    auto L2 = [&s2](int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(  4 ) );
        if( s2->Disconnect() )
        {
            std::cout << "Slot 2 Disconnected" << std::endl;
        }
    };

    auto L3 = [&s3](int x, float y)
    {
        std::cout << std::this_thread::get_id() << ": " << "Lambda Function: " << x << ", " << y << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(  6 ) );
        if( s3->Disconnect() )
        {
            std::cout << "Slot 3 Disconnected" << std::endl;
        }
    };
    // Save the slots, they are disconnected
    // when their destructor is called.
    *s1 = std::move( Signal.Connect( L1 ) );
    *s2 = std::move( Signal.Connect( L2 ) );
    *s3 = std::move( Signal.Connect( L3 ) );

    for(int i=0; i < 10; i++)
    {
        //std::cout << "Calling " << Signal.NumSlots() << " slots" << std::endl;
        Signal(3,3.14159f);
        std::this_thread::sleep_for(std::chrono::milliseconds(434));

    }

}


int main(int argc, char ** argv)
{
    //MutexTests(); // giving errors
    SignalsExample();

    Signals2Example();

    ThreadedSignalsExample();
    return 0;
}
