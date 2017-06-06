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
    std::cout << " Signals2 Example" << std::endl;
    std::cout << "================================" << std::endl;
    gnl::Signal2<void(int,float)> S1;

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

int main(int argc, char ** argv)
{
    SignalsExample();

    Signals2Example();

    ThreadedSignalsExample();
    return 0;
}
