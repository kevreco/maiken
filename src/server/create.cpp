/**
Copyright (c) 2017, Philip Deegan.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Philip Deegan nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "maiken/server.hpp"

std::unique_ptr<maiken::SocketServer> maiken::SocketServer::CREATE(int16_t argc, char *argv[]) KTHROW(kul::Exception){
    using namespace kul::cli;

    std::vector<Arg> argV { 
    	Arg('a', STR_ARG    , ArgType::STRING), Arg('A', STR_ADD  , ArgType::STRING),
        Arg('b', STR_BINC   , ArgType::STRING), Arg('B', STR_BPATH, ArgType::STRING),
        Arg('C', STR_DIR    , ArgType::STRING),
        Arg('d', STR_DEP    , ArgType::MAYBE) , Arg('D', STR_DEBUG),
	};

    std::vector<Cmd> cmdV { 
        Cmd(STR_COMPILE),  Cmd(STR_LINK),      Cmd(STR_PROFILES),
        Cmd(STR_DBG),      Cmd(STR_PACK),      Cmd(STR_INFO),
        Cmd(STR_TRIM),     Cmd(STR_TREE)
    };
    Args args(cmdV, argV);
    try{
        args.process(argc, argv);
    }catch(const kul::cli::Exception& e){
        KEXIT(1, e.what());
    }
    return std::make_unique<maiken::SocketServer>(8080);
}
