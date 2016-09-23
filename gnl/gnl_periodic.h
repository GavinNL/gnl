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
        void Start(  std::chrono::microseconds interval, std::uint64_t count, F&& f, Args&&... args);


        ~Periodic();

        /**
         * @brief SetCount
         * @param count - the total amount of times to call the function. Can be set
         *                while the Periodic is active.
         */
        void SetCount( std::uint64_t count);

        /**
         * @brief Stop
         * Stops the Periodic.
         */
        void Stop();


        /**
         * @brief SetInterval
         * @param interval - the interval between function calls
         */
        void SetInterval(const std::chrono::microseconds & interval );

    private:

        bool                      __stop         = false;
        std::chrono::microseconds __interval     = std::chrono::microseconds(1000000);
        std::uint64_t             __count        = 0xFFFFFFFFFFFFFFFF;

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

inline void Periodic::SetCount( std::uint64_t count)
{
    std::lock_guard<std::mutex> L(__data_mutex);
    __count = count;
}

inline void Periodic::Stop()
{
    std::lock_guard<std::mutex> L(__data_mutex);
    __stop = true;
}


inline void Periodic::SetInterval(const std::chrono::microseconds & interval )
{
    __interval = interval;
}



template<class F, class... Args>
inline void Periodic::Start(  std::chrono::microseconds interval, std::uint64_t count, F&& f, Args&&... args)
{
    using return_type = typename std::result_of< F(Args...) >::type;

    if( __timerfuture.valid() )
    {
        this->__stop = true;
        __timerfuture.get();
    }

    this->__interval = interval;
    this->__count    = count;
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

}


}


#endif
