/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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
