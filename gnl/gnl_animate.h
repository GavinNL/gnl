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
#include <vector>
#include <cmath>

namespace gnl
{


template<typename T, typename floatType=float>
/**
 * @brief The Animate class
 *
 * The Animate class is a time-dependant variable that changes based
 * on tweening functions. The value of the variable is depenant on the
 * real time clock.
 *
 * The value of the variable does not change until you actually access
 * the value by the get() function or by casting it to another variable
 *
 * eg:
 * Animate<float> V = 1.0f;
 * V.To(10.0f, 1.0f, Animate<float>::LinearTween() );
 *
 * float f  = V; // f = 1.0
 * std::this_thread::sleep_for( std::chrono::milliseconds(900) );
 * float f2 = V; // f2 = 9.0 (or close to)
 *
 *
 *
 */
class Animate
{
    public:
        using timepoint    = std::chrono::time_point<std::chrono::high_resolution_clock>;
        using tween_func   = std::function< floatType(floatType)>;

        // no easing, no acceleration
        struct linear{ floatType operator()(floatType t) { return t; }};
        // accelerating from zero velocity
        struct easeInQuad{ floatType operator()(floatType t) { return t*t; }};
        // decelerating to zero velocity
        struct easeOutQuad{ floatType operator()(floatType t) { return t*(2-t); }};
        // acceleration until halfway, then deceleration
        struct easeInOutQuad{ floatType operator()(floatType t) { return t<.5 ? 2*t*t : -1+(4-2*t)*t; }};
        // accelerating from zero velocity
        struct easeInCubic{ floatType operator()(floatType t) { return t*t*t; }};
        // decelerating to zero velocity
        struct easeOutCubic{ floatType operator()(floatType t) { return (--t)*t*t+1; }};
        // acceleration until halfway, then deceleration
        struct easeInOutCubic{ floatType operator()(floatType t) { return t<.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1; }};
        // accelerating from zero velocity
        struct easeInQuart{ floatType operator()(floatType t) { return t*t*t*t; }};
        // decelerating to zero velocity
        struct easeOutQuart{ floatType operator()(floatType t) { return 1-(--t)*t*t*t; }};
        // acceleration until halfway, then deceleration
        struct easeInOutQuart{ floatType operator()(floatType t) { return t<.5 ? 8*t*t*t*t : 1-8*(--t)*t*t*t; }};
        // accelerating from zero velocity
        struct easeInQuint{ floatType operator()(floatType t) { return t*t*t*t*t; }};
        // decelerating to zero velocity
        struct easeOutQuint{ floatType operator()(floatType t) { return 1+(--t)*t*t*t*t; }};
        // acceleration until halfway, then deceleration
        struct easeInOutQuint{ floatType operator()(floatType t) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t; }};

        struct easeBackEase{ floatType operator()(floatType t) { return t*t*t-t*std::sin(t*static_cast<floatType>(3.141592653589)); }};


        struct easeOutBounce {

            floatType operator () ( floatType t) {
                if (t < static_cast<floatType>(1/2.75)) {
                    return ( static_cast<floatType>(7.5625)*t*t);
                } else if (t < (2/2.75)) {
                    return (static_cast<floatType>(7.5625)*(t-=static_cast<floatType>(1.5/2.75))*t + static_cast<floatType>(.75));
                } else if (t < static_cast<floatType>(2.5/2.75)) {
                    return (static_cast<floatType>(7.5625)*(t-=static_cast<floatType>(2.25/2.75))*t + static_cast<floatType>(.9375));
                } else {
                    return (static_cast<floatType>(7.5625)*(t-=static_cast<floatType>(2.625/2.75))*t + static_cast<floatType>(.984375));
                }
            }
        };

        struct easeInBounce {

            floatType operator () ( floatType t )
            {
                static easeOutBounce E;
                return 1 - E(1-t);
            }
        };


        struct easeInOutBounce {
            floatType operator() (floatType t)
            {
                static easeOutBounce E;
                if (t < 0.5) return E(t*2) * 0.5;//jQuery.easing.easeInBounce (x, t*2, 0, c, d) * .5 + b;
                return E(t*2-1) * 0.5 + 0.5;
                        //jQuery.easing.easeOutBounce (x, t*2-d, 0, c, d) * .5 + c*.5 + b;
            }
        };


        struct queue_elem
        {
            timepoint    start_time;
            timepoint    end_time;
            T            start_value;
            T            end_value;
            tween_func   tween;
        };

        Animate() : current_start( static_cast<T>(0) )
        {

        }

        Animate(const T & value) : current_start(value), tweens(nullptr)
        {
        }

        Animate( T && value) : current_start( std::move(value) ), tweens(nullptr)
        {
        }


        Animate& operator=(const T & other)
        {
            if( tweens )
            {
                tweens->back().end_value = other;
            } else {
                current_start = other;
            }
            return *this;
        }

        Animate& operator=( T && other)
        {
            if( tweens )
            {
                tweens->back().end_value = std::move(other);
            } else{
                current_start = std::move(other);
            }
            return *this;
        }

        /**
         * @brief Stable - Used to determine if the animation is active or not
         * @return true if the animation is over
         */
        bool Stable() const
        {
            auto now = std::chrono::system_clock::now();

            if( tweens )
            {
                if( tweens->back().end_time < now )
                {
                    return true;
                }
                else
                {
                   return false;
                }
            }
            return true;
        }

        template<typename AA=void>
        T get() const
        {
           // static_assert(false, "get() changes the structure of Animate, Use Animate::ConstSampler(const Animate&) to animate a constant Animate");
        }

        /**
         * @brief The ConstSampler struct
         * A wrapper around a const Animate, used to extract animated values from a const Animate.
         */
        struct ConstSampler
        {
            ConstSampler( const Animate & A)
            {
                if(A.tweens )
                {
                    i   = A.it;
                    end = A.tweens->end();

                    value = std::prev(end)->end_value;// end->end_value;
                } else {
                    value = A.current_start;
                    end = i;
                }
            }

            T get()
            {
                auto now = std::chrono::system_clock::now();
                if( i != end )
                {
                    const queue_elem & F = *i;

                    floatType t = std::chrono::duration<floatType>(now - F.start_time).count() / std::chrono::duration<floatType>(F.end_time - F.start_time).count();

                    if( t < 1.0 )
                    {
                        t = F.tween(t);
                        return F.end_value*t + F.start_value*( 1 - t ); //F.tween( F.start_value, F.end_value, t);
                    }
                    else
                    {

                        ++i;
                        if( i == end )
                        {
                            return value;
                        }
                        return get();

                    }
                }

                return value;
            }

            bool Stable() const
            {
                return i==end;
            }

            T value;
            typename std::vector< queue_elem >::const_iterator i;
            typename std::vector< queue_elem >::const_iterator end;
        };

        /**
         * @brief get - gets the current value of
         * @return A new value in the frame
         *
         * This function changes the internal structure and therefore it is non-const.
         * See Animate::ConstSampler for dealing with const Animates.
         */
        T get()
        {
            auto now = std::chrono::system_clock::now();

            if( tweens )
            {
                queue_elem & F = *it;

                floatType t = std::chrono::duration<floatType>(now - F.start_time).count() / std::chrono::duration<floatType>(F.end_time - F.start_time).count();

                if( t < 1.0 )
                {
                    t = F.tween(t);
                    return F.end_value*t + F.start_value*( 1 - t ); //F.tween( F.start_value, F.end_value, t);
                }
                else
                {
                    current_start = F.end_value;

                    it++;

                    if( it == tweens->end() )
                    {
                        delete tweens;
                        tweens = nullptr;
                    }

                    return get();
                }
            }

            return current_start;
        }

        /**
         * @brief To - Animates the value to a new value
         * @param end_value - the final value of to animate to
         * @param duration_in_seconds - time in seconds to transition
         * @param Tween - one of the tweening functions used to animate the value
         */
        void To(T end_value, floatType duration_in_seconds, const tween_func & Tween=linear() )
        {
            To(end_value, std::chrono::microseconds( (long long)(duration_in_seconds*1e6) ), Tween  );
        }

        /**
         * @brief To - Animates the value to a new value
         * @param end_value - the final value of to animate to
         * @param duration - time in microseconds, or use any other std::chrono::duration type
         * @param Tween - one of the tweening functions used to animate the value
         */
        void To(T end_value, std::chrono::microseconds duration, const tween_func & Tween=linear()   )
        {
            if( tweens )
            {
                auto st = tweens->back().end_time;
                auto et = st + duration;

                auto d = std::distance( tweens->begin(), it);
                tweens->push_back( queue_elem{ st, et, tweens->back().end_value, end_value, Tween } );
                it = tweens->begin()+d;
            }
            else
            {
                tweens  = new std::vector<queue_elem>();
                auto st = std::chrono::system_clock::now();
                auto et = st + duration;

                tweens->push_back( queue_elem{ st, et, current_start, end_value, Tween } );
                it = tweens->begin();
            }

        }

        /**
         * @brief operator T
         *
         * Used to convert the Animate into it's base type
         */
        operator T()
        {
           return get();
        }

        protected:
            T                                   current_start; // current value.
            std::vector< queue_elem >           *tweens = nullptr;
            typename std::vector< queue_elem >::iterator it;

};


template<typename T, typename p=float>
class Animate2
{
public:
    using timepoint    = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using tween_func   = std::function< p(p)>;

    struct queue_elem
    {
        T            v = static_cast<T>(0);
        timepoint    t = std::chrono::system_clock::now();
        tween_func   f = [](p t) {return t;};
        queue_elem   *next = nullptr;

        ~queue_elem()
        {
            if(next) delete next;
        }
    };

    Animate2(const T & f = static_cast<T>(0))
    {
        head = new queue_elem;
        tail = head;
    }

    ~Animate2()
    {
        delete head;
    }

    Animate2& set(const T & v)
    {
        if( head->next )
            delete head->next;

        head->v = v;
        tail = head;
        return *this;
    }

    Animate2& stop()
    {
        set( get() );
        return *this;
    }

    template<class rep, std::intmax_t num, std::intmax_t den>
    Animate2 & to(const T & v, const std::chrono::duration<rep, std::ratio<num,den> > & dur )
    {
        if( head == tail)
        {
         //   std::cout << "No next" << std::endl;
            head->t = std::chrono::system_clock::now();

            auto * next = new queue_elem;
            next->v = v;
            next->t = head->t + dur;
            head->next = next;
            tail = next;
        }
        else
        {
            auto * next = new queue_elem;
            next->v = v;
            next->t = tail->t + dur;
            tail->next = next;
            tail = next;
        }
        return *this;
    }

    operator T()
    {
        return get();
    }

    T get() const
    {
        auto now = std::chrono::system_clock::now();

        if( head == tail)
        {
            return head->v;
        }
        else
        {
            queue_elem & F1 = *head;
            queue_elem & F2 = *head->next;
            p t = std::chrono::duration<p>(now - F1.t).count() / std::chrono::duration<p>(F2.t - F1.t).count();
            if( t > 1.0)
            {

                auto * old = head;
                head = head->next;
                old->next = nullptr;
                delete old;
                //std::cout << "Head deleted" << std::endl;
                return get();
            }
            t = F2.f(t);
            return F2.v*t + F1.v*( static_cast<p>(1.0) - t ); //F.tween( F.start_value, F.end_value, t);
        }

    }

    mutable queue_elem *head = nullptr;
    mutable queue_elem *tail = nullptr;
};

}

#endif
