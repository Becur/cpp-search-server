#include "search_server.h"

using std::string;
using std::string_view;
using std::vector;
using std::map;
using std::set;
using std::invalid_argument;
using namespace std::literals::string_literals;

SearchServer::SearchServer(const string& stop_words_text)
: SearchServer(SplitIntoWords(stop_words_text)){}

SearchServer::SearchServer(const string_view stop_words_text)
: SearchServer(SplitIntoWords(string(stop_words_text))){}

void SearchServer::AddDocument(int document_id, const string_view document, DocumentStatus status,
                     const vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw invalid_argument("Invalid document_id"s);
    }
    id_text_documents[document_id] = string(document);
    
    const vector<string_view> words = SplitIntoWordsNoStop(string_view(id_text_documents[document_id]));

    const double inv_word_count = 1.0 / words.size();
    for (const string_view word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
        document_to_word_freqs_[document_id][word]+= inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.insert(document_id);
}

bool SearchServer::IsValidWord(const string_view word) {
    return std::none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

vector<Document> SearchServer::FindTopDocuments(const string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}
vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy& policy, const string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}
vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy& policy, const string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::par,
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(const string_view raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}
vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy& policy, const string_view raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}
vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy& policy, const string_view raw_query) const {
    return FindTopDocuments(std::execution::par, raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const{
     static const std::map<std::string_view, double> EMPTY_MAP;
    return document_to_word_freqs_.count(document_id) ? document_to_word_freqs_.at(document_id) : EMPTY_MAP;
}

set<int>::iterator SearchServer::begin(){
    return document_ids_.begin();
}

set<int>::iterator SearchServer::end(){
    return document_ids_.end();
}

void SearchServer::RemoveDocument(int document_id){
    if(!documents_.count(document_id)){
        return ;
    }
    for(auto& data_word : document_to_word_freqs_[document_id]){
        word_to_document_freqs_[data_word.first].erase(document_id);
    }
    document_to_word_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(document_ids_.find(document_id));
    id_text_documents.erase(document_id);
}

std::pair<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(
    const string_view raw_query,
    int document_id) const {
    const auto query = ParseQuery(raw_query);

    vector<string_view> matched_words;
    if (!std::any_of(query.minus_words.begin(), query.minus_words.end(),
               [&](const string_view word){
                   return !(word_to_document_freqs_.count(word) == 0) 
                       && (word_to_document_freqs_.at(word).count(document_id));
               })){
        matched_words = vector<string_view>(query.plus_words.size());
        matched_words.erase(
        std::copy_if(query.plus_words.begin(), query.plus_words.end(),
            matched_words.begin(),
            [&](const string_view word){
                return !(word_to_document_freqs_.count(word) == 0) 
                    && (word_to_document_freqs_.at(word).count(document_id));
            }), matched_words.end());
    }
    return {matched_words, documents_.at(document_id).status};
}

std::pair<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::sequenced_policy& policy, 
    const std::string_view raw_query, 
    int document_id) const{
    return MatchDocument(raw_query, document_id);
}

std::pair<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::parallel_policy& policy, 
    const std::string_view raw_query,
    int document_id) const{
    auto query = ParseQuery(std::execution::par, raw_query);
    vector<string_view> matched_words;
    if (!std::any_of(std::execution::par,
               query.minus_words.begin(), query.minus_words.end(),
               [&](const string_view word){
                   return !(word_to_document_freqs_.count(word) == 0) 
                       && (word_to_document_freqs_.at(word).count(document_id));
               })){
        matched_words = vector<string_view>(query.plus_words.size());
        matched_words.erase(
        std::copy_if(std::execution::par,
            query.plus_words.begin(), query.plus_words.end(),
            matched_words.begin(),
            [&](const string_view word){
                return !(word_to_document_freqs_.count(word) == 0) 
                    && (word_to_document_freqs_.at(word).count(document_id));
            }), matched_words.end());
        std::sort(std::execution::par, matched_words.begin(), matched_words.end());
        matched_words.erase(
            std::unique(std::execution::par, matched_words.begin(), matched_words.end()),
            matched_words.end());
    }
    return {matched_words, documents_.at(document_id).status};
}

bool SearchServer::IsStopWord(const string_view word) const {
    return stop_words_.count(string(word)) > 0;
}

vector<string_view> SearchServer::SplitIntoWordsNoStop(const string_view text) const {
    vector<string_view> words;
    for (const string_view word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Word "s + string(word) + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const string_view text) const {
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string_view word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        throw invalid_argument("Query word "s + string(text) + " is invalid");
    }

    return {word, is_minus, IsStopWord(string(word))};
}

SearchServer::Query SearchServer::ParseQuery(const string_view text) const {
    Query result;
    for (const string_view word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            } else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    std::sort(result.plus_words.begin(), result.plus_words.end());
    result.plus_words.erase(
        std::unique(result.plus_words.begin(), result.plus_words.end()),
        result.plus_words.end());
    std::sort(result.minus_words.begin(), result.minus_words.end());
    result.minus_words.erase(
        std::unique(result.minus_words.begin(), result.minus_words.end()), 
        result.minus_words.end());
    return result;
}

SearchServer::Query SearchServer::ParseQuery(
    const std::execution::parallel_policy& policy, 
    const string_view text) const {
    Query result;
    for (const string_view word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            } else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    return result;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

double SearchServer::ComputeWordInverseDocumentFreq(const string_view word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

void SearchServer::RemoveDocument(const std::execution::sequenced_policy& policy, int document_id){
   RemoveDocument(document_id); 
}

void SearchServer::RemoveDocument(const std::execution::parallel_policy& policy, int document_id){
    if(!documents_.count(document_id)){
        return ;
    }
    vector<string_view> words_document(document_to_word_freqs_[document_id].size());
    std::transform(
    document_to_word_freqs_[document_id].begin(), document_to_word_freqs_[document_id].end(),
    words_document.begin(),
    [](const std::pair<string_view, double>& word_freq){
        return word_freq.first;});
    std::for_each(std::execution::par,
                 words_document.begin(), words_document.end(),
                 [&](const string_view word){
                     word_to_document_freqs_[word].erase(document_id);});
    document_to_word_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(document_ids_.find(document_id));
    id_text_documents.erase(document_id);
}
