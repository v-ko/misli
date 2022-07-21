from pathlib import Path
from PySide6.QtCore import QRect, QSize, Qt
from PySide6.QtGui import QColor, QImage, QPaintEvent, QPainter, QPixmap, QResizeEvent
from PySide6.QtWidgets import QLabel
from misli.entity_library.change import Change
from misli.gui.utils.qt_widgets import bind_and_apply_state
from misli.gui.view_library.view_state import view_state_type
from misli.logging import get_logger

from pamet import register_note_view_type
from pamet.helpers import Url
from pamet.model.image_note import ImageNote
from pamet.views.note.base_note_view import NoteView, NoteViewState
from pamet.views.note.image.image_label import ImageLabel
from pamet.views.note.qt_helpers import draw_link_decorations

log = get_logger(__name__)


@view_state_type
class ImageNoteViewState(NoteViewState, ImageNote):
    pass


@register_note_view_type(state_type=ImageNoteViewState,
                         note_type=ImageNote,
                         edit=False)
class ImageNoteWidget(ImageLabel, NoteView):

    def __init__(self, parent, initial_state):
        ImageLabel.__init__(self, parent)
        NoteView.__init__(self, initial_state)

        bind_and_apply_state(self, initial_state, self.on_state_change)

    def on_state_change(self, change: Change):
        state: ImageNoteViewState = change.last_state()

        if change.updated.color or change.updated.background_color:
            fg_col = QColor(*state.get_color().to_uint8_rgba_list())
            bg_col = QColor(*state.get_background_color().to_uint8_rgba_list())

            palette = self.palette()
            palette.setColor(self.backgroundRole(), bg_col)
            palette.setColor(self.foregroundRole(), fg_col)
            self.setPalette(palette)

        if change.updated.local_image_url:
            local_url: Url = state.local_image_url
            self.update_image_cache(local_url)
        if change.updated.geometry:
            self.setGeometry(QRect(*state.geometry))

    def paintEvent(self, event: QPaintEvent):
        painter = QPainter()
        draw_link_decorations(self, painter)