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

namespace gnl
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
        Periodic& Start(  F&& f, Args&&... args);


        ~Periodic();

        /**
         * @brief SetCount
         * @param count - the total amount of times to call the function. Can be set
         *                while the Periodic is active.
         */
        Periodic& Count( std::uint64_t count);

        /**
         * @brief Stop
         * Stops the Periodic.
         */
        void Stop();


        /**
         * @brief SetInterval
         * @param interval - the interval between function calls
         */
        Periodic& Interval(const std::chrono::microseconds & interval );


        /**
         * @brief SetInterval
         * @param seconds - number of seconds between function calls
         * @return
         */
        Periodic& Interval(const double & seconds )
        {
            Interval( std::chrono::microseconds( static_cast<std::uint64_t>(seconds*1e6) ) );
            return *this;
        }

    private:

        bool                      __stop         = false;
        std::chrono::microseconds __interval     = std::chrono::microseconds(1000000);
        std::uint64_t             __count        = std::numeric_limits<std::uint64_t>::max();

        std::future<bool>         __timerfuture;
        std::mutex                __data_mutex;

        std::condition_variable   __condition;
};



inline Periodic::~Periodic()
{
    if( __timerfuture.valid() )
    {
        this->__stop = true;
        this->__condition.notify_all();
        __timerfuture.get();
    }
}

inline Periodic& Periodic::Count( std::uint64_t count)
{
    std::lock_guard<std::mutex> L(__data_mutex);
    __count = count;
    return *this;
}

inline void Periodic::Stop()
{
    std::lock_guard<std::mutex> L(__data_mutex);
    __stop = true;
}


inline Periodic & Periodic::Interval(const std::chrono::microseconds & interval )
{
    __interval = interval;
    return *this;
}



template<class F, class... Args>
inline Periodic& Periodic::Start(F&& f, Args&&... args)
{
    using return_type = typename std::result_of< F(Args...) >::type;

    if( __timerfuture.valid() )
    {
        this->__stop = true;
        __timerfuture.get();
    }


    this->__stop     = false;

    std::function< void() > fun = std::bind( std::forward<F>(f), std::forward<Args>(args)... );

    auto Main_Loop = [this, fun]()
    {

        decltype(__interval) dt(0);

        while( __count)
        {
            decltype(__interval) wait_time = __interval - dt;

            std::unique_lock<std::mutex> Lock( this->__data_mutex );

            if( std::cv_status::no_timeout == this->__condition.wait_for( Lock, wait_time ) )
            {
                //std::cout << "Wait didn't time out, Attempting to exit" << std::endl;
            }

            wait_time = this->__interval;
            --__count;
            Lock.unlock();

            if( this->__stop ) // checking this condition after the condition variable has been triggered so we
            {                  // so we do not call the function if we have forced the periodic to stop during
                break;         // the wait period
            }

            fun();

        }

        return true;
    };


    __timerfuture = std::async( std::launch::async,
                              Main_Loop
                              );

    return *this;

}


}


#endif
