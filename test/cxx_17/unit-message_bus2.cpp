#include <catch2/catch.hpp>

#include <gnl/message_bus2.h>
#include <stdio.h>
#include <iostream>

using namespace gnl;


struct M1
{
    int x;
};
struct M2
{
    int y;
};

struct CallFlags_t
{
    bool static_function_1_called = false;
    bool static_function_2_called = false;
    bool static_function_3_called = false;
};

static CallFlags_t call_flags;

void static_function1(M1 const & m)
{
    assert(&m);
    call_flags.static_function_1_called = true;
}
void static_function2(M1 const & m)
{
    assert(&m);
    call_flags.static_function_2_called = true;
}
void static_function3(M2 const & m)
{
    assert(&m);
    call_flags.static_function_3_called = true;
}

class Object
{
public:
    uint32_t counter=0;

    void method(M1 const & m)
    {
        counter++;
    }
};

TEST_CASE("Connect multiple static function to the message bus.")
{
    gnl::msgBus MsgBus;


    WHEN("We connect function1 function2 and function3 to the message bus")
    {
        auto s1 = MsgBus.connect<M1>( static_function1 ); // static function increments the counter
        auto s2 = MsgBus.connect<M1>( static_function2 ); // static function increments the counter
        auto s3 = MsgBus.connect<M2>( static_function3 ); // static function increments the counter

        THEN("The Return type is a pair of < message type_index, index in vector >")
        {
            REQUIRE( s1.first == std::type_index(typeid(M1)) ) ;
            REQUIRE( s1.second == 0 ) ;

            REQUIRE( s2.first == std::type_index(typeid(M1)) ) ;
            REQUIRE( s2.second == 1 ) ;

            REQUIRE( s3.first == std::type_index(typeid(M2)) ) ;
            REQUIRE( s3.second == 0 ) ;

            WHEN("S2 is removed")
            {
                MsgBus.disconnect(s1);

                auto s4 = MsgBus.connect<M1>( static_function2 );
                REQUIRE( s4.first == std::type_index(typeid(M1)) ) ;
                REQUIRE( s4.second == 0 ) ;
            }
        }


        WHEN("When we send message M1 to the bus")
        {
            call_flags = CallFlags_t();

            MsgBus.send( M1() );

            THEN("Only the functions which take M1 get called")
            {
                REQUIRE( call_flags.static_function_1_called == true);
                REQUIRE( call_flags.static_function_2_called == true);
                REQUIRE( call_flags.static_function_3_called == false);
            }
        }
        WHEN("When we send message M2 to the bus")
        {
            call_flags = CallFlags_t();

            MsgBus.send( M2() );

            THEN("Only the functions which take M2 get called")
            {
                REQUIRE( call_flags.static_function_1_called == false);
                REQUIRE( call_flags.static_function_2_called == false);
                REQUIRE( call_flags.static_function_3_called == true);
            }
        }
        WHEN("When we disconnect function 2")
        {
            call_flags = CallFlags_t();

            REQUIRE( call_flags.static_function_1_called == false);
            REQUIRE( call_flags.static_function_2_called == false);
            REQUIRE( call_flags.static_function_3_called == false);

            MsgBus.disconnect(s2);

            MsgBus.send( M1() );
            MsgBus.send( M2() );

            AND_THEN("Only function 1 gets called")
            {
                REQUIRE( call_flags.static_function_1_called == true);
                REQUIRE( call_flags.static_function_2_called == false);
                REQUIRE( call_flags.static_function_3_called == true);
            }
        }
        AND_WHEN("We push messages onto bus")
        {
            call_flags = CallFlags_t();

            MsgBus.push( M1() );
            MsgBus.push( M2() );

            THEN("The messages aren't called.")
            {
                REQUIRE( call_flags.static_function_1_called == false);
                REQUIRE( call_flags.static_function_2_called == false);
                REQUIRE( call_flags.static_function_3_called == false);

                WHEN("We call dispatch() ")
                {
                    MsgBus.dispatch();

                    THEN("The messages are called")
                    {
                        REQUIRE( call_flags.static_function_1_called == true);
                        REQUIRE( call_flags.static_function_2_called == true);
                        REQUIRE( call_flags.static_function_3_called == true);
                    }
                }
            }
        }
    }
}


TEST_CASE("Connecting Lambdas")
{

    GIVEN("A message bus  ")
    {
        gnl::msgBus MsgBus;

        THEN("We can connect lambda functions to it")
        {
            bool called = false;
            auto f1= [&](M1 const & M)
            {
                assert(&M);
                called = true;
            };

            MsgBus.connect<M1>(f1);

            MsgBus.send(M1());

            REQUIRE( called == true);
        }
    }

}


TEST_CASE("Connecting Class methods")
{
    GIVEN("A message bus and a class with a method")
    {
        gnl::msgBus MsgBus;

        struct CX
        {
        public:
            bool called = false;
            void slot(M1 const & M)
            {
                assert(&M);
                called = true;
            }
        };


        THEN("We can connect a class method to it by using std::bind")
        {
            CX cx;

            MsgBus.connect<M1>( std::bind(&CX::slot, &cx, std::placeholders::_1));

            WHEN("We send a message to the message bus")
            {
                MsgBus.send(M1());
                THEN("The method gets called")
                {
                    REQUIRE( cx.called == true);
                }
            }

        }
    }

}


TEST_CASE("Using push/dispatch to delay execution")
{
    GIVEN("A message bus and two slots, each sending a message to the other via push()")
    {
        gnl::msgBus MsgBus;

        struct CX
        {
        public:
            gnl::msgBus * bus;

            CX(gnl::msgBus & b) : bus(&b){}

            bool slot1called = false;
            bool slot2called = false;

            void slot1(M1 const & M)
            {
                assert(&M);
                slot1called = true;
                bus->push( M2() );
            }

            void slot2(M2 const & M)
            {
                slot2called = true;
                bus->push( M1() );
            }
        };

        CX cx(MsgBus);

        MsgBus.connect<M1>( std::bind(&CX::slot1, &cx, std::placeholders::_1) );
        MsgBus.connect<M2>( std::bind(&CX::slot2, &cx, std::placeholders::_1) );

        WHEN("When we push a message onto the bus")
        {
            MsgBus.push( M1() );
            THEN("Dispatching the message, will only call one of the functions")
            {
                MsgBus.dispatch();
                REQUIRE( cx.slot1called == true);
                REQUIRE( cx.slot2called == false);

                MsgBus.dispatch();
                REQUIRE( cx.slot2called == true);
            }
        }

    }

}
