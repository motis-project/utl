#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "boost/align/aligned_allocator.hpp"

#include "utl/serialization/unique_ptr.h"
#include "utl/serialization/vector.h"
#include "utl/struct/for_each_field.h"

namespace utl {

using offset_t = uint64_t;

// =============================================================================
// SERIALIZE
// -----------------------------------------------------------------------------
constexpr auto const MAX_ALIGN = 16;
using byte_buf =
    std::vector<uint8_t,
                boost::alignment::aligned_allocator<uint8_t, MAX_ALIGN>>;

struct serializer {
  void ensure_size(offset_t const size, offset_t alignment = 0) {
    auto aligned_size = size;

    if (alignment != 0) {
      auto unaligned_ptr = reinterpret_cast<void*>(&buf_[curr_offset_]);
      auto space = static_cast<size_t>(alignment) * 8u;
      auto const aligned_ptr =
          std::align(alignment, size, unaligned_ptr, space);
      auto const new_offset =
          aligned_ptr ? static_cast<uint8_t*>(aligned_ptr) - &buf_[0] : 0;
      auto const adjustment = new_offset - curr_offset_;
      curr_offset_ += adjustment;
      aligned_size += adjustment;
    }

    auto const space_left =
        static_cast<int64_t>(buf_.size()) - static_cast<int64_t>(curr_offset_);
    if (space_left < static_cast<int64_t>(aligned_size)) {
      auto const missing = aligned_size - space_left;
      buf_.resize(buf_.size() + missing);
    }
  };

  template <typename T>
  void special(T*, offset_t const pos) {
    using Type = std::remove_reference_t<std::remove_const_t<T>>;
    if constexpr (!std::is_scalar_v<Type>) {
      auto const old_base = &buf_[0];
      utl::for_each_field(*get<T>(pos),
                          [&](auto& f) { special(&f, offset(old_base, &f)); });
    }
  }

  template <typename T>
  void special(utl::vector<T>*, offset_t const pos) {
    auto const vec_data_size = sizeof(T) * get<utl::vector<T>>(pos)->used_size_;
    ensure_size(vec_data_size, std::alignment_of_v<T>);

    auto const v = get<utl::vector<T>>(pos);
    std::memcpy(&buf_[curr_offset_], v->el_, vec_data_size);
    auto const start = curr_offset_;
    curr_offset_ += vec_data_size;

    v->el_ = reinterpret_cast<T*>(start);
    v->allocated_size_ = v->used_size_;
    v->self_allocated_ = false;

    for (auto it = start; it != start + vec_data_size; it += sizeof(T)) {
      special(get<T>(it), it);
    }
  }

  template <typename T>
  void special(T**, offset_t const pos) {
    auto v = get<T*>(pos);
    if (auto const it = offsets_.find(*v); it != end(offsets_)) {
      *v = reinterpret_cast<T*>(it->second);
    } else {
      pending_.emplace_back(pos);
    }
  }

  template <typename T>
  void special(utl::unique_ptr<T>*, offset_t const pos) {
    constexpr auto const ptr_data_size = sizeof(T);

    ensure_size(ptr_data_size, std::alignment_of_v<T>);

    auto const v = get<utl::unique_ptr<T>>(pos);
    auto const old_ptr = v->el_;
    auto const start = curr_offset_;
    std::memcpy(&buf_[curr_offset_], v->el_, ptr_data_size);
    curr_offset_ += ptr_data_size;

    v->el_ = reinterpret_cast<T*>(start);
    v->self_allocated_ = false;

    offsets_[old_ptr] = start;

    special(get<T>(start), start);
  }

  template <typename T>
  byte_buf serialize(T& value) {
    constexpr auto obj_size = sizeof(value);
    using written_type_t =
        std::remove_reference_t<std::remove_const_t<decltype(value)>>;

    ensure_size(obj_size);
    std::memcpy(&buf_[0], &value, obj_size);
    curr_offset_ = obj_size;

    auto const old_base = &buf_[0];
    utl::for_each_field(*get<written_type_t>(0u),
                        [&](auto& v) { special(&v, offset(old_base, &v)); });

    resolve_pending();

    return std::move(buf_);
  }

  template <typename T>
  static offset_t offset(uint8_t const* base, T* t) {
    return reinterpret_cast<uint8_t*>(t) - base;
  }

  template <typename T>
  T* get(offset_t offset) {
    return reinterpret_cast<T*>(&buf_[offset]);
  }

  void resolve_pending() {
    for (auto& offset : pending_) {
      auto const ptr = get<void*>(offset);
      if (auto const it = offsets_.find(*ptr); it != end(offsets_)) {
        *ptr = reinterpret_cast<void*>(it->second);
      } else {
        std::cout << "warning: dangling pointer " << *ptr << " serialized\n";
      }
    }
  }

  byte_buf buf_;
  offset_t curr_offset_{0};
  std::map<void*, offset_t> offsets_;
  std::vector<offset_t> pending_;
};

template <typename T>
byte_buf serialize(T& t) {
  return serializer().serialize(t);
}

// =============================================================================
// DESERIALIZE
// -----------------------------------------------------------------------------
template <typename T>
void regain_pointers_from_offsets(uint8_t* base, T* el) {
  using written_type_t = std::remove_reference_t<std::remove_const_t<T>>;
  if constexpr (std::is_pointer_v<written_type_t>) {
    *el = reinterpret_cast<written_type_t>(base +
                                           reinterpret_cast<offset_t>(*el));
  } else if constexpr (std::is_scalar_v<written_type_t>) {
    return;
  } else {
    utl::for_each_ptr_field(
        *el, [&](auto& f) { regain_pointers_from_offsets(base, f); });
  }
}

template <typename T>
void regain_pointers_from_offsets(uint8_t* base, utl::vector<T>* el) {
  el->el_ = reinterpret_cast<T*>(base + reinterpret_cast<offset_t>(el->el_));
  for (auto& m : *el) {
    regain_pointers_from_offsets(base, &m);
  }
}

template <typename T>
void regain_pointers_from_offsets(uint8_t* base, utl::unique_ptr<T>* el) {
  el->el_ = reinterpret_cast<T*>(base + reinterpret_cast<offset_t>(el->el_));
  regain_pointers_from_offsets(base, el->el_);
}

template <typename T>
T* deserialize(uint8_t* base) {
  auto const el = reinterpret_cast<T*>(&base[0]);
  regain_pointers_from_offsets(base, el);
  return el;
}

}  // namespace utl