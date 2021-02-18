from typing import Callable
from collections import defaultdict

import misli
from misli.logging import BColors
from misli.change import Change
from misli.helpers import get_new_id, find_many_by_props, find_one_by_props

from . import components_lib
from .actions_lib import Action
from .base_view import View

log = misli.get_logger(__name__)

COMPONENTS_CHANNEL = '__components__'
misli.add_channel(COMPONENTS_CHANNEL)

_views = {}
_views_per_parent = {}

_added_views_per_parent = defaultdict(list)
_removed_views_per_parent = defaultdict(list)
_updated_views = []

_view_model_by_id = {}
_old_view_models = {}  # A bit redundant (view.last_state can be used)

_action_handlers = []
_actions_for_dispatch = []


# Action channel interface
def push_action(action: Action):
    args_str = ', '.join([str(a) for a in action.args])
    kwargs_str = ', '.join(['%s=%s' % (k, v)
                            for k, v in action.kwargs.items()])

    green = BColors.OKGREEN
    end = BColors.ENDC

    log.info(
        'Action %s%s %s%s ARGS=*(%s) KWARGS=**{%s}' %
        (green, action.run_state.name, action.name, end, args_str, kwargs_str))

    _actions_for_dispatch.append(action)
    misli.call_delayed(_handle_actions, 0)


# The first action state returned is the top-level action start state
# The rest of the states are nested in it and won't invoke a separate on_action
# By the same logic the last action returned is the Finished state of the
# top-level action
@log.traced
def on_action(handler: Callable):
    _action_handlers.append(handler)


# @log.traced
def _handle_actions():
    if not _actions_for_dispatch:
        return

    for handler in _action_handlers:
        handler([a.to_dict() for a in _actions_for_dispatch])

    _actions_for_dispatch.clear()


# Runtime component interface
@log.traced
def create_view(obj_class, *args, **kwargs):
    _view = components_lib.create_view(obj_class, *args, **kwargs)
    add_view(_view)
    return _view


def add_view(_view: View):
    _views[_view.id] = _view
    _view_model_by_id[_view.id] = _view.last_model
    _views_per_parent[_view.id] = []

    if _view.parent_id:
        if _view.parent_id not in _views_per_parent:
            raise Exception(
                'Cannot add component with missing parent with id: %s' %
                _view.parent_id)

        _added_views_per_parent[_view.parent_id].append(_view)
        _views_per_parent[_view.parent_id].append(_view)

    misli.call_delayed(_update_views, 0)


# @log.traced
def view(id: str):
    if id not in _views:
        return None
    return _views[id]


def view_model(view_id):
    if view_id not in _view_model_by_id:
        return None

    return _view_model_by_id[view_id].copy()


def view_children(view_id):
    return _views_per_parent[view_id]


@log.traced
def views():
    return [c for c_id, c in _views.items()]


@log.traced
def find_views(**props):
    return find_many_by_props(_views, **props)


@log.traced
def find_view(**props):
    return find_one_by_props(_views, **props)


@log.traced
def update_view_model(new_model):
    _view = view(new_model.id)
    old_model = _view.last_model
    _view_model_by_id[_view.id] = new_model

    if _view.id not in _old_view_models:
        _old_view_models[_view.id] = old_model

    if _view not in _updated_views:
        _updated_views.append(_view)

    misli.call_delayed(_update_views, 0)


def remove_view(_view: View):
    del _views_per_parent[_view.id]
    del _views[_view.id]

    if _view.parent_id:
        if _view.parent_id not in _views_per_parent:
            raise Exception(
                'Cannot add component with missing parent with id: %s' %
                _view.parent_id)

        _views_per_parent[_view.parent_id].remove(_view)
        _removed_views_per_parent[_view.parent_id].append(_view)

    misli.call_delayed(_update_views, 0)


# @log.traced
def _update_views():
    # (added, removed, updated)
    child_changes_per_parent_id = defaultdict(lambda: ([], [], []))

    for _view in _updated_views:
        old_state = _old_view_models[_view.id]
        new_state = view_model(_view.id)
        _view.update_cached_model(new_state)
        _view.handle_model_update(old_state, new_state)

        if _view.parent_id:
            child_changes_per_parent_id[_view.parent_id][2].append(
                _view)

    for parent_id, added in _added_views_per_parent.items():
        child_changes_per_parent_id[parent_id][0].extend(added)

    for parent_id, removed in _removed_views_per_parent.items():
        child_changes_per_parent_id[parent_id][1].extend(removed)

    _updated_views.clear()
    _added_views_per_parent.clear()
    _removed_views_per_parent.clear()
    _old_view_models.clear()

    for view_id, changes in child_changes_per_parent_id.items():
        _view = view(view_id)
        _view.handle_child_changes(*changes)