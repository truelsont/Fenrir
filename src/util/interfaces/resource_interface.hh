/*
The purpose of this class is to force compile-time checks for resource abstractions.
*/
#pragma once

#include "fenrir/util/Constants.hh"

namespace fenrir{
    class ResourceInterface{
    public:
        virtual ResourceCodes cleanup() = 0;

        virtual ~ResourceInterface(){

        }
    };
}