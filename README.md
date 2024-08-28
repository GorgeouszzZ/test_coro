# Input eg
``` shell
# Don't enter any of the same numbers during the build linked list.
# build linked list 1
1
1000
888
# build linked list 2
155
666
300
0 # sequetial
# or
1 # interleaved(coro)
```
0/1 means sequetial/interleaved(coro)
# Result eg
``` shell
# sequetial -- 15.754 us
Timer started.
Enter 3 values: 1
1000
888
input 1 run time: 3.64164e+06 us.
Enter 3 values: 155
666
300
input 2 run time: 4.07103e+06 us.
init linked list 1 run time: 19.75 us.
init linked list 2 run time: 11.217 us.
0
input mode run time: 738877 us.
29 39 -25 124 -29 -22 compute & print run time: 15.754 us.
Timer stopped. Total time: 8.4516e+06 us.

# interleaved(coro) -- 10.409 us
Timer started.
Enter 3 values: 1
1000
888
input 1 run time: 6.37685e+06 us.
Enter 3 values: 155
666
300
input 2 run time: 3.46264e+06 us.
init linked list 1 run time: 17.824 us.
init linked list 2 run time: 12.443 us.
1
input mode run time: 1.14565e+06 us.
coro init run time: 96.497 us.
29 124 39 -29 -25 -22 compute & print run time: 10.409 us.
Timer stopped. Total time: 1.09853e+07 us.
```