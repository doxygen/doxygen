#include "dijkstra.hpp"
#include "opts.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <ostream>
#include <unordered_map>

using Dijkstra::Graph;
using Dijkstra::Node;
using std::size_t;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

bool Node::operator==(const Node &other) const {
  // return this->i == other.i && this->j == other.j &&
  //        this->word == other.word && this->cost == other.cost;
  return this->cost == other.cost && this->word == other.word &&
         this->i == other.i && this->j == other.j;
}

bool Node::operator!=(const Node &other) const {
  // return this->i == other.i && this->j == other.j &&
  //        this->word == other.word && this->cost == other.cost;
  return !(*this == other);
}

bool Node::operator<(const Node &other) const {
  return this->cost < other.cost;
}

bool Node::operator>(const Node &other) const {
  return this->cost > other.cost;
}

std::ostream &operator<<(std::ostream &os, const Node &nd) {
  os << "word: " << nd.word << " cost: " << nd.cost << " i: " << nd.i
     << " j: " << nd.j;
  return os;
}

void Node::print() const { std::cout << *this << std::endl; }

std::hash<std::string> hasher;
size_t Node::operator()() const {
  return std::hash<size_t>()(hasher(word) * 31 + cost * 37 + i * 41 + j * 43);
}

// A function to get the labels of the edges in a path
vector<string> Dijkstra::getEdgeLabels(vector<Node> path) {
  vector<string> labels;
  for (auto each : path) {
    if (each.cost < 0)
      continue;
    labels.push_back(each.word);
  }
  return labels;
}

Graph Dijkstra::initializeMatchingGraph(string token) {
  Graph G;

  for (size_t i = 0; i <= token.size(); i++) {
    auto str = string(1, token[i]);
    auto to = Node{(int)i, (int)i + 1, str, INT32_MIN};
    if (i == token.size()) {
      to.cost = -100;
      to.word = EOS;
    }
    G[i].push_back({to});
  }

  return G;
}

//  getpng
// 0123456
// 0,3 -> "get"
// 0,3 -> "getter"
// 0,3 -> "great"
// 3,6 -> "past"
// 3,6 -> "point"
// 3,6 -> "pointer"
// 1,3 -> "entry"
// 4,6 -> "not"
//
// "get", "pnt"
// "get", "point"
//
// "g"

vector<Node> Dijkstra::dijkstra(Graph &G, string token) {

  auto start = Node{0, 0, token.substr(0, 1), 0};
  auto end = Node{(int)token.length(), (int)token.size() + 1, EOS, -100};

  // for (auto &[first, last] : G) {
  //   std::cout << first;
  //   for (auto each : last) {
  //     std::cout << " -> " << each.to << std::endl;
  //   }
  // }

  MyQueue<Node, vector<Node>, std::less<Node>> frontier;
  unordered_set<Node, node_hasher> explored;
  unordered_map<Node, Node, node_hasher> prev;

  frontier.push(start);

  while (!frontier.empty()) {
    auto current = frontier.top();
    frontier.pop();

    if (current == end) {
      break;
    }

    // INFO("cur: " << current)

    explored.insert(current);

    for (Edge successors : G[current.j]) {
      auto successor = successors.to;
      if (explored.count(successor) == 0 && !frontier.contains(successor)) {
        // INFO("\tsuc: " << successor)
        prev[successor] = current;
        frontier.push(successor);
        continue;
      }

      auto previous = *frontier.find(successor);
      if (previous.word == successor.word && previous.cost <= successor.cost) {
        if (frontier.eraseNode(previous)) {
          frontier.push(successor);
        }
      }
    }
  }

  vector<Node> path;
  {
    Node u = end;
    while (true) {
      path.push_back(u);
      try {
        auto found = prev.at(u);
        u = found;
        if (u == start) {
          break;
        }
      } catch (const std::out_of_range &e) {
        break;
      }
    }
  }
  reverse(path.begin(), path.end());
  return path;
}
