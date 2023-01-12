#pragma once

#include <iostream>
#include <string>

#include "search_server.h"
#include "document.h"
#include "log_duration.h"

void MatchDocuments(SearchServer& search_server, const std::string& raw_query);

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL);

template <typename DocumentPredicate>
void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query, DocumentPredicate document_predicate);

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
                     const std::vector<int>& ratings);

template <typename DocumentPredicate>
void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query, DocumentPredicate document_predicate){
    std::cout << "Результаты поиска по запросу: " << raw_query << "\n";
    LOG_DURATION_STREAM("Operation time", std::cout);
    for(const Document& doc : search_server.FindTopDocuments(raw_query, document_predicate)){
        std::cout << doc << "\n";
    }
}
