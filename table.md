
# Producer-Consumer Performance Test Results

This file contains the results from the producer-consumer multi-threaded experiment with varying numbers of producers.
The experiment was executed on a 16-threaded machine.

### 1 Producer
| # Consumers | Time per Consumer (μs) |
|-------------|------------------------|
| 1           | 225                    |
| 2           | 107                    |
| 3           | 87                     |
| 4           | 85                     |
| 5           | 84                     |
| 6           | 84                     |
| 7           | 83                     |
| 8           | 83                     |
| 9           | 83                     |
| 10          | 83                     |
| ...         | 83                     |
| 1000        | 83                     |

### 4 Producers
| # Consumers | Time per Consumer (μs) |
|-------------|------------------------|
| 1           | 767                    |
| 2           | 681                    |
| 3           | 590                    |
| 4           | 394                    |
| 5           | 348                    |
| 6           | 314                    |
| 7           | 298                    |
| 8           | 247                    |
| 9           | 237                    |
| 10          | 238                    |
| ...         | 237                    |
| 1000        | 237                    |

### 8 Producers
| # Consumers | Time per Consumer (μs) |
|-------------|------------------------|
| 1           | 931                    |
| 2           | 885                    |
| 3           | 881                    |
| 4           | 869                    |
| 5           | 828                    |
| 6           | 766                    |
| 7           | 744                    |
| 8           | 642                    |
| 9           | 559                    |
| 10          | 544                    |
| 11          | 496                    |
| 12          | 483                    |
| 13          | 478                    |
| 14          | 465                    |
| 15          | 467                    |
| 16          | 465                    |
| 1000        | 465                    |

---

These results show the performance of the producer-consumer model with different numbers of producers and consumers. As the number of producers increases, we see a clear reduction in latency per consumer as the workload is better distributed.