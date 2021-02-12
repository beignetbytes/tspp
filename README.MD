# TSPP General Use Timeseries Containers for CPP 
[![Build Status](https://travis-ci.com/beignetbytes/tspp.svg?branch=master)](https://travis-ci.com/beignetbytes/tspp)
[![codecov](https://codecov.io/gh/beignetbytes/tspp/branch/master/graph/badge.svg?token=1SW3GH14TF)](https://codecov.io/gh/beignetbytes/tspp)


# Project Overview


# Goals and Non-Goals



# Quick Start


# Examples


# Benchmark Performance



<br>

**System Specs**
|                 |        |  
|-----------------|------- |
| ***Processor*** | Intel Core i7-9750H @ 2.60GHz     |
| ***RAM***       | 16 gb DDR4   |
<br>

**Results**

To run on your machine compile: 

``

And then run **


<br>

# Features/TODO List


| Feature                                           | Support | Category            | Dependencies   | Compiler Version |  
|---------------------------------------------------|---------|---------------------|----------------|--------------|
| Time Filters                                      | ✔      | Core                 |                |        |
| Positional Indexing                               | ✔      | Core                 |                |        |
| Key Indexing                                      | ✔      | Core                 |                |        |
| Shifts                                            | ✔      | Core                 |                |        |
| Inner Join (Merge & Hash Join)                    | ✔      | Core                 |                |        |
| Left Join (Merge & Hash Join)                     | ✔      | Core                 |                |       |
| "As-Of" Join (Merge)                              |  ✔    | Core                 |                |        |
| Multiple Inner Join                               |        | Core                 |                |       |
| Concat/Interweave                                 | ✔      | Core                 |                |       |
| Time Aggregation                                  |  ✔     | Core                 |                |       |
| Time Aggregation Helpers with chrono index        | ✔      | Specializations      |                |       |
| Closure application (User Defined Functions)      | ✔      | Core                 |                |       |
| SIMD Support                                      | ✔      | Core                 |                |       |
| Native Null Filling/Interpolations                |        | Core                 |                |        |
| Buffer Based Moving Window Operations             | ✔      | Core                 |                |        |
| Update Based Moving Window Operations             |       | Core                 |                |        |
| "Skip" Operations (i.e. diff...etc.)              | ✔      | Core                 |                |     |
| C++ iterators                                     | ✔      | Core                 |                |     |
| Parquet IO*                                       | ✔      | IO                   | arrow/parquet  |    |
| Comprehensive Documentation                       |       | Meta                 |               |       |
| Test Coverage                                     |       | Meta                 |                |       |
| More Examples                                     |       | Meta                 |                |       |
<br>

Features marked "*" need additional performance tuning and perhaps a refactoring into a more generic framework. Note that although compatibility is only listed as Rust >=1.48, TSXLIB-RS might work with lower Rust versions as well it just has not been tested.


# License

Licensed under either of

 * [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)
 * [MIT license](http://opensource.org/licenses/MIT)

**Contributions**

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.