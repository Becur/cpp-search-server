#pragma once

#include <vector>
#include <deque>
#include <string>

#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        std::vector<Document> docs;
    };
    
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int empty_query_res = 0;
    const SearchServer& server;
    
    void AddFind(const std::vector<Document>& docs);
}; 

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> res = server.FindTopDocuments(raw_query, document_predicate);
    AddFind(res);
    return res;
}