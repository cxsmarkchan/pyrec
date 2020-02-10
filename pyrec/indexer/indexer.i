%{
  #include "pyrec/core/indexer/indexer_interface.h"
  #include "pyrec/core/indexer/indexer_client.h"
  #include "pyrec/core/indexer/hash_indexer.h"
%}

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

class LocalIndexerClient {
 public:
  explicit LocalIndexerClient(const IndexerServiceInterface& server_interface);
};

class RemoteIndexerClient : public IndexerClient {
  explicit RemoteIndexerClient(const char* ip, int port);
};

}  // namespace service
}  // namespace pyrec

