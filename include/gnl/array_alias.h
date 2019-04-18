#ifndef GNL_VECTOR_VIEW_H
#define GNL_VECTOR_VIEW_H

#include<vector>


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif


namespace GNL_NAMESPACE
{

template<typename T>
class array_alias
{
    public:
        using value_type     = T;
        using alias_type     = array_alias<T>;

        class _iterator :  public std::iterator<std::random_access_iterator_tag, T>
        {
                unsigned char * p;
                std::ptrdiff_t stride;
              public:

                _iterator(unsigned char* x, std::ptrdiff_t _stride) : p(x), stride(_stride) {}
                _iterator(const _iterator& mit) : p(mit.p), stride(mit.stride) {}

                // return the number of elements between two iterators
                bool operator<(const _iterator & other) const
                {
                    return p < other.p;
                }

                std::ptrdiff_t operator-(const _iterator & other) const
                {
                    return (p - other.p) / stride;
                }
                _iterator operator-(int inc) const
                {
                    return _iterator( p - inc*stride, stride);
                }
                _iterator operator+(int inc) const
                {
                    return _iterator( p + inc*stride, stride);
                }

                _iterator& operator--() {p-=stride;return *this;}
                _iterator  operator--(int) {_iterator tmp(*this); operator--(); return tmp;}

                _iterator& operator++() {p+=stride;return *this;}
                _iterator operator++(int) {_iterator tmp(*this); operator++(); return tmp;}

                bool operator==(const _iterator& rhs) const {return p==rhs.p;}
                bool operator!=(const _iterator& rhs) const {return p!=rhs.p;}
                T& operator*() {return *static_cast<T*>(static_cast<void*>(p));}
         };

        using iterator       = _iterator;
        using const_iterator = const iterator;

protected:
        template<typename base_type>
        array_alias(){}
public:
        template<typename base_type>
        array_alias( std::vector<base_type> & v, size_t offset=0, size_t stride = sizeof(value_type) )
        {
            static_assert ( sizeof(base_type) % sizeof(value_type) == 0
                            || sizeof(value_type) % sizeof(base_type) == 0, "Aliased type is miss-aligned");

            m_size   = (sizeof(base_type) * v.size()) / stride;
            m_data   = static_cast<unsigned char*>(static_cast<void*>(v.data() )) + offset;
            m_stride = stride;
        }

        template<typename Iterator>
        array_alias( Iterator first, Iterator end,
                      size_t offset=0, size_t stride = sizeof(typename Iterator::value_type))
        {
            using category = typename std::iterator_traits<Iterator>::iterator_category;
            static_assert( std::is_same<category, std::random_access_iterator_tag>::value, "Iterator must be randomaccess" );

            auto x = end-first;
            m_size = x;
            if( x < 0) m_size = -x;
            void * a = &(*first);
            m_data   = static_cast<unsigned char*>(a);
            m_stride = stride;
            if( x < 0 ) m_stride *= -1;
        }

        T const & operator[](size_t i)
        {
            return *static_cast<T*>(static_cast<void*>(m_data + m_stride*i));
        }

        iterator begin()
        {
            return iterator( m_data, m_stride);
        }
        iterator end()
        {
            return iterator( m_data + m_stride*size(), m_stride);
        }
     //
     //   const_iterator begin() const
     //   {
     //       return static_cast<value_type*>(m_data);
     //   }
     //   const_iterator end() const
     //   {
     //       return begin() + size();
     //   }

        size_t size() const { return m_size; };

        void reverse()
        {
            alias_type r = *this;
            m_data   = m_data + (m_size-1)*m_stride;
            m_stride = -m_stride;
        }

        alias_type reverse() const
        {
            alias_type r = *this;
            r.m_data   = m_data + (m_size-1)*m_stride;
            r.m_stride = -m_stride;
            r.m_size   = m_size;
            return r;
        }
        unsigned char    * m_data;
        size_t             m_size;
        std::ptrdiff_t     m_stride;
};

}

#endif
