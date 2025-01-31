#pragma once

#include <pybind11/pybind11.h>

#include <sofa/core/objectmodel/BaseObject.h>

#include "Binding_Base.h"

template class pybind11::class_<sofa::core::objectmodel::BaseObject,
                                sofa::core::objectmodel::Base,
                                sofa::core::sptr<sofa::core::objectmodel::BaseObject>>;

namespace sofapython3
{

    using sofa::core::objectmodel::BaseObject;
    py::object getItem(const BaseObject& self, const std::string& path);
    void moduleAddBaseObject(py::module &m);
} /// namespace sofapython


