%{
  #include "core/retrieval/retrieval_interface.h"
  #include "core/retrieval/retrieval_client.h"
  #include "core/retrieval/content_based.h"
%}

%include "pyrec/indexer/indexer.i"

%shared_ptr(pyrec::service::RetrievalClientInternal)
%shared_ptr(pyrec::service::RemoteRetrievalClientInternal)
%shared_ptr(pyrec::service::LocalRetrievalClientInternal)
%include "core/retrieval/retrieval_client.h"

namespace pyrec {
namespace service {

class RetrievalServiceInterface {
 public:
  int Run(const char* ip, int port);
};

class CBRetrievalServerInterface : public RetrievalServiceInterface {
 public:
  const CBRetrievalServerInterface& SetIndexer(
      std::shared_ptr<IndexerClientInternal>);
  const CBRetrievalServerInterface& SetRetrievalId(pyrec::types::FieldId);
  const CBRetrievalServerInterface& AddItemKey(pyrec::types::FieldId);
  const CBRetrievalServerInterface& SetRequestNum(int);
  const CBRetrievalServerInterface& AddFilterRule(
      pyrec::types::FieldId key, const std::string& value);
  const CBRetrievalServerInterface& SetExtractKey(
      pyrec::types::FieldId scope, pyrec::types::FieldId key);
  int Create();
};

}  // namespace service
}  // namespace pyrec

