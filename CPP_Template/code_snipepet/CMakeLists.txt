cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(template_test)

set(SRC
#	use_integer_sequence.cc
#	use_integral_constant.cc
#	use_is_object.cc
#	use_is_scalar.cc
#	struct_member_pointer.cc
#	test.cc
#	use_bit_field.cc
#	use_bitset_reference.cc
	nontype_template.cc
)

add_executable(${PROJECT_NAME} ${SRC})
