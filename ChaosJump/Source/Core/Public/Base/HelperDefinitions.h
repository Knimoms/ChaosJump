#pragma once

#define DECLARE_DEFAULT_DELETER(type) \
struct type##Deleter \
{\
    void operator()(type##* raw##type) const;\
};\

#define DEFINE_DEFAULT_DELETER(type) \
void type##Deleter::operator()(type##* raw##type) const \
{ \
    delete raw##type; \
}