#include "dijkstra.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <ostream>

using Dijkstra::Graph;
using Dijkstra::Node;
using std::map;
using std::size_t;
using std::string;
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
  return this->cost != other.cost && this->word != other.word &&
         this->i != other.i && this->j != other.j;
}

bool Node::operator<(const Node &other) const {
  return this->cost < other.cost;
}

bool Node::operator>(const Node &other) const {
  return this->cost > other.cost;
}

const void Node::print() const {
  std::cout << "word: " << this->word << " cost: " << this->cost
            << " i: " << this->i << " j: " << this->j << std::endl;
}

std::ostream& operator<<(std::ostream &os, const Node &nd) {
  os << "word: " << nd.word << " cost: " << nd.cost
            << " i: " << nd.i << " j: " << nd.j << std::endl;
  return os;
}

std::hash<std::string> hasher;
size_t Node::operator()() const {
  return std::hash<size_t>()(hasher(word) * 31 + cost * 37 + i * 41 + j * 43);
}

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
    if (token[i] == '\0' || token[i + 1] == '\0') {
      break;
    }
    auto str = string(1, token[i]);
    auto n_str = string(1, token[i + 1]);
    Node from = {.i = i, .j = (j - i), .word = str, .cost = INT32_MIN};
    auto to = Node{.i = from.j,
                                          .j = from.j + 1,
                                          .word = n_str,
                                          .cost = INT32_MIN};
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

vector<Node> Dijkstra::dijkstra(Graph G, string token,
                                Node start,
                                Node end) {

  // for (auto &[first, last] : G) {
  //   std::cout << first.first << " -> " << first.second;
  //   for (auto each : last) {
  //     std::cout << " -> " << each.to.word << std::endl;
  //   }
  // }

  struct Compare {
    bool operator()(const Node &a,
                    const Node &b) {
      return a.cost < b.cost;
    }
  };
  MyQueue<Node, vector<Node>, Compare>
      frontier;
  unordered_set<Node, node_hasher> explored;
  auto comp = [](const Node &a,
                    const Node &b){
    return a.word != b.word && a.i != b.i && a.j != b.j;
  };
  map<Node, Node, decltype(comp)> prev(comp);
  map<string, string> prevstr;

  frontier.push(start);

  while (!frontier.empty()) {
    auto current = frontier.top();
    frontier.pop();
    if (current == end) {
      std::cout << "ENDED" << std::endl;
      break;
    }

    explored.insert(current);

    for (Edge successors : G[current.j + 1]) {
      auto successor = successors.to;
      if (explored.count(successor) == 0 && !frontier.contains(successor)) {
        prev[successor] = current;
        prevstr[successor.word] = current.word;
        frontier.push(successor);
        continue;
      }

      bool hasErased = false;
      auto previous = *frontier.find(successor);
      if (previous.word == successor.word &&
          previous.cost <= successor.cost) {
        if (frontier.eraseNode(previous)) {
          frontier.push(successor);
        }
      }
    }
  }

  std::cout << "BEST: " << std::endl;

  for(auto &[key, value]: prevstr) {
    std::cout << key << " -> " << value << std::endl;
  }

  vector<Node> path;
  // {
  //   auto u = end;
  //   while (u != start) {
  //     path.push_back(*u);
  //   }
  // }

  {
    Node u = end;
    while (u.i != start.i) {
      path.push_back(u);
      u.print();
      auto found = std::find_if(
          prev.begin(), prev.end(), [&u](const std::pair<Node, Node> &a) {
            return a.first.i == u.i && a.first.j == u.j &&
                   a.first.word == u.word && a.first.cost == u.cost;
          });
      if (found == prev.end()) {
        break;
      }
      u = found->second;
    }
  }
  reverse(path.begin(), path.end());
  return path;
}
