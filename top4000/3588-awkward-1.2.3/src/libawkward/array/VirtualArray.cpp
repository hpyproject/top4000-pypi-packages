// BSD 3-Clause License; see https://github.com/scikit-hep/awkward-1.0/blob/main/LICENSE

#define FILENAME(line) FILENAME_FOR_EXCEPTIONS("src/libawkward/array/VirtualArray.cpp", line)
#define FILENAME_C(line) FILENAME_FOR_EXCEPTIONS_C("src/libawkward/array/VirtualArray.cpp", line)

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "awkward/common.h"
#include "awkward/Reducer.h"
#include "awkward/io/json.h"
#include "awkward/array/RegularArray.h"

#include "awkward/array/VirtualArray.h"

namespace awkward {
  ////////// VirtualForm

  VirtualForm::VirtualForm(bool has_identities,
                           const util::Parameters& parameters,
                           const FormKey& form_key,
                           const FormPtr& form,
                           bool has_length)
      : Form(has_identities, parameters, form_key)
      , form_(form)
      , has_length_(has_length) { }

  bool
  VirtualForm::has_form() const {
    return form_.get() != nullptr;
  }

  const FormPtr
  VirtualForm::form() const {
    return form_;
  }

  bool
  VirtualForm::has_length() const {
    return has_length_;
  }

  const TypePtr
  VirtualForm::type(const util::TypeStrs& typestrs) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->type(typestrs);
    }
  }

  void
  VirtualForm::tojson_part(ToJson& builder, bool verbose) const {
    builder.beginrecord();
    builder.field("class");
    builder.string("VirtualArray");
    builder.field("form");
    if (form_.get() == nullptr) {
      builder.null();
    }
    else {
      form_.get()->tojson_part(builder, verbose);
    }
    builder.field("has_length");
    builder.boolean(has_length_);
    identities_tojson(builder, verbose);
    parameters_tojson(builder, verbose);
    form_key_tojson(builder, verbose);
    builder.endrecord();
  }

  const FormPtr
  VirtualForm::shallow_copy() const {
    return std::make_shared<VirtualForm>(has_identities_,
                                         parameters_,
                                         form_key_,
                                         form_,
                                         has_length_);
  }

  const FormPtr
  VirtualForm::with_form_key(const FormKey& form_key) const {
    return std::make_shared<VirtualForm>(has_identities_,
                                         parameters_,
                                         form_key,
                                         form_,
                                         has_length_);
  }

  const std::string
  VirtualForm::purelist_parameter(const std::string& key) const {
    std::string out = parameter(key);
    if (out == std::string("null")) {
      if (form_.get() == nullptr) {
        return out;
      }
      return form_.get()->purelist_parameter(key);
    }
    else {
      return out;
    }
  }

  bool
  VirtualForm::purelist_isregular() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->purelist_isregular();
    }
  }

  int64_t
  VirtualForm::purelist_depth() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->purelist_depth();
    }
  }

  bool
  VirtualForm::dimension_optiontype() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->dimension_optiontype();
    }
  }

  const std::pair<int64_t, int64_t>
  VirtualForm::minmax_depth() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->minmax_depth();
    }
  }

  const std::pair<bool, int64_t>
  VirtualForm::branch_depth() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->branch_depth();
    }
  }

  int64_t
  VirtualForm::numfields() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->numfields();
    }
  }

  int64_t
  VirtualForm::fieldindex(const std::string& key) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->fieldindex(key);
    }
  }

  const std::string
  VirtualForm::key(int64_t fieldindex) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->key(fieldindex);
    }
  }

  bool
  VirtualForm::haskey(const std::string& key) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->haskey(key);
    }
  }

  const std::vector<std::string>
  VirtualForm::keys() const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
        std::string("VirtualForm cannot determine its type without an expected Form")
        + FILENAME(__LINE__));
    }
    else {
      return form_.get()->keys();
    }
  }

  bool
  VirtualForm::equal(const FormPtr& other,
                     bool check_identities,
                     bool check_parameters,
                     bool check_form_key,
                     bool compatibility_check) const {
    if (compatibility_check) {
      if (form_.get() != nullptr) {
        return form_.get()->equal(other,
                                  check_identities,
                                  check_parameters,
                                  check_form_key,
                                  compatibility_check);
      }
      else {
        return true;
      }
    }

    if (check_identities  &&
        has_identities_ != other.get()->has_identities()) {
      return false;
    }
    if (check_parameters  &&
        !util::parameters_equal(parameters_, other.get()->parameters(), false)) {
      return false;
    }
    if (check_form_key  &&
        !form_key_equals(other.get()->form_key())) {
      return false;
    }
    if (VirtualForm* t = dynamic_cast<VirtualForm*>(other.get())) {
      // Called by Form.__eq__ in Python; should be an equivalence relation.
      if (form_.get() == nullptr  &&  t->form().get() != nullptr) {
        return false;
      }
      else if (form_.get() != nullptr  &&  t->form().get() == nullptr) {
        return false;
      }
      else if (form_.get() != nullptr  &&  t->form().get() != nullptr) {
        if (!form_.get()->equal(t->form(),
                                check_identities,
                                check_parameters,
                                check_form_key,
                                compatibility_check)) {
          return false;
        }
      }
      return has_length_ == t->has_length();
    }

    else {
      return false;
    }
  }

  const FormPtr
  VirtualForm::getitem_field(const std::string& key) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
          std::string("Cannot determine field without an expected Form")
          + FILENAME(__LINE__));
    }
    else {
      return form_.get()->getitem_field(key);
    }
  }

  const FormPtr
  VirtualForm::getitem_fields(const std::vector<std::string>& keys) const {
    if (form_.get() == nullptr) {
      throw std::invalid_argument(
          std::string("Cannot determine fields without an expected Form")
          + FILENAME(__LINE__));
    }
    else {
      return form_.get()->getitem_fields(keys);
    }
  }

  ////////// VirtualArray

  VirtualArray::VirtualArray(const IdentitiesPtr& identities,
                             const util::Parameters& parameters,
                             const ArrayGeneratorPtr& generator,
                             const ArrayCachePtr& cache,
                             const std::string& cache_key,
                             const kernel::lib ptr_lib)
      : Content(identities, parameters)
      , generator_(generator)
      , cache_(cache)
      , cache_key_(cache_key)
      , ptr_lib_(ptr_lib) { }

  VirtualArray::VirtualArray(const IdentitiesPtr& identities,
                             const util::Parameters& parameters,
                             const ArrayGeneratorPtr& generator,
                             const ArrayCachePtr& cache,
                             const kernel::lib ptr_lib)
      : Content(identities, parameters)
      , generator_(generator)
      , cache_(cache)
      , cache_key_(ArrayCache::newkey())
      , ptr_lib_(ptr_lib) { }

  const ArrayGeneratorPtr
  VirtualArray::generator() const {
    return generator_;
  }

  const ArrayCachePtr
  VirtualArray::cache() const {
    return cache_;
  }

  const kernel::lib
  VirtualArray::ptr_lib() const {
    return ptr_lib_;
  }

  const ContentPtr
  VirtualArray::peek_array() const {
    if (cache_.get() != nullptr  &&  !cache_.get()->is_broken()) {
      return cache_.get()->get(cache_key());
    }
    return ContentPtr(nullptr);
  }

  kernel::lib check_key(const std::string& cache_key) {
    auto fully_qualified_index = cache_key.find_last_of(':');

    if (fully_qualified_index != std::string::npos) {
      if (cache_key.substr(fully_qualified_index + 1, cache_key.length()) == "cuda") {
        return kernel::lib::cuda;
      }
    }

    return kernel::lib::cpu;
  }

  const ContentPtr
  VirtualArray::array() const {
    ContentPtr out(nullptr);
    kernel::lib src_ptrlib = check_key(cache_key_);

    if (cache_.get() != nullptr) {
      if (src_ptrlib != ptr_lib_) {
        out = cache_.get()->get(cache_key())->copy_to(ptr_lib_);
      }
      else {
        out = cache_.get()->get(cache_key());
      }
    }
    if (out.get() == nullptr) {
      if (src_ptrlib != ptr_lib_) {
        out = generator_.get()->generate_and_check()->copy_to(src_ptrlib);
      }
      else {
        out = generator_.get()->generate_and_check();
      }
    }
    if (cache_.get() != nullptr) {
      cache_.get()->set(kernel::fully_qualified_cache_key(ptr_lib_, cache_key()),
                        out);
    }
    return out;
  }

  const std::string
  VirtualArray::cache_key() const {
    return cache_key_;
  }

  const std::string
  VirtualArray::classname() const {
    return "VirtualArray";
  }

  void
  VirtualArray::setidentities(const IdentitiesPtr& identities) {
    throw std::runtime_error(
      std::string("FIXME: VirtualArray::setidentities(identities)")
      + FILENAME(__LINE__));
  }

  void
  VirtualArray::setidentities() {
    throw std::runtime_error(
      std::string("FIXME: VirtualArray::setidentities")
      + FILENAME(__LINE__));
  }

  const TypePtr
  VirtualArray::type(const util::TypeStrs& typestrs) const {
    return form(true).get()->type(typestrs);
  }

  const FormPtr
  VirtualArray::form(bool materialize) const {
    FormPtr generator_form = generator_.get()->form();
    if (materialize  &&  generator_form.get() == nullptr) {
      generator_form = array().get()->form(materialize);
    }
    int64_t generator_length = generator_.get()->length();
    return std::make_shared<VirtualForm>(identities_.get() != nullptr,
                                         parameters_,
                                         FormKey(nullptr),
                                         generator_form,
                                         generator_length >= 0);
  }

  kernel::lib
  VirtualArray::kernels() const {
    if (identities_.get() == nullptr) {
      return ptr_lib_;
    }
    else if (ptr_lib_ == identities_.get()->ptr_lib()) {
      return ptr_lib_;
    }
    else {
      return kernel::lib::size;
    }
  }

  void
  VirtualArray::caches(std::vector<ArrayCachePtr>& out) const {
    generator_.get()->caches(out);

    if (cache_.get() != nullptr) {
      bool found = false;
      for (auto oldcache : out) {
        if (oldcache.get() == cache_.get()) {
          found = true;
          break;
        }
      }
      if (!found) {
        out.push_back(cache_);
      }
    }
  }

  const std::string
  VirtualArray::tostring_part(const std::string& indent,
                            const std::string& pre,
                            const std::string& post) const {
    std::stringstream out;
    out << indent << pre << "<" << classname()
        << " cache_key=\"" << cache_key_ << "\">\n";
    if (identities_.get() != nullptr) {
      out << identities_.get()->tostring_part(
               indent + std::string("    "), "", "\n");
    }
    if (!parameters_.empty()) {
      out << parameters_tostring(indent + std::string("    "), "", "\n");
    }
    out << generator_.get()->tostring_part(indent + std::string("    "),
                                           "", "\n");
    if (cache_.get() != nullptr) {
      out << cache_.get()->tostring_part(indent + std::string("    "),
                                         "", "\n");
    }
    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      out << peek.get()->tostring_part(
               indent + std::string("    "), "<array>", "</array>\n");
    }
    out << indent << "</" << classname() << ">" << post;
    return out.str();
  }

  void
  VirtualArray::tojson_part(ToJson& builder,
                          bool include_beginendlist) const {
    return array().get()->tojson_part(builder, include_beginendlist);
  }

  void
  VirtualArray::nbytes_part(std::map<size_t, int64_t>& largest) const { }

  int64_t
  VirtualArray::length() const {
    int64_t out = generator_.get()->length();
    if (out < 0) {
      out = array().get()->length();
    }
    return out;
  }

  const ContentPtr
  VirtualArray::shallow_copy() const {
    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                              identities_,
                                              parameters_,
                                              generator_,
                                              cache_,
                                              cache_key_);
    out.get()->set_cache_depths_from(this);
    return out;
  }

  const ContentPtr
  VirtualArray::deep_copy(bool copyarrays,
                          bool copyindexes,
                          bool copyidentities) const {
    return array().get()->deep_copy(copyarrays, copyindexes, copyidentities);
  }

  void
  VirtualArray::check_for_iteration() const { }

  const ContentPtr
  VirtualArray::getitem_nothing() const {
    return array().get()->getitem_nothing();
  }

  const ContentPtr
  VirtualArray::getitem_at(int64_t at) const {
    int64_t regular_at = at;
    if (regular_at < 0) {
      regular_at += length();
    }
    if (!(0 <= regular_at  &&  regular_at < length())) {
      util::handle_error(failure("index out of range",
                                 kSliceNone,
                                 at,
                                 FILENAME_C(__LINE__)),
                         classname(),
                         identities_.get());
    }
    return getitem_at_nowrap(regular_at);
  }

  const ContentPtr
  VirtualArray::getitem_at_nowrap(int64_t at) const {
    return array().get()->getitem_at_nowrap(at);
  }

  const ContentPtr
  VirtualArray::getitem_range(int64_t start, int64_t stop) const {
    if (generator_.get()->length() < 0) {
      return array().get()->getitem_range(start, stop);
    }
    else {
      ContentPtr peek = peek_array();
      if (peek.get() != nullptr) {
        return peek.get()->getitem_range(start, stop);
      }

      int64_t regular_start = start;
      int64_t regular_stop = stop;
      kernel::regularize_rangeslice(&regular_start, &regular_stop,
        true, start != Slice::none(), stop != Slice::none(),
        generator_.get()->length());
      return getitem_range_nowrap(regular_start, regular_stop);
    }
  }

  const ContentPtr
  VirtualArray::getitem_range_nowrap(int64_t start, int64_t stop) const {
    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      return peek.get()->getitem_range_nowrap(start, stop);
    }

    if (generator_.get()->length() >= 0  &&
        start == 0  &&
        stop == generator_.get()->length()) {
      return shallow_copy();
    }

    Slice slice;
    slice.append(SliceRange(start, stop, 1));
    slice.become_sealed();
    FormPtr sliceform(nullptr);

    if (generator_.get()->form().get() != nullptr) {
      sliceform = generator_.get()->form().get()->getitem_range();
    }

    ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                 sliceform, stop - start, shallow_copy(), slice);
    ArrayCachePtr cache(nullptr);

    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                              Identities::none(),
                                              parameters_,
                                              generator,
                                              cache);
    out.get()->set_cache_depths_from(this);
    return out;
  }

  const ContentPtr
  VirtualArray::getitem_field(const std::string& key) const {
    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      return peek.get()->getitem_field(key);
    }

    Slice slice;
    slice.append(SliceField(key));
    slice.become_sealed();
    FormPtr sliceform(nullptr);

    util::Parameters params;
    if (generator_.get()->form().get() != nullptr) {
      sliceform = generator_.get()->form().get()->getitem_field(key);
      std::string record = sliceform.get()->purelist_parameter("__record__");
      if (record != std::string("null")) {
        params["__record__"] = record;
      }
      std::string doc = sliceform.get()->purelist_parameter("__doc__");
      if (doc != std::string("null")) {
        params["__doc__"] = doc;
      }
    }

    ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                 sliceform, generator_.get()->length(), shallow_copy(), slice);
    ArrayCachePtr cache(nullptr);
    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                              Identities::none(),
                                              params,
                                              generator,
                                              cache);
    out.get()->set_cache_depths_from(sliceform);
    return out;
  }

  const ContentPtr
  VirtualArray::getitem_field(const std::string& key,
                              const Slice& only_fields) const {
    return array().get()->getitem_field(key, only_fields);
  }

  const ContentPtr
  VirtualArray::getitem_fields(const std::vector<std::string>& keys) const {
    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      return peek.get()->getitem_fields(keys);
    }

    Slice slice;
    slice.append(SliceFields(keys));
    slice.become_sealed();
    FormPtr sliceform(nullptr);

    if (generator_.get()->form().get() != nullptr) {
      sliceform = generator_.get()->form().get()->getitem_fields(keys);
    }

    ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                 sliceform, generator_.get()->length(), shallow_copy(), slice);
    ArrayCachePtr cache(nullptr);
    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                              Identities::none(),
                                              util::Parameters(),
                                              generator,
                                              cache);
    out.get()->set_cache_depths_from(sliceform);
    return out;
  }

  const ContentPtr
  VirtualArray::getitem_fields(const std::vector<std::string>& keys,
                               const Slice& only_fields) const {
    return array().get()->getitem_fields(keys, only_fields);
  }

  const ContentPtr
  VirtualArray::carry(const Index64& carry, bool allow_lazy) const {
    if (carry.iscontiguous()) {
      if (carry.length() == length()) {
        return shallow_copy();
      }
      else {
        return getitem_range_nowrap(0, carry.length());
      }
    }

    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      return peek.get()->carry(carry, allow_lazy);
    }

    Slice slice;
    std::vector<int64_t> shape({ carry.length() });
    std::vector<int64_t> strides({ 1 });
    slice.append(SliceArray64(carry, shape, strides, false));
    slice.become_sealed();
    FormPtr form(nullptr);
    ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                 form, carry.length(), shallow_copy(), slice);
    ArrayCachePtr cache(nullptr);
    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                               Identities::none(),
                                               forward_parameters(),
                                               generator,
                                               cache);
    out.get()->set_cache_depths_from(this);
    return out;
  }

  int64_t
  VirtualArray::purelist_depth() const {
    if (cache_depths_.empty()) {
      return form(true).get()->purelist_depth();
    }
    else {
      return cache_depths_[0];
    }
  }

  const std::pair<int64_t, int64_t>
  VirtualArray::minmax_depth() const {
    if (cache_depths_.empty()) {
      return form(true).get()->minmax_depth();
    }
    else {
      return std::pair<int64_t, int64_t>(cache_depths_[1], cache_depths_[2]);
    }
  }

  const std::pair<bool, int64_t>
  VirtualArray::branch_depth() const {
    if (cache_depths_.empty()) {
      return form(true).get()->branch_depth();
    }
    else {
      return std::pair<bool, int64_t>(cache_depths_[3], cache_depths_[4]);
    }
  }

  int64_t
  VirtualArray::numfields() const {
    return form(true).get()->numfields();
  }

  int64_t
  VirtualArray::fieldindex(const std::string& key) const {
    return form(true).get()->fieldindex(key);
  }

  const std::string
  VirtualArray::key(int64_t fieldindex) const {
    return form(true).get()->key(fieldindex);
  }

  bool
  VirtualArray::haskey(const std::string& key) const {
    return form(true).get()->haskey(key);
  }

  const std::vector<std::string>
  VirtualArray::keys() const {
    return form(true).get()->keys();
  }

  const std::string
  VirtualArray::validityerror(const std::string& path) const {
    return array().get()->validityerror(path + ".array");
  }

  const ContentPtr
  VirtualArray::shallow_simplify() const {
    return array().get()->shallow_simplify();
  }

  const ContentPtr
  VirtualArray::num(int64_t axis, int64_t depth) const {
    return array().get()->num(axis, depth);
  }

  const std::pair<Index64, ContentPtr>
  VirtualArray::offsets_and_flattened(int64_t axis, int64_t depth) const {
    return array().get()->offsets_and_flattened(axis, depth);
  }

  bool
  VirtualArray::mergeable(const ContentPtr& other, bool mergebool) const {
    return array().get()->mergeable(other, mergebool);
  }

  bool
  VirtualArray::referentially_equal(const ContentPtr& other) const {
    if (identities_.get() == nullptr  &&  other.get()->identities().get() != nullptr) {
      return false;
    }
    if (identities_.get() != nullptr  &&  other.get()->identities().get() == nullptr) {
      return false;
    }
    if (identities_.get() != nullptr  &&  other.get()->identities().get() != nullptr) {
      if (!identities_.get()->referentially_equal(other->identities())) {
        return false;
      }
    }
    if (VirtualArray* raw = dynamic_cast<VirtualArray*>(other.get())) {
      return ptr_lib_ == raw->ptr_lib()  &&
             cache_key_ == raw->cache_key()  &&
             generator_.get()->referentially_equal(raw->generator())  &&
             parameters_ == raw->parameters();
    }
    else {
      return false;
    }
  }

  const ContentPtr
  VirtualArray::mergemany(const ContentPtrVec& others) const {
    return array().get()->mergemany(others);
  }

  const SliceItemPtr
  VirtualArray::asslice() const {
    return array().get()->asslice();
  }

  const ContentPtr
  VirtualArray::fillna(const ContentPtr& value) const {
    return array().get()->fillna(value);
  }

  const ContentPtr
  VirtualArray::rpad(int64_t target, int64_t axis, int64_t depth) const {
    return array().get()->rpad(target, axis, depth);
  }

  const ContentPtr
  VirtualArray::rpad_and_clip(int64_t target,
                              int64_t axis,
                              int64_t depth) const {
    return array().get()->rpad_and_clip(target, axis, depth);
  }

  const ContentPtr
  VirtualArray::reduce_next(const Reducer& reducer,
                          int64_t negaxis,
                          const Index64& starts,
                          const Index64& shifts,
                          const Index64& parents,
                          int64_t outlength,
                          bool mask,
                          bool keepdims) const {
    return array().get()->reduce_next(reducer,
                                      negaxis,
                                      starts,
                                      shifts,
                                      parents,
                                      outlength,
                                      mask,
                                      keepdims);
  }

  const ContentPtr
  VirtualArray::sort_next(int64_t negaxis,
                          const Index64& starts,
                          const Index64& parents,
                          int64_t outlength,
                          bool ascending,
                          bool stable,
                          bool keepdims) const {
    return array().get()->sort_next(negaxis,
                                    starts,
                                    parents,
                                    outlength,
                                    ascending,
                                    stable,
                                    keepdims);
  }

  const ContentPtr
  VirtualArray::argsort_next(int64_t negaxis,
                             const Index64& starts,
                             const Index64& parents,
                             int64_t outlength,
                             bool ascending,
                             bool stable,
                             bool keepdims) const {
    return array().get()->argsort_next(negaxis,
                                       starts,
                                       parents,
                                       outlength,
                                       ascending,
                                       stable,
                                       keepdims);
  }

  const ContentPtr
  VirtualArray::localindex(int64_t axis, int64_t depth) const {
    return array().get()->localindex(axis, depth);
  }

  const ContentPtr
  VirtualArray::combinations(int64_t n,
                             bool replacement,
                             const util::RecordLookupPtr& recordlookup,
                             const util::Parameters& parameters,
                             int64_t axis,
                             int64_t depth) const {
    return array().get()->combinations(n,
                                       replacement,
                                       recordlookup,
                                       parameters,
                                       axis,
                                       depth);
  }

  const ContentPtr
  VirtualArray::getitem(const Slice& where) const {
    ContentPtr peek = peek_array();
    if (peek.get() != nullptr) {
      return peek.get()->getitem(where);
    }

    if (where.length() == 1) {
      SliceItemPtr head = where.head();

      if (SliceRange* range =
          dynamic_cast<SliceRange*>(head.get())) {
        if (range->step() == 0) {
            throw std::invalid_argument(
              std::string("slice step cannot be zero") + FILENAME(__LINE__));
        }
        else if (generator_.get()->length() >= 0) {
          int64_t regular_start = range->start();
          int64_t regular_stop = range->stop();
          kernel::regularize_rangeslice(&regular_start,
                                        &regular_stop,
                                        range->step() > 0,
                                        range->start() != Slice::none(),
                                        range->stop() != Slice::none(),
                                        generator_.get()->length());
          int64_t length;
          if ((range->step() > 0  &&  regular_stop - regular_start > 0)  ||
              (range->step() < 0  &&  regular_stop - regular_start < 0)) {
            int64_t numer = abs(regular_start - regular_stop);
            int64_t denom = abs(range->step());
            int64_t d = numer / denom;
            int64_t m = numer % denom;
            length = d + (m != 0 ? 1 : 0);
          }
          else {
            length = 0;
          }
          FormPtr form(nullptr);
          ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                     form, length, shallow_copy(), where);
          ArrayCachePtr cache(nullptr);
          std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                                    Identities::none(),
                                                    forward_parameters(),
                                                    generator,
                                                    cache);
          out.get()->set_cache_depths_from(this);
          return out;
        }
        else {
          return array().get()->getitem(where);
        }
      }

      else if (SliceEllipsis* ellipsis =
               dynamic_cast<SliceEllipsis*>(head.get())) {
        FormPtr form(nullptr);
        ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                     form, generator_.get()->length(), shallow_copy(), where);
        ArrayCachePtr cache(nullptr);
        std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                                  Identities::none(),
                                                  forward_parameters(),
                                                  generator,
                                                  cache);
        out.get()->set_cache_depths_from(this);
        return out;
      }

      else if (SliceNewAxis* newaxis =
               dynamic_cast<SliceNewAxis*>(head.get())) {
        FormPtr form(nullptr);
        ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                     form, 1, shallow_copy(), where);
        ArrayCachePtr cache(nullptr);
        std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                                  Identities::none(),
                                                  forward_parameters(),
                                                  generator,
                                                  cache);
        out.get()->set_cache_depths_from(this);
        out.get()->add_to_cache_depths(1);
        return out;
      }

      else if (SliceArray64* slicearray =
               dynamic_cast<SliceArray64*>(head.get())) {
        FormPtr form(nullptr);
        ArrayGeneratorPtr generator = std::make_shared<SliceGenerator>(
                     form, slicearray->length(), shallow_copy(), where);
        ArrayCachePtr cache(nullptr);
        std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                                  Identities::none(),
                                                  forward_parameters(),
                                                  generator,
                                                  cache);
        out.get()->set_cache_depths_from(this);
        return out;
      }

      else if (SliceField* field =
               dynamic_cast<SliceField*>(head.get())) {
        return getitem_field(field->key());
      }

      else if (SliceFields* fields =
               dynamic_cast<SliceFields*>(head.get())) {
        return getitem_fields(fields->keys(), where.tail().only_fields());
      }

      else {
        return array().get()->getitem(where);
      }
    }

    else {
      return array().get()->getitem(where);
    }
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceItemPtr& head,
                             const Slice& tail,
                             const Index64& advanced) const {
    if (head.get() == nullptr) {
      return shallow_copy();
    }
    else {
      return array().get()->getitem_next(head, tail, advanced);
    }
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceAt& at,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(at)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceRange& range,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(range)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceArray64& array,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(array)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceField& field,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(field)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceFields& fields,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(fields)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceJagged64& jagged,
                             const Slice& tail,
                             const Index64& advanced) const {
    throw std::runtime_error(
      std::string("undefined operation: VirtualArray::getitem_next(jagged)")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::getitem_next_jagged(const Index64& slicestarts,
                                    const Index64& slicestops,
                                    const SliceItemPtr& slicecontent,
                                    const Slice& tail) const {
    return array().get()->getitem_next_jagged(slicestarts,
                                              slicestops,
                                              slicecontent,
                                              tail);
  }

  const ContentPtr
  VirtualArray::getitem_next_jagged(const Index64& slicestarts,
                                    const Index64& slicestops,
                                    const SliceArray64& slicecontent,
                                    const Slice& tail) const {
    return array().get()->getitem_next_jagged(slicestarts,
                                              slicestops,
                                              slicecontent,
                                              tail);
  }

  const ContentPtr
  VirtualArray::getitem_next_jagged(const Index64& slicestarts,
                                    const Index64& slicestops,
                                    const SliceMissing64& slicecontent,
                                    const Slice& tail) const {
    return array().get()->getitem_next_jagged(slicestarts,
                                              slicestops,
                                              slicecontent,
                                              tail);
  }

  const ContentPtr
  VirtualArray::getitem_next_jagged(const Index64& slicestarts,
                                    const Index64& slicestops,
                                    const SliceJagged64& slicecontent,
                                    const Slice& tail) const {
    return array().get()->getitem_next_jagged(slicestarts,
                                              slicestops,
                                              slicecontent,
                                              tail);
  }

  const ContentPtr
  VirtualArray::getitem_next_jagged(const Index64& slicestarts,
                                    const Index64& slicestops,
                                    const SliceVarNewAxis& slicecontent,
                                    const Slice& tail) const {
    return array().get()->getitem_next_jagged(slicestarts,
                                              slicestops,
                                              slicecontent,
                                              tail);
  }

  const ContentPtr
  VirtualArray::getitem_next(const SliceVarNewAxis& varnewaxis,
                             const Slice& tail,
                             const Index64& advanced) const {
    return array().get()->getitem_next(varnewaxis, tail, advanced);
  }

  const SliceJagged64
  VirtualArray::varaxis_to_jagged(const SliceVarNewAxis& varnewaxis) const {
    return array().get()->varaxis_to_jagged(varnewaxis);
  }

  const ContentPtr
  VirtualArray::copy_to(kernel::lib ptr_lib) const {
    IdentitiesPtr identities(nullptr);
    if (identities_.get() != nullptr) {
      identities = identities_.get()->copy_to(ptr_lib);
    }
    std::shared_ptr<VirtualArray> out = std::make_shared<VirtualArray>(
                                              identities,
                                              parameters_,
                                              generator_,
                                              cache_,
                                              cache_key_,
                                              ptr_lib);
    out.get()->set_cache_depths_from(this);
    return out;
  }

  const ContentPtr
  VirtualArray::numbers_to_type(const std::string& name) const {
    return array().get()->numbers_to_type(name);
  }

  bool
  VirtualArray::is_unique() const {
    throw std::runtime_error(
      std::string("FIXME: operation not yet implemented: VirtualArray::is_unique")
      + FILENAME(__LINE__));
  }

  const ContentPtr
  VirtualArray::unique() const {
    throw std::runtime_error(
      std::string("FIXME: operation not yet implemented: VirtualArray::unique")
      + FILENAME(__LINE__));
  }

  bool
  VirtualArray::is_subrange_equal(const Index64& start, const Index64& stop) const {
    throw std::runtime_error(
      std::string("FIXME: operation not yet implemented: VirtualArray::is_subrange_equal")
      + FILENAME(__LINE__));
  }

  const util::Parameters
  VirtualArray::forward_parameters() const {
    util::Parameters params;
    std::string record = purelist_parameter("__record__");
    if (record != std::string("null")) {
      params["__record__"] = record;
    }
    std::string doc = purelist_parameter("__doc__");
    if (doc != std::string("null")) {
      params["__doc__"] = doc;
    }
    return params;
  }

  void
  VirtualArray::set_cache_depths_from(const VirtualArray* original) {
    FormPtr form = original->generator().get()->form();
    if (form.get() != nullptr) {
      set_cache_depths_from(form);
    }
    else if (!original->cache_depths_.empty()) {
      cache_depths_.clear();
      cache_depths_.insert(cache_depths_.end(),
                           original->cache_depths_.begin(),
                           original->cache_depths_.end());
    }
  }

  void
  VirtualArray::set_cache_depths_from(const FormPtr& form) {
    cache_depths_.clear();

    if (form.get() != nullptr) {
      cache_depths_.push_back(form.get()->purelist_depth());

      const std::pair<int64_t, int64_t> minmax = form.get()->minmax_depth();
      cache_depths_.push_back(minmax.first);
      cache_depths_.push_back(minmax.second);

      const std::pair<bool, int64_t> branch = form.get()->branch_depth();
      cache_depths_.push_back(branch.first);
      cache_depths_.push_back(branch.second);
    }
  }

  void
  VirtualArray::add_to_cache_depths(int64_t delta) {
    if (!cache_depths_.empty()) {
      cache_depths_[0] += delta;
      cache_depths_[1] += delta;
      cache_depths_[2] += delta;
      // Slot 3 represents a boolean, not to be added.
      // cache_depths_[3];
      cache_depths_[4] += delta;
    }
  }

}
