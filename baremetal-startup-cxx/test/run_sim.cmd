echo on

trace _ZL9timestamp

trace  _ZL22global_value_with_init
trace _ZL24global_bool_keep_running
trace _ZL26global_f32_value_with_init real
trace _ZL26global_f64_value_with_init real
trace _ZL26global_u16_value_with_init
trace _ZL26global_u32_value_with_init
trace _ZL26global_u64_value_with_init
trace _ZL26global_u8a_value_with_init
trace _ZL26global_u8b_value_with_init
trace _ZL26global_u8c_value_with_init
trace _ZL26global_u8d_value_with_init
trace _ZL30global_value1_with_constructor
trace _ZL30global_value2_with_constructor


until pc 0 main
pc 0
run 200
pc 0

set 0 _ZL24global_bool_keep_running 0

until pc 0 _Exit
pc 0

run 10

q
