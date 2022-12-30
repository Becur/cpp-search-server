#pragma once
#include <ostream>
#include <vector>

using std::ostream;

template<typename It>
struct IteratorRange{
    It begin, end;
    int size;
    
};

template<typename It>
ostream& operator<<(ostream& out, IteratorRange<It> itran){
    for(It i = itran.begin; i != itran.end; ++i){
        out << *i;
    }
    return out;
}

template<typename It> 
class Paginator{
    public:
    Paginator(It begin, It end, long unsigned int size){
        for(auto i = begin;;){
            int j;
            bool is_end = false;
            for(j = 0;; ++j, ++i){
                if(i == end){
                    is_end = true;
                    break;
                }
                if(j == size){
                    break;
                }
            }
            pages.push_back({i - j, i, j});
            if(is_end){
                break;
            }
        }
    }
    
    auto begin() const{
        return pages.begin();
    }
    
    auto end() const{
        return pages.end();
    }
    private:
    std::vector<IteratorRange<It>> pages;
};

template <typename Container>
auto Paginate(const Container& c, long unsigned int page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}
