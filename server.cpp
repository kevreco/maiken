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

#include "kul/log.hpp"
#include "kul/signal.hpp"

#ifdef _WIN32
#define bzero ZeroMemory
#endif

class Post : public kul::http::_1_1PostRequest{
    public:
        Post(
                const std::string& host, 
                const std::string& path = "", 
                const uint16_t& port = 80,
                const std::string& bdy = "")
            : kul::http::_1_1PostRequest(host, path, port, bdy){
        }
        void handleResponse(const kul::hash::map::S2S& h, const std::string& b) override {
            KUL_DBG_FUNC_ENTER
            for(const auto& p : h)
                KOUT(NON) << "HEADER: " << p.first << " : " << p.second;
            KOUT(NON) << "POST RESPONSE: " << b;
        }
};


int main(int argc, char* argv[]) {
    kul::Signal sig;

    uint8_t ret = 0;
    const int64_t s = kul::Now::MILLIS();
    try{
        std::unique_ptr<maiken::SocketServer> serv(std::make_unique<maiken::SocketServer>(8080));
        
        kul::Thread t([&](){
            serv->start();
        });

        sig.intr([&](int16_t){
            KERR << "Interrupted";
            
            if(serv) serv->stop();
            t.join();
            exit(2);
        });

        t.run();
        kul::this_thread::sleep(100);

        std::ostringstream ss(std::ios::out | std::ios::binary);

        {
            cereal::PortableBinaryOutputArchive oarchive(ss);

            int m1 = 112114, m2 = 112114, m3 = 112114;
            oarchive(m1, m2, m3);
        }

        std::string s1(ss.str());

        kul::tcp::Socket<char> sock;
        if(!sock.connect("localhost", 8080)) KEXCEPT(kul::tcp::Exception, "TCP FAILED TO CONNECT!");
        sock.write(s1.c_str(), s1.size());

        char buf[_KUL_TCP_REQUEST_BUFFER_];
        bzero(buf, _KUL_TCP_REQUEST_BUFFER_);
        sock.read(buf, _KUL_TCP_REQUEST_BUFFER_);
        KLOG(INF) << buf;

        serv->stop();
        t.join();
    }
    catch(const kul::Exit& e){
        if(e.code() != 0) KERR << kul::os::EOL() << "ERROR: " << e;
        ret = e.code();
    }
    catch(const kul::proc::ExitException& e){
        KERR << e;
        ret = e.code();
    }
    catch(const kul::Exception& e){
        KERR << e.stack();
        ret = 2;
    }
    catch(const std::exception& e){
        KERR << e.what();
        ret = 3;
    }

    return ret;
}