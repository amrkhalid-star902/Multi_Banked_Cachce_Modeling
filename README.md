# Increasing cache bandwidth
* Many real-time applications nowadays requires very low latency systems.
* Memory system are the main bottleneck in the process of reducing latency.
* The following modifications on cache system are done to increase bandwidth :
  
      Pipelining the access of the cache.
      Dividing the cache into multiple banks.
      Allowing multiple requests to be handled on a cache miss.


# Pipelining the cache access to increase bandwidth

* Pipelining the cache allows a higher clock frequency at a cost of increasing latency.
* The pipeline has four stages :

      Scheduling the next request.
      Tag compere.
      Data access.
      Handling the response back to the core.

# Pros and Cons of pipelined caches
## Advantages
* Higher frequency and bandwidth can be achieved.
* Higher associativity of the cache.

## Disadvantages
* Increasing the clock cycles count required to access the cache
* Higher branch miss penalty


