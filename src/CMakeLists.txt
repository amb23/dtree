
add_library(dtree
    algos/multi_numeric.cpp
    impurity_measures.cpp
    labels.cpp
)

target_include_directories(dtree
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIR}
    ${fmt_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
)

target_link_libraries(dtree
    PRIVATE
    spdlog::spdlog
    Boost::serialization
)
