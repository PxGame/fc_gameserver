#FC_GameServer

#build step

- Install all the needed build tools and libraries

```
sudo apt-get install g++ git make zlib1g-dev libboost-all-dev libssl-dev cmake
sudo apt-get install libspdlog-dev
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

- install mysql
```
sudo apt-get install mysql-server
sudo apt-get install mysql-client
sudo apt-get install libmysqlcppconn-dev 
```

- generic

```
cmake -std=c++11 -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
make

//build
-DCMAKE_BUILD_TYPE=Debug
-DCMAKE_BUILD_TYPE=Release
```

- ssl

```
openssl genrsa -aes256 -out ca.key 1024
//pass phrase for ca.key:a19940318

openssl req -new -x509 -key ca.key -out ca.crt

```

