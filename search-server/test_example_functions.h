#pragma once
#include "search_server.h"
#include <string>
#include "document.h"
#include "log_duration.h"
#include <iostream>

using std::string;
using namespace std::literals::string_literals;
using std::cout;

void MatchDocuments(SearchServer& search_server, const string& raw_query);

void FindTopDocuments(const SearchServer& search_server, const string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL);

template <typename DocumentPredicate>
void FindTopDocuments(const SearchServer& search_server, const string& raw_query, DocumentPredicate document_predicate);

void AddDocument(SearchServer& search_server, int document_id, const string& document, DocumentStatus status,
                     const vector<int>& ratings);

template <typename DocumentPredicate>
void FindTopDocuments(const SearchServer& search_server, const string& raw_query, DocumentPredicate document_predicate){
    cout << "Результаты поиска по запросу: "s << raw_query << "\n"s;
    LOG_DURATION_STREAM("Operation time"s, cout);
    for(const Document& doc : search_server.FindTopDocuments(raw_query, document_predicate)){
        cout << doc << "\n";
    }
}
