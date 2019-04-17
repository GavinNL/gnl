#include <gnl/socket_shell.h>

#include<catch2/catch.hpp>

SCENARIO("Replacing Variables")
{
    std::map<std::string, std::string> V;
    V["VAR"] = "value";
    V["SUBVAR"] = "VAR";

    THEN("We can replace variables using ${VAR}")
    {
        std::string cmd = "${VAR} some text --${VAR}--";
        gnl::replace_with_vars(cmd, V);

        REQUIRE( cmd == "value some text --value--");
    }
    THEN("We perform double replacements using ${${VAR}}")
    {
        std::string cmd = "${${SUBVAR}} some text --${${SUBVAR}}--";
        gnl::replace_with_vars(cmd, V);

        REQUIRE( cmd == "value some text --value--");
    }

}

TEST_CASE("extract_pipes()")
{

    std::string S = "cmd1 ${var1} | cmd2 $( cmd3 | cmd4 )";

    auto cmds = gnl::extract_pipes(S);

    REQUIRE( cmds.size() == 2);

    {
        REQUIRE( cmds[0] == "cmd1 ${var1} ");
        REQUIRE( cmds[1] == " cmd2 $( cmd3 | cmd4 )");
    }

}
