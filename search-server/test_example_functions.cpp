#include "test_example_functions.h"

using std::string;
using namespace std::literals::string_literals;
using std::cout;

void MatchDocuments(SearchServer& search_server, const std::string& raw_query){
    cout << "Матчинг документов по запросу: "s << raw_query << "\n"s;
    LOG_DURATION_STREAM("Operation time"s, cout);
    for(const int id : search_server){
        auto res = search_server.MatchDocument(raw_query, id);
        cout << "{ document_id = "s << id << ", status = "s << static_cast<int>(res.second);
        cout << ", words ="s;
        for(const std::string_view word : res.first){
            cout << " "s << word;
        }
        cout << "}\n"s;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query, DocumentStatus status){
    cout << "Результаты поиска по запросу: "s << raw_query << "\n"s;
    LOG_DURATION_STREAM("Operation time"s, cout);
    for(const Document& doc : search_server.FindTopDocuments(raw_query, status)){
        cout << doc << "\n";
    }
}

void AddDocument(SearchServer& search_server, int document_id, const std::string& document,
                DocumentStatus status, const std::vector<int>& ratings){
    LOG_DURATION("Operation time"s);
    search_server.AddDocument(document_id, document, status, ratings);
}