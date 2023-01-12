#include <set>
#include <string>
#include <map>
#include <iostream>
#include <vector>

#include "remove_duplicates.h"

using std::set;
using std::string;

void RemoveDuplicates(SearchServer& search_server){
    std::map<set<string>, std::set<int>> words_document;
    for(const int doc_id : search_server){
        set<string> words;
        for(const auto& data : search_server.GetWordFrequencies(doc_id)){
            words.insert(data.first);
        }
        words_document[words].insert(doc_id);
    }
    for(const auto& doc : words_document){
        if(doc.second.size() > 1){
            auto i = doc.second.begin();
            for(++i; i != doc.second.end(); ++i){
                std::cout << "Found duplicate document id " << *i << std::endl;
                search_server.RemoveDocument(*i);
            }
        }
    }
}