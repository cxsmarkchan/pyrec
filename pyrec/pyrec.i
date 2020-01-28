%module pyrec
%{
  #include "pyrec/core/service/recommend.h"
%}

namespace pyrec {
namespace service {
template<class Server>
class ServerWrapper {
 public:
  int Run(const char* ip, int port);
};

} // namespace service
} // namespace pyrec

%template(RecommendServerWrapper) pyrec::service::ServerWrapper<pyrec::service::RecommendServer>;
