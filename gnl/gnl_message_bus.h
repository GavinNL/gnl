#include <typeindex>
//#include <any>
//#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>

#include <functional>
namespace gnl
{

/**
 * @brief The message_bus class
 *
 * This message_bus class is used as a central hub to communicate between
 * multiple nodes.
 *
 * Each node connected to the message_bus can register itself to recieve
 * a particular message. Anytime another node sends that message to the
 * message bus, all nodes which ahve registered to receive that message
 * will be notified.
 *
 */
class message_bus
{
public:

    /**
     * @brief The node class
     *
     * This class must be inherited by all nodes wanting
     * to recieve Msg_t messages from the message bus.
     */
    template<typename Msg_t>
    class node
    {
      public:
        virtual void onNotify(const Msg_t & msg) =0;
    };

    /**
     * @brief send_message
     * @param f
     *
     * Send a message to the message bus.
     */
    template<typename Msg_t>
    void send_message(Msg_t const & f)
    {
        static const std::type_index id = std::type_index( typeid(Msg_t) );

        for(auto & F : m_nodes[id] )
        {
            reinterpret_cast<node<Msg_t>*>(F)->onNotify(f);
        }
    }

    /**
     * @brief register_node
     * @param n
     *
     * Register a node as a reciever.
     */
    template<typename Msg_t>
    void register_node(node<Msg_t> * n)
    {
        m_nodes[std::type_index( typeid(Msg_t) )].push_back( n );
    }

    /**
     * @brief unregister_node
     * @param n
     *
     * Unregisters a node as a listener. Node n will no longer
     * recieve messages of type Msg_t.
     */
    template<typename Msg_t>
    void unregister_node(node<Msg_t> * n)
    {
        auto i = std::type_index(typeid(Msg_t));
        m_nodes[i].erase(std::remove(m_nodes[i].begin(), m_nodes[i].end(), n),
                       m_nodes[i].end());

    }

    /**
     * @brief unregister_node
     * @param n
     *
     * Unregisters node, n, from all messages.
     */
    void unregister_node(void * n)
    {
        for(auto & V : m_nodes)
        {
            V.second.erase(std::remove(V.second.begin(), V.second.end(), n),
                           V.second.end());
        }
    }

    std::unordered_map< std::type_index, std::vector<void*> > m_nodes;
};


class message_bus_2
{
public:

    /**
     * @brief The node class
     *
     * This class must be inherited by all nodes wanting
     * to recieve Msg_t messages from the message bus.
     */
    template<typename Msg_t>
    class node
    {
      public:
        virtual void onNotify(const Msg_t & msg) =0;
    };

    using func_t = std::function<void(void)>;
    /**
     * @brief send_message
     * @param f
     *
     * Send a message to the message bus.
     */
    template<typename Msg_t>
    void send_message(Msg_t const & f)
    {
        static const std::type_index id = std::type_index( typeid(Msg_t) );

        func_t Fu = [=]()
        {
            for(auto & F : m_nodes[id] )
            {
                reinterpret_cast<node<Msg_t>*>(F)->onNotify(f);
            }
        };


        m_calls.push(Fu);
    }

    std::queue< func_t>  m_calls;

    void dispatch()
    {
        while(m_calls.size())
        {
            m_calls.front()();
            m_calls.pop();
        }
    }
    /**
     * @brief register_node
     * @param n
     *
     * Register a node as a reciever.
     */
    template<typename Msg_t>
    void register_node(node<Msg_t> * n)
    {
        m_nodes[std::type_index( typeid(Msg_t) )].push_back( n );
    }

    /**
     * @brief unregister_node
     * @param n
     *
     * Unregisters a node as a listener. Node n will no longer
     * recieve messages of type Msg_t.
     */
    template<typename Msg_t>
    void unregister_node(node<Msg_t> * n)
    {
        auto i = std::type_index(typeid(Msg_t));
        m_nodes[i].erase(std::remove(m_nodes[i].begin(), m_nodes[i].end(), n),
                       m_nodes[i].end());

    }

    /**
     * @brief unregister_node
     * @param n
     *
     * Unregisters node, n, from all messages.
     */
    void unregister_node(void * n)
    {
        for(auto & V : m_nodes)
        {
            V.second.erase(std::remove(V.second.begin(), V.second.end(), n),
                           V.second.end());
        }
    }

    std::unordered_map< std::type_index, std::vector<void*> > m_nodes;
};


}
