#include "remove_duplicates.h"
#include <set>
#include <string>
#include <map>
#include <iostream>

using std::set;
using std::string;

void RemoveDuplicates(SearchServer& search_server){
    std::vector<std::pair<int, set<string>>> documents_word;
    documents_word.reserve(search_server.GetDocumentCount());
    for(const int doc_id : search_server){
        documents_word.push_back({doc_id, set<string>()});
        for(const auto& data : search_server.GetWordFrequencies(doc_id)){
            documents_word.back().second.insert(data.first);
        }
    }

    for(int i = 0; i < documents_word.size(); ++i){
        for(int j = i + 1; j < documents_word.size(); ++j){
            if((documents_word[i].first != documents_word[j].first) 
            && (documents_word[i].second == documents_word[j].second)){
                std::cout << "Found duplicate document id "s << documents_word[j].first << std::endl;
                search_server.RemoveDocument(documents_word[j].first);
                documents_word.erase(documents_word.begin() + j);
                --j;
            }
        }
    }
}