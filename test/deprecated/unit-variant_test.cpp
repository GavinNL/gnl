#include <gnl/path.h>
#include <gnl/variant.h>

#include<catch2/catch.hpp>



class Test
{
    public:
        Test()
        {
            std::cout << "TEST Test Constructor" << std::endl;
        }
        ~Test()
        {
            std::cout << "TEST Test destructor" << std::endl;
        }

        Test(const Test &T )
        {
            (void)T;
            std::cout << "TEST Copy Constructor" << std::endl;
        }
        Test( Test && T )
        {
            (void)T;
            std::cout << "TEST Move Constructor" << std::endl;
        }

        Test& operator=( const Test & T )
        {
            (void)T;
            std::cout << "TEST Copy Operator" << std::endl;
            return *this;
        }

        Test& operator=( Test && T )
        {
            (void)T;
            std::cout << "TEST move Operator" << std::endl;
            return *this;
        }
        int x;

};

class TestThrow
{
    public:
        TestThrow()
        {
            throw std::runtime_error("test");
        }


};
TEST_CASE( "Variant Test" )
{

    // variant holds an int, float, char, and a shared_ptr<int>

    gnl::Variant<int, float, char, Test, std::shared_ptr<int>, TestThrow > D;
    gnl::Variant<int, float, char, Test, std::shared_ptr<int>, TestThrow > D2;

    // make D an int
    D = 3;

    REQUIRE( D.is<int>() );
    REQUIRE( D.as<int>() == 3 );

    D2 = std::move(D);

    REQUIRE( D2.is<int>() );
    REQUIRE( !D.is<int>() );

    REQUIRE( D2.as<int>() == 3 );

//    std::cout << D2 << std::endl;
    // make D a char
    D = 'V';

    REQUIRE( D.is<char>() );
    REQUIRE( D.as<char>() == 'V' );


    // make D a Test class
    D = Test();
    D.as<Test>().x = 3;

    REQUIRE( D.is<Test>() );
    REQUIRE( D.as<Test>().x == 3 );


    try
    {
        D = TestThrow();
    }
    catch (std::exception & e)
    {
        (void)e;
    }

    REQUIRE(D.as<int>() ==3);

    // Make D a shared_ptr<int>
    D = std::make_shared<int>(5);

    // make a weak pointer out of the shared ptr.
    std::weak_ptr<int> w = D.as<std::shared_ptr<int> >();


    REQUIRE( *D.as<std::shared_ptr<int>>() == 5 );

    // weak pointer should be able to hold the lock
    REQUIRE( w.lock() );

    // Make D an int again
    D = 2; // this should delete the shared pointer, and therefore the weak pointer should now point to nothing

    REQUIRE( !w.lock() );

}
