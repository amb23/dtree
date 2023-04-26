#pragma once

namespace dtree {

template <typename... Ts>
struct Overloaded : public Ts...
{
    using Ts::operator()...;
};

} // dtree
