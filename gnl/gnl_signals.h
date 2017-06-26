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

#ifndef GNL_SIGNALS_H
#define GNL_SIGNALS_H

#include <algorithm>
#include <functional>
#include <vector>
#include <tuple>
#include <future>

namespace gnl
{

class base_slot
{
    public:
        virtual ~base_slot() {}
};

/**
 * @brief The Signal class
 *
 * Signal class:  Attach multiple function objects
 * and call all function objects with a single call
 * to the () operator
 */
template<typename func_t>
class signal
{
    public:
        using id                = unsigned long;  //id type
        using function_t        = std::function<func_t>; // function object type

        using value_t           = std::pair<id,function_t>;

        class container_t
        {
        public:
            void remove(id ID)
            {
                if( ID == 0 ) return;

                std::lock_guard<std::mutex> L( m_mutex);


                m_container.erase(std::remove_if(m_container.begin(),
                                                 m_container.end(),
                                                 [ID](const value_t & x){ return x.first==ID;}),
                                  m_container.end());

            }

            public:
                std::vector<value_t>  m_container;
                std::mutex            m_mutex;
        };

        using container_p       = std::shared_ptr< container_t >;

        class slot : public base_slot
        {
        private:
            id           m_id;
            container_p  m_Container;

        public:
            slot() : m_id(0){}

            slot(id ID, container_p p ) : m_id(ID) , m_Container(p)
            {
            }

            slot(const slot & other) = delete;

            slot(slot && other)
            {
                m_id        = std::move( other.m_id );
                other.m_id  = 0;
                m_Container = std::move( other.m_Container);
            }

            virtual ~slot()
            {
                disconnect();
            }

            slot & operator=(const slot & other) = delete;

            slot & operator=(slot && other)
            {
                if( this != &other)
                {
                    m_id        = std::move( other.m_id );
                    m_Container = std::move( other.m_Container);
                    other.m_id = 0;
                }
                return *this;
            }

            /**
             * @brief Disconnect
             *
             * Disconnects the slot from the signal caller. This is automatically
             * called when the slot is destroyed.
             */
            virtual bool disconnect()
            {
                if( m_id == 0 ) return false;
                auto i = m_id;
                m_id = 0;
                m_Container->remove(i);
                return true;

            }
        };


        signal() : m_signals( new container_t() )
        {

        }

        /**
         * @brief Connect
         * @param f - function object to connect
         * @param position - position in the list of signals, default is -1 (back of the list)
         * @return The slot ID used to disconnect the slot.
         *
         * Connects a function object to the signal
         */
        slot connect( function_t f , int position = -1)
        {
            static id ID = 1;
            std::lock_guard<std::mutex> L( get_container().m_mutex );

            auto & container = get_container().m_container;
            if(position<0)
                container.emplace_back( ID, f );
            else
                container.insert( container.begin()+position, value_t(ID,f) );

            return  slot(ID++, m_signals);

        }

        void disconnect(id SlotID)
        {
            std::lock_guard<std::mutex> L( get_container().m_mutex );
            auto it = get_container().begin();
            while(it != get_container().end() )
            {
                if( (*it).first == SlotID )
                {
                    std::cout << "Disconnecting slot: " << SlotID << std::endl;
                    it = get_container().erase(it);
                    return;
                }
                ++it;
            }
        }

        /**
         * @brief operator <<
         * @param f - function object to connect
         *
         * Same as the Connect( ) method.
         */
        slot operator << ( function_t f )
        {
            return connect(f);
        }

        template<typename ..._Funct>
        void operator()( _Funct &&... _Args )
        {
            std::lock_guard<std::mutex> L( m_signals->m_mutex );
            for(auto & f : get_container().m_container)
            {
                if( f.first != 0 )
                {
                    f.second( std::forward<_Funct>(_Args)...);
                }
            }
        }

        std::size_t NumSlots() const
        {
            return m_signals->m_container.size();
        }


    protected:
        container_t&                 get_container() const { return *m_signals; }

        container_p                  m_signals;
      //  std::shared_ptr<std::mutex>  m_mutex;
};


/**
 * @brief The Signal2 class
 *
 * Dispatcher signal. Queue up function calls, then invoke all of them
 * using the Dispatch() method.
 *
 * Once Dispatch is called, the queue is cleared.
 *
 * The queue of function calls is double buffered.
 */
template<typename func_t>
class dispatcher_signal : public signal<func_t>
{
    public:

        template<typename ..._Funct>
        void operator()( _Funct &&... _Args )
        {
            Queue( std::forward<_Funct>(_Args)...);
        }

        template<typename ..._Funct>
        void operator()(const _Funct &... _Args )
        {
            Queue( std::forward<_Funct>(_Args)...);
        }

        void dispatch( )
        {
            std::swap(m_Q, m_Q1);
            for( auto & t : m_Q1 )
            {
                t();
            }
            m_Q1.clear();
        }

        protected:
            template<typename ..._Funct>
            void Queue(_Funct&&... _Args)
            {
                for(auto & f : this->get_container().m_container )
                {
                    m_Q.push_back( std::bind( f.second,std::forward< _Funct >(_Args)... ) );
                }
            }

            std::vector< std::function<void(void)> > m_Q;
            std::vector< std::function<void(void)> > m_Q1;
};


template<typename func_t>
class threaded_signal : public signal<func_t>
{
    public:

        template<typename ..._Funct>
        void operator()( _Funct&&... _Args )
        {
            for( auto & f : this->get_container().m_container )
            {
                if( f.first != 0 )
                {
                    std::thread T( f.second, std::forward< _Funct >(_Args)...);
                    T.detach();
                }
            }

        }
};

}

#endif
