#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "boost/align/aligned_allocator.hpp"

#include "utl/parser/util.h"
#include "utl/serialization/buf.h"
#include "utl/serialization/file.h"
#include "utl/serialization/offset_t.h"
#include "utl/serialization/string.h"
#include "utl/serialization/unique_ptr.h"
#include "utl/serialization/vector.h"
#include "utl/struct/for_each_field.h"

namespace utl {

// =============================================================================
// SERIALIZE
// -----------------------------------------------------------------------------
template <typename Target>
struct serializer {
  struct pending_offset {
    void* origin_ptr_;
    offset_t pos_;
  };

  explicit serializer(Target& t) : t_{t} {}

  template <typename T>
  void special(T const* origin, offset_t const pos) {
    using Type = std::remove_reference_t<std::remove_const_t<T>>;
    if constexpr (!std::is_scalar_v<Type>) {
      utl::for_each_ptr_field(*origin, [&](auto& member) {
        auto const member_offset =
            static_cast<offset_t>(reinterpret_cast<char const*>(member) -
                                  reinterpret_cast<char const*>(origin));
        special(member, pos + member_offset);
      });
    } else if constexpr (std::is_pointer_v<Type>) {
      if (*origin == nullptr) {
        return;
      }
      if (auto const it = offsets_.find(*origin); it != end(offsets_)) {
        write(pos, it->second);
      } else {
        pending_.emplace_back(pending_offset{*origin, pos});
      }
    }
  }

  template <typename T>
  void special(utl::vector<T> const* origin, offset_t const pos) {
    auto const size = sizeof(T) * origin->used_size_;
    auto const start = write(origin->el_, size, std::alignment_of_v<T>);

    write(pos + offsetof(utl::vector<T>, el_), start);
    write(pos + offsetof(utl::vector<T>, allocated_size_), origin->used_size_);
    write(pos + offsetof(utl::vector<T>, self_allocated_), false);

    auto i = 0u;
    for (auto it = start; it != start + size; it += sizeof(T)) {
      special(origin->el_ + i++, it);
    }
  }

  void special(utl::string const* origin, offset_t const pos) {
    if (origin->is_short()) {
      return;
    }

    auto const start =
        write(origin->data(), origin->size(), std::alignment_of_v<char>);
    write(pos + offsetof(utl::string, h_.ptr_), start);
    write(pos + offsetof(utl::string, h_.self_allocated_), false);
  }

  template <typename T>
  void special(utl::unique_ptr<T> const* origin, offset_t const pos) {
    auto const start = write(origin->el_, sizeof(T), std::alignment_of_v<T>);
    write(pos + offsetof(utl::unique_ptr<T>, el_), start);
    write(pos + offsetof(utl::unique_ptr<T>, self_allocated_), false);
    offsets_[origin->el_] = start;
    special(origin->el_, start);
  }

  template <typename T>
  void serialize(T& value) {
    using written_type_t =
        std::remove_reference_t<std::remove_const_t<decltype(value)>>;

    auto const start =
        write(&value, sizeof(value), std::alignment_of_v<written_type_t>);
    utl::for_each_ptr_field(value, [&](auto& member) {
      auto const member_offset =
          static_cast<offset_t>(reinterpret_cast<char const*>(member) -
                                reinterpret_cast<char const*>(&value));
      special(member, start + member_offset);
    });
    resolve_pending();
  }

  void resolve_pending() {
    for (auto& p : pending_) {
      if (auto const it = offsets_.find(p.origin_ptr_); it != end(offsets_)) {
        write(p.pos_, it->second);
      } else {
        std::cout << "warning: dangling pointer " << p.origin_ptr_
                  << " serialized at offset " << p.pos_ << "\n";
      }
    }
  }

  offset_t write(void const* ptr, offset_t const size, offset_t alignment = 0) {
    return t_.write(ptr, size, alignment);
  }

  template <typename T>
  void write(offset_t const pos, T const& val) {
    t_.write(pos, val);
  }

  std::map<void*, offset_t> offsets_;
  std::vector<pending_offset> pending_;
  Target& t_;
};

template <typename Target, typename T>
void serialize(Target& t, T& el) {
  return serializer<Target>(t).serialize(el);
}

template <typename T>
byte_buf serialize(T& el) {
  auto b = buf{};
  serializer<buf>(b).serialize(el);
  return std::move(b.buf_);
}

// =============================================================================
// DESERIALIZE
// -----------------------------------------------------------------------------
struct range {
  range(uint8_t* from, uint8_t* to) : from_{from}, to_{to} {}

  template <typename T, typename Ptr>
  T regain(Ptr* ptr) const {
    auto const offset = reinterpret_cast<offset_t>(ptr);
    if (to_ != nullptr) {
      verify(offset < static_cast<offset_t>(to_ - from_),
             "pointer out of range");
    }
    return reinterpret_cast<T>(from_ + offset);
  }

  uint8_t* from_{nullptr};
  uint8_t* to_{nullptr};
};  // namespace utl

template <typename T>
void regain_pointers_from_offsets(range const& r, T* el) {
  using written_type_t = std::remove_reference_t<std::remove_const_t<T>>;
  if constexpr (std::is_pointer_v<written_type_t>) {
    *el = r.regain<written_type_t>(*el);
  } else if constexpr (std::is_scalar_v<written_type_t>) {
    return;
  } else {
    utl::for_each_ptr_field(
        *el, [&](auto& f) { regain_pointers_from_offsets(r, f); });
  }
}

template <typename T>
void regain_pointers_from_offsets(range const& r, utl::vector<T>* el) {
  el->el_ = r.regain<T*>(el->el_);
  for (auto& m : *el) {
    regain_pointers_from_offsets(r, &m);
  }
}

inline void regain_pointers_from_offsets(range const& r, utl::string* el) {
  if (el->is_short()) {
    return;
  } else {
    el->h_.ptr_ = r.regain<char*>(el->h_.ptr_);
  }
}

template <typename T>
void regain_pointers_from_offsets(range const& r, utl::unique_ptr<T>* el) {
  el->el_ = r.regain<T*>(el->el_);
  regain_pointers_from_offsets(r, el->el_);
}

template <typename T>
T* deserialize(uint8_t* from, uint8_t* to = nullptr) {
  range r{from, to};
  auto const el = reinterpret_cast<T*>(from);
  regain_pointers_from_offsets(r, el);
  return el;
}

}  // namespace utl