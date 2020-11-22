from misli.gui import components_lib

from .browser_window import BrowserWindow
from .browser_tab import BrowserTabComponent
from ..map_page.qt_component import MapPageQtComponent

from ..notes.text.qt_component import TextNoteQtComponent
from ..notes.text.note_edit_qt_component import TextNoteEditQtComponent


components_lib.add('BrowserWindow', BrowserWindow)
components_lib.add('BrowserTab', BrowserTabComponent)
components_lib.add('MapPage', MapPageQtComponent)

components_lib.add('Text', TextNoteQtComponent)
components_lib.add('Redirect', TextNoteQtComponent)

components_lib.add('TextEdit', TextNoteEditQtComponent)
components_lib.map_edit_component('Text', 'TextEdit')
components_lib.map_edit_component('Redirect', 'TextEdit')
