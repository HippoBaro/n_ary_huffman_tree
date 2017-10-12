//
// Created by Hippolyte Barraud on 12/10/2017.
//

#include <cstddef>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <experimental/optional>
#include <array>
#include <numeric>

namespace hippobaro {

    // std::priority_queue doesn't work with unique_pointers because top() return a const ref,
    // preventing us from using std::move(). So we implement a pop_top() function that returns a value_type directly.
    template<typename _Tp, typename _Sequence = std::vector<_Tp>,
            typename _Compare = std::less<typename _Sequence::value_type> >
    class priority_queue : std::priority_queue<_Tp, _Sequence, _Compare> {
    public:
        typedef typename _Sequence::value_type value_type;
    public:

        explicit
        priority_queue(const _Compare &__x, const _Sequence &__s) :
                std::priority_queue<_Tp, _Sequence, _Compare>(__x, __s) {}

        explicit
        priority_queue(const _Compare &__x = _Compare(), _Sequence &&__s = _Sequence()) :
                std::priority_queue<_Tp, _Sequence, _Compare>(__x, std::move(__s)) {}

        using std::priority_queue<_Tp, _Sequence, _Compare>::empty;
        using std::priority_queue<_Tp, _Sequence, _Compare>::size;
        using std::priority_queue<_Tp, _Sequence, _Compare>::top;
        using std::priority_queue<_Tp, _Sequence, _Compare>::push;
        using std::priority_queue<_Tp, _Sequence, _Compare>::pop;
        using std::priority_queue<_Tp, _Sequence, _Compare>::emplace;
        using std::priority_queue<_Tp, _Sequence, _Compare>::swap;

        value_type pop_top() {
            std::pop_heap(this->c.begin(), this->c.end(), this->comp);
            value_type top = std::move(this->c.back());
            this->c.pop_back();
            return top;
        }

    };

    template<typename DataType, typename WeightType, unsigned int N>
    struct huffman_node {
        const std::experimental::optional<DataType> word;
        const WeightType weight;
        const std::array<std::unique_ptr<huffman_node<DataType, WeightType, N>>, N> childs;

        huffman_node(huffman_node const &) = delete;
        huffman_node(huffman_node &&) = delete;
        huffman_node &operator=(huffman_node const &) = delete;

        explicit huffman_node<DataType, WeightType, N>
                (WeightType freq, std::array<std::unique_ptr<huffman_node<DataType, WeightType, N>>, N> childs) :
                word(), weight(std::move(freq)), childs(std::move(childs)) {}

        explicit huffman_node<DataType, WeightType, N>(DataType data, WeightType freq) :
                word(std::move(data)), weight(std::move(freq)), childs() {}

        explicit huffman_node<DataType, WeightType, N>() :
                word(), weight(0), childs() {}
    };

    template<typename DataType, typename WeightType, unsigned int N>
    auto print_codewords(std::unique_ptr<huffman_node<DataType, WeightType, N>> const &node, std::string str = "") {
        if (!node)
            return;
        if (node->word) {
            std::cout << *node->word << ": " << str << std::endl;
            return;
        }
        for (unsigned int i = 0; i < N; ++i) {
            print_codewords(node->childs[i], str + std::to_string(i));
        }
    }

    template<unsigned int N, typename DataType, typename WeightType>
    auto create_huffman_tree(std::unordered_map<DataType, WeightType> const &dataset) {
        auto comp = [](auto &l, auto &r) { return (l->weight > r->weight); };
        priority_queue<std::unique_ptr<huffman_node<DataType, WeightType, N>>,
                std::vector<std::unique_ptr<huffman_node<DataType, WeightType, N>>>, decltype(comp)> queue(comp);

        for (auto &&item : dataset) {
            queue.emplace(std::make_unique<huffman_node<DataType, WeightType, N>>(item.first, item.second));
        }

        while (queue.size() % (N - 1) != 1 % (N - 1)) {
            queue.emplace(std::make_unique<huffman_node<DataType, WeightType, N>>());
        }

        while (queue.size() > 1) {
            std::array<std::unique_ptr<huffman_node<DataType, WeightType, N>>, N> childs;
            for (unsigned int i = 0; i < N; ++i) {
                childs[i] = queue.pop_top();
            }

            queue.emplace(std::make_unique<huffman_node<DataType, WeightType, N>>(
                    std::accumulate(childs.begin(), childs.begin() + N, WeightType(),
                                    [](WeightType i,
                                       std::unique_ptr<huffman_node<DataType, WeightType, N>> const &other) {
                                        return i + other->weight;
                                    }), std::move(childs)));
        }

        return queue.pop_top();
    }
};

int main() {
    hippobaro::print_codewords(hippobaro::create_huffman_tree<12, char, unsigned int>(
            {
                    {'a', 5},
                    {'b', 9},
                    {'c', 12},
                    {'d', 13},
                    {'e', 16},
                    {'f', 45}}
    ));
    return 0;
}
