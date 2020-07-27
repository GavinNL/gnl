#ifndef GNL_ARRAYVIEW_H
#define GNL_ARRAYVIEW_H

#include <cstdlib>
#include <stdexcept>

namespace gnl
{

template<typename T>
class array_view
{
public:
    using value_type           = T;
    using pointer_type         = value_type*;
    using reference_type       = value_type&;
    using const_reference_type       = value_type const&;
    using size_type            = size_t;
    using array_view_type      = array_view<value_type>;
    using difference_type      = std::ptrdiff_t;

    /**
     * @brief array_view
     * @param raw_pointer_to_first_element
     * @param length - The total number of elements in the raw array. Can be negative
     * @param skip - The distance between elements. defaults to 1. Setting to 2 will skip one element
     *
     */
    array_view( pointer_type raw_pointer_to_first_element, int64_t length, size_t _skip=1) :
        m_first(raw_pointer_to_first_element),
        m_raw_size(length)
    {
        m_first = raw_pointer_to_first_element;

        // could be negative
        auto raw_size = &m_first[length] - m_first;
        m_raw_size    = raw_size;

        m_skip = (raw_size < 0 ? -1 : 1) * _skip;

    }


    reference_type       operator[](int64_t i) { return *(m_first+i*m_skip); }
    const_reference_type operator[](int64_t i) const { return *(m_first+i*m_skip); }

    reference_type       at(int64_t i)
    {
        if( i>=size() )
            throw std::out_of_range( std::string("Attempting to access element") + std::to_string(i) + std::string(". Last index: ") + std::to_string(size()-1) );
        return *(m_first+i*m_skip);

    }
    const_reference_type       at(int64_t i) const
    {
        if( i>=size() )
            throw std::out_of_range( std::string("Attempting to access element") + std::to_string(i) + std::string(". Last index: ") + std::to_string(size()-1) );
        return *(m_first+i*m_skip);
    }

    /**
     * @brief slice
     * @param first_index
     * @param last_index
     * @param skip
     * @return
     *
     * return a new array_view that is a slice of the original.
     *
     * if A = {0,1,2,3,4,5}
     * then A.slice(5,0,2) = {5,3,1}
     */
    array_view_type subarray( int64_t first_index, int64_t offset_from_first_index, size_t _skip=1 )
    {
        auto & t = *this;
        auto total_items_in_raw_array = &t[first_index + offset_from_first_index] - &t[first_index];

        return array_view_type(  &t[first_index], total_items_in_raw_array, _skip * std::abs(m_skip) );

        return *this;
    }


    /**
     * @brief front
     * @return
     *
     * Returns a reference to the front of the view
     */
    reference_type front()  {
        return *m_first;
    }

    const_reference_type front() const {
        return *m_first;
    }

    /**
     * @brief back
     * @return
     *
     * Returns a reference to the back of the view
     */
    const_reference_type back() const {
        return (*this)[ size()-1 ];
    }
    reference_type back()  {
        return (*this)[ size()-1 ];
    }

    difference_type raw_array_length() const
    {
        return m_raw_size;
    }
    /**
     * @brief reverse
     * Reverses the view on the array.
     */
    void reverse()
    {
        m_first = &back();
        //std::swap(m_first,m_last);
        m_skip     *= -1;
        m_raw_size *= -1;
    }

    /**
     * @brief size
     * @return
     * THe number of elements in the view
     */
    difference_type       size() const { return m_raw_size / m_skip; }

    /**
     * @brief pop_front
     * Pops the front element of the view. This does not change the original data.
     */
    void pop_front()
    {
        m_first     += m_skip;
        m_raw_size  -= m_skip;
    }

    /**
     * @brief pop_back
     *
     * Pops the back element of the view. This does not change the original data.
     */
    void pop_back()
    {
        m_raw_size -= m_skip;
    }

    difference_type skip() const
    {
            return m_skip;
    }

protected:
    pointer_type      m_first;
    difference_type   m_raw_size;
    difference_type   m_skip;
};

}
#endif // GNL_ARRAYVIEW_H
