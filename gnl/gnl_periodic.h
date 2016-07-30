#ifndef GNL_PERIODIC_H
#define GNL_PERIODIC_H

#include <chrono>
#include <functional>
#include <atomic>
#include <iostream>
#include <cstdint>
#include <limits>
#include <vector>
#include <future>
#include <algorithm>


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
 * A class that will periodically call a function at a given interval.
 *
 * This class can operate in two modes, Regular or Spawner.
 *
 * In regular mode, the function is called at regular intervals.
 *
 * In spawner mode, the function will be spawned in a new thread at
 * regular intervals. This is useful if the function that is to be
 * called might take a bit longer than you'd like it and would prefer
 * to run it in the background.
 *
 * Destroying the Periodic class will wait for all threads to finish.
 *
 * A periodic must be constructed on the heap if you want to be able to copy
 * or move the object. This is why static factor methods are provided
 * to construct shared pointers to a Periodic class.
 *
 */
class Periodic
{
public:
    ~Periodic()
    {
        if( !__quit_flag.load( std::memory_order_acquire ) )
        {
            Stop();
        };
    }


    template<typename duration_type>
    void SetInterval( duration_type duration)
    {
        __interval = duration;
    }


    void SetNumberOfCalls(std::uint64_t number)
    {
        __count = number;
    }

    /**
     * @brief Stop
     *
     * Stops the execution of the function call.
     */
    void Stop()
    {
        __count = 0;
        if( __isrunning )
        {
            __quit_flag.store(  true, std::memory_order_release );
            Wait();
        }
    }


    /**
     * @brief Wait
     *
     * Waits for the function call to finish. This will wait forever if you are calling the function an infinite amount of times!
     */
    void Wait()
    {
        if( __runfuture.valid() )
        {
            //std::cout << "Waiting " << std::endl;
            __runfuture.get();
        }
    }

    /**
     * @brief Is_Running
     * @return
     *
     * Returns true if the timer is running and is calling functions.
     */
    bool Is_Running() const NOEXCEPT
    {
        return __isrunning;
    }


    int Running_Threads() const
    {
        return __runningthreads;
    }
    /**
     * @brief Start_Delayed
     * @param func - the function to call, use std::bind to bind a function to a function objects
     * @param microseconds - the number of microseconds to wait between calls
     * @param count - number of times to call teh function
     * @return A shared pointer to a Periodic class
     *
     * Static factory function that starts calling the function, func, but delays the execution of the first call using the definied interval.
     *
     */
    template<typename Rettype>
    static std::shared_ptr< Periodic > Start_Delayed( std::function<Rettype(void)> func , std::uint64_t microseconds , std::uint64_t count = std::numeric_limits<std::uint64_t>::max() )
    {
        std::shared_ptr< Periodic > P( new Periodic() );

        P->__interval = std::chrono::microseconds(microseconds);
        P->Execute< Rettype >( func , std::chrono::microseconds( microseconds ) , count, true);
        return P;
    }


    /**
     * @brief Start
     * @param func - the function to call, use std::bind to bind a function to a function objects
     * @param microseconds - the number of microseconds to wait between calls
     * @param count - number of times to call the function
     * @return A shared pointer to a Periodic class
     *
     * Static factory function that starts calling the function immediately.
     */
    template<typename Rettype>
    static std::shared_ptr< Periodic > Start( std::function<Rettype(void)> func , std::uint64_t microseconds , std::uint64_t count = std::numeric_limits<std::uint64_t>::max() )
    {
        std::shared_ptr< Periodic > P( new Periodic( std::chrono::microseconds(microseconds), false, count) );

        P->Execute< Rettype >( func );
        return P;
    }


    /**
     * @brief Start_Spawner
     * @param func - the function to call, use std::bind to bind a function to a function objects
     * @param microseconds - the number of microseconds to wait between calls
     * @param count - number of times to call teh function
     *
     * Static factory function that starts the Periodic in spawner mode, that is, everytime func() is called, it will be called in a new thread.
     */
    template<typename Rettype>
    static std::shared_ptr< Periodic > Start_Spawner( std::function<Rettype(void)> func , std::uint64_t microseconds , std::uint64_t count = std::numeric_limits<std::uint64_t>::max() )
    {
        std::shared_ptr< Periodic > P( new Periodic( std::chrono::microseconds(microseconds), true, count) );

        P->__interval = std::chrono::microseconds(microseconds);
        P->Execute< Rettype >( func );
        return P;
    }

    /**
     * @brief Start_Spawner
     * @param func - the function to call, use std::bind to bind a function to a function objects
     * @param microseconds - the number of microseconds to wait between calls
     * @param count - number of times to call teh function
     * @return A shared pointer to a Periodic class
     *
     * Starts the Periodic in spawner mode, that is, everytime func() is called, it will be called in a new thread.
     */
    template<typename Rettype>
    static std::shared_ptr< Periodic > Start_Spawner_Delayed( std::function<Rettype(void)> func , std::uint64_t microseconds , std::uint64_t count = std::numeric_limits<std::uint64_t>::max() )
    {
        std::shared_ptr< Periodic > P( new Periodic( std::chrono::microseconds(microseconds), true, count, true) );

        P->__interval = std::chrono::microseconds(microseconds);

        P->Execute< Rettype >( func  );
        return P;
    }


    static std::shared_ptr< Periodic > New( std::uint64_t microseconds , std::uint64_t count = std::numeric_limits<std::uint64_t>::max() )
    {
        std::shared_ptr< Periodic > P( new Periodic( std::chrono::microseconds(microseconds), true, count, true) );
    }


    template<class F, class... Args>
    static std::shared_ptr<Periodic> Start_Test(F&& f, Args&&... args)
    {

        using return_type = typename std::result_of<F(Args...)>::type;

        auto fun = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        fun();

        return nullptr;

    }


    template<typename Rettype>
    bool __run(std::function<Rettype(void)> func )
    {
        if( this->__start_delayed)
            std::this_thread::sleep_for( this->__interval );

        std::vector< std::future<bool> > thread_map;

        //std::cout << "============RUN CALLED ====================" << std::endl;
       // std::cout << "Count: " << this->__count << std::endl;


        while( !__quit_flag.load(std::memory_order_acquire) && this->__count )
        {
          //  std::cout << "quit flag : " << this->__quit_flag.load(std::memory_order_acquire) << std::endl;

            if( this->__spawner_mode )
            {
                auto future = std::async(std::launch::async, [this, func]
                {
                    this->__runningthreads++;
                    func();
                    return true;
                });


                thread_map.push_back( std::move(future) );

                if( thread_map.size() > 10 )
                {

                    thread_map.erase(std::remove_if(thread_map.begin(),
                                                    thread_map.end(),
                                                    [this](std::future<bool> & x)
                    {
                                         auto fin = x.wait_for(std::chrono::milliseconds(0))==std::future_status::ready;
                                         if( fin  ) this->__runningthreads--;
                                         return fin;
                                     }
                                     ),
                            thread_map.end()) ;

                }

                std::this_thread::sleep_for( this->__interval );
            }
            else
            {
                //std::cout << "Calling function: " << std::endl;
                func();
                // std::cout << "Sleeping: " << this->__interval.count()  << std::endl;
                std::this_thread::sleep_for( this->__interval );
            }
            --this->__count;

        }

       // std::cout << "Loop exited.  Size: " << thread_map.size() << std::endl;
        for(int i=0 ; i < thread_map.size() ; i++)
        {
            thread_map[i].get();
            this->__runningthreads--;
        }

        thread_map.clear( );
        this->__isrunning = false;
        return true;
    }


private:
    Periodic( std::chrono::microseconds interval = std::chrono::microseconds(1000000),
              bool spawner_mode = false,
              std::uint64_t count = std::numeric_limits<std::uint64_t>::max(),
              bool delayed = false
            )
        : __quit_flag(false), __spawner_mode(spawner_mode), __count(count), __interval(interval), __start_delayed(delayed)
    {}

    template<typename Rettype>
    /**
         * @brief Execute
         * @param func - the function object to call
         * @param interval - the time interval to wait before periodic calls
         * @param count - number of times to call the function
         * @param start_delayed - true or false to start the timer in delayed mode
         */
    void Execute( std::function<Rettype(void)> func )
    {
        if( !__quit_flag.load( std::memory_order_acquire ) )
        {
            Stop();
        };
        //   Wait();
        __quit_flag.store(false, std::memory_order_release);

        //========================== Run in regular mode ==============================
        //std::cout << "Running in regular mode: " << this->__quit_flag.load(std::memory_order_acquire) << std::endl;
        this->__isrunning = true;
        __runfuture = std::async( std::launch::async, std::bind( &Periodic::__run<Rettype>, this, func) );
    }


private:

    bool                         __spawner_mode = false;
    int                          __runningthreads = 0;
    std::chrono::microseconds    __interval = std::chrono::microseconds(1000000);

    std::future<bool>            __runfuture;

    std::uint64_t                __count = 10;

    bool                         __start_delayed = false;

    bool                         __isrunning = false;

    std::atomic<bool>            __quit_flag;


};





}

#endif

