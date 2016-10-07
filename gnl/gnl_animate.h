#ifndef GNL_ANIMATE
#define GNL_ANIMATE

#include <functional>
#include <chrono>
#include <vector>

namespace gnl
{

template<typename FloatType=float>
struct LinearTween
{
    FloatType operator()(FloatType t) const
    {
        return t;
    }
};



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
 * V.To(10.0f, LinearTween<float>(), 1.0f);
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

        struct queue_elem
        {
            timepoint    start_time;
            timepoint    end_time;
            T            start_value;
            T            end_value;
            tween_func   tween;
        };

        Animate()
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

        T get() const
        {
            auto now = std::chrono::system_clock::now();

            if( tweens )
            {
                if( tweens->back().end_time < now )
                {
                    return tweens->back().end_value;
                }
                else
                {
                    auto cit = it; // make temp iterator

                    while( cit->end_time < now) cit++;

                    queue_elem & F = *cit;

                    floatType  t = std::chrono::duration<floatType>(now - F.start_time).count() / std::chrono::duration<floatType>(F.end_time - F.start_time).count();

                    t = F.tween(t);
                    return F.end_value*t + F.start_value*( 1 - t ); //F.tween( F.start_value, F.end_value, t);
                }
            }
            return current_start;
        }

        T get()
        {
            auto now = std::chrono::system_clock::now();

            if( tweens )
            {
                queue_elem & F = *it;

                floatType        t = std::chrono::duration<floatType>(now - F.start_time).count() / std::chrono::duration<floatType>(F.end_time - F.start_time).count();

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

        void To(T end_value, floatType duration_in_seconds, const tween_func & Tween=LinearTween<floatType>() )
        {
            To(end_value, std::chrono::microseconds( (long long)(duration_in_seconds*1e6) ), Tween  );
        }

        void To(T end_value, std::chrono::microseconds duration, const tween_func & Tween=LinearTween<floatType>()   )
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


        operator T() const
        {
           return get();
        }

        operator T()
        {
           return get();
        }

        protected:
        T                                   current_start; // current value.
        std::vector< queue_elem >           *tweens = nullptr;
        typename std::vector< queue_elem >::iterator it;

};

}

#endif
