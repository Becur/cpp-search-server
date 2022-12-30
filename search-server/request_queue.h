#pragma once
#include "search_server.h"
#include "document.h"
#include <vector>
#include <deque>
#include <string>

using std::string;
using std::vector;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        vector<Document> res = server.FindTopDocuments(raw_query, document_predicate);
        AddFind(res);
        return res;
    }
    
    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
    
    vector<Document> AddFindRequest(const string& raw_query);
    
    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        vector<Document> docs;
    };
    
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int empty_query_res = 0;
    const SearchServer& server;
    
    void AddFind(const vector<Document>& docs);
}; 