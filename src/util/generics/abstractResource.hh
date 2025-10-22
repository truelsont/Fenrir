/*
The purpose of this class is to force compile-time checks for resource abstractions.
*/

#pragma once

#include "fenrir/util/Constants.hh"

namespace fenrir::abstract{
    class AbstractResource {
    public:
        
        virtual ResourceCodes cleanup() = 0;

        virtual ~AbstractResource(){

        }
    };
}