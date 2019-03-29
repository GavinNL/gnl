#include <gnl/entity_component_system.h>
#include <iostream>

#include <catch2/catch.hpp>


using namespace gnl;

struct Physics : ComponentBase<0> // should have a unique id starting at 0
{
   // COMPONENT_ID(0);

};

struct Graphics : ComponentBase<1>
{
    //COMPONENT_ID(1);
};

struct Transform : ComponentBase<2>
{
    //COMPONENT_ID(2);
};

#if 1

SCENARIO("Testing CompInfo<T>")
{
    CompInfo<Physics> C;

    REQUIRE( C.new_handle() == 0);
    REQUIRE( C.new_handle() == 1);
    REQUIRE( C.new_handle() == 2);
    REQUIRE( C.new_handle() == 3);
    REQUIRE( C.new_handle() == 4);

    C.destroy_handle(3);
    REQUIRE( C.m_free.size() == 1);
    REQUIRE( C.m_free.back() == 3);
    REQUIRE( C.new_handle() == 3);
    REQUIRE( C.m_free.size() == 0);


}

SCENARIO("Detaching components")
{
    GIVEN("An Entity System with some components attached")
    {
        EntitySystem ECS;

        auto i1 = ECS.NewEntity();
        auto i2 = ECS.NewEntity();
        auto i3 = ECS.NewEntity();
        auto i4 = ECS.NewEntity();

        // i1 has physics and graphics
        ECS.NewComponent<Physics>(i1);
        ECS.NewComponent<Graphics>(i1);

        // i2 has physics
        ECS.NewComponent<Physics>(i2);

        // i3 has physics
        ECS.NewComponent<Graphics>(i3);

        // i1 has physics and graphics
        ECS.NewComponent<Physics>(i4);
        ECS.NewComponent<Graphics>(i4);

        auto & e1 = ECS.get_entity(i1);

        THEN("p1 and e1 are attached")
        {
            auto & p1 = ECS.get_component<Physics>( e1.component_handle<Physics>() );

            REQUIRE( e1.has<Physics>() );
            REQUIRE( p1.parent_handle() == e1.handle() );
            REQUIRE( p1.is_attached() == true );
            REQUIRE( p1.is_destroyed() == false );
            REQUIRE( e1.is_destroyed() == false );

            WHEN("We detach entity, e1 from p1")
            {
                EntitySystem::detach( e1, p1);

                THEN("p1 is no longer attached()")
                {
                    REQUIRE( p1.has_parent() == false);
                    REQUIRE( p1.is_attached()  == false );
                }
                THEN("p1 and e1 are still alive")
                {
                    REQUIRE( p1.is_destroyed() == false );
                    REQUIRE( e1.is_destroyed() == false );
                }
            }
        }
    }
}



SCENARIO("Destroying Components [private functions]")
{
    GIVEN("An empty Entity System")
    {
        EntitySystem ECS;

        WHEN("We create entities with Physics and Graphics components ")
        {
            auto i1 = ECS.NewEntity();

            REQUIRE( i1 == 0);


            REQUIRE( ECS.get_entity(i1).handle() == 0 );

            // i1 has physics and graphics
            auto p1 = ECS.NewComponent<Physics>(i1);
            auto g1 = ECS.NewComponent<Graphics>(i1);

            auto & e1 = ECS.get_entity(i1);

            auto & P = ECS.get_component<Physics>(p1);
            auto & G = ECS.get_component<Graphics>(g1);

            THEN("the physics component is destroyed and the entity no longer contains a physics component")
            {
                REQUIRE( e1.has<Physics>() == true);
                REQUIRE( e1.has<Graphics>() == true);
                REQUIRE( P.parent_handle() == e1.handle() );
                REQUIRE( G.parent_handle() == e1.handle() );
                REQUIRE( P.is_attached() );
                REQUIRE( G.is_attached() );
                REQUIRE( P.has_parent() );
                REQUIRE( G.has_parent() );
            }

            WHEN("We destroy the physics component using its handle")
            {
                ECS._destroy_component_t<Physics>(p1);

                THEN("the physics component is destroyed and the entity no longer contains a physics component")
                {
                    REQUIRE( P.is_destroyed() );
                    REQUIRE( !P.is_attached() );
                    REQUIRE( !P.has_parent() );

                    REQUIRE( !G.is_destroyed() );
                    REQUIRE(  G.is_attached() );

                    REQUIRE( e1.has<Physics>() == false);
                }
                THEN("the graphics component is still attached")
                {
                    REQUIRE( !G.is_destroyed() );
                    REQUIRE(  G.is_attached()  );
                    REQUIRE(  G.has_parent()   );
                    REQUIRE(  G.parent_handle() == e1.handle() );

                    REQUIRE( e1.has<Graphics>() == true);
                }
            }
        }
    }
}

#endif

SCENARIO("Destroying Components [Public functions]")
{
    GIVEN("An empty Entity System")
    {
        EntitySystem ECS;

        WHEN("We create entities with Physics and Graphics components ")
        {
            auto i1 = ECS.NewEntity();

            REQUIRE( i1 == 0);

            REQUIRE( ECS.get_entity(i1).handle() == 0 );

            // i1 has physics and graphics
            auto p1 = ECS.NewComponent<Physics>(i1);
            auto g1 = ECS.NewComponent<Graphics>(i1);

            auto & e1 = ECS.get_entity(i1);

            auto & P = ECS.get_component<Physics>(p1);
            auto & G = ECS.get_component<Graphics>(g1);

            THEN("the physics component is destroyed and the entity no longer contains a physics component")
            {
                REQUIRE( e1.has<Physics>() == true);
                REQUIRE( e1.has<Graphics>() == true);
                REQUIRE( P.parent_handle() == e1.handle() );
                REQUIRE( G.parent_handle() == e1.handle() );
                REQUIRE( P.is_attached() );
                REQUIRE( G.is_attached() );
                REQUIRE( P.has_parent() );
                REQUIRE( G.has_parent() );
            }

            WHEN("We destroy the physics component using its handle")
            {
                ECS.DestroyComponent<Physics>(i1);

                THEN("the physics component is destroyed and the entity no longer contains a physics component")
                {
                    REQUIRE( P.is_destroyed() );
                    REQUIRE( !P.is_attached() );
                    REQUIRE( !P.has_parent() );

                    REQUIRE( !G.is_destroyed() );
                    REQUIRE(  G.is_attached() );

                    REQUIRE( e1.has<Physics>() == false);
                }
                THEN("the graphics component is still attached")
                {
                    REQUIRE( !G.is_destroyed() );
                    REQUIRE(  G.is_attached()  );
                    REQUIRE(  G.has_parent()   );
                    REQUIRE(  G.parent_handle() == e1.handle() );

                    REQUIRE( e1.has<Graphics>() == true);
                }
            }
        }
    }
}

SCENARIO("Destroying an Entity with attached Components")
{
    GIVEN("An empty Entity System")
    {
        EntitySystem ECS;

        WHEN("We create NewEntities() ")
        {
            auto i1 = ECS.NewEntity();
            auto i2 = ECS.NewEntity();
            auto i3 = ECS.NewEntity();
            auto i4 = ECS.NewEntity();

            REQUIRE( i1 == 0);
            REQUIRE( i2 == 1);
            REQUIRE( i3 == 2);
            REQUIRE( i4 == 3);

            REQUIRE( ECS.get_entity(i1).handle() == 0 );
            REQUIRE( ECS.get_entity(i2).handle() == 1 );
            REQUIRE( ECS.get_entity(i3).handle() == 2 );
            REQUIRE( ECS.get_entity(i4).handle() == 3 );

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i1);
            ECS.NewComponent<Graphics>(i1);

            // i2 has physics
            ECS.NewComponent<Physics>(i2);

            // i3 has physics
            ECS.NewComponent<Graphics>(i3);

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i4);
            ECS.NewComponent<Graphics>(i4);

            auto & e4 = ECS.get_entity(i4);

            WHEN("We destroy an entity with components")
            {
                auto & p = ECS.get_component<Physics>(  e4.component_handle<Physics>() );
                auto & g = ECS.get_component<Graphics>( e4.component_handle<Graphics>() );

                ECS.DestroyEntity(i4);
                THEN("All attached components are destroyed")
                {
                    REQUIRE( !p.is_attached() );
                    REQUIRE( !g.is_attached() );

                    REQUIRE( p.is_destroyed() );
                    REQUIRE( g.is_destroyed() );
                }
            }
        }
    }
}


#if 1


SCENARIO("Checking if Entities have components using the private template functions")
{
    GIVEN("An Entity System with some components attached")
    {
        EntitySystem ECS;

        WHEN("We create NewEntities() ")
        {
            auto i1 = ECS.NewEntity();
            auto i2 = ECS.NewEntity();
            auto i3 = ECS.NewEntity();
            auto i4 = ECS.NewEntity();

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i1);
            ECS.NewComponent<Graphics>(i1);

            // i2 has physics
            ECS.NewComponent<Physics>(i2);

            // i3 has physics
            ECS.NewComponent<Graphics>(i3);

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i4);
            ECS.NewComponent<Graphics>(i4);

            auto & e1 = ECS.get_entity(i1);
            auto & e2 = ECS.get_entity(i2);
            auto & e3 = ECS.get_entity(i3);
            auto & e4 = ECS.get_entity(i4);

            THEN("We can can check if an entity has components attached [private]")
            {


                REQUIRE( ECS._has<Physics, Graphics>(e1) );
                REQUIRE( !ECS._has<Physics, Graphics>(e2) );
                REQUIRE( !ECS._has<Physics, Graphics>(e3) );
                REQUIRE( ECS._has<Physics, Graphics>(e4) );

                REQUIRE( ECS._has<Physics>(e2) );
                REQUIRE( ECS._has<Graphics>(e3) );
            }

            THEN("We can get a reference to all it's components [private]")
            {
                auto C = ECS._get_reference_to_components_as_tuple<Physics, Graphics>(e1);

                static_assert( std::is_same< decltype( std::get<0>(C) ),  Physics&>::value   , "");
                static_assert( std::is_same< decltype( std::get<1>(C) ),  Graphics&>::value  , "");

                REQUIRE( std::get<0>(C).parent_handle() == std::get<1>(C).parent_handle() );
            }
        }
    }
}




SCENARIO("Iterating Through Entities")
{
    GIVEN("An Entity System with some components attached")
    {
        EntitySystem ECS;

        WHEN("We create NewEntities() ")
        {
            auto i1 = ECS.NewEntity();
            auto i2 = ECS.NewEntity();
            auto i3 = ECS.NewEntity();
            auto i4 = ECS.NewEntity();

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i1);
            ECS.NewComponent<Graphics>(i1);

            // i2 has physics
            ECS.NewComponent<Physics>(i2);

            // i3 has physics
            ECS.NewComponent<Graphics>(i3);

            // i1 has physics and graphics
            ECS.NewComponent<Physics>(i4);
            ECS.NewComponent<Graphics>(i4);


            THEN("We can obtain a vector of tuples of references to components")
            {
                auto entities = ECS.find<Physics, Graphics>();
                for(auto & [E, P, G] : entities)
                {
                    static_assert( std::is_same<decltype(E), Entity&>::value, "variable is not a reference to an entity");
                    static_assert( std::is_same<decltype(P), Physics&>::value, "variable is not a reference to an Physics");
                    static_assert( std::is_same<decltype(G), Graphics&>::value, "variable is not a reference to an Graphics");

                    REQUIRE( P.parent_handle() == E.handle() );
                    REQUIRE( G.parent_handle() == E.handle() );
                    REQUIRE( E.has<Physics>() );
                    REQUIRE( E.has<Graphics>() );
                    REQUIRE( E.component_handle<Physics>() == P.handle() );
                    REQUIRE( E.component_handle<Graphics>() == G.handle() );
                }
            }
            THEN("We can obtain a vector of tuples of references to components in different order")
            {
                auto entities = ECS.find<Graphics, Physics>();
                for(auto & [E, G, P] : entities)
                {
                    static_assert( std::is_same<decltype(E), Entity&>::value, "variable is not a reference to an entity");
                    static_assert( std::is_same<decltype(P), Physics&>::value, "variable is not a reference to an Physics");
                    static_assert( std::is_same<decltype(G), Graphics&>::value, "variable is not a reference to an Graphics");

                    REQUIRE( P.parent_handle() == E.handle() );
                    REQUIRE( G.parent_handle() == E.handle() );
                    REQUIRE( E.has<Physics>() );
                    REQUIRE( E.has<Graphics>() );
                    REQUIRE( E.component_handle<Physics>() == P.handle() );
                    REQUIRE( E.component_handle<Graphics>() == G.handle() );
                }
            }

            THEN("We can execute a function")
            {

                auto Lambda1 = [](Entity & E, Physics & P, Graphics & G)
                                {
                                    REQUIRE( P.parent_handle() == E.handle() );
                                    REQUIRE( G.parent_handle() == E.handle() );
                                    REQUIRE( E.has<Physics>() );
                                    REQUIRE( E.has<Graphics>() );
                                    REQUIRE( E.component_handle<Physics>() == P.handle() );
                                    REQUIRE( E.component_handle<Graphics>() == G.handle() );
                                };

                auto Lambda2 = [](Entity & E, Graphics & P, Physics & G)
                                {
                                    REQUIRE( P.parent_handle() == E.handle() );
                                    REQUIRE( G.parent_handle() == E.handle() );
                                    REQUIRE( E.has<Physics>() );
                                    REQUIRE( E.has<Graphics>() );
                                    REQUIRE( E.component_handle<Physics>() == P.handle() );
                                    REQUIRE( E.component_handle<Graphics>() == G.handle() );
                                };

                std::function< void(Entity&,Physics&,Graphics&) >  f1 = Lambda1;
                std::function< void(Entity&,Graphics&,Physics&) >  f2 = Lambda2;

                ECS.for_all<Physics, Graphics>(f1);
                ECS.for_all<Graphics, Physics>(f2);

                ECS.for_all(f1);
                ECS.for_all(f2);

            }
        }
    }
}

#endif
