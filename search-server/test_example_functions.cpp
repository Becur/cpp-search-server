#include "test_example_functions.h"

void MatchDocuments(SearchServer& search_server, const string& raw_query){
    cout << "Матчинг документов по запросу: "s << raw_query << "\n"s;
    LOG_DURATION_STREAM("Operation time"s, cout);
    for(const int id : search_server){
        auto res = search_server.MatchDocument(raw_query, id);
        cout << "{ document_id = "s << id << ", status = "s << static_cast<int>(res.second);
        cout << ", words ="s;
        for(const string& word : res.first){
            cout << " "s << word;
        }
        cout << "}\n"s;
    }
}

void FindTopDocuments(const SearchServer& search_server, const string& raw_query, DocumentStatus status){
    cout << "Результаты поиска по запросу: "s << raw_query << "\n"s;
    LOG_DURATION_STREAM("Operation time"s, cout);
    for(const Document& doc : search_server.FindTopDocuments(raw_query, status)){
        cout << doc << "\n";
    }
}

void AddDocument(SearchServer& search_server, int document_id, const string& document,
                DocumentStatus status, const vector<int>& ratings){
    LOG_DURATION("Operation time"s);
    search_server.AddDocument(document_id, document, status, ratings);
}