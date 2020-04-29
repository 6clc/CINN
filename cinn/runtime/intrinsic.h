#pragma once
#include <string>
#include <vector>

#include "cinn/ir/buffer.h"
#include "cinn/runtime/cinn_runtime.h"
#include "cinn/runtime/intrinsic_types.h"

/**
 * \file This file implements some runtime concepts used in analysis and codegen.
 */
namespace cinn {

namespace ir {
class Expr;
}  // namespace ir

namespace runtime {

//! cinn_buffer_t::new_(buffer)
static const char* buffer_create = "cinn_buffer_t::new_";
//! cinn_buffer_t::delete_(buffer)
static const char* buffer_destroy = "cinn_buffer_t::delete_";

static const char* buffer_load = "cinn_buffer_load";

static const char* buffer_malloc = "cinn_buffer_malloc";
static const char* buffer_free   = "cinn_buffer_free";

static const char* buffer_get_data_handle       = "cinn_buffer_get_data_handle";
static const char* buffer_get_data_const_handle = "cinn_buffer_get_data_const_handle";

//! Buffer load an element of some primitive type
// @{
static const char* buffer_load_float32 = "buffer_load_float32";
static const char* buffer_load_float64 = "buffer_load_float64";
// @}

static const char* pod_value_ty                    = "cinn_pod_value_t";
static const char* float_to_cinn_pod_value_repr    = "float_to_cinn_pod_value";
static const char* buffer_p_to_cinn_pod_value_repr = "buffer_p_to_cinn_pod_value";

static const char* pod_values_to_array_repr = "pod_values_to_array";

static const char* get_address_repr = "get_address";

class pod_values_to_array;

ir::Expr BufferCreate(ir::Buffer buffer);
/**
 * Get an expression to load an element from a buffer.
 * @param buffer
 * @param shape
 * @param indices
 */
ir::Expr BufferLoad(ir::Buffer buffer, const std::vector<ir::Expr>& indices);

/**
 * Get an expression to malloc a buffer.
 * @param buffer
 * @return
 */
ir::Expr BufferMalloc(ir::Buffer buffer);
ir::Expr BufferMalloc(ir::Var buffer_var);

ir::Expr BufferGetDataHandle(ir::Buffer buffer, bool is_const = true);

//! Convert the Type in compile time to runtime type.
cinn_type_t ToRuntimeType(Type type);

}  // namespace runtime
}  // namespace cinn
