#include <algorithm>
#include <cstdint>
#include <stack>
#include <cstdio>
#include <cassert>
#include <string>

class RandGenerator {
    private:
        uint64_t seed = 0;
        constexpr static int32_t BASE = 1103515245;
        constexpr static int32_t OFFSET = 12345;
        constexpr static int32_t MOD = 1e9 + 7;

    public:
        uint32_t treap_rand() {
            seed = (BASE * seed + OFFSET) % MOD;
            return seed;
        }

        void reset() {
            seed = 0;
        }
};

template<typename T>
class Treap;

template<typename T>
class TreapNode {

    private:
        T val;
        TreapNode<T> *left;
        TreapNode<T> *right;
        int32_t count; /* number of repeated elements contained in current node */
        int32_t size; /* number of nodes in current subtree */
        uint32_t weight;
        friend class Treap<T>;

    public:
        TreapNode() : val(T()), left(nullptr), right(nullptr), count(0), size(0), weight(0) {}

        TreapNode(T val,TreapNode *left, TreapNode *right,
                int32_t count, int32_t size, uint32_t weight) :
            val(val), left(left), right(right),
            count(count), size(size), weight(weight) {}
};

template<typename T>
class Treap {

    private:
        RandGenerator rand;

        TreapNode<T> *treap_root = nullptr;

    public:
        void insert(T val) {
            /* Your code here. */
        }

        void remove(T val) {
            /* Your code here. */
        }

        T pre_element(T val) {
            /* Your code here. */
        }

        T suc_element(T val) {
            /* Your code here. */
        }

        int32_t rank(T val) {
            /* Your code here. */
        }

        int32_t kth_element(int32_t rk) {
            /* Your code here. */
        }

        void clear() {
            /* Your code here. */
        }

        std::string pre_traverse() {
            /* Your code here. */
        }
};
