/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <vector>
#include <string>

/// This should come from python3.
#include <Python.h>

#include <sofa/simulation/SceneLoaderFactory.h>

#include <pybind11/pybind11.h>

#include "config.h"

namespace sofapython3
{
namespace py { using namespace pybind11; }
using sofa::simulation::SceneLoader ;

/// Forward definition
class PythonEnvironmentData ;

class SOFAPYTHON3_API PythonEnvironment
{
public:
    static void Init();
    static void Release();

    static py::module SOFAPYTHON3_API importFromFile(const std::string& module,
                                     const std::string& path,
                                     py::object& globals);

    /// Add a path to sys.path, the list of search path for Python modules.
    static void addPythonModulePath(const std::string& path);

    /// Add each line of a file to sys.path
    static void addPythonModulePathsFromConfigFile(const std::string& path);

    /// Add all the directories matching <pluginsDirectory>/*/python to sys.path
    /// NB: can also be used for projects <projectDirectory>/*/python
    static void addPythonModulePathsForPlugins(const std::string& pluginsDirectory);

    /// set the content of sys.argv.
    static void setArguments(const std::string& filename,
                             const std::vector<std::string>& arguments=std::vector<std::string>(0));

    /// basic script functions
    static std::string  getError();
    static bool         runString(const std::string& script);
    static bool         runFile(const std::string& filename,
                                const std::vector<std::string>& arguments=std::vector<std::string>(0) );

    /// returns the file information associated with the current frame.
    static std::string getStackAsString() ;

    /// returns the last entry in the stack so that we can provide information to user.
    static std::string getPythonCallingPointString() ;

    /// returns the calling point as a file info structure to be used with the message api.
    static sofa::helper::logging::FileInfo::SPtr getPythonCallingPointAsFileInfo() ;

    /// should the future scene loadings reload python modules?
    static void setAutomaticModuleReload( bool );

    /// excluding a module from automatic reload
    static void excludeModuleFromReload( const std::string& moduleName );

    /// to be able to react when a scene is loaded
    struct SceneLoaderListerner : public SceneLoader::Listener
    {
        /// possibly unload python modules to force importing their eventual modifications
        virtual void rightBeforeLoadingScene();
        static SceneLoaderListerner* getInstance() {
            static SceneLoaderListerner sceneLoaderListerner;
            return &sceneLoaderListerner; }
        /// singleton
    private:
        SceneLoaderListerner(){}
    };

    /// use this RAII-class to ensure the gil is properly acquired and released
    /// in a scope. these should be surrounding any python code called from c++,
    /// i.e. in all the methods in PythonEnvironment and all the methods in
    /// PythonScriptController.
    using gil = pybind11::gil_scoped_acquire;
//    class SOFAPYTHON3_API gil {
//        const PyGILState_STATE state;
//        const char* const trace;
//    public:
//        gil(const char* trace = nullptr);
//        ~gil();
//    };

    class SOFAPYTHON3_API no_gil {
        PyThreadState* const state;
        const char* const trace;
    public:
        no_gil(const char* trace = nullptr);
        ~no_gil();
    };

    struct system_exit : std::exception { };

private:
    static PythonEnvironmentData* getStaticData() ;
};

} // namespace sofapython3

