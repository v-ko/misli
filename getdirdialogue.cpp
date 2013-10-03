/* This program is licensed under GNU GPL . For the full notice see the
 * license.txt file or google the full text of the GPL*/

#include "getdirdialogue.h"
#include "ui_getdirdialogue.h"
#include "mislidesktopgui.h"

GetDirDialogue::GetDirDialogue(MisliDesktopGui *misli_dg_):
    ui(new Ui::GetDirDialogue)
{
    ui->setupUi(this);
    misli_dg=misli_dg_;
    addAction(ui->actionEscape);//the action is defined in the .ui file and is not used . QActions must be added to a widget to work
}

GetDirDialogue::~GetDirDialogue()
{
    delete ui;
}

void GetDirDialogue::showEvent(QShowEvent *)
{
  ui->lineEdit->setText("");
}

//Functions
void GetDirDialogue::input_done()
{
    QString path = ui->lineEdit->text();
    QDir dir;

    if(path.size()!=0){ //if path is empty the current dir is used and we don't want that
        dir.cd(path);
    }

    if( !dir.exists() ){
        ui->explainLabel->setText(tr("Directory doesn't exist"));
        return;
    }else {
        misli_i()->add_dir(dir.absolutePath());
        misli_dg->misli_w->export_settings();
        close();
    }
}

void GetDirDialogue::get_dir_dialogue()
{
    ui->lineEdit->setText(fileDialogue.getExistingDirectory(this,tr("Choose directory")));
}

MisliInstance *GetDirDialogue::misli_i()
{
    return misli_dg->misli_i;
}
