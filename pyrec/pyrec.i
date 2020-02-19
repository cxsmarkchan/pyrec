%module pyrec
%{
  #include "core/util/types.h"
  #include "core/service/base.h"
  #include "core/recommend/recommend.h"
%}

%include "tools/swig/stl.i"

%include "core/util/types.h"
%include "pyrec/indexer/indexer.i"
%include "pyrec/retrieval/retrieval.i"
%include "pyrec/context/context.i"

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
