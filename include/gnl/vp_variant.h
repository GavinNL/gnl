#ifndef GNL_VPVARIANT_H
#define GNL_VPVARIANT_H

#include <variant>
#include <iostream>
#include <memory>

namespace gnl
{

template<typename T>
class vp_variant;

template<typename T>
class link_ref
{
public:
    T * m_value;
    link_ref( T & v) : m_value(&v){}
};

template<typename T>
link_ref<vp_variant<T>> link( vp_variant<T> & other)
{
    return link_ref<vp_variant<T>>(other);
}

template<typename T>
class vp_variant
{
    public:
        using value_type  = T;
        using pointer_type = std::shared_ptr<value_type>;
        using const_pointer_type = std::shared_ptr<const value_type>;
        using object_type = vp_variant<T>;

        using variant_type = std::variant<value_type, pointer_type, const_pointer_type>;

        mutable variant_type m_value;

        struct ValueGetter
        {
            value_type operator()(value_type const & a) {
                return a;
            }
            value_type operator()(pointer_type const& b) {
              return *b;
            }
            value_type operator()(const_pointer_type const& b) {
              return *b;
            }
        };
        struct RefGetter
        {
            value_type& operator()(value_type  & a) {
                return a;
            }
            value_type& operator()(pointer_type & b) {
              return *b;
            }
            value_type& operator()(const_pointer_type const& b) {
                (void)b;
                throw std::runtime_error("Not implemented yet.");
              //return *b;
            }
        };
        struct ValueSetter
        {
            variant_type & me;
            ValueSetter(variant_type & ME) : me(ME){}

            value_type operator()(value_type const & a) {
                return a;
            }
            value_type operator()(pointer_type const& b) {
              return *b;
            }
        };
        struct AssignmentVisitor
        {
            object_type & me;
            object_type & other_obj;

            AssignmentVisitor(object_type & ME, object_type & other) : me(ME), other_obj(other){}

            /**
             * @brief operator ()
             * @param a
             * If the other object is a value, then we need to convert it into
             * a pointer
             */
            void operator()(value_type const & a)
            {
                other_obj.m_value = std::make_shared<value_type>(a);
                me.m_value = other_obj.m_value;
            }
            void operator()(pointer_type const& b)
            {
              me.m_value = b;
            }
        };

        bool is_stack() const
        {
            return std::holds_alternative<value_type>(m_value);
        }
        bool is_linked() const
        {
            return std::holds_alternative<pointer_type>(m_value)
                    || std::holds_alternative<const_pointer_type>(m_value);
        }

        /**
         * @brief operator =
         * @param other
         * @return
         *
         * assignment by object
         */
        object_type& operator=( object_type const & other)
        {
            if( other.is_stack() )
            {
                // other object is a value. So we need to convert it into a shared
                // pointer
                m_value = other.m_value;
            }
            else
            {
                // other object is a pointer.
                m_value = *std::get<pointer_type>(other.m_value);
            }

            return *this;
        }

        /**
         * @brief operator =
         * @param x
         * @return
         *
         * Assignment by value
         */
        object_type& operator=(value_type const & x)
        {
            auto & v = m_value;
            if( std::holds_alternative<value_type>(v) )
            {
                m_value = x;
            }
            else if( std::holds_alternative<pointer_type>(v) )
            {
                *std::get<pointer_type>(v) = x;
            }
            else
            {
                m_value = x;
            }
            return *this;
        }

        /**
         * @brief operator =
         * @param x
         * @return
         *
         * assignment by linking
         */
        object_type& operator=( link_ref<object_type> x)
        {
            auto & v = x.m_value->m_value;
            if( std::holds_alternative<value_type>(v) )
            {
                x.m_value->m_value = std::make_shared<value_type>( std::get<value_type>(x.m_value->m_value) );

                const_pointer_type X = std::get<pointer_type>(x.m_value->m_value);
                m_value = X;
            }
            else if( std::holds_alternative<pointer_type>(v) )
            {
                const_pointer_type X = std::get<pointer_type>(x.m_value->m_value);
                m_value = X;
            }
            else
            {
                m_value = x.m_value->m_value;
            }
            return *this;
        }

        /**
         * @brief operator value_type
         *
         * Convert the vp_variant into a pointer
         */
        operator value_type() const
        {
            return std::visit(ValueGetter(), m_value);
        }

        variant_type & get_variant()
        {
            return m_value;
        }

};

}

//template<typename T>
//bool operator==(gnl::vp_variant<T> const& v, T const & x)
//{
//    if( std::holds_alternative<T>(v.m_value) )
//    {
//       return std::get<T>(v.m_value) == x;
//    }
//    else
//    {
//       return *std::get<std::shared_ptr<T> >(v.m_value) == x;
//    }
//}

template<typename T>
std::ostream& operator<< (std::ostream& os, gnl::vp_variant<T> const& v) {
    if( std::holds_alternative<T>(v.m_value) )
    {
        os << std::get<T>(v.m_value);
    }
    else
    {
        // other object is a pointer.
        os << *std::get<std::shared_ptr<T> >(v.m_value) << "[ref]";
    }
  return os;
}

#endif
