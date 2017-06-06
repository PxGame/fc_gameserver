#FC_GameServer

#build step

- Install all the needed build tools and libraries

```
sudo apt-get install g++ git make zlib1g-dev libboost-all-dev libssl-dev cmake
```

- install cpprestsdk

```
sudo apt-get install libcpprest-dev
```

- set project to using c++11 and include library

```
//CMakeLists.txt

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries(${PROJECT_NAME} pthread boost_system crypto ssl cpprest)
```

- install QT

```
sudo apt-get install cmake qt5-default qtcreator
```

