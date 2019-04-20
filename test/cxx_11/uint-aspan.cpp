#include <gnl/array_alias.h>

#include<catch2/catch.hpp>

#include <iostream>
#include<array>
#include <cmath>

struct vec3
{
    uint32_t x;
    uint32_t y;
    uint32_t z;

    vec3()
    {

    }

    vec3(uint32_t _x, uint32_t _y, uint32_t _z) : x(_x), y(_y), z(_z)
    {

    }
};

TEST_CASE( "Constructing with iterators")
{
    std::vector<uint32_t> int_vec;

    GIVEN("A vector of uint32_t with 6 elements")
    {
        int_vec.push_back(1);
        int_vec.push_back(2);
        int_vec.push_back(3);
        int_vec.push_back(4);
        int_vec.push_back(5);
        int_vec.push_back(6);

        WHEN("We construct an aspan using vec3 as the aliased type")
        {
            gnl::aspan<vec3> R( std::begin(int_vec), std::end(int_vec) );

            THEN("The size of the span is 2")
            {
                REQUIRE( R.size() == 2);
            }
            THEN("We can access access each 3-consecutive values as a vec3")
            {
                REQUIRE( R[0].x == 1);
                REQUIRE( R[0].y == 2);
                REQUIRE( R[0].z == 3);

                REQUIRE( R[1].x == 4);
                REQUIRE( R[1].y == 5);
                REQUIRE( R[1].z == 6);
            }
            WHEN("We reverse the span.")
            {
                R.reverse();
                THEN("The size remains at 2")
                {
                    REQUIRE( R.size() == 2);
                }
                THEN("The elements are reversed in the span")
                {
                    REQUIRE( R[0].x == 4);
                    REQUIRE( R[0].y == 5);
                    REQUIRE( R[0].z == 6);

                    REQUIRE( R[1].x == 1);
                    REQUIRE( R[1].y == 2);
                    REQUIRE( R[1].z == 3);
                }
            }
        }
        WHEN("We construct the reverse view using end-3 and begin-3 iterators")
        {
            // we need to subtract 3 from each iterator because
            // vec3 contains 3 floats
            gnl::aspan<vec3> R( std::end(int_vec)-3, std::begin(int_vec)-3);

            THEN("The size is 2")
            {
                REQUIRE(R.size() == 2);
            }

            THEN("The elements are reversed")
            {
                REQUIRE( R[0].x == 4);
                REQUIRE( R[0].y == 5);
                REQUIRE( R[0].z == 6);

                REQUIRE( R[1].x == 1);
                REQUIRE( R[1].y == 2);
                REQUIRE( R[1].z == 3);
            }
        }

        WHEN("We construct the aspan with the begin+1 and end iterators")
        {
            gnl::aspan<vec3> R( std::begin(int_vec)+1, std::end(int_vec));

            THEN("The size is only 1 because the iterators are not aligned properly")
            {
                REQUIRE( R.size() == 1);
            }

            THEN("The the x value of the first element is acutally the second element in the original data")
            {
                REQUIRE( R[0].x == 2);
                REQUIRE( R[0].y == 3);
                REQUIRE( R[0].z == 4);
            }
        }
    }
}

TEST_CASE( "Constructing with const containers")
{
    std::vector<uint32_t> int_vec1;

    int_vec1.push_back(1);
    int_vec1.push_back(2);
    int_vec1.push_back(3);
    int_vec1.push_back(4);
    int_vec1.push_back(5);
    int_vec1.push_back(6);

    GIVEN("A const vector of uint32_t with 6 elements")
    {
        auto const & int_vec = int_vec1;

        WHEN("We construct an aspan using vec3 as the aliased type")
        {
            gnl::aspan<const vec3> R( std::begin(int_vec), std::end(int_vec) );

            THEN("The size of the span is 2")
            {
                REQUIRE( R.size() == 2);
            }
            THEN("We can access access each 3-consecutive values as a vec3")
            {
                REQUIRE( R[0].x == 1);
                REQUIRE( R[0].y == 2);
                REQUIRE( R[0].z == 3);

                REQUIRE( R[1].x == 4);
                REQUIRE( R[1].y == 5);
                REQUIRE( R[1].z == 6);
            }
            WHEN("We reverse the span.")
            {
                R.reverse();
                THEN("The size remains at 2")
                {
                    REQUIRE( R.size() == 2);
                }
                THEN("The elements are reversed in the span")
                {
                    REQUIRE( R[0].x == 4);
                    REQUIRE( R[0].y == 5);
                    REQUIRE( R[0].z == 6);

                    REQUIRE( R[1].x == 1);
                    REQUIRE( R[1].y == 2);
                    REQUIRE( R[1].z == 3);
                }
            }
        }
        WHEN("We construct the reverse view using end-3 and begin-3 iterators")
        {
            // we need to subtract 3 from each iterator because
            // vec3 contains 3 floats
            gnl::aspan<const vec3> R( std::end(int_vec)-3, std::begin(int_vec)-3);

            THEN("The size is 2")
            {
                REQUIRE(R.size() == 2);
            }

            THEN("The elements are reversed")
            {
                REQUIRE( R[0].x == 4);
                REQUIRE( R[0].y == 5);
                REQUIRE( R[0].z == 6);

                REQUIRE( R[1].x == 1);
                REQUIRE( R[1].y == 2);
                REQUIRE( R[1].z == 3);
            }
        }

        WHEN("We construct the aspan with the begin+1 and end iterators")
        {
            gnl::aspan<const vec3> R( std::begin(int_vec)+1, std::end(int_vec));

            THEN("The size is only 1 because the iterators are not aligned properly")
            {
                REQUIRE( R.size() == 1);
            }

            THEN("The the x value of the first element is acutally the second element in the original data")
            {
                REQUIRE( R[0].x == 2);
                REQUIRE( R[0].y == 3);
                REQUIRE( R[0].z == 4);
            }
        }
    }
}

#if 1


TEST_CASE( "Alias a vector<vec3> into a vector of <uint32>" )
{

    GIVEN("A vector of 2 vec3s")
    {
        std::vector<vec3> int_vec;

        int_vec.push_back({1,2,3});
        int_vec.push_back({4,5,6});

        THEN("We can alias it into 6 vec<uint32_t>")
        {
            gnl::aspan< uint32_t > X( std::begin(int_vec), std::end(int_vec));
            REQUIRE( X.size() == 6);

            REQUIRE( X[0] == 1);
            REQUIRE( X[1] == 2);
            REQUIRE( X[2] == 3);

            REQUIRE( X[3] == 4);
            REQUIRE( X[4] == 5);
            REQUIRE( X[5] == 6);

            WHEN("We can sort the view")
            {
                std::sort( X.begin(), X.end() , std::greater<uint32_t>());

                THEN("The base data is changed")
                {
                    REQUIRE( int_vec[0].x == 6);
                    REQUIRE( int_vec[0].y == 5);
                    REQUIRE( int_vec[0].z == 4);

                    REQUIRE( int_vec[1].x == 3);
                    REQUIRE( int_vec[1].y == 2);
                    REQUIRE( int_vec[1].z == 1);
                }
            }
        }

    }

}

TEST_CASE( "Alias a vector<vec3> into a 3 aliased vectors" )
{
    GIVEN("A vector of 2 vec3s")
    {
        std::vector<vec3> int_vec;

        int_vec.push_back({1,2,3});
        int_vec.push_back({4,5,6});

        THEN("We can create 3 aliased uint32_t spans which are aligned to the individual memeber variables")
        {
            gnl::aspan< uint32_t > X(std::begin(int_vec), std::end(int_vec), offsetof(vec3, x) , sizeof(vec3));
            gnl::aspan< uint32_t > Y(std::begin(int_vec), std::end(int_vec), offsetof(vec3, y) , sizeof(vec3));
            gnl::aspan< uint32_t > Z(std::begin(int_vec), std::end(int_vec), offsetof(vec3, z) , sizeof(vec3));

            REQUIRE( X.size() == 2);
            REQUIRE( Y.size() == 2);
            REQUIRE( Z.size() == 2);

            REQUIRE( X[0] == 1);
            REQUIRE( X[1] == 4);

            REQUIRE( Y[0] == 2);
            REQUIRE( Y[1] == 5);

            REQUIRE( Z[0] == 3);
            REQUIRE( Z[1] == 6);
        }
    }
}


TEST_CASE( "Testing iterators" )
{
    GIVEN("A vector of 2 vec3s")
    {
        std::vector<vec3> int_vec;

        int_vec.push_back({1,2,3});
        int_vec.push_back({4,5,6});

        THEN("We can create 3 aliased uint32_t vectors which are aligned to the individual memeber variables")
        {
            gnl::aspan< uint32_t > X(int_vec, offsetof(vec3, x) , sizeof(vec3));
            gnl::aspan< uint32_t > Y(int_vec, offsetof(vec3, y) , sizeof(vec3));
            gnl::aspan< uint32_t > Z(int_vec, offsetof(vec3, z) , sizeof(vec3));

            REQUIRE( X.size() == 2);
            REQUIRE( Y.size() == 2);
            REQUIRE( Z.size() == 2);

            REQUIRE( X[0] == 1);
            REQUIRE( X[1] == 4);

            REQUIRE( Y[0] == 2);
            REQUIRE( Y[1] == 5);

            REQUIRE( Z[0] == 3);
            REQUIRE( Z[1] == 6);

            THEN("We can get iterators to the start and end")
            {
                auto b = std::begin(X);
                auto e = std::end(X);

                WHEN("We take the difference of two iterators")
                {
                    auto diff = e-b;

                    REQUIRE(diff == 2);

                    REQUIRE(std::distance(b,e) == 2);
                    REQUIRE(std::distance(e,b) == -2);
                }

                WHEN("We preincrement")
                {
                    REQUIRE(*++b == 4);
                    REQUIRE(*--b == 1);
                }
                WHEN("We post increment")
                {
                    REQUIRE(*b++ == 1);
                    REQUIRE(*b-- == 4);
                }

                WHEN("Adding to iterator")
                {
                    auto c = b + 1;
                    auto d = c - 1;
                    REQUIRE( *c == 4 );
                    REQUIRE( *d == 1 );
                }
            }

            THEN("We can get iterators to the start and end")
            {
                auto b = std::begin(Y);
                auto e = std::end(Y);

                WHEN("We take the difference of two iterators")
                {
                    auto diff = e-b;

                    REQUIRE(diff == 2);
                }

                WHEN("We preincrement")
                {
                    REQUIRE(*++b == 5);
                    REQUIRE(*--b == 2);
                }
                WHEN("We post increment")
                {
                    REQUIRE(*b++ == 2);
                    REQUIRE(*b-- == 5);
                }

                WHEN("Adding to iterator")
                {
                    auto c = b + 1;
                    auto d = c - 1;
                    REQUIRE( *c == 5 );
                    REQUIRE( *d == 2 );
                }
            }
            THEN("We can get iterators to the start and end")
            {
                auto b = std::begin(Z);
                auto e = std::end(Z);

                WHEN("We take the difference of two iterators")
                {
                    auto diff = e-b;

                    REQUIRE(diff == 2);
                }

                WHEN("We preincrement")
                {
                    REQUIRE(*++b == 6);
                    REQUIRE(*--b == 3);
                }
                WHEN("We post increment")
                {
                    REQUIRE(*b++ == 3);
                    REQUIRE(*b-- == 6);
                }

                WHEN("Adding to iterator")
                {
                    auto c = b + 1;
                    auto d = c - 1;
                    REQUIRE( *c == 6 );
                    REQUIRE( *d == 3 );
                }
            }
        }
    }
}


TEST_CASE( "Test Sorting" )
{
    GIVEN("A vector of 2 vec3s")
    {
        std::vector<vec3> int_vec;

        int_vec.push_back({1,2,3});
        int_vec.push_back({4,5,6});

        gnl::aspan< uint32_t > X(int_vec, offsetof(vec3, x) , sizeof(vec3));
        gnl::aspan< uint32_t > Y(int_vec, offsetof(vec3, y) , sizeof(vec3));
        gnl::aspan< uint32_t > Z(int_vec, offsetof(vec3, z) , sizeof(vec3));


        WHEN("We sort X")
        {
            std::sort( std::begin(X), std::end(X), std::greater<uint32_t>() );

            THEN("Only the X vector gets sorted")
            {
                REQUIRE( X[0] == 4);
                REQUIRE( X[1] == 1);

                REQUIRE( Y[0] == 2);
                REQUIRE( Y[1] == 5);

                REQUIRE( Z[0] == 3);
                REQUIRE( Z[1] == 6);
            }
        }

        WHEN("We sort Y")
        {
            std::sort( std::begin(Y), std::end(Y), std::greater<uint32_t>() );

            THEN("Only the Y vector gets sorted")
            {
                REQUIRE( X[0] == 1);
                REQUIRE( X[1] == 4);

                REQUIRE( Y[0] == 5);
                REQUIRE( Y[1] == 2);

                REQUIRE( Z[0] == 3);
                REQUIRE( Z[1] == 6);
            }
        }

        WHEN("We sort Z")
        {
            std::sort( std::begin(Z), std::end(Z), std::greater<uint32_t>() );

            THEN("Only the Z vector gets sorted")
            {
                REQUIRE( X[0] == 1);
                REQUIRE( X[1] == 4);

                REQUIRE( Y[0] == 2);
                REQUIRE( Y[1] == 5);

                REQUIRE( Z[0] == 6);
                REQUIRE( Z[1] == 3);
            }
        }

    }

}


TEST_CASE( "Test Reverse")
{
    std::vector<uint32_t> int_vec;

    GIVEN("A vector of uint32_t with 6 elements")
    {
        int_vec.push_back(1);
        int_vec.push_back(2);
        int_vec.push_back(3);
        int_vec.push_back(4);
        int_vec.push_back(5);
        int_vec.push_back(6);

        gnl::aspan<uint32_t> R(int_vec);

        REQUIRE( R.size() == 6);
        REQUIRE( R[0] == 1);
        REQUIRE( R[1] == 2);
        REQUIRE( R[2] == 3);
        REQUIRE( R[3] == 4);
        REQUIRE( R[4] == 5);
        REQUIRE( R[5] == 6);

        WHEN("We reverse")
        {
            auto & RR = R;
            RR.reverse();

            REQUIRE( RR.size() == 6);

            REQUIRE( RR[0] == 6);
            REQUIRE( RR[1] == 5);
            REQUIRE( RR[2] == 4);
            REQUIRE( RR[3] == 3);
            REQUIRE( RR[4] == 2);
            REQUIRE( RR[5] == 1);


            WHEN("We sort the reverse view")
            {
                std::sort( RR.begin(), RR.end() );

                THEN("THe base type")
                {
                    REQUIRE( RR[0] == 1);
                    REQUIRE( RR[1] == 2);
                    REQUIRE( RR[2] == 3);
                    REQUIRE( RR[3] == 4);
                    REQUIRE( RR[4] == 5);
                    REQUIRE( RR[5] == 6);
                }
            }

        }
        WHEN("We reverse")
        {
            gnl::aspan<vec3> rr(int_vec);

            auto & RR = rr;
            RR.reverse();

            REQUIRE( RR.size() == 2);

            REQUIRE( RR[0].x == 4);
            REQUIRE( RR[0].y == 5);
            REQUIRE( RR[0].z == 6);

            REQUIRE( RR[1].x == 1);
            REQUIRE( RR[1].y == 2);
            REQUIRE( RR[1].z == 3);

        }
    }
    GIVEN("A vector of 2 vec3s")
    {
        std::vector<vec3> int_vec;

        int_vec.push_back({1,2,3});
        int_vec.push_back({4,5,6});

        gnl::aspan< uint32_t > X(int_vec, offsetof(vec3, x) , sizeof(vec3));
        gnl::aspan< uint32_t > Y(int_vec, offsetof(vec3, y) , sizeof(vec3));
        gnl::aspan< uint32_t > Z(int_vec, offsetof(vec3, z) , sizeof(vec3));

        X.reverse();
        Y.reverse();
        Z.reverse();

        REQUIRE(X[0] == 4);
        REQUIRE(X[1] == 1);

        REQUIRE(Y[0] == 5);
        REQUIRE(Y[1] == 2);

        REQUIRE(Z[0] == 6);
        REQUIRE(Z[1] == 3);

    }

}


#endif
