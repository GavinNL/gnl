#include <gnl/interp.h>
#include <vector>

#include<catch2/catch.hpp>
#include <string>

#include <gnl/arrayview.h>

using namespace gnl;

SCENARIO( "Standard Densely-packed array_view of a raw array" ) {

    GIVEN( "A densely packed array_view ranging from ints 0-11" )
    {
        int raw[] = {0,
                     1,
                     2,
                     3,
                     4,
                     5,
                     6,
                     7,
                     8,
                     9,
                     10,
                     11};
        auto A    = array_view<int>(raw , sizeof(raw)/sizeof(int), 1 );

        THEN("The size is 12 and the front and back refernce 0 and 11")
        {
            REQUIRE( A.size()  == 12 );
            REQUIRE( A.front() == 0  );
            REQUIRE( A.back()  == 11 );
        }
        THEN("Each element [i] == i")
        {
            REQUIRE( A[0]  == 0 );
            REQUIRE( A[1]  == 1 );
            REQUIRE( A[2]  == 2 );
            REQUIRE( A[3]  == 3 );
            REQUIRE( A[4]  == 4 );
            REQUIRE( A[5]  == 5 );
            REQUIRE( A[6]  == 6 );
            REQUIRE( A[7]  == 7 );
            REQUIRE( A[8]  == 8 );
            REQUIRE( A[9]  == 9 );
            REQUIRE( A[10] == 10);
            REQUIRE( A[11] == 11);
        }
        THEN( "Skip size == 1")
        {
            REQUIRE( A.skip() == 1);
        }
        WHEN( "the front is popped" ) {
            A.pop_front();

            THEN("The front and back references are shifted")
            {
                REQUIRE( A.size()  == 11  );
                REQUIRE( A.front() ==  1  );
                REQUIRE( A.back()  == 11  );
            }

            THEN( "The array_view now references elements 1-11" ) {
                REQUIRE( A[0]  == 1+0 );
                REQUIRE( A[1]  == 1+1 );
                REQUIRE( A[2]  == 1+2 );
                REQUIRE( A[3]  == 1+3 );
                REQUIRE( A[4]  == 1+4 );
                REQUIRE( A[5]  == 1+5 );
                REQUIRE( A[6]  == 1+6 );
                REQUIRE( A[7]  == 1+7 );
                REQUIRE( A[8]  == 1+8 );
                REQUIRE( A[9]  == 1+9 );
                REQUIRE( A[10] == 1+10);
            }
        }
        WHEN( "the back is popped" ) {
            A.pop_back();

            THEN("The front and back references are changed")
            {
                REQUIRE( A.size()  == 11 );
                REQUIRE( A.front() ==  0  );
                REQUIRE( A.back()  == 10  );
            }
            THEN( "The array_view now references elements 0-10" ) {
                REQUIRE( A[0]  == 0 );
                REQUIRE( A[1]  == 1 );
                REQUIRE( A[2]  == 2 );
                REQUIRE( A[3]  == 3 );
                REQUIRE( A[4]  == 4 );
                REQUIRE( A[5]  == 5 );
                REQUIRE( A[6]  == 6 );
                REQUIRE( A[7]  == 7 );
                REQUIRE( A[8]  == 8 );
                REQUIRE( A[9]  == 9 );
                REQUIRE( A[10] == 10);
            }


        }

        WHEN( "when A is reversed" )
        {
            A.reverse();

            THEN("The raw size is 12. The skip is -1 and the size is 12")
            {
                REQUIRE( A.raw_array_length() == -12 );
                REQUIRE( A.skip() == -1 );
                REQUIRE( A.size() == 12 );
            }
            THEN("The front and back references are reversed")
            {
                REQUIRE( A.front() == 11 );
                REQUIRE( A.back()  == 0  );
            }
            THEN( "The raw array can be referenced in reverse order using the array_view" ) {

                REQUIRE( A[11-0]   == 0 );
                REQUIRE( A[11-1]   == 1 );
                REQUIRE( A[11-2]   == 2 );
                REQUIRE( A[11-3]   == 3 );
                REQUIRE( A[11-4]   == 4 );
                REQUIRE( A[11-5]   == 5 );
                REQUIRE( A[11-6]   == 6 );
                REQUIRE( A[11-7]   == 7 );
                REQUIRE( A[11-8]   == 8 );
                REQUIRE( A[11-9]   == 9 );
                REQUIRE( A[11-10]  == 10);
                REQUIRE( A[11-11]  == 11);
            }
            THEN("Popping the front changes the size and the front() reference")
            {
                A.pop_front();
                REQUIRE( A.size()  == 11  );
                REQUIRE( A.front() == 10  );
                REQUIRE( A.back()  == 0  );
            }
            THEN("Popping the back changes the size and the back() reference")
            {
                A.pop_back();
                REQUIRE( A.size()  == 11  );
                REQUIRE( A.front() == 11  );
                REQUIRE( A.back()  == 1  );
            }
        }
        WHEN("The array_view is sliced by slice(2,5) ")
        {
            auto b = A.subarray(2, 5);

            THEN( "skip size == 1")
            {
                REQUIRE( b.skip() == 1 );
            }
            THEN( "The size changes to 5")
            {
                REQUIRE( b.size() == 5);
            }
            THEN( "The front and back refences 2 and 6")
            {
                REQUIRE( b.front() == 2);
                REQUIRE( b.back()  == 6);
            }
            THEN( "the array_view references elements 2-8 only")
            {
                REQUIRE( b[0] == 2);
                REQUIRE( b[1] == 3);
                REQUIRE( b[2] == 4);
                REQUIRE( b[3] == 5);
                REQUIRE( b[4] == 6);
            }
            WHEN( "when reversed" )
            {
                b.reverse();

                THEN("The front and back references are reversed")
                {
                    REQUIRE( b.front() == 6 );
                    REQUIRE( b.back()  == 2  );
                }
                THEN( "The raw array can be referenced in reverse order using the array_view" ) {

                    REQUIRE( b[0] == 6);
                    REQUIRE( b[1] == 5);
                    REQUIRE( b[2] == 4);
                    REQUIRE( b[3] == 3);
                    REQUIRE( b[4] == 2);
                }
            }
        }

        WHEN("The array_view is sliced by slice(0,12,2) ")
        {
            auto b = A.subarray(0, 12, 2);

            THEN( "skip size == 2")
            {
                REQUIRE( b.skip() == 2 );
            }
            THEN( "The size changes to 5")
            {
                REQUIRE( b.size() == 6);
            }
            THEN( "The front and back refences 2 and 6")
            {
                REQUIRE( b.front() == 0);
                REQUIRE( b.back()  == 10);
            }
            THEN( "the array_view references elements 2-8 only")
            {
                REQUIRE( b[0] == 0);
                REQUIRE( b[1] == 2);
                REQUIRE( b[2] == 4);
                REQUIRE( b[3] == 6);
                REQUIRE( b[4] == 8);
                REQUIRE( b[5] == 10);

                WHEN( "when reversed" )
                {
                    b.reverse();

                    THEN("The front and back references are reversed")
                    {
                        REQUIRE( b.front() == 10 );
                        REQUIRE( b.back()  == 0  );
                    }
                    THEN( "The raw array can be referenced in reverse order using the array_view" ) {

                        REQUIRE( b[5-0] == 0);
                        REQUIRE( b[5-1] == 2);
                        REQUIRE( b[5-2] == 4);
                        REQUIRE( b[5-3] == 6);
                        REQUIRE( b[5-4] == 8);
                        REQUIRE( b[5-5] == 10);
                    }
                }
            }

            WHEN( "The Sliced array is sliced again" )
            {
                auto c = b.subarray(0,6,3);

                THEN( "skip size == 6")
                {
                    REQUIRE( c.skip() == 6 );
                }
                THEN( "The size changes to 2")
                {
                    REQUIRE( c.size() == 2);
                }
                THEN( "The front and back refences 2 and 6")
                {
                    REQUIRE( c.front() == 0);
                    REQUIRE( c.back()  == 6);
                }
                THEN( "the array_view references elements 2-8 only")
                {
                    REQUIRE( c[0] == 0);
                    REQUIRE( c[1] == 6);
                }
            }
        }

        WHEN("The array_view is sliced by  b = A.slice(11,-12,2) ")
        {
            auto b = A.subarray(11, -12, 2);

            THEN( "skip size == -2")
            {
                REQUIRE( b.skip() == -2 );
            }
            THEN( "The size changes to 6")
            {
                REQUIRE( b.size() == 6);
            }
            THEN( "The front and back refences 2 and 6")
            {
                REQUIRE( b.front() == 11);
                REQUIRE( b.back()  == 1);
            }
            THEN( "the array_view references elements 2-8 only")
            {
                REQUIRE( b[0] == 11);
                REQUIRE( b[1] == 9);
                REQUIRE( b[2] == 7);
                REQUIRE( b[3] == 5);
                REQUIRE( b[4] == 3);
                REQUIRE( b[5] == 1);
            }

            WHEN( "The Sliced array is sliced again using c = b.subarray(0,6,3) ")
            {
                auto c = b.subarray(0,6,3);
                THEN( "skip size == -6")
                {
                    REQUIRE( c.skip() == -6 );
                }
                THEN( "The size changes to 2")
                {
                    REQUIRE( c.size() == 2);
                }

                THEN( "The array_view references elements 2-8 only")
                {
                    REQUIRE( c[0] == 11);
                    REQUIRE( c[1] == 5);
                }
                THEN( "The front and back refences 2 and 6")
                {
                    REQUIRE( c.front() == 11);
                    REQUIRE( c.back()  == 5 );
                }
            }
        }
    }
}
