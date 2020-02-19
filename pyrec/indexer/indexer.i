%{
  #include "core/indexer/indexer_interface.h"
  #include "core/indexer/indexer_client.h"
  #include "core/indexer/hash_indexer.h"
%}

%shared_ptr(pyrec::service::IndexerClientInternal)
%shared_ptr(pyrec::service::RemoteIndexerClientInternal)
%shared_ptr(pyrec::service::LocalIndexerClientInternal)
%include "core/indexer/indexer_client.h"

namespace pyrec {
namespace service {

class IndexerServiceInterface {
 public:
  int Run(const char* ip, int port);
};

class HashIndexerServerInterface : public IndexerServiceInterface {
 public:
  int CreateFromCsv(const char* file_name,
                    const pyrec::types::CsvFormat& format);
  size_t IndexSize() const;
};

}  // namespace service
}  // namespace pyrec

