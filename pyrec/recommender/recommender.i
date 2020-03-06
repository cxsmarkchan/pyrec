%{
  #include "core/recommender/recommender_interface.h"
  #include "core/recommender/recommender_client.h"
  #include "core/recommender/recommender.h"
%}

%include "pyrec/retrieval/retrieval.i"
%include "pyrec/context/context.i"

%shared_ptr(pyrec::service::RecommenderClientInternal)
%shared_ptr(pyrec::service::RemoteRecommenderClientInternal)
%shared_ptr(pyrec::service::LocalRecommenderClientInternal)
%include "core/recommender/recommender_client.h"

namespace pyrec {
namespace service {

class RecommenderServiceInterface {
 public:
  int Run(const char* ip, int port);
};

class RecommenderServerInterface : public RecommenderServiceInterface {
 public:
  RecommenderServerInterface& AddContext(
      pyrec::types::FieldId context_id,
      std::shared_ptr<ContextClientInternal>);
  RecommenderServerInterface& AddRetrieval(
      std::shared_ptr<RetrievalClientInternal>);
  RecommenderServerInterface& SetRequestNum(int);
  int Create();
};

}  // namespace service
}  // namespace pyrec
