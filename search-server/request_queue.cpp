 #include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
 : server(search_server){}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    vector<Document> res = server.FindTopDocuments(raw_query, status);
    AddFind(res);
    return res;
}

 vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    vector<Document> res = server.FindTopDocuments(raw_query);
    AddFind(res);
    return res;
 }

int RequestQueue::GetNoResultRequests() const {
    return empty_query_res;
}

void RequestQueue::AddFind(const vector<Document>& docs){
    if(requests_.size() == min_in_day_){
        if(requests_.back().docs.empty()){
            --empty_query_res;
        }
        requests_.pop_back();
    }
    requests_.push_front({docs});
    if(docs.empty()){
        ++empty_query_res;
    }
}