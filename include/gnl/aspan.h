#ifndef GNL_VECTOR_VIEW_H
#define GNL_VECTOR_VIEW_H

#include<vector>


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif


namespace GNL_NAMESPACE
{

/**
 * @brief The aspan class
 *
 * aspan is a wrapper around a randomaccess container (vector/array) but allows
 * the underlying element type to be alised as a different type. For example,
 * A vector<float> of 27 elements could be aliased as a aspan<mat3x3> with
 * 3 elements, where each group of 9 floats would be aliased as a 3x3 matrix
 * (assuming the matrix's underlying storage is contiguious)
 *
 * eg:
 *
 * std::vector<float> raw(27);
 * gnl::aspan<mat3x3> matrix( std::begin(raw), std::end(raw));
 *
 * access matrix[0], matrix[1], matrix[2].
 *
 *
 * Example 2: Create a span by skipping every other element in the underlying container.
 * You can create a span by skipping values in the underlying container.
 * Two additional arguments are added, the first being the byte offset from the start
 * and the second being the byte-stride; the number of bytes to skip to get to the next
 * value in the container.
 *
 * gnl::aspan<mat3x3> matrix_even( std::begin(raw), std::end(raw), 0,  sizeof(mat3x3)*2 );
 *
 *
 * Example 3: Create two spans which alias the x and y components of a vector of 2d algebra
 * vectors
 *
 * struct vec2
 * {
 *  float x;
 *  float y;
 * }
 * std::vector<vec2> xy_vectors(100);
 *
 * gnl::span<float> X( std::begin(xy_vectors), std::end(xy_vectors), 0);
 * gnl::span<float> Y( std::begin(xy_vectors), std::end(xy_vectors), offsetof(vec2,x) );
 *
 * X and Y can now be accessed like it was a single array of floats.
 */
template<typename T>
class aspan
{
    public:
        using value_type     = T;
        using alias_type     = aspan<T>;

        using char_type = typename std::conditional< std::is_const<T>::value, const unsigned char, unsigned char>::type;
        using void_pointer_type = typename std::conditional< std::is_const<T>::value, const void*, void*>::type;

        class _iterator :  public std::iterator<std::random_access_iterator_tag, T>
        {
                char_type * p;
                std::ptrdiff_t stride;
              public:

                _iterator(char_type* x, std::ptrdiff_t _stride) : p(x), stride(_stride) {}
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
        aspan(){}
public:
        template<typename base_type>
        aspan( std::vector<base_type> & v, size_t offset=0, size_t stride = sizeof(value_type) ) : aspan( std::begin(v), std::end(v), offset, stride)
        {
           // static_assert ( sizeof(base_type) % sizeof(value_type) == 0
           //                 || sizeof(value_type) % sizeof(base_type) == 0, "Aliased type is miss-aligned");
           //
           // m_size   = (sizeof(base_type) * v.size()) / stride;
           // m_data   = static_cast<unsigned char*>(static_cast<void*>(v.data() )) + offset;
           // m_stride = stride;
        }

        template<typename Iterator>
        aspan( Iterator first, Iterator end,
                      size_t offset=0, size_t stride = sizeof(value_type))
        {
            using category = typename std::iterator_traits<Iterator>::iterator_category;
            static_assert( std::is_same<category, std::random_access_iterator_tag>::value, "Iterator must be random-access" );

            using base_type = typename Iterator::value_type;
            static_assert ( sizeof(base_type) % sizeof(value_type) == 0
                            || sizeof(value_type) % sizeof(base_type) == 0, "Aliased type is miss-aligned");

            auto bytes = sizeof(base_type) * std::distance(first, end);//end-first;
            m_stride = stride;
            if( end < first )
            {
                bytes = sizeof(base_type) * std::distance(end, first);//end-first;
                m_stride = -stride;
            }
            m_size = bytes / stride;

            void_pointer_type a = &(*first);
            m_data   = static_cast<char_type*>(a)+offset;

        }

        /**
         * @brief operator []
         * @param i
         * @return
         *
         * Access a value in the span
         */
        T const & operator[](size_t i)
        {
            return *static_cast<T*>(static_cast<void_pointer_type>(m_data + m_stride*i));
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

        size_t size() const
        {
            return m_size;
        }

        void reverse()
        {
            alias_type r = *this;
            m_data   = m_data + (m_size-1)*m_stride;
            m_stride = -m_stride;
        }

        /**
         * @brief reverse
         * @return
         * Reverse the span such that the last element becomes the first element
         * and vice-versa
         */
        alias_type reverse() const
        {
            alias_type r = *this;
            r.m_data   = m_data + (m_size-1)*m_stride;
            r.m_stride = -m_stride;
            r.m_size   = m_size;
            return r;
        }


        char_type        * m_data;
        size_t             m_size;
        std::ptrdiff_t     m_stride;
};

}

#endif
