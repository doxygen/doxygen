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

std::hash<std::string> hasher;
size_t Node::operator()() const {
  return std::hash<size_t>()(hasher(word) + cost + i + j);
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
    Node to = {.i = from.j, .j = from.j + 1, .word = n_str, .cost = INT32_MIN};
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
    if (current == end) {
      std::cout << "ENDED" << std::endl;
      break;
    }

    explored.insert(current);

    for (Edge successors : G[current.j + 1]) {
      Node successor = successors.to;
      if (explored.count(successor) == 0 && !frontier.contains(successor)) {
        prev[successor] = current;
        frontier.push(successor);
        continue;
      }

      bool hasErased = false;
      Node previous = *frontier.find(successor);
      if (previous.word == successor.word && previous.cost <= successor.cost) {
        if (frontier.eraseNode(previous)) {
          frontier.push(successor);
        }
      }
    }
  }

  std::cout << prev.size() << std::endl;

  std::cout << "BEST: " << std::endl;

  for (auto &[key, value] : prev) {
    // current.print();
    // std::cout  << " -> ";
    // std::cout << "key: " << key;
    key.print();
    std::cout << "-> ";
    value.print();
    std::cout << std::endl;
  }

  vector<Node> path;
  // {
  //   Node u = end;
  //   int i = 0;
  //   while (u != start) {
  //     path.push_back(u);
  //     u.print();
  //     printf("%p\n", u.prev);
  //     break;
  //   }
  // }
  {
    Node u = end;
    int i = 0;
    while (u.i != start.i) {
      path.push_back(u);
      u.print();
      u = prev[u];
      i++;
      if (i == prev.size())
        break;
    }
  }
  reverse(path.begin(), path.end());
  return path;
}
