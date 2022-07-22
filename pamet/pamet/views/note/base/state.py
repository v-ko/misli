from dataclasses import field

import pamet
from misli.gui import view_state_type, ViewState
from pamet.model import Note


@view_state_type
class NoteViewState(ViewState, Note):
    note_gid: str = ''
    badges: list = field(default_factory=list, init=False, repr=False)

    def __post_init__(self):
        if not self.note_gid:
            raise Exception('All note views should have a mapped note.')

    def update_from_note(self, note: Note):
        self.replace(**note.asdict())

    def get_note(self):
        return pamet.find_one(gid=self.note_gid)
