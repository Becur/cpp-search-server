#include <string>

#include "document.h"


using std::ostream;
using namespace std::literals::string_literals;

ostream& operator<<(ostream& out, Document doc){
    out << "{ document_id = "s << doc.id;
    out << ", relevance = "s << doc.relevance;
    out << ", rating = "s << doc.rating << " }"s;
    return out;
}

Document::Document() = default;

Document::Document(int id, double relevance, int rating)
    : id(id)
    , relevance(relevance)
    , rating(rating) {
}