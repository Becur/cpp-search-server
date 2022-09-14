#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        documents_size[document_id] = words.size();
        for(const string& word : words){
            documents_[word].push_back(document_id);
        }
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    map<string, vector<int>> documents_;

    map<int, int> documents_size;

    int document_count_ = 0;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const set<string>& query_words) const {
        map<int, double> matched_documents;
        vector <string> negative_words, positive_words;
        for(const string& query_word : query_words){
            if(query_word[0] == '-'){
                negative_words.push_back(string(query_word.begin() + 1, query_word.end()));
            }
            else{
                positive_words.push_back(query_word);
            }
        }
        for (int id = 0; id < document_count_; ++id){
            for(const string& query_word : positive_words){
                if(documents_.count(query_word)){
                    set <int> uniq_id(documents_.at(query_word).begin(), documents_.at(query_word).end());
                    matched_documents[id] += log (static_cast <double> (document_count_) / vset.size()) *
                        (count(documents_.at(query_word).begin(), documents_.at(query_word).end( ), id) /
                         static_cast <double> (documents_size.at(id)));
                }
            }
        }
        for(const string& negative_word : negative_words){
            if(documents_.count(negative_word)){
                for(const int& id : documents_.at(negative_word)){
                    matched_documents.erase(id);
                }
            }
        }
        vector <Document> docs;
        for (const auto& [id, relevance] : matched_documents){
            if(relevance > 0){
                Document doc;
                doc.id = id;
                doc.relevance = relevance;
                docs.push_back(doc);
            }
        }
        return docs;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
    return 0;
}
