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


# Multi banked caches to increase bandwidth
* The cache is divided into multiple banks , where each bank is accessed simultaneously.
* This approach works well when access is spread across multiple banks which known as interleaving.
![image](https://github.com/amrkhalid-star902/Multi_Banked_Cachce_Modelling/assets/84184457/9dcd9e4c-7444-4661-bea5-9c52908e7384)


# Non blocking caches
* Blocking cache

  
  ![image](https://github.com/amrkhalid-star902/Multi_Banked_Cachce_Modelling/assets/84184457/4710880d-b983-49c2-ab4f-724a8c19a159)




* Non blocking cache (hit under miss)



![image](https://github.com/amrkhalid-star902/Multi_Banked_Cachce_Modelling/assets/84184457/144fe645-fb1d-42d6-9dbc-c4ca89492bae)



* Miss under Miss Cache
* It can handle multiple misses at the same time without stalling
* It stalls in the following cases :

      When there is not enough entries in the queue.
      When all instructions in the queue are waiting for there operands.



![image](https://github.com/amrkhalid-star902/Multi_Banked_Cachce_Modelling/assets/84184457/292cad6b-f3c3-4808-882d-41dd8b2db569)



# Challenges in implementing non-blocking caches
* The misses are handled out-of-order , for example the misses occur in order x-y-z but it can be handled in y-z-x.
* The cache continue serving load/store operations without waiting for miss to be handled , this can cause a problem of accessing pending address by a subsequent load/store operation.
*  A data structure called MSHR is used to keep track of different misses and forward the data to the waiting destinations.

# MSHR
* An MSHR, or Memory Request Queue, is a data structure used in modern computer architectures to handle memory requests, particularly in systems with a hierarchical memory organization, such as caches and memory hierarchies. It acts as an intermediary between the processor and the memory subsystem, allowing for efficient handling of memory accesses.

## Key Features and Functionality
* **Buffering of Memory Requests**: The MSHR provides a buffer to store memory requests from the processor. It acts as a temporary storage for pending requests, allowing the processor to continue executing instructions without waiting for memory accesses to complete.

* **Request Tagging**: Each memory request in the MSHR is associated with a unique tag or identifier that helps track the corresponding memory block. This tag allows the MSHR to match incoming memory responses from the memory subsystem with the original request, enabling proper data forwarding and coherence management.

* **Coalescing and Deduplication**: The MSHR can coalesce or merge multiple requests targeting the same memory block into a single request. This reduces redundant requests and optimizes memory bandwidth utilization by minimizing the number of memory transactions.

* **Handling Memory Responses**: When a memory response is received from the memory subsystem, the MSHR matches it with the original request using the associated tag. The MSHR then forwards the response to the processor and updates internal state information, such as marking the request as completed or initiating further actions if necessary.
