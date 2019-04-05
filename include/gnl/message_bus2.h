#pragma once

#ifndef GNL_MESSAGE_BUS2_H
#define GNL_MESSAGE_BUS2_H

#include <iostream>
#include <cstdint>
#include <utility>
#include <functional>
#include <any>
#include <typeindex>

#include <map>
#include <vector>
#include <list>

#ifndef GNL_NAMESPACE
#define GNL_NAMESPACE gnl
#endif

#include <thread>


namespace GNL_NAMESPACE
{



struct msgBus
{
public:

    using slot_id = std::shared_ptr< std::pair< std::type_index, std::size_t> >;



    template<typename Class_t,  typename Msg_t>
    slot_id connect( void (Class_t::*x)(Msg_t const & m), Class_t * me )
    {
        return connect<Msg_t>( std::bind(x, me, std::placeholders::_1) );
    }

    /**
     * @brief connect
     * @param f
     * @return
     *
     * Connect a function to the message bus. The function signature
     * only
     */
    template<typename Msg_t>
    slot_id connect( std::function<void(Msg_t const&)> const & f)
    {
        return _connect_functional<Msg_t>(f);
    }

#if defined GNL_ALLOW_NON_CONST
    // not implemented yet
    template<typename Msg_t>
    slot_id connect( std::function<void(Msg_t&)> const & f)
    {
        //return _connect_functional<Msg_t>(f);
    }

    template<typename Msg_t>
    slot_id connect( std::function<void(Msg_t)> const & f)
    {
        //return _connect_functional<Msg_t>(f);
    }
#endif

    void disconnect(slot_id index)
    {
        m_signals[index->first].m_functions[index->second].first.reset();
        m_signals[index->first].m_free_indices.push_back(index->second);
    }


    size_t queue_size() const
    {
        return m_queue.size();
    }




protected:

    enum qual_type
    {
        const_ref,
        ref,
        value
    };

    struct SignalInfo
    {
        using value_type = std::pair<std::any, qual_type>;
        std::vector<value_type> m_functions;
        std::vector<size_t>   m_free_indices;
    };

    std::map<std::type_index, SignalInfo > m_signals;

    // queue of messages to dispatch.
    std::list< std::function<void(void)> > m_queue;

    template<typename Msg_t>
    void _execute(Msg_t const M) const
    {
        std::type_index i(typeid(Msg_t));

        using function_type = std::function<void(Msg_t const&)>;

        auto it = m_signals.find(i);

        if( it != m_signals.end())
        {
            for(auto & x : it->second.m_functions )
            {
                if( x.first.has_value() )
                    std::any_cast<function_type>(x.first)(M);
            }
        }
    }

    /**
     * @brief push
     * @param M
     *
     * Push a message on the queue. But do not send the message
     * through the message bus until dispatch() is called.
     */
    template<typename Msg_t>
    void _push(Msg_t const M)
    {
        m_queue.push_back(
                    [this,M]() { this->_execute(M); }
                     );
    }

    /**
     * @brief dispatch
     *
     * Processes all the messages that are currently in the queue.
     */
    void _dispatch(size_t n)
    {
        auto s = m_queue.size();
        s = std::min(s,n);
        while(s--)
        {
            m_queue.front()();
            m_queue.pop_front();
        }
    }


    template<typename Msg_t>
    slot_id _connect_functional( std::function<void(Msg_t const&)> f)
    {
        std::type_index i(typeid(Msg_t));

        auto & SI = m_signals[i];

        auto ret = std::make_shared<std::pair< std::type_index, std::size_t>>(i, size_t(0));

        if( SI.m_free_indices.size() )
        {
            ret->second = SI.m_free_indices.back();
            SI.m_functions[ ret->second ].first = f;
            SI.m_functions.pop_back();
        }
        else
        {
            ret->second = SI.m_functions.size();
            SI.m_functions.push_back( std::make_pair(f, const_ref) );
        }

        return ret;
    }

};

class event_bus : public msgBus
{

public:
    /**
     * @brief send
     * @param M
     *
     * Send a message to any listeners currently listening for
     * Msg_t.
     */
    template<typename Msg_t>
    void send(Msg_t const M) const
    {
        _execute(M);
    }
};


class event_queue : public msgBus
{
public:
    template<typename Msg_t>
    void send(Msg_t const M)
    {
        _push(M);
    }

    void dispatch(size_t n = std::numeric_limits<size_t>::max() )
    {
        _dispatch(n);
    }
};


class event_queue_bus : public msgBus
{
public:
    template<typename Msg_t>
    void send(Msg_t const M) const
    {
        _execute(M);
    }

    template<typename Msg_t>
    void push(Msg_t const M)
    {
        _push(M);
    }

    void dispatch(size_t n = std::numeric_limits<size_t>::max())
    {
        _dispatch(n);
    }
};

}

#endif
