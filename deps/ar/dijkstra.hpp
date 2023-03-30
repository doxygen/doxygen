#include <algorithm>
#include <optional>
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include <map>

namespace Dijkstra {

struct Node {
  int i;       // index of the first character in the matching sequence
  int j;       // index of the last character in the matching sequence
  std::string word; // the word associated with this node
  int cost;    // the cost of the path to this node
  Node *prev;

public:
  bool operator==(const Node &other) const;
  bool operator!=(const Node &other) const;
  bool operator<(const Node &other) const;
  bool operator>(const Node &other) const;
  size_t operator()() const;

  const void print() const;
};

// A struct to represent an edge in the matching graph
struct Edge {
  Node to;   // the destination node of the edge
};

using Vertex = int;
using Graph = std::map<Vertex, std::vector<Edge>>;

struct node_hasher {
  size_t operator()(const Node &e) const {
    size_t hash_result = 0;
    hash_result += e.cost * 31;
    hash_result += e.i * 32;
    hash_result += e.j * 33;
    // calculate the hash result
    return hash_result;
  }
};

template <class T, class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class MyQueue : public std::priority_queue<T, Container, Compare> {
public:
  typedef typename std::priority_queue<
      T, Container, Compare>::container_type::const_iterator const_iterator;

  const_iterator find(const T &val) const {
    auto row = this->c.cbegin();
    auto last = this->c.cend();
    while (row != last) {
      if (*row == val)
        return row;
      ++row;
    }
    return last;
  }

  const bool contains(const T &val) const {
    return this->find(val) != this->c.cend();
  }

  bool eraseNode(const T &value) {
    auto it = std::find(this->c.begin(), this->c.end(), value);

    if (it == this->c.end()) {
      return false;
    }
    if (it == this->c.begin()) {
      // deque the top element
      this->pop();
    } else {
      // remove element and re-heap
      this->c.erase(it);
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
    }
    return true;
  }
};

Graph initializeMatchingGraph(std::string token);

// A function to get the labels of the edges in a path
std::vector<std::string> getEdgeLabels(std::vector<Node> path, Graph G);

std::vector<Node> dijkstra(Graph G, std::string token, Node start, Node end);
} // namespace Dijkstra
