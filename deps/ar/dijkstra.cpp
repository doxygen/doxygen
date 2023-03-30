#include "dijkstra.hpp"
#include <iostream>
#include <ostream>

using Dijkstra::Graph;
using Dijkstra::Node;
using std::make_pair;
using std::map;
using std::pair;
using std::size_t;
using std::string;
using std::unordered_set;
using std::vector;

bool Node::operator==(const Node &other) const {
  // return this->i == other.i && this->j == other.j &&
  //        this->word == other.word && this->cost == other.cost;
  return this->cost == other.cost && this->slice == other.slice &&
         this->word == other.word;
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

std::ostream &operator<<(std::ostream &os, Node &dt) {
  os << "word: " << dt.word << " slice: " << dt.slice << " cost: " << dt.cost
     << " i: " << dt.i << " j: " << dt.j;
  return os;
}

size_t Node::operator()() const { return std::hash<size_t>()(cost + i + j); }

// A function to get the labels of the edges in a path
vector<string> Dijkstra::getEdgeLabels(vector<Node> path, Graph G) {

  vector<string> labels;
  if (path.size() > 0) {
    for (int i = 0; i < path.size() - 1; i++) {
      Node from = path[i];
      Node to = path[i + 1];
      labels.push_back(to.word);
      break;
    }
  }
  return labels;
}

Graph Dijkstra::initializeMatchingGraph(string token) {
  Graph G;

  int j = token.size();
  for (auto i = 0; i < token.length(); i++) {
    auto str = string(1, token[i]);
    auto n_str = string(1, token[i + 1]);
    Node from = {
        .i = i, .j = (j - i), .word = str, .slice = str, .cost = INT32_MIN};
    Node to = {.i = from.j,
               .j = from.j + 1,
               .word = n_str,
               .slice = n_str,
               .cost = INT32_MIN};
    std::cout << str << " " << n_str << std::endl;
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

vector<Node> Dijkstra::dijkstra(Graph G, string token, Node start, Node end) {

  // for (auto &[first, last] : G) {
  //   std::cout << first.first << " -> " << first.second;
  //   for (auto each : last) {
  //     std::cout << " -> " << each.to.word << std::endl;
  //   }
  // }

  MyQueue<Node, vector<Node>, std::less<Node>> frontier;
  unordered_set<Node, node_hasher> explored;
  map<Node, Node> prev;

  frontier.push(start);

  while (!frontier.empty()) {
    Node current = frontier.top();
    frontier.pop();
    std::cout << "current: " << current << std::endl;
    if (current.j == end.j) {
      std::cout << "ENDED" << std::endl;
      break;
    }

    explored.insert(current);

    for (Edge successors : G[current.j + 1]) {
      Node successor = successors.to;
      if (explored.count(successor) == 0 && !frontier.contains(successor)) {
        std::cout << "\t" << successor << std::endl;
        prev[successor] = current;
        frontier.push(successor);
        successor.prev = &current;
        continue;
      }

      bool hasErased = false;
      Node previous = *frontier.find(successor);
      if (previous.slice == successor.slice &&
          previous.word == successor.word &&
          previous.cost <= successor.cost) {
        hasErased = frontier.eraseNode(previous);
      }
      if (hasErased) {
        std::cout << "successor: " << successor << "\n\t" << previous
                  << std::endl;
        prev.erase(previous);
        frontier.push(successor);
      }
    }
  }

  std::cout << prev.size() << std::endl;

  std::cout << "BEST: " << std::endl;

  for (auto &[successor, current] : prev) {
    std::cout << current.word << " -> " << successor.word << std::endl;
  }

  vector<Node> path;
  {
    Node u = end;
    while (u != start) {
      path.push_back(u);
      if (prev[u].word == "") {
        break;
      }
      u = prev[u];
    }
  }
  reverse(path.begin(), path.end());
  return path;
}
