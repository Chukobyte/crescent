"""
Sanity check tests meant to be run by normal python.  These tests shouldn't require the engine.
For tests that do need the engine and embedded pocketpy, those should be added within the unit tests.
"""

from crescent import Node
from crescent_internal_py import _NodeEventManager


was_callback_called = False


def set_was_callback_called(value: bool) -> None:
    global was_callback_called
    was_callback_called = value


def test_callback_with_args(*args) -> None:
    x, y = args[0]
    assert x == 2
    assert y == 4


def test_node_event_manager() -> None:
    event_manager = _NodeEventManager()

    node = Node(0)

    # Test create event
    node_event = event_manager.create_event(node.entity_id, "moved")
    assert node_event
    assert event_manager.has_event(node_event.entity_id, "moved")
    # Test get event
    node_event = event_manager.get_event(node.entity_id, "moved")
    assert node_event
    # Test subscribe to event
    sub_node = Node(1)
    event_manager.subscribe_to_event(node.entity_id, "moved", sub_node.entity_id, lambda *args: set_was_callback_called(True))
    # Test broadcast event
    event_manager.broadcast_event(node_event.entity_id, "moved")
    global was_callback_called
    assert was_callback_called
    # Test delete
    event_manager.remove_entity_and_connections(sub_node.entity_id)
    was_callback_called = False
    event_manager.broadcast_event(node_event.entity_id, "moved")
    assert not was_callback_called
    # Testing broadcasting with params
    event_manager.subscribe_to_event(node.entity_id, "shake", sub_node.entity_id, test_callback_with_args)
    assert event_manager.has_event(node.entity_id, "shake")
    event_manager.broadcast_event(node.entity_id, "shake", 2, 4)
    # Test clear
    event_manager.clear_all_data()
    assert not event_manager.has_event(node.entity_id, "moved")
    assert not event_manager.has_event(node.entity_id, "shake")
    assert not event_manager.events
    assert not event_manager.entity_subscribers


test_node_event_manager()
