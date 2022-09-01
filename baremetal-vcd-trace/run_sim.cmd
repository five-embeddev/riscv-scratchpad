echo on
trace timestamp
trace count_123
trace wakeup_count
trace mei_count
trace mti_count
trace msi_count
trace ecall_count
until pc 0 main
pc 0
run 1000
pc 0
mem timestamp
mem count_123
mem wakeup_count
mem mei_count
mem mti_count
mem msi_count
mem ecall_count
interrupt 0 raise mei
run 100
interrupt 0 clear mei
pc 0
mem timestamp
mem count_123
mem wakeup_count
mem mei_count
mem mti_count
mem msi_count
mem ecall_count
run 1000
mem timestamp
mem count_123
mem wakeup_count
mem mei_count
mem mti_count
mem msi_count
mem ecall_count

run 5000
mem timestamp
run 5000
mem timestamp

run 300
interrupt 0 raise msi
run 10
interrupt 0 clear msi


run 5000
mem timestamp

q
