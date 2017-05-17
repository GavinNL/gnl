#ifndef GNL_CONSOLE_H
#define GNL_CONSOLE_H

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace gnl
{


class Console
{
public:
    enum Colour
    {
        Black     = 0,
        Red       = 1,
        Green     = 2,
        Yellow    = 3,
        Blue      = 4,
        Magenta   = 5,
        Cyan      = 6,
        White     = 7,
        Default   = 9
    };

    Console(Colour f=Default, Colour b=Default, bool bol=false) : fg(30+f), bg(40+b), bold(bol)
    {

    }

    Console(int f, int b, bool bol) : fg(f), bg(b), bold(bol)
    {

    }

    int fg  ;
    int bg  ;
    int bold;

};


inline std::ostream & operator<<(std::ostream &os, const gnl::Console & p)
{
    //os << "033[" << p.bold << ";" << p.bg << ";" << p.fg << "m";
    os << "\033[" << p.bold << ";" << p.bg << ";" << p.fg << "m";
    return os;
}


static const Console Black   = Console( Console::Black  , Console::Default);
static const Console Red     = Console( Console::Red    , Console::Default);
static const Console Blue    = Console( Console::Blue   , Console::Default);
static const Console Green   = Console( Console::Green  , Console::Default);
static const Console Yellow  = Console( Console::Yellow , Console::Default);
static const Console Cyan    = Console( Console::Cyan   , Console::Default);
static const Console Magenta = Console( Console::Magenta, Console::Default);
static const Console White   = Console( Console::White  , Console::Default);

static const Console K  = Console( Console::Black  , Console::Default);
static const Console R  = Console( Console::Red    , Console::Default);
static const Console B  = Console( Console::Blue   , Console::Default);
static const Console G  = Console( Console::Green  , Console::Default);
static const Console Y  = Console( Console::Yellow , Console::Default);
static const Console C  = Console( Console::Cyan   , Console::Default);
static const Console M  = Console( Console::Magenta, Console::Default);
static const Console W  = Console( Console::White  , Console::Default);

static const Console Kk  = Console( Console::Black  , Console::Black);
static const Console Rk  = Console( Console::Red    , Console::Black);
static const Console Bk  = Console( Console::Blue   , Console::Black);
static const Console Gk  = Console( Console::Green  , Console::Black);
static const Console Yk  = Console( Console::Yellow , Console::Black);
static const Console Ck  = Console( Console::Cyan   , Console::Black);
static const Console Mk  = Console( Console::Magenta, Console::Black);
static const Console Wk  = Console( Console::White  , Console::Black);

static const Console Kr  = Console( Console::Black  , Console::Red);
static const Console Rr  = Console( Console::Red    , Console::Red);
static const Console Br  = Console( Console::Blue   , Console::Red);
static const Console Gr  = Console( Console::Green  , Console::Red);
static const Console Yr  = Console( Console::Yellow , Console::Red);
static const Console Cr  = Console( Console::Cyan   , Console::Red);
static const Console Mr  = Console( Console::Magenta, Console::Red);
static const Console Wr  = Console( Console::White  , Console::Red);

static const Console Kb  = Console( Console::Black  , Console::Blue);
static const Console Rb  = Console( Console::Red    , Console::Blue);
static const Console Bb  = Console( Console::Blue   , Console::Blue);
static const Console Gb  = Console( Console::Green  , Console::Blue);
static const Console Yb  = Console( Console::Yellow , Console::Blue);
static const Console Cb  = Console( Console::Cyan   , Console::Blue);
static const Console Mb  = Console( Console::Magenta, Console::Blue);
static const Console Wb  = Console( Console::White  , Console::Blue);

static const Console Kg  = Console( Console::Black  , Console::Green);
static const Console Rg  = Console( Console::Red    , Console::Green);
static const Console Bg  = Console( Console::Blue   , Console::Green);
static const Console Gg  = Console( Console::Green  , Console::Green);
static const Console Yg  = Console( Console::Yellow , Console::Green);
static const Console Cg  = Console( Console::Cyan   , Console::Green);
static const Console Mg  = Console( Console::Magenta, Console::Green);
static const Console Wg  = Console( Console::White  , Console::Green);

static const Console Ky  = Console( Console::Black  , Console::Yellow);
static const Console Ry  = Console( Console::Red    , Console::Yellow);
static const Console By  = Console( Console::Blue   , Console::Yellow);
static const Console Gy  = Console( Console::Green  , Console::Yellow);
static const Console Yy  = Console( Console::Yellow , Console::Yellow);
static const Console Cy  = Console( Console::Cyan   , Console::Yellow);
static const Console My  = Console( Console::Magenta, Console::Yellow);
static const Console Wy  = Console( Console::White  , Console::Yellow);

static const Console Kc  = Console( Console::Black  , Console::Cyan);
static const Console Rc  = Console( Console::Red    , Console::Cyan);
static const Console Bc  = Console( Console::Blue   , Console::Cyan);
static const Console Gc  = Console( Console::Green  , Console::Cyan);
static const Console Yc  = Console( Console::Yellow , Console::Cyan);
static const Console Cc  = Console( Console::Cyan   , Console::Cyan);
static const Console Mc  = Console( Console::Magenta, Console::Cyan);
static const Console Wc  = Console( Console::White  , Console::Cyan);

static const Console Km  = Console( Console::Black  , Console::Magenta);
static const Console Rm  = Console( Console::Red    , Console::Magenta);
static const Console Bm  = Console( Console::Blue   , Console::Magenta);
static const Console Gm  = Console( Console::Green  , Console::Magenta);
static const Console Ym  = Console( Console::Yellow , Console::Magenta);
static const Console Cm  = Console( Console::Cyan   , Console::Magenta);
static const Console Mm  = Console( Console::Magenta, Console::Magenta);
static const Console Wm  = Console( Console::White  , Console::Magenta);

static const Console Kw  = Console( Console::Black  , Console::White);
static const Console Rw  = Console( Console::Red    , Console::White);
static const Console Bw  = Console( Console::Blue   , Console::White);
static const Console Gw  = Console( Console::Green  , Console::White);
static const Console Yw  = Console( Console::Yellow , Console::White);
static const Console Cw  = Console( Console::Cyan   , Console::White);
static const Console Mw  = Console( Console::Magenta, Console::White);
static const Console Ww  = Console( Console::White  , Console::White);


static std::ostream& print_time(const Console & C)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << C << "[" << std::put_time(std::localtime(&now_c), "%F %T") << "] - ";

    return std::cout;
}

#ifdef GNL_LOG_ALL
    #define GNL_INFO 1
    #define GNL_DEBUG 1
    #define GNL_VERBOSE 1
    #define GNL_TIMER 1
#endif

#ifdef GNL_INFO
    #define GNL_INFO 1
#else
    #define GNL_INFO 0
#endif

#ifdef GNL_DEBUG
    #define GNL_DEBUG 1
#else
    #define GNL_DEBUG 0
#endif

#ifdef GNL_VERBOSE
    #define GNL_VERBOSE 1
#else
    #define GNL_VERBOSE 0
#endif

#ifdef GNL_TIMER
    #define GNL_TIMER 1
#else
    #define GNL_TIMER 0
#endif

#ifdef _WIN32
#define GNL_LOGD  if(GNL_DEBUG   ) gnl::print_time() << "[Debug] - "
#define GNL_LOGI  if(GNL_INFO    ) gnl::print_time() << "[Info] - "
#define GNL_LOGV  if(GNL_VERBOSE ) gnl::print_time() << "[Verbose] - "
#define GNL_LOGT  if(GNL_TIMER   ) gnl::print_time() << "[Timer] - "
#else
#define GNL_LOGD  if(GNL_DEBUG   ) gnl::print_time(gnl::Red) << "[Debug] - "
#define GNL_LOGI  if(GNL_INFO    ) gnl::print_time(gnl::G) << "[Info] - "
#define GNL_LOGV  if(GNL_VERBOSE ) gnl::print_time(gnl::C) << "[Verbose] - "
#define GNL_LOGT  if(GNL_TIMER   ) gnl::print_time(gnl::M) << "[Timer] - "
#endif



}
#endif
