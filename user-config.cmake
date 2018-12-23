#########################################################
# You have to give some path to the project to make it  #
# compile.                                              #
#                                                       #
# Here is some predefined path depending on your system #
# and your compiler. Uncomment what you will use and    #
# check they are pointing the right path.               #
#########################################################

#-------------------------------------------------------#
#                      WINDOWS                          #
#-------------------------------------------------------#
### 64bit (x64) ###
# set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.2\\msvc2017_64\\") # <Set Path to Qt>
# set (OPENSSL_DIR "path to openssl") # <OTIONAL : Set the past to custom OpenSSL version>

### 32bit (x86) ###
# set (CMAKE_PREFIX_PATH "C:\\Qt\\5.10.2\\msvc2015\\") # <Set Path to Qt>
# set (OPENSSL_DIR "path to openssl") # <OTIONAL : Set the past to custom OpenSSL version>

#-------------------------------------------------------#
#                      macOS                            #
#-------------------------------------------------------#
# set (CMAKE_PREFIX_PATH "/Users/feldrise/Qt/5.11.2/clang_64/") # <Set Path to Qt>
# set (OPENSSL_DIR "path to openssl") # <OTIONAL : Set the past to custom OpenSSL version>

#-------------------------------------------------------#
#                      Linux                            # 
#-------------------------------------------------------#
# set (CMAKE_PREFIX_PATH "path/to/qt") # <Set Path to Qt>
# set (OPENSSL_DIR "path to openssl") # <OTIONAL : Set the past to custom OpenSSL version>

#-------------------------------------------------------#
#                  Compiler Option                      #
#-------------------------------------------------------#
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # <Enable some flags for GNU compiler>