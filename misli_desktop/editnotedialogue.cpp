/*  This file is part of Misli.

    Misli is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Misli is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Misli.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "editnotedialogue.h"
#include "ui_editnotedialogue.h"

#include "misliwindow.h"
#include "../canvas.h"
#include "mislidesktopgui.h"

EditNoteDialogue::EditNoteDialogue(MisliDesktopGui * misli_dg_) :
    linkMenu(this),
    chooseNFMenu(tr("NoteFile"),&linkMenu),
    actionChooseTextFile(tr("Text file (in beta)"),&linkMenu),
    actionChoosePicture(tr("Picure (in beta)"),&linkMenu),
    actionSystemCallNote(tr("System call note (very beta)"),&linkMenu),
    ui(new Ui::EditNoteDialogue)
{
    ui->setupUi(this);
    misli_dg=misli_dg_;
    addAction(ui->actionEscape);

    linkMenu.addMenu(&chooseNFMenu);
    linkMenu.addAction(&actionChoosePicture);
    linkMenu.addAction(&actionChooseTextFile);
    linkMenu.addAction(&actionSystemCallNote);

    chooseNFMenuIsOpenedFromEditNoteDialogue = false;

    connect(&chooseNFMenu,SIGNAL(aboutToShow()),this,SLOT(updateChooseNFMenu()));
    connect(ui->makeLinkButton,SIGNAL(clicked()),this,SLOT(show_link_menu()));
    connect(&chooseNFMenu,SIGNAL(triggered(QAction*)),this,SLOT(make_link_note(QAction*)));
    //connect(&linkMenu,SIGNAL(aboutToHide()),this,SLOT(resetChooseNFMenuFlag()));
    connect(&actionChoosePicture,SIGNAL(triggered()),this,SLOT(choose_picture()));
    connect(&actionChooseTextFile,SIGNAL(triggered()),this,SLOT(choose_text_file()));
    connect(&actionSystemCallNote,SIGNAL(triggered()),this,SLOT(set_system_call_prefix()));
}

MisliInstance * EditNoteDialogue::misli_i()
{
    return misli_dg->misli_i;
}

EditNoteDialogue::~EditNoteDialogue()
{
    delete ui;
}

void EditNoteDialogue::new_note()
{
    setWindowTitle(tr("Make new note"));

    x_on_new_note=misli_dg->misli_w->canvas->current_mouse_x; //cursor position relative to the gl widget
    y_on_new_note=misli_dg->misli_w->canvas->current_mouse_y;

    move(QCursor::pos());

    ui->textEdit->setText("");
    edited_note=NULL;

    show();
    raise();
    activateWindow();
    ui->textEdit->setFocus(Qt::ActiveWindowFocusReason);
}

int EditNoteDialogue::edit_note(){ //false for new note , true for edit

    QString text;

    setWindowTitle(tr("Edit note"));

    edited_note=misli_i()->curr_misli_dir()->curr_nf()->get_first_selected_note();
    if(edited_note==NULL){return 1;}

    move(QCursor::pos());

    text=edited_note->text;
    set_textEdit_text(text);

    show();
    raise();
    activateWindow();
    ui->textEdit->setFocus(Qt::ActiveWindowFocusReason);

    return 0;
}

void EditNoteDialogue::input_done()
{
    QString text = ui->textEdit->toPlainText().trimmed();
    text = text.replace("\r\n","\n"); //for the f-n windows standart

    Note null_note;

    float x,y;
    Note *nt;
    float txt_col[] = {0,0,1,1};
    float bg_col[] = {0,0,1,0.1};

    if( edited_note==NULL){//If we're making a new note
        misli_dg->misli_w->canvas->unproject(x_on_new_note,y_on_new_note,x,y); //get mouse pos in real coordinates
        nt=misli_i()->curr_misli_dir()->curr_nf()->add_note(text,x,y,null_note.z,null_note.a,null_note.b,null_note.font_size,QDateTime::currentDateTime(),QDateTime::currentDateTime(),txt_col,bg_col);
        nt->auto_size();
        nt->link_to_selected();
    }else {//else we're in edit mode
        x=edited_note->x;
        y=edited_note->y;
        if(edited_note->text!=text){
            edited_note->text=text;
            edited_note->t_mod=QDateTime::currentDateTime();
        }

        //font,color,etc
        edited_note->init();
    }
    misli_i()->curr_misli_dir()->curr_nf()->save();
    misli_dg->misli_w->update_current_nf();
    misli_dg->edit_w->close();

    edited_note=NULL;
}

void EditNoteDialogue::make_link_note(QAction *act)
{
    //FIXME tova e hack around
    if(this->isVisible()){
        ui->textEdit->setText("this_note_points_to:"+act->text());
        ui->textEdit->setFocus();
        ui->textEdit->moveCursor (QTextCursor::End);
        this->hide();//hacks all the way
        this->show();
    }else{
        misli_i()->curr_misli_dir()->set_current_note_file(act->text());
    }
}
void EditNoteDialogue::set_textEdit_text(QString text)
{
    ui->textEdit->setPlainText(text);
}

void EditNoteDialogue::updateChooseNFMenu()
{
    chooseNFMenu.clear();

    for(unsigned int i=0;i<misli_i()->curr_misli_dir()->note_file.size();i++){
        chooseNFMenu.addAction(misli_i()->curr_misli_dir()->note_file[i]->name);
    }
}

void EditNoteDialogue::show_link_menu()
{
    linkMenu.popup(cursor().pos());
}

void EditNoteDialogue::choose_picture()
{
    QFileDialog dialog;
    QString file = dialog.getOpenFileName(this,tr("Choose a picture"));

    ui->textEdit->setText("define_picture_note:"+file);
    ui->textEdit->setFocus();
    ui->textEdit->moveCursor (QTextCursor::End);
}
void EditNoteDialogue::choose_text_file()
{
    QFileDialog dialog;
    QString file = dialog.getOpenFileName(this,tr("Choose a picture"));

    ui->textEdit->setText("define_text_file_note:"+file);
    ui->textEdit->setFocus();
    ui->textEdit->moveCursor (QTextCursor::End);
}
void EditNoteDialogue::set_system_call_prefix()
{
    ui->textEdit->setText("define_system_call_note:");
    ui->textEdit->setFocus();
    ui->textEdit->moveCursor (QTextCursor::End);
}
