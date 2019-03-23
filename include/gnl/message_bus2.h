#pragma once

#ifndef GNL_MESSAGE_BUS2_H
#define GNL_MESSAGE_BUS2_H

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

struct Foo {
    void print_sum(int n1, int n2)
    {
       // std::cout << n1+n2 << '\n';
    }
    int data = 10;
};

struct msgBus
{
public:

    using slot_id = std::pair< std::type_index, std::size_t>;


    static msgBus & global()
    {
        static auto g = std::make_shared<msgBus>();
        return *g;
    }

    template<typename Msg_t>
    static slot_id sconnect( std::function<void(Msg_t const&)> const & f)
    {
        return global().connect<Msg_t>(f);
    }

    static void sdisconnect(slot_id index)
    {
        global().disconnect(index);
    }

    template<typename Msg_t>
    static void ssend(Msg_t const M)
    {
        global().send(M);
    }


    template<typename Msg_t>
    slot_id connect_functional( std::function<void(Msg_t const&)> f)
    {
        std::type_index i(typeid(Msg_t));

        auto & SI = m_signals[i];

        auto ret = std::make_pair(i, size_t(0));

        if( SI.m_free_indices.size() )
        {
            ret.second = SI.m_free_indices.back();
            SI.m_functions[ ret.second ] = f;
            SI.m_functions.pop_back();
        }
        else
        {
            ret.second = SI.m_functions.size();
            SI.m_functions.push_back(f);
        }

        return ret;
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
        return connect_functional<Msg_t>(f);
    }

    void disconnect(slot_id index)
    {
        m_signals[index.first].m_functions[index.second].reset();
    }

    template<typename Msg_t>
    void send(Msg_t const M) const
    {
        std::type_index i(typeid(Msg_t));

        using function_type = std::function<void(Msg_t const&)>;

        auto it = m_signals.find(i);

        if( it != m_signals.end())
        {
            for(auto & x : it->second.m_functions )
            {
                if( x.has_value() ) std::any_cast<function_type>(x)(M);
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
    void push(Msg_t const M)
    {
        m_queue.push_back(
                    [this,M]() { this->send(M); }
                     );
    }

    /**
     * @brief dispatch
     *
     * Processes all the messages that are currently in the queue.
     */
    void dispatch()
    {
        auto s = m_queue.size();
        while(s--)
        {
            m_queue.front()();
            m_queue.pop_front();
        }

    }

    struct SignalInfo
    {
        std::vector<std::any> m_functions;
        std::vector<size_t>   m_free_indices;
    };

    std::map<std::type_index, SignalInfo > m_signals;

    // queue of messages to dispatch.
    std::list< std::function<void(void)> > m_queue;
    //std::vector< std::function<void(void)> > m_queue;

};

}

#endif
