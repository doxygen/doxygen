#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <math.h>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

using std::cout;
using std::endl;
using std::function;
using std::greater;
using std::map;
using std::pair;
using std::priority_queue;
using std::reverse;
using std::string;
using std::vector;

template <class T, class Container = vector<T>,
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

// A struct to represent a node in the matching graph
struct Node {
  int i;       // index of the first character in the matching sequence
  int j;       // index of the last character in the matching sequence
  string word; // the word associated with this node
  int cost;    // the cost of the path to this node
  int g_score; // g_score

  bool operator==(const Node &other) const {
    // return this->i == other.i && this->j == other.j &&
    //        this->word == other.word && this->cost == other.cost;
    return this->word == other.word && this->cost == other.cost;
  }

  bool operator<(const Node &other) const {
    return this->cost + this->g_score < other.cost + other.g_score;
  }

  bool operator>(const Node &other) const {
    return this->cost + this->g_score > other.cost + other.g_score;
  }
};

// A struct to represent an edge in the matching graph
struct Edge {
  Node from; // the source node of the edge
  Node to;   // the destination node of the edge
};

// A function to compute the matching sequence between two strings
vector<pair<string, string>> BYP(string token, string word,
                                 function<double(string)> p) {
  vector<pair<string, string>> matching_seq;
  int n = token.size();
  int m = word.size();
  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      string subtoken = token.substr(i, j - i + 1);
      double tolerance = p(word);
      if (subtoken.size() <= word.size() + tolerance) {
        double distance = 0;
        for (int k = 0; k < subtoken.size(); k++) {
          if (subtoken[k] != word[k]) {
            distance++;
          }
        }
        if (distance <= tolerance) {
          matching_seq.push_back({subtoken, word});
        }
      }
    }
  }
  return matching_seq;
}

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

// A function to initialize the matching graph

// A function to compute the minimum cost path in the matching graph using
// Dijkstra's algorithm
vector<Node> dijkstra(map<Node, vector<Edge>> G, Node start, Node end) {

  map<Node, int> dist;
  map<Node, Node> prev;

  MyQueue<Node, vector<Node>, std::greater<Node>> pq;
  pq.push(start);
  std::unordered_set<Node, node_hasher> explored;

  while (!pq.empty()) {
    Node current = pq.top();
    pq.pop();

    if (current == end) {
      break;
    }

    explored.insert(current);

    printf("from: %d - %d - %s - %d\n", current.i, current.j,
           current.word.c_str(), current.cost);
    for (Edge e : G[current]) {
      Node successor = e.to;
      printf("\tsuccessor: %d - %d - %s - %d\n", successor.i, successor.j,
             successor.word.c_str(), successor.cost);
      printf("\tdist: %d\n", dist[current]);
      printf("\talt: %d\n", dist[current] + e.from.cost);
      int alt = dist[current] + e.from.cost;
      if (explored.count(successor) == 0 && !pq.contains(successor)) {
        successor.g_score = alt;
        prev[successor] = current;
        pq.push(successor);
        continue;
      }

      // Does not happen that often
      bool hasErased = false;
      Node previous = *pq.find(successor);
      if (previous == successor && (successor.cost + dist[successor]) <
                                       (previous.cost + dist[previous])) {
        hasErased = pq.eraseNode(previous);
        if (hasErased) {
          pq.push(successor);
        }
      }
    }
  }

  for (auto &[first, second] : prev) {
    printf("%s -> %s\n", first.word.c_str(), second.word.c_str());
  }

  vector<Node> path;
  Node u = end;
  while (true) {
    path.push_back(u);
    if (!prev.count(u)) {
      break;
    }
    u = prev[u];
  }
  reverse(path.begin(), path.end());
  return path;
}

// A function to get the labels of the edges in a path
vector<string> getEdgeLabels(vector<Node> path, map<Node, vector<Edge>> G) {
  vector<string> labels;
  for (int i = 0; i < path.size() - 1; i++) {
    Node from = path[i];
    Node to = path[i + 1];
    for (Edge e : G[from]) {
      if (e.to == to) {
        labels.push_back(e.from.word);
        break;
      }
    }
  }
  return labels;
}

map<Node, vector<Edge>> initializeMatchingGraph(string token) {
  map<Node, vector<Edge>> G;

  int j = token.size();
  for (auto i = 0; i < token.size(); i++) {
    auto str = string(1, token[i]);
    auto n_str = string(1, token[i + 1]);
    Node from = {.i = i, .j = (j - i), .word = str, .cost = INT32_MAX};
    Node to = {.i = from.j + 1, .j = from.j, .word = n_str, .cost = INT32_MAX};
    G[from].push_back({from, to});
  }

  return G;
}

// The main function that performs string matching
vector<string> stringMatching(string token, vector<string> dict,
                              function<double(string)> p,
                              function<int(string)> c) {

  map<Node, vector<Edge>> G = initializeMatchingGraph(token);

  int index = 0;
  for (string word : dict) {
    vector<pair<string, string>> matching_seq = BYP(token, word, p);
    for (pair<string, string> match : matching_seq) {
      cout << "first: " << match.first.c_str()
           << " second: " << match.second.c_str() << endl;

      // hello
      // hel
      int i = token.find(match.first);    // 0
      int j = i + match.first.size() - 1; // 0 + 3

      Node from = {i, j, match.first, c(match.first)};

      Node to = {j + 1, j, match.second, c(match.second)};

      cout << "from: " << from.i << " " << from.j << " " << from.word << " "
           << from.cost << endl;
      // G(E) <- G(E) U {(i, j), word, c(word)};
      G[from].push_back({.from = from, .to = to});
    }
  }

  Node start = {0, 0, "h", 1};
  Node end = {2, 2, "l", (int)token.size()};
  vector<Node> path = dijkstra(G, start, end);
  for (Node label : path) {
    cout << "labels: " << label.word << " ";
  }
  return getEdgeLabels(path, G);
}

// Example usage
int main() {
  string token = "hell";
  vector<string> dict = {"hello", "help", "hollow", "hallo", "heal", "heel"};
  auto p = [](string word) {
    return word.length() / log(word.length());
  }; // tolerance function always returns 1
  auto c = [](string word) {
    return word.size();
  }; // cost function returns length of word
  vector<string> labels = stringMatching(token, dict, p, c);
  printf("DONE\n");
  for (string label : labels) {
    cout << "found: " << label << " ";
  }
  cout << endl;
}
