#ifndef GNL_SIGNALS_H
#define GNL_SIGNALS_H

#include <functional>
#include <vector>
#include <tuple>
#include <future>

namespace gnl
{
#if 0
template<int...> struct seq { };

template<int N, int ...S> struct gens : gens<N-1, N-1, S...> { };

template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };


template <typename return_type, typename ...Args>
/**
 * @brief The SavedFunctionCall struct
 * A wrapper around a tuple to store the function arguments.
 */
struct SavedFunctionCall
{
    using function_t = std::function<return_type(Args...)>;
    using tuple_t    = std::tuple<Args...>;

public:

    SavedFunctionCall( const tuple_t & t) : params(t)
    {
    }

    return_type operator () ( function_t &  f ) const
    {
        return callFunc( f , typename gens<sizeof...(Args)>::type() );
    }

private:
    template<int ...S>
    return_type callFunc(std::function<return_type(Args...)> & f, seq<S...>) const
    {
        return f( std::get<S>(params) ...);
    }

    tuple_t params;
};
#endif

class BaseSlot
{
    public:
        virtual ~BaseSlot() {}
};

/**
 * @brief The Signal class
 *
 * Signal class:  Attach multiple function objects
 * and call all function objects with a single call
 * to the () operator
 */
//template<typename return_type, typename... _Funct>
template<typename func_t>
class Signal
{
    public:
        using id                = unsigned long;  //id type
        using function_t        = std::function<func_t>; // function object type

        using value_t           = std::pair<id,function_t>;

        using container_t       = std::vector< value_t  >;
        using container_p       = std::shared_ptr< container_t >;
        using mutex_p           = std::shared_ptr< std::mutex >;


        class Slot : public BaseSlot
        {
        private:
            id           m_id;
            container_p  m_Container;
            mutex_p      m_mutex;

        public:
            Slot() : m_id(0){}

            Slot(id ID, container_p p, mutex_p m) : m_id(ID) , m_Container(p), m_mutex(m)
            {
            }

            Slot(const Slot & other) = delete;

            Slot(Slot && other)
            {
                m_id        = std::move( other.m_id );
                other.m_id  = 0;
                m_Container = std::move( other.m_Container);
                m_mutex     = std::move(other.m_mutex);
            }

            virtual ~Slot()
            {
                Disconnect();
            }

            Slot & operator=(const Slot & other) = delete;

            Slot & operator=(Slot && other)
            {
                if( this != &other)
                {
                    m_id        = std::move( other.m_id );
                    m_Container = std::move( other.m_Container);
                    m_mutex     = std::move(other.m_mutex);
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
            virtual void Disconnect()
            {
                if( m_id == 0 ) return;
                std::lock_guard<std::mutex> L( *m_mutex);

                for(auto & t : *m_Container)
                {
                    if( t.first == m_id)
                    {
                        //std::cout << "Slot Disconnected" << std::endl;
                        t.first = 0;
                        break;
                    }

                }
                m_mutex.reset();
                m_id = 0;
            }
        };


        Signal() : m_signals( new container_t() ), m_mutex( new std::mutex() )
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
        Slot Connect( function_t f , int position = -1)
        {
            static id ID = 1;
            std::lock_guard<std::mutex> L( *m_mutex );

            if(position<0)
                 get_container().emplace_back( ID, f );
            else
                get_container().insert( get_container().begin()+position, value_t(ID,f) );

            return  Slot(ID++, m_signals, m_mutex);

        }

        void Disconnect(id SlotID)
        {
            std::lock_guard<std::mutex> L( *m_mutex );;
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
        Slot operator << ( function_t f )
        {
            return Connect(f);
        }

        template<typename ..._Funct>
        void operator()( _Funct &&... _Args )
        {
            std::lock_guard<std::mutex> L( *m_mutex );
            for(auto & f : get_container())
            {
                if( f.first != 0 )
                {
                    f.second( std::forward<_Funct>(_Args)...);
                }
            }
        }


    protected:
        container_t&                 get_container() const { return *m_signals; }

        container_p                  m_signals;
        std::shared_ptr<std::mutex>  m_mutex;
};


template<typename func_t>
class Signal2 : public Signal<func_t>
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

        void Dispatch( )
        {
            for( auto & t : m_Q )
            {
                t();
            }
        }

        protected:
            template<typename ..._Funct>
            void Queue(_Funct&&... _Args)
            {
                for(auto & f : this->set_container() )
                {
                    m_Q.push_back( std::bind( f.second,std::forward< _Funct >(_Args)... ) );
                }

                //m_queue.push_back( tuple_t( std::forward< _Funct >(_Args)...) );
            }

            //std::vector<  tuple_t  >   m_queue;
            std::vector< std::function<void(void)> > m_Q;
};


template<typename func_t>
class ThreadedSignal : public Signal<func_t>
{
    public:

        template<typename ..._Funct>
        void operator()( _Funct&&... _Args )
        {

            for( auto & f : this->get_container() )
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
