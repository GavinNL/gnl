/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#ifndef GNL_PERIODIC_H
#define GNL_PERIODIC_H

#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>
#include <cstdint>

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{

/**
 * @brief The Periodic class
 *
 *  Calls a function at regular intervals.
 *
 * example:
 *
 * int process(int x )
 * {
 *     static int i=0;
 *     int j = i++;
 *     std::cout << "Function Called: " << x << std::endl;
 *     return 0;
 * }
 *
 * int  main()
 * {
 *      gnl::Periodic P;
 *      P.Interval( 1.0 ).Start( process, 10);  // call the process function with the argument 10, once every seconds
 *      std::this_thread::sleep_for( std::chrono::seconds(10));
 *      return 0;
 * }
 *
 */
class Periodic
{
    public:
        Periodic(){}

        template<class F, class... Args>
        /**
         * @brief Start
         * @param interval - interval between calls. Can use any std::duration type which can be converted
         *                   into microseconds
         * @param count    - number of times to call the function in total
         *
         * @param f        - arugments to the function
         * @param args     - arugments to the function
         *
         */
        Periodic& start(  F&& f, Args&&... args);


        ~Periodic();

        /**
         * @brief SetCount
         * @param count - the total amount of times to call the function. Can be set
         *                while the Periodic is active.
         */
        Periodic& count( std::uint64_t count);

        /**
         * @brief Stop
         * Stops the Periodic.
         */
        void stop();


        /**
         * @brief SetInterval
         * @param interval - the interval between function calls
         */
        Periodic& interval(const std::chrono::microseconds & _interval );


        /**
         * @brief SetInterval
         * @param seconds - number of seconds between function calls
         * @return
         */
        Periodic& interval(const double & seconds )
        {
            interval( std::chrono::microseconds( static_cast<std::uint64_t>(seconds*1e6) ) );
            return *this;
        }

    private:

        bool                      m_stop         = false;
        std::chrono::microseconds m_interval     = std::chrono::microseconds(1000000);
        std::uint64_t             m_count        = std::numeric_limits<std::uint64_t>::max();

        std::future<bool>         m_future;
        std::mutex                m_mutex;

        std::condition_variable   m_condition;
};



inline Periodic::~Periodic()
{
    if( m_future.valid() )
    {
        this->m_stop = true;
        this->m_condition.notify_all();
        m_future.get();
    }
}

inline Periodic& Periodic::count( std::uint64_t _count)
{
    std::lock_guard<std::mutex> L(m_mutex);
    m_count = _count;

    return *this;
}

inline void Periodic::stop()
{
    std::lock_guard<std::mutex> L(m_mutex);
    m_stop = true;
}


inline Periodic & Periodic::interval(const std::chrono::microseconds & _interval )
{
    m_interval = _interval;
    return *this;
}



template<class F, class... Args>
inline Periodic& Periodic::start(F&& f, Args&&... args)
{
    //using return_type = typename std::result_of< F(Args...) >::type;

    if( m_future.valid() )
    {
        this->m_stop = true;
        m_future.get();
    }


    this->m_stop     = false;

    std::function< void() > fun = std::bind( std::forward<F>(f), std::forward<Args>(args)... );

    auto Main_Loop = [this, fun]()
    {

        decltype(m_interval) dt(0);

        while( m_count)
        {
            decltype(m_interval) wait_time = m_interval - dt;

            std::unique_lock<std::mutex> Lock( this->m_mutex );

            if( std::cv_status::no_timeout == this->m_condition.wait_for( Lock, wait_time ) )
            {
                //std::cout << "Wait didn't time out, Attempting to exit" << std::endl;
            }

            wait_time = this->m_interval;
            --m_count;
            Lock.unlock();

            if( this->m_stop ) // checking this condition after the condition variable has been triggered so we
            {                  // so we do not call the function if we have forced the periodic to stop during
                break;         // the wait period
            }

            fun();

        }

        return true;
    };


    m_future = std::async( std::launch::async,
                              Main_Loop
                              );

    return *this;

}


}


#endif
