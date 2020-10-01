#
# Tests the Analyser class bindings
#
import unittest


class AnalyserTestCase(unittest.TestCase):
    VALUE = 'value'

    def test_create_destroy(self):
        from libcellml import Analyser

        x = Analyser()
        del x

    def test_inheritance(self):
        import libcellml
        from libcellml import Analyser

        x = Analyser()
        self.assertIsInstance(x, libcellml.logger.Logger)

        # Test access to inherited methods.
        self.assertIsNone(x.issue(0))
        self.assertIsNone(x.issue(-1))
        self.assertEqual(x.issueCount(), 0)
        x.addIssue(libcellml.Issue())
        self.assertEqual(x.issueCount(), 1)

    def test_analyse_model(self):
        from libcellml import Analyser
        from libcellml import AnalyserModel
        from libcellml import Model

        # Analyse an empty model and make sure that we get no errors and an
        # UNKNOWN type for the analyser model.

        m = Model('my_model')
        a = Analyser()

        a.analyseModel(m)

        self.assertEqual(0, a.errorCount())
        self.assertEqual(AnalyserModel.Type.UNKNOWN, a.model().type())

    def test_coverage(self):
        from libcellml import Analyser
        from libcellml import AnalyserEquation
        from libcellml import AnalyserEquationAst
        from libcellml import AnalyserModel
        from libcellml import AnalyserVariable
        from libcellml import Parser
        from test_resources import file_contents

        # Try to create an analyser equation/model/variable, something that is not allowed.

        self.assertRaises(AttributeError, AnalyserEquation)
        self.assertRaises(AttributeError, AnalyserModel)
        self.assertRaises(AttributeError, AnalyserVariable)

        # Analyse a model, so we can then do some coverage.

        p = Parser()
        m = p.parseModel(file_contents('generator/noble_model_1962/model.cellml'))

        a = Analyser()
        a.analyseModel(m)

        # Ensure coverage for AnalyserModel.

        am = a.model()

        self.assertTrue(am.isValid())

        self.assertIsNotNone(am.voi())

        self.assertEqual(4, am.stateCount())
        self.assertIsNotNone(am.states())
        self.assertIsNotNone(am.state(3))

        self.assertEqual(17, am.variableCount())
        self.assertIsNotNone(am.variables())
        self.assertIsNotNone(am.variable(3))

        self.assertEqual(16, am.equationCount())
        self.assertIsNotNone(am.equations())
        self.assertIsNotNone(am.equation(3))

        self.assertFalse(am.needEqFunction())
        self.assertFalse(am.needNeqFunction())
        self.assertFalse(am.needLtFunction())
        self.assertFalse(am.needLeqFunction())
        self.assertFalse(am.needGtFunction())
        self.assertFalse(am.needGeqFunction())
        self.assertFalse(am.needAndFunction())
        self.assertFalse(am.needOrFunction())
        self.assertFalse(am.needXorFunction())
        self.assertFalse(am.needNotFunction())
        self.assertFalse(am.needMinFunction())
        self.assertFalse(am.needMaxFunction())
        self.assertFalse(am.needSecFunction())
        self.assertFalse(am.needCscFunction())
        self.assertFalse(am.needCotFunction())
        self.assertFalse(am.needSechFunction())
        self.assertFalse(am.needCschFunction())
        self.assertFalse(am.needCothFunction())
        self.assertFalse(am.needAsecFunction())
        self.assertFalse(am.needAcscFunction())
        self.assertFalse(am.needAcotFunction())
        self.assertFalse(am.needAsechFunction())
        self.assertFalse(am.needAcschFunction())
        self.assertFalse(am.needAcothFunction())

        # Ensure coverage for AnalyserVariable.

        av = am.variable(3)

        self.assertEqual(AnalyserVariable.Type.CONSTANT, av.type())
        self.assertEqual(3, av.index())
        self.assertIsNotNone(av.initialisingVariable())
        self.assertIsNotNone(av.variable())
        self.assertIsNotNone(av.equation())

        # Ensure coverage for AnalyserEquation.

        ae = am.equation(3)

        self.assertEqual(AnalyserEquation.Type.RATE, ae.type())
        self.assertIsNotNone(ae.ast())
        self.assertIsNotNone(ae.dependencies())
        self.assertTrue(ae.isStateRateBased())
        self.assertIsNotNone(ae.variable())

        # Ensure coverage for AnalyserEquationAst.

        aea = ae.ast()

        self.assertEqual(AnalyserEquationAst.Type.ASSIGNMENT, aea.type())
        self.assertEqual('', aea.value())
        self.assertIsNone(aea.variable())
        self.assertIsNone(aea.parent())
        self.assertIsNotNone(aea.leftChild())
        self.assertIsNotNone(aea.rightChild())

        aea.setType(AnalyserEquationAst.Type.EQ)
        aea.setValue(AnalyserTestCase.VALUE)
        aea.setVariable(av.variable())
        aea.setParent(aea)
        aea.setLeftChild(None)
        aea.setRightChild(None)

        self.assertEqual(AnalyserEquationAst.Type.EQ, aea.type())
        self.assertEqual(AnalyserTestCase.VALUE, aea.value())
        self.assertIsNotNone(aea.variable())
        self.assertIsNotNone(aea.parent())
        self.assertIsNone(aea.leftChild())
        self.assertIsNone(aea.rightChild())


if __name__ == '__main__':
    unittest.main()