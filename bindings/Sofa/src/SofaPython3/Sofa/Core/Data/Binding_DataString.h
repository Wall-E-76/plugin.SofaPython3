#pragma once

#include <pybind11/pybind11.h>
#include <sofa/core/objectmodel/BaseData.h>

namespace sofapython3
{
/// Makes an alias for the pybind11 namespace to increase readability.
namespace py { using namespace pybind11; }
using sofa::core::objectmodel::BaseData;

void moduleAddDataString(py::module& m);

class DataString : public BaseData
{
public:
    py::str __str__();
    py::str __repr__();
    py::bool_ __eq__(BaseData* other);
    py::size_t __len__();
    py::object __getitem__(const py::size_t index);
};

}
