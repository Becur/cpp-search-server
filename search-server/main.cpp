#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "search_server.h"

using namespace std;

template <typename Type>
Type ignore_unused(Type val){
    return val;
}

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
    }
}
/*
Разместите код остальных тестов здесь
*/
void TestAddDocument(){
    vector <Document> res_find;
    SearchServer ss;
    res_find = ss.FindTopDocuments("Hello world, my pp is Anna -Alex"s);
    ASSERT(res_find.empty());
    ss.SetStopWords("in the is"s);
    ss.AddDocument(0, "Hello world, my name is Anna"s, DocumentStatus::ACTUAL, {-1, 5, 7, -2});
    ss.AddDocument(1, "Hello Anna"s, DocumentStatus::ACTUAL, {14, -2});
    ss.AddDocument(3, "pp"s, DocumentStatus::REMOVED, {1, 1});
    ss.AddDocument(5, "Hello Alex"s, DocumentStatus::ACTUAL, {1, 1});
    ss.AddDocument(6, "in the is"s, DocumentStatus::ACTUAL, {1, 1});
    res_find = ss.FindTopDocuments("Hello world, my pp is Anna -Alex"s);
    ASSERT_EQUAL(res_find.size(), 2);
    ASSERT_EQUAL(res_find[0].id, 0);
    ASSERT_EQUAL(res_find[1].id, 1);
    ASSERT(res_find[0].relevance > res_find[1].relevance);
    ASSERT(res_find[0].relevance - (log(5.0 / 3.0) + log(5.0 / 1.0) + log(5.0 / 1.0) + log(5.0 / 1.0) + log(5.0 / 2.0)) 
           * (1.0 / 5.0) < 1e-6);
    ASSERT(res_find[1].relevance - (log(5.0 / 3.0) + log(5.0 / 2.0)) * (1.0 / 2.0) < 1e-6);
    ASSERT_EQUAL(res_find[0].rating, (-1 + 5 + 7 - 2) / 4);
    ASSERT_EQUAL(res_find[1].rating, (14 - 2) / 2);
    res_find = ss.FindTopDocuments("Hello world, my pp is Anna -Alex"s,[]
                                (int document_id, DocumentStatus document_status, int rating) {
                                    ignore_unused(document_id);
                                    ignore_unused(document_status);
                                    return rating > 3; });
    ASSERT_EQUAL(res_find.size(), 1);
    ASSERT_EQUAL(res_find[0].id, 1);
    ASSERT(res_find[0].relevance - (log(5.0 / 3.0) + log(5.0 / 2.0)) * (1.0 / 2.0) < 1e-6);
    ASSERT_EQUAL(res_find[0].rating, (14 - 2) / 2);
    res_find = ss.FindTopDocuments("Hello world, my pp is Anna -Alex"s, DocumentStatus::REMOVED);
    ASSERT_EQUAL(res_find.size(), 1);
    ASSERT(res_find[0].relevance - log(5.0 / 1.0) * (1.0 / 1.0) < 1e-6);
    ASSERT_EQUAL(res_find[0].id, 3);
    ASSERT_EQUAL(res_find[0].rating, (1 + 1) / 2);
}

void TestSearchServer(){
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestAddDocument);
}
