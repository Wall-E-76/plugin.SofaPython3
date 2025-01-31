# coding: utf8

import unittest
import Sofa
import Sofa.Core
import Sofa.Helper
import SofaRuntime
from MyRestShapeForceField import *
from numpy.linalg import norm as np_norm


def createIntegrationScheme(node, use_implicit_scheme):
    if use_implicit_scheme is True:
        node.addObject('EulerImplicitSolver', name='odeImplicitSolver',
                       rayleighStiffness='0.1', rayleighMass='0.1')
    else:
        node.addObject('EulerExplicitSolver', name='odeExplicitSolver')


def createSolver(node, use_iterative_solver):
    if use_iterative_solver == True:
        node.addObject('CGLinearSolver', name='linearSolver',
                       iterations=30, tolerance=1.e-9, threshold=1.e-9)
    else:
        SofaRuntime.importPlugin("SofaSparseSolver")
        node.addObject('SparseLDLSolver', name='ldlSolver')


def createParticle(node, node_name, use_implicit_scheme, use_iterative_solver):
    p = node.addChild(node_name)
    createIntegrationScheme(p, use_implicit_scheme)
    createSolver(p, use_iterative_solver)
    dofs = p.addObject('MechanicalObject', name="MO", position=[0, 0, 0])
    p.addObject('UniformMass', totalMass=1.0)

    print ("dofs.rest_position " + str(dofs.rest_position.value))
    myRSSFF = NaiveRestShapeSpringsForcefield(name="Springs",
                                           stiffness=50,
                                           mstate=dofs, rest_pos=dofs.rest_position)
    p.addObject(myRSSFF)


def rssffScene(use_implicit_scheme=True, use_iterative_solver=True):
    SofaRuntime.importPlugin("SofaAllCommonComponents")
    node = Sofa.Core.Node("root")
    node.gravity = [0, -10, 0]
    createParticle(node, "particle", use_implicit_scheme, use_iterative_solver)
    return node


class Test(unittest.TestCase):
    def test_0_explicit(self):
        root = rssffScene(use_implicit_scheme=False,
                          use_iterative_solver=False)

        # do some steps here
        Sofa.Simulation.init(root)

        for i in range(0, 100):
            Sofa.Simulation.animate(root, root.dt.value)
            print(root.particle.MO.position.value)
            self.assertLess(np_norm(
                root.particle.MO.rest_position.value - root.particle.MO.position.value), 0.41,
                "Passed threshold on step " + str(i) + ".")
        return

    def test_1_implicit_iterative(self):
        root = rssffScene(use_implicit_scheme=True, use_iterative_solver=True)

        # do some steps here
        Sofa.Simulation.init(root)

        for i in range(0, 100):
            Sofa.Simulation.animate(root, root.dt.value)
            print(root.particle.MO.position.value)
            self.assertLess(np_norm(
                root.particle.MO.rest_position.value - root.particle.MO.position.value), 0.26,
                "Passed threshold on step " + str(i) + ".")
        return

    def test_2_implicit_direct(self):
        root = rssffScene(use_implicit_scheme=True, use_iterative_solver=False)

        # do some steps here
        Sofa.Simulation.init(root)

        for i in range(0, 100):
            Sofa.Simulation.animate(root, root.dt.value)
            print(root.particle.MO.position.value)
            self.assertLess(np_norm(
                root.particle.MO.rest_position.value - root.particle.MO.position.value), 0.26,
                "Passed threshold on step " + str(i) + ".")
        return

    def test_animation(self):
        node = Sofa.Core.Node("TestAnimation")
        node.addObject("RequiredPlugin", name="SofaOpenglVisual")
        node.addObject("RequiredPlugin", name="SofaSparseSolver")
        node.addObject("OglLineAxis")
        node.addObject("DefaultAnimationLoop", name="loop")
        node.addObject("EulerImplicit")
        node.addObject("CGLinearSolver", tolerance=1e-12, threshold=1e-12)
        # node.addObject("SparseLDLSolver")

        #object1.addChild( MyForceField("customFF", ks=5.0) )
        a = node.addChild(RestShapeObject(MyForceField(
            "customFF", ks=5.0), name="python", position=[[i*1-10.0, 0, 0] for i in range(200)]))
        a.mechanical.showColor = [1.0, 0.0, 0.0, 1.0]
        b = node.addChild(RestShapeObject(CreateObject("RestShapeSpringsForceField",
                                                       stiffness=5.0), name="c++", position=[[i*0.5-1.0, 0, 0] for i in range(1)]))
        b.mechanical.showColor = [1.0, 1.0, 0.0, 1.0]

        return node
