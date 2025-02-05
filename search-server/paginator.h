#pragma once

#include <ostream>
#include <vector>

template<typename It>
struct IteratorRange{
    It begin, end;
    int size; 
};

template <typename Container>
auto Paginate(const Container& c, long unsigned int page_size);

template<typename It>
std::ostream& operator<<(std::ostream& out, IteratorRange<It> itran);

template<typename It> 
class Paginator{
    public:
    Paginator(It begin, It end, long unsigned int size);
    
    auto begin() const;
    
    auto end() const;
    
    private:
    std::vector<IteratorRange<It>> pages;
};

//----------------------------------------------------------------//

template <typename Container>
auto Paginate(const Container& c, long unsigned int page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}

template<typename It>
Paginator<It>::Paginator(It begin, It end, long unsigned int size){
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


template<typename It>
auto Paginator<It>::begin() const{
    return pages.begin();
}

template<typename It>
auto Paginator<It>::end() const{
    return pages.end();
}


template<typename It>
std::ostream& operator<<(std::ostream& out, IteratorRange<It> itran){
    for(It i = itran.begin; i != itran.end; ++i){
        out << *i;
    }
    return out;
}