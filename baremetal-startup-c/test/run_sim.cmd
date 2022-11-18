echo on

trace timestamp
trace global_value_with_init
trace global_value1_with_constructor
trace global_value2_with_constructor

trace global_u32_value_with_init
trace global_u64_value_with_init
trace global_f32_value_with_init real
trace global_f64_value_with_init real
trace global_u16_value_with_init
trace global_u8a_value_with_init
trace global_u8b_value_with_init
trace global_u8c_value_with_init
trace global_u8d_value_with_init
trace global_bool_keep_running

until pc 0 main
pc 0
run 1000
pc 0

set 0 global_bool_keep_running 0

run 1000
pc 0

q
