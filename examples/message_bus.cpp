#include <gnl/gnl_message_bus.h>
#include <iostream>

#include <functional>

struct Msg_A
{

};

struct Msg_B
{

};

class Node1 :
        public gnl::message_bus_2::node<Msg_A>,
        public gnl::message_bus_2::node<Msg_B>
{
public:
    virtual void onNotify(const Msg_A & m) override
    {
        std::cout << "Msg_A recieved by Node1" << std::endl;
    }
    virtual void onNotify(const Msg_B & m) override
    {
        std::cout << "Msg_B recieved by Node1" << std::endl;
    }
};

class Node2 : public gnl::message_bus_2::node<Msg_A>
{
public:

    virtual void onNotify(const Msg_A & m) override
    {
        std::cout << "Msg_A recieved by Node2" << std::endl;
    }
};

using func_t = std::function<void(void)>;

std::queue<func_t> messages;

template<typename T>
void push_msg(T const & msg)
{

}

int main(int argc, char ** argv)
{


    gnl::message_bus_2 B;

    Node1 n1;
    Node2 n2;

    B.register_node<Msg_A>(&n1);
    B.register_node<Msg_B>(&n1);
    B.register_node<Msg_A>(&n2);


    std::cout << "----- Sending Msg_A ---- " << std::endl;
    B.send_message( Msg_A() );
    std::cout << "----- Sending Msg_B ---- " << std::endl;
    B.send_message( Msg_B() );

    std::cout << "----- Unregistering Node1 from Msg_A ---- " << std::endl;
    B.unregister_node<Msg_A>(&n1);

    std::cout << "----- Sending Msg_B ---- " << std::endl;
    B.send_message( Msg_A() );

    B.dispatch();
    //Node3 n3;

    return 0;
}

