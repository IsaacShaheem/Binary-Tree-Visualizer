import unittest

from backend.app import create_app


class FlaskApiTests(unittest.TestCase):
    def setUp(self):
        self.app = create_app()
        self.client = self.app.test_client()

    def tearDown(self):
        for service in self.app.config["TREE_SERVICES"].values():
            service.close()

    def test_insert_and_reset(self):
        response = self.client.post("/api/insert", json={"value": 30})
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.get_json()["tree"]["key"], 30)

        response = self.client.post("/api/reset")
        self.assertEqual(response.status_code, 200)
        self.assertIsNone(response.get_json()["tree"])

    def test_insert_validates_integer(self):
        response = self.client.post("/api/insert", json={"value": "30"})
        self.assertEqual(response.status_code, 400)

    def test_delete_updates_tree(self):
        for value in [10, 5, 15]:
            response = self.client.post("/api/insert", json={"value": value})
            self.assertEqual(response.status_code, 200)

        response = self.client.post("/api/delete", json={"value": 5})
        self.assertEqual(response.status_code, 200)
        payload = response.get_json()
        self.assertTrue(payload["deleted"])
        self.assertIsNone(payload["tree"]["left"])
        self.assertEqual(payload["tree"]["right"]["key"], 15)

    def test_delete_nonexistent_and_validation(self):
        response = self.client.post("/api/delete", json={"value": 999})
        self.assertEqual(response.status_code, 200)
        self.assertFalse(response.get_json()["deleted"])

        response = self.client.post("/api/delete", json={"value": "999"})
        self.assertEqual(response.status_code, 400)

    def test_tree_type_query_param_selects_bst_and_avl(self):
        response = self.client.post("/api/insert?type=bst", json={"value": 10})
        self.assertEqual(response.status_code, 200)

        response = self.client.post("/api/insert?type=avl", json={"value": 20})
        self.assertEqual(response.status_code, 200)

        bst_response = self.client.get("/api/tree?type=bst")
        avl_response = self.client.get("/api/tree?type=avl")

        self.assertEqual(bst_response.get_json()["tree"]["key"], 10)
        self.assertEqual(avl_response.get_json()["tree"]["key"], 20)

    def test_invalid_tree_type_returns_400(self):
        response = self.client.get("/api/tree?type=heap")
        self.assertEqual(response.status_code, 400)

    def test_avl_insert_reports_rotation_metadata(self):
        for value in [10, 20]:
            response = self.client.post("/api/insert?type=avl", json={"value": value})
            self.assertEqual(response.status_code, 200)
            self.assertIsNone(response.get_json()["meta"]["rotation"])

        response = self.client.post("/api/insert?type=avl", json={"value": 30})
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.get_json()["meta"]["rotation"], "RR")


if __name__ == "__main__":
    unittest.main()
