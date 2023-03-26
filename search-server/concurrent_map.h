#include <string>
#include <vector>
#include <map>
#include <mutex>


template <typename Key, typename Value>
class ConcurrentMap {
public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys");
    
    struct Bucket{
        std::mutex mut;
        std::map<Key, Value> dict;
    };
    
    struct Access {
        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;

        Access(Bucket& bucket, Key key)
        : guard(std::lock_guard<std::mutex>(bucket.mut))
        , ref_to_value(bucket.dict[key]){
        }
    };

    void erase(Key key){
        std::lock_guard<std::mutex> guard(buckets[key % buckets.size()].mut);
        buckets[key % buckets.size()].dict.erase(key);
    }

    explicit ConcurrentMap(size_t bucket_count) 
    : buckets(std::vector<Bucket>(bucket_count)){}

    Access operator[](const Key& key){
        return {buckets[static_cast<uint64_t>(key) % buckets.size()], key};
    }

    std::map<Key, Value> BuildOrdinaryMap(){
        std::map<Key, Value> dict;
        for (auto& [mut, map] : buckets){
            std::lock_guard<std::mutex> guard(mut);
            dict.insert(map.begin(), map.end());
        }
        return dict;
    }

private:
    std::vector <Bucket> buckets;
};