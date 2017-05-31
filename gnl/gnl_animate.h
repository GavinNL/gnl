/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef GNL_ANIMATE
#define GNL_ANIMATE

#include <functional>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdint>
#include <list>
#include <assert.h>

namespace gnl
{

/**
 * @brief The Animate2 class
 *
 * Newer implementation of the animate class using a list.
 */
template<typename T, typename p=double>
class Animate3
{
public:
    using tween_func   = std::function< p(p)>;
    using animate_type = Animate3<T, p>;
    using clock        = std::chrono::system_clock;
    using timepoint    = std::chrono::time_point<clock>;

    struct queue_elem
    {
        T            v = static_cast<T>(0);
        timepoint    t = clock::now();
        tween_func   f = [](p t) {return t;};

        bool operator==(queue_elem const & other)
        {
            return v==other.v && t==other.t;
        }

        queue_elem( const T & F, timepoint t0 ) : v(F), t(t0)
        {

        }

        queue_elem(const queue_elem & other) : v(other.v), t(other.t), f(other.f)
        {
        }

        ~queue_elem()
        {
        }
    };

    Animate3(const T & f = static_cast<T>(0))
    {
        m_Q.push_back( queue_elem(f, clock::now() ) );
    }

    ~Animate3()
    {
    }

    Animate3(const animate_type & other) : m_Q( other.m_Q )
    {
    }

    Animate3(Animate3 && other)
    {

        if( this != &other)
        {
            m_Q =  std::move( other.m_Q );
        }
    }
    /**
     * @brief set
     * @param v
     * @return
     *
     * Clears all transitions and sets the value
     */
    Animate3& set(const T & v)
    {
        auto f = m_Q.front();
        f.v = v;
        f.t = clock::now();
        m_Q.clear();
        m_Q.push_back( f );



        return *this;
    }

    /**
     * @brief stop
     * @return
     *
     * Stops the transitions
     */
    Animate3 & stop()
    {
        set( get() );
        return *this;
    }

    /**
     * @brief to
     * @param v
     * @param seconds - time in seconds
     * @return
     *
     * Transition to a new value over a duration.
     */
    Animate3 & to( const T & v, double seconds)
    {
        return to(  v, std::chrono::nanoseconds( static_cast<intmax_t>(seconds*1e9) ) );
    }

    /**
     * @brief to
     * @param v - value to transition to
     * @param dur - duration value
     * @return
     *
     * Transition to a new value over a duration.
     *
     */
    template<class rep, std::intmax_t num, std::intmax_t den>
    Animate3 & to(const T & v, const std::chrono::duration<rep, std::ratio<num,den> > & dur )
    {
        assert( m_Q.size() != 0 );
        auto & head = m_Q.back();

        if(m_Q.size()==1)
        {
            head.t = clock::now();
        }

        auto mdur = std::chrono::duration_cast< timepoint::duration >(dur);
        m_Q.push_back( queue_elem(v, timepoint(head.t+mdur) ) );
        return *this;
    }

    /**
     * @brief operator T
     *
     * Cast the to the template value
     */
    operator T()
    {
        return get();
    }

    /**
     * @brief get
     * @return
     *
     * Gets the current value.
     */
    T get() const
    {
        auto now = clock::now();

        if( m_Q.size() == 1)
        {
            return m_Q.front().v;
        }
        else
        {
            queue_elem & F1 =  m_Q.front();
            queue_elem & F2 = *(++m_Q.begin());

           // std::cout << "t0 = " << F1.t.time_since_epoch().count()  << "t1 = " << F2.t.time_since_epoch().count()  << std::endl;
            p t = std::chrono::duration<p>(now - F1.t).count() / std::chrono::duration<p>(F2.t - F1.t).count();

            if( t > 1.0)
            {
                m_Q.pop_front();
                return get();
            }
            t = F2.f(t);
            return F2.v*t + F1.v*( static_cast<p>(1.0) - t ); //F.tween( F.start_value, F.end_value, t);
        }

    }

    /**
     * @brief stable
     * @return
     *
     * Returns true if the object is not transitioning
     */
    bool stable() const
    {

        return m_Q.size()==1;
    }

    /**
     * @brief time_left
     * @return
     *
     * Returns the number of seconds left for the tranition.
     */
    double time_left() const
    {
        return std::max( static_cast<p>(0), std::chrono::duration<p>( m_Q.back().t - clock::now() ).count());
    }

    mutable std::list<queue_elem> m_Q;

};

template<typename T, typename p=double>
using Animate = Animate3<T,p>;

}

#endif
