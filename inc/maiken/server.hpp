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
#ifndef _MAIKEN_SERVER_HPP_
#define _MAIKEN_SERVER_HPP_

#include "maiken/defs.hpp"

#include "kul/http.hpp"

#include "kul/cli.hpp"
#include "kul/log.hpp"
#include "kul/proc.hpp"
#include "kul/threads.hpp"

#include <cereal/archives/portable_binary.hpp>

#include <sstream>

int main(int argc, char* argv[]);

namespace maiken{

namespace server{
class Exception : public kul::Exception{
    public:
        Exception(const char*f, const uint16_t& l, const std::string& s) : kul::Exception(f, l, s){}
};
}

class SocketServer : public Constants, public kul::tcp::SocketServer<char>{
    public:
        bool handle(
                char*const in, 
                const size_t& inLen, 
                char*const out, 
                size_t& outLen) override {

            std::string s(in, inLen);

            std::stringstream ss;

            std::istringstream iss(s);

            {
                cereal::PortableBinaryInputArchive iarchive(iss); // Create an input archive

                int m1, m2, m3;
                iarchive(m1, m2, m3); // Read the data from the archive
                ss << "m1: " << m1 << " m2: " << m2 << " m3: " << m3;
            }

            std::strcpy(out, ss.str().c_str());
            return true; // if true, close connection
        }
        SocketServer(const uint16_t port = 8080) : kul::tcp::SocketServer<char>(port){}

        static
        std::unique_ptr<maiken::SocketServer>
        CREATE(int16_t argc, char *argv[]) KTHROW(server::Exception);
};


}
#endif /* _MAIKEN_SERVER_HPP_ */
