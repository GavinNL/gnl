#ifndef GNL_ENTITY_SYSTEM_H
#define GNL_ENTITY_SYSTEM_H

#include <vector>
#include <array>
#include <any>
#include <limits>
#include <map>
#include <functional>
#include <tuple>
#include <type_traits>
#include <iostream>

namespace gnl
{

template<typename T>
class handle
{
    public:
    using value_type = size_t;

    value_type m_handle;

        handle() : m_handle(std::numeric_limits<value_type>::max()){}

        explicit handle(value_type s) : m_handle(s)
        {
        }

        handle(const handle & other) : m_handle( other.m_handle)
        {

        }
        handle(handle && other) : m_handle(other.m_handle)
        {
            other.clear();
        }
        handle & operator=(handle const & other)
        {
            m_handle = other.m_handle;
            return *this;
        }
        handle & operator=(handle && other)
        {
            m_handle  = std::move(other.m_handle);
            return *this;
        }
        operator bool() const
        {
            return m_handle !=  std::numeric_limits<value_type>::max();
        }

        void clear()
        {
            m_handle = std::numeric_limits<value_type>::max();
        }

        value_type get() const { return m_handle;}
        void set(value_type v) {m_handle=v;}

};

class Entity
{
    protected:
        using raw_handle_type = size_t;

    public:
        using handle_type = size_t;
        using component_container_type = std::array<raw_handle_type, 4>;

        handle_type              m_handle;
        component_container_type m_components;

        Entity()
        {
            m_components.fill( std::numeric_limits<raw_handle_type>::max() );
        }

        handle_type handle() const
        {
            return m_handle;
        }

        template<typename _Component>
        bool has() const
        {
            return m_components[ _Component::ID ] != std::numeric_limits<handle_type>::max();
        }

        template<typename _Component>
        handle_type component_handle() const
        {
            return m_components[ _Component::ID ];
        }

        bool is_destroyed() const
        {
            return m_handle == std::numeric_limits<raw_handle_type>::max();
        }
    friend class EntitySystem;
};

template<uint32_t componentID>
struct ComponentBase
{
    using handle_type = size_t;
    enum id_enumtype : uint32_t
    {
        ID = componentID
    };
    handle_type m_parent_entity_handle = std::numeric_limits<handle_type>::max();
    handle_type m_handle = std::numeric_limits<handle_type>::max();

    handle_type parent_handle() const
    {
        return m_parent_entity_handle;
    }

    handle_type handle() const
    {
        return m_handle;
    }

    bool is_destroyed() const
    {
        return handle() == std::numeric_limits<handle_type>::max();
    }
    bool is_attached() const
    {
        return parent_handle() != std::numeric_limits<handle_type>::max();
    }
    bool has_parent() const
    {
        return parent_handle() != std::numeric_limits<handle_type>::max();
    }

};

class HandleWrapper;

template<typename T>
struct CompInfo
{
    using handle_type       = size_t;
    using component_id_type = size_t;
    using value_type        = T;
    using container_type    = std::vector<value_type>;

    container_type           m_items;
    std::vector<handle_type> m_free;

    value_type & get(handle_type i)
    {
        return m_items.at(i);
    }
    value_type const & get(handle_type i) const
    {
        return m_items.at(i);
    }

    handle_type new_handle()
    {
        if( m_free.size() )
        {
            auto handle = m_free.back();
            m_free.pop_back();
            m_items.at(handle) = value_type();
            m_items.at(handle).m_handle = handle;
            return handle;
        }
        m_items.emplace_back();
        m_items.back().m_handle = m_items.size()-1;
        return m_items.size()-1;
    }

    void destroy_handle(handle_type handle)
    {
        m_items.at(handle) = value_type();
        m_free.push_back(handle);
    }


    friend class EntitySystem;
};

class EntitySystem
{
public:
    using handle_type = size_t;
    using component_id_type = uint32_t;

    EntitySystem()
    {
        m_CompInfo.resize(64);
    }

    template<typename ComponentType>
    handle_type NewComponent(handle_type entity_handle)
    {
        using CompInfoType = CompInfo<ComponentType>;

        CompInfoType & Comp = get_component_info<ComponentType>();

        auto component_handle = Comp.new_handle();

        auto & E = get_entity(entity_handle);
        auto & C = get_component<ComponentType>(component_handle);

        attach(E , C);

        return component_handle;
    }

    /**
     * @brief DestroyComponent
     * @param entity_handle
     *
     * Destroy the compoennt, ComponentType, of entity, entity_handle.
     */
    template<typename ComponentType>
    void DestroyComponent(handle_type entity_handle)
    {
        using CompInfoType = CompInfo<ComponentType>;

        CompInfoType & Comp = get_component_info<ComponentType>();

        auto & E = get_entity(entity_handle);

        auto componentHandle = E.component_handle<ComponentType>();

        _destroy_component_t<ComponentType>(componentHandle);
    }

    template<typename ComponentType>
    CompInfo<ComponentType> & get_component_info()
    {
        using CompInfoType = CompInfo<ComponentType>;

        if( !m_CompInfo[ComponentType::ID].has_value() )
        {
            m_CompInfo[ComponentType::ID] = CompInfoType();

            CompInfoType & Comp = std::any_cast<CompInfoType&>( m_CompInfo[ ComponentType::ID ] );

            // lambda function to detach a component by its component ID rather than it
            // template class
            _detach_from_parent[ComponentType::ID] = [&](handle_type ComponentHandle)
            {
                //std::cout << "        _detach_from_parent["<<ComponentType::ID<<"](" << ComponentHandle << ");" << std::endl;;
                auto & C = get_component<ComponentType>(ComponentHandle);
                auto & E = get_entity(C.m_parent_entity_handle);

                if(is_attached(E,C) )
                    detach(E,C);
            };
            _destroy_component[ComponentType::ID] = [&](handle_type ComponentHandle)
            {
                _destroy_component_t<ComponentType>(ComponentHandle);
            };
            return Comp;
        }
        else
        {
            CompInfoType & Comp = std::any_cast<CompInfoType&>( m_CompInfo[ ComponentType::ID ] );
            return Comp;
        }
    }

    std::map<component_id_type, std::function<void(handle_type)> > _detach_from_parent;
    std::map<component_id_type, std::function<void(handle_type)> > _destroy_component;
    /**
     * @brief attach
     * @param E
     * @param comp
     *
     * Attaches an entity to a component. This should be protected.
     */
    template<typename Component_t>
    static void attach(Entity & E, Component_t & comp)
    {
        E.m_components[Component_t::ID] = comp.m_handle;
        comp.m_parent_entity_handle = E.m_handle;
    }

    /**
     * @brief detach
     * @param E
     * @param comp
     *
     * Detaches a component from an entity but does not
     * destroy it the component.
     */
    template<typename Component_t>
    static void detach(Entity & E, Component_t & comp)
    {
        if( E.is_destroyed() || comp.is_destroyed() )
        {
            throw std::runtime_error("Either entity or component is already destroyed");
            return;
        }

        if( E.m_handle == comp.m_parent_entity_handle )
        {
            //std::cout << "      Detaching Entity " << E.m_handle << " from Component " << comp.m_handle << std::endl;
            E.m_components[Component_t::ID] = std::numeric_limits<handle_type>::max();
            comp.m_parent_entity_handle     = std::numeric_limits<handle_type>::max();
            return;
        }
        throw std::runtime_error("Entity is not a parent of the component");
    }

    template<typename Component_t>
    static bool is_attached(Entity & E, Component_t & comp)
    {
        return comp.parent_handle() == E.handle();
    }
    /**
     * @brief DestroyComponent<T>
     * @param entity_id
     *
     * Destroyes the Component, T, for entity id returning its id to be used for something else
     */
    template<typename ComponentType>
    void _destroy_component_t(handle_type component_handle)
    {
       if( component_handle == std::numeric_limits<handle_type>::max() )
       {
           throw std::runtime_error("Invalid component handle");
       }

       auto & c = get_component<ComponentType>(component_handle);

       if( c.has_parent() )
       {
           auto & E = get_entity( c.parent_handle() );
           detach( E, c); // detach the component from its parent
          // throw std::runtime_error("This component is currently attached to an entity");
       }

       if( !c.is_destroyed() )
       {
          //std::cout << "_destroy_component_t: component is not destroyed...destroying" << std::endl;
          get_component_info<ComponentType>().destroy_handle(component_handle);
       }

    }


    /**
     * @brief NewEntity
     * @return
     *
     * Create a new empty entity and returns the handle to it.
     */
    handle_type NewEntity()
    {
        return handle_type(m_entities.new_handle());
    }

    /**
     * @brief DestroyEntity
     * @param entity_handle
     *
     * Destroy the entity. This will destroy all components attached
     * to the entity as well.
     */
    void DestroyEntity(handle_type entity_handle)
    {
        //std::cout << "Destroying entity: " << entity_handle << std::endl;
        auto & E = get_entity(entity_handle);// m_entities.get(entity_id);
        // loop trhough all the components
        component_id_type ComponentID=0;
        for(auto component_handle : E.m_components)
        {
            // detaches
            if( component_handle != std::numeric_limits<handle_type>::max() )
            {
                //std::cout << "   Detaching Entity " << entity_handle << " from Component-"<<ComponentID<<", " << component_handle  << std::endl;
                _detach_from_parent[ComponentID](component_handle);
                _destroy_component[ComponentID](component_handle);
            }

            ComponentID++;
        }
        m_entities.destroy_handle(entity_handle);
    }
    Entity & get_entity(handle_type entity_id)
    {

        return m_entities.get(entity_id);
    }
    template<typename Component_t>
    Component_t & get_component(handle_type componentHandle)
    {
        using CompInfoType = CompInfo<Component_t>;
        return std::any_cast<CompInfoType&>( m_CompInfo[ Component_t::ID ] ).get(componentHandle);
    }


    /**
     * @brief find
     * @return returns a vector of tuples
     *
     * returns a vector of tuples where each tuple is of hte form:
     *
     * std::tuple<Entity&, Component1&, Component&2...>
     *
     *  auto entities = ECS.find<Physics, Graphics>();
     *  for(auto & [E, P, G] : entities)
     *  {
     *  }
     */
    template<typename T1, typename... T2>
    auto find( )
    {
        auto cv = get_component_vectors<T1, T2...>();

        std::vector<std::tuple<Entity&, T1&, T2&... >> return_value;

        for(auto & c1 : std::get<0>(cv) )
        {
            auto entity_h = c1.parent_handle();

            auto & E = get_entity(entity_h);

            auto b = _has<T1, T2...>(E);
            if(b)
            {
                return_value.emplace_back( std::tuple_cat( std::tuple<Entity&>(E), _get_reference_to_components_as_tuple<T1,T2...>(E) ) );
            }
        }
        return return_value;
    }

    /**
     * @brief find
     * @return returns a vector of tuples
     *
     * returns a vector of tuples where each tuple is of hte form:
     *
     * std::tuple<Entity&, Component1&, Component&2...>
     *
     *  auto entities = ECS.find<Physics, Graphics>();
     *  for(auto & [E, P, G] : entities)
     *  {
     *  }
     */
    //
    template<typename T1, typename... T2>
    void for_all( std::function< void(Entity&, T1&, T2&...)> callable )
    {
        auto cv = get_component_vectors<T1, T2...>();

        std::vector<std::tuple<Entity&, T1&, T2&... >> return_value;

        for(auto & c1 : std::get<0>(cv) )
        {
            auto entity_h = c1.parent_handle();

            auto & E = get_entity(entity_h);

            auto b = _has<T1, T2...>(E);
            if(b)
            {
                auto params = std::tuple_cat(std::tuple<Entity&>(E), _get_reference_to_components_as_tuple<T1,T2...>(E) );
                std::apply(callable , params);
            }
        }
    }

    template<typename comp_t>
    typename CompInfo<comp_t>::container_type & get_component_vector()
    {
        using ComponentType = comp_t;
        using CompInfoType = CompInfo<ComponentType>;

        return std::any_cast<CompInfoType&>(m_CompInfo[ comp_t::ID ]).m_items;
    }

    std::vector< std::any>      m_CompInfo; // a vector of CompInfo<T>
    CompInfo<Entity>            m_entities;

    protected:


public:

    template<typename T>
    auto get_component_vectors()
    {
        auto f1 = std::make_tuple( get_component_vector<T>() );
        return f1;
    }

    template<typename T, typename T2, typename... Args>
    auto get_component_vectors()
    {
        auto f = std::make_tuple( get_component_vector<T>() );
        auto g = get_component_vectors<T2, Args...>();

        return std::tuple_cat(f,g);
    }

    //=====================================================================================
    // Recursive template function to get a tuple of references to all components of the
    // Entity
    //
    // eg:
    //          auto & E = get_entity(id);
    //          auto   C = _get_reference_to_components_as_tuple<Physics, Graphics, Sound>(E);
    //
    // If the entity does not have the components specified, it is undefined behaviour.
    //=====================================================================================
    template<typename T>
    auto _get_reference_to_components_as_tuple(Entity & E)
    {
        auto f1 = std::tuple<T&>( get_component_vector<T>()[E.m_components[T::ID]] );
        return f1;
    }

    template<typename T, typename T2, typename... Args>
    auto _get_reference_to_components_as_tuple(Entity & E)
    {
        auto f = std::tuple<T&>( get_component_vector<T>()[E.m_components[T::ID]] );
        auto g = _get_reference_to_components_as_tuple<T2, Args...>(E);

        return std::tuple_cat(f,g);
    }
    //=====================================================================================

    template<typename T>
    bool _has(Entity const & E) const
    {
        return E.has<T>();
    }

    template<typename T, typename T2, typename... Args>
    bool _has(Entity const & E) const
    {
        return _has<T>(E) && _has<T2, Args...>(E);
    }


};

#define COMPONENT_ID(N) enum id_enumtype : uint32_t { ID = N }; size_t m_parent_entity_handle=std::numeric_limits<size_t>::max(); size_t m_handle =std::numeric_limits<size_t>::max()

}

#endif

