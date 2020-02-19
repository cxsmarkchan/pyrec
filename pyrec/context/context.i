%{
  #include "core/context/context_interface.h"
  #include "core/context/context_client.h"
  #include "core/context/kv_context.h"
%}

%shared_ptr(pyrec::service::ContextClientInternal)
%shared_ptr(pyrec::service::RemoteContextClientInternal)
%shared_ptr(pyrec::service::LocalContextClientInternal)
%include "core/context/context_client.h"

namespace pyrec {
namespace service {

class ContextServiceInterface {
 public:
  int Run(const char* ip, int port);
};

class KvContextServerInterface : public ContextServiceInterface {
 public:
  KvContextServerInterface& SetKeyId(pyrec::types::FieldId,
                                     pyrec::types::FieldId);
  KvContextServerInterface& AddField(pyrec::types::FieldId);
  KvContextServerInterface& SetIndexerKvServer(
      std::shared_ptr<IndexerClientInternal> indexer);

  int Create();
};

}  // namespace service
}  // namespace pyrec
