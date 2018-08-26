#include <queue>
#include <set>

#include "utl/serialization/serialization.h"

#include "../catch.hpp"

struct node;

using node_id_t = uint32_t;

struct edge {
  node* from_;
  node* to_;
};

struct node {
  void add_edge(edge* e) { edges_.emplace_back(e); }
  node_id_t id() const { return id_; }

  node_id_t id_{0};
  utl::vector<edge*> edges_;
  utl::string name_;
};

struct graph {
  node* make_node(utl::string name) {
    return nodes_
        .emplace_back(utl::make_unique<node>(
            node{next_node_id_++, utl::vector<edge*>{0u}, std::move(name)}))
        .get();
  }

  edge* make_edge(node_id_t const from, node_id_t const to) {
    return edges_
        .emplace_back(
            utl::make_unique<edge>(edge{nodes_[from].get(), nodes_[to].get()}))
        .get();
  }

  utl::vector<utl::unique_ptr<node>> nodes_;
  utl::vector<utl::unique_ptr<edge>> edges_;
  node_id_t next_node_id_{0};
};

std::set<node const*> bfs(node const* entry) {
  std::queue<node const*> q;
  std::set<node const*> visited;

  q.emplace(entry);

  while (!q.empty()) {
    auto const next = q.front();
    q.pop();

    if (visited.find(next) != end(visited)) {
      continue;
    }

    visited.emplace(next);

    for (auto const& e : next->edges_) {
      q.emplace(e->to_);
    }
  }

  return visited;
}

TEST_CASE("graph serialization") {
  utl::byte_buf buf;
  {
    graph g;

    auto const n1 = g.make_node(utl::string{"NODE A"});
    auto const n2 = g.make_node(utl::string{"NODE B"});
    auto const n3 = g.make_node(utl::string{"NODE C"});

    auto const e1 = g.make_edge(n1->id(), n2->id());
    auto const e2 = g.make_edge(n2->id(), n3->id());
    auto const e3 = g.make_edge(n3->id(), n1->id());

    n1->add_edge(e1);
    n2->add_edge(e2);
    n3->add_edge(e3);

    buf = utl::serialize(g);
  }  // EOL graph

  auto const g = utl::deserialize<graph>(&buf[0]);
  auto const visited = bfs(g->nodes_[0].get());
  unsigned i = 0;
  CHECK((*std::next(begin(visited), i++))->name_ == utl::string{"NODE A"});
  CHECK((*std::next(begin(visited), i++))->name_ == utl::string{"NODE B"});
  CHECK((*std::next(begin(visited), i++))->name_ == utl::string{"NODE C"});
}