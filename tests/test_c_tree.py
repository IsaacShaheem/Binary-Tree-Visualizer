import unittest

from backend.c_tree import TreeEngine


def assert_avl_invariants(test_case, node):
    if node is None:
        return -1

    left_height = assert_avl_invariants(test_case, node["left"])
    right_height = assert_avl_invariants(test_case, node["right"])
    expected_height = max(left_height, right_height) + 1
    expected_balance = left_height - right_height

    test_case.assertEqual(node["height"], expected_height)
    test_case.assertEqual(node["balance"], expected_balance)
    test_case.assertLessEqual(abs(expected_balance), 1)
    return expected_height


class TreeEngineTests(unittest.TestCase):
    def test_avl_ll_rotation_exports_balanced_root(self):
        with TreeEngine("avl") as tree:
            tree.insert(30)
            tree.insert(20)
            tree.insert(10)

            snapshot = tree.export()

        self.assertEqual(snapshot["key"], 20)
        self.assertEqual(snapshot["balance"], 0)
        self.assertEqual(snapshot["left"]["key"], 10)
        self.assertEqual(snapshot["right"]["key"], 30)

    def test_bst_delete_two_child_node(self):
        with TreeEngine("bst") as tree:
            for value in [8, 4, 12, 10, 14]:
                self.assertTrue(tree.insert(value))

            self.assertTrue(tree.delete(12))

            self.assertFalse(tree.search(12))
            self.assertTrue(tree.search(10))
            self.assertTrue(tree.search(14))

    def test_bst_delete_leaf_one_child_root_and_missing(self):
        with TreeEngine("bst") as tree:
            for value in [8, 4, 12, 10]:
                self.assertTrue(tree.insert(value))

            self.assertTrue(tree.delete(10))
            self.assertFalse(tree.search(10))

            self.assertTrue(tree.delete(12))
            self.assertFalse(tree.search(12))

            self.assertTrue(tree.delete(8))
            self.assertFalse(tree.search(8))

            self.assertFalse(tree.delete(999))

    def test_avl_delete_leaf_one_child_two_child_root_and_missing(self):
        with TreeEngine("avl") as tree:
            for value in [20, 10, 30, 5, 15, 25, 40, 22]:
                self.assertTrue(tree.insert(value))

            self.assertTrue(tree.delete(5))
            self.assertFalse(tree.search(5))

            self.assertTrue(tree.delete(25))
            self.assertFalse(tree.search(25))
            self.assertTrue(tree.search(22))

            self.assertTrue(tree.delete(30))
            self.assertFalse(tree.search(30))
            self.assertTrue(tree.search(40))

            current_root = tree.export()["key"]
            self.assertTrue(tree.delete(current_root))
            self.assertFalse(tree.search(current_root))

            self.assertFalse(tree.delete(999))
            assert_avl_invariants(self, tree.export())

    def test_avl_delete_rebalances_ll_rr_lr_rl_shapes(self):
        cases = [
            ([30, 20, 40, 10, 25], 40),
            ([20, 10, 30, 25, 40], 10),
            ([50, 30, 70, 40], 70),
            ([50, 30, 70, 60], 30),
        ]

        for values, deleted_value in cases:
            with self.subTest(values=values, deleted_value=deleted_value):
                with TreeEngine("avl") as tree:
                    for value in values:
                        self.assertTrue(tree.insert(value))

                    self.assertTrue(tree.delete(deleted_value))
                    assert_avl_invariants(self, tree.export())


if __name__ == "__main__":
    unittest.main()
