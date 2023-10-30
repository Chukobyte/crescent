#pragma once

#define CRE_PYPP_CRESCENT_SOURCE ""\
"from typing import List\n"\
"\n"\
"import crescent_internal\n"\
"\n"\
"\n"\
"class Node:\n"\
"    def __init__(self, entity_id: int):\n"\
"        self.entity_id = entity_id\n"\
"\n"\
"    def get_name(self) -> str:\n"\
"        return crescent_internal.node_get_name(self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def name(self) -> str:\n"\
"        return crescent_internal.node_get_name(self.entity_id)\n"\
"\n"\
"    def get_children(self) -> List[\"Node\"]:\n"\
"        children_ids = crescent_internal.node_get_children(self.entity_id)\n"\
"        # TODO: Get nodes from internal global dict\n"\
"        children = []\n"\
"        for child_id in children_ids:\n"\
"            children.append(Node(child_id))\n"\
"        return children\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        return self.entity_id == other.entity_id\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"Node(entity_id: {self.entity_id})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"Node(entity_id: {self.entity_id})\"\n"\
"\n"

#define CRE_PYPP_CRESCENT_INTERNAL_PY_SOURCE ""\
"from typing import Dict\n"\
"\n"\
"from crescent import Node\n"\
"\n"\
"CRE_ENTITY_TO_NODE_MAP: Dict[int, Node] = {}\n"\
"\n"\
"\n"\
"def get_entity(entity_id: int) -> Node:\n"\
"    return CRE_ENTITY_TO_NODE_MAP[entity_id]\n"\
"\n"\
"\n"\
"def set_entity(node: Node) -> None:\n"\
"    CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node\n"\
"\n"

